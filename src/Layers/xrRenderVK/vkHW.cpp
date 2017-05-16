// dx10HW.cpp: implementation of the DX10 specialisation of CHW.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Layers/xrRender/HW.h"
#include "xrEngine/XR_IOConsole.h"
#include "Include/xrAPI/xrAPI.h"

#ifndef _EDITOR
void fill_vid_mode_list(CHW* _hw);
void free_vid_mode_list();

void fill_render_mode_list();
void free_render_mode_list();
#else
void fill_vid_mode_list(CHW* _hw) {}
void free_vid_mode_list() {}
void fill_render_mode_list() {}
void free_render_mode_list() {}
#endif

CHW HW;

CHW::CHW() : m_move_window(true) {}
CHW::~CHW() {}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "S.T.A.L.K.E.R.";
    app_info.applicationVersion = VK_MAKE_VERSION(5, 0, 0);
    app_info.pEngineName = "OpenXRay";
    app_info.engineVersion = VK_MAKE_VERSION(5, 0, 0);
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef DEBUG
    const char* const validationLayers[] = {"VK_LAYER_LUNARG_standard_validation"};
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = validationLayers;
#endif
    const char* const instanceExtensionNames[] = {VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#else
        VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
    };
    createInfo.enabledExtensionCount = 2;
    createInfo.ppEnabledExtensionNames = instanceExtensionNames;
    createInfo.pApplicationInfo = &app_info;

    vkCreateInstance(&createInfo, NULL, &instance);
    initResources(resources);
}

void CHW::DestroyD3D()
{
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
}

void CHW::CreateDevice(HWND m_hWnd, bool move_window)
{
    m_move_window = move_window;
    CreateD3D();

    // Get the number of devices (GPUs) available.
    uint32_t deviceCount = 0;
    VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    // Allocate space and get the list of devices.
    std::vector<VkPhysicalDevice> adapters(deviceCount);
    res = vkEnumeratePhysicalDevices(instance, &deviceCount, adapters.data());
    adapter = adapters[0]; // Select the first GPU.

    // Get the physical device properties.
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(adapter, &properties);
    Caps.id_vendor = properties.vendorID;
    Caps.id_device = properties.deviceID;

    // Get the physical device features.
    vkGetPhysicalDeviceFeatures(adapter, &features);
    vkGetPhysicalDeviceMemoryProperties(adapter, &memoryProperties);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, NULL);

    // Iterate over each queue to learn whether it supports presenting:
    std::vector<VkBool32> supportsPresent(queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(adapter, i, surface, &supportsPresent[i]);
    }

    std::vector<VkQueueFamilyProperties> queueProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, queueProps.data());

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && supportsPresent[i] == VK_TRUE)
        {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }

    // Generate error if could not find queues that support graphics
    // and present
    if (graphicsQueueFamilyIndex == UINT32_MAX)
    {
        Msg("Failed to initialize graphics hardware.\n"
            "Please try to restart the game.\n");
        FlushLog();
        MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!",
            MB_OK | MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 0);
    }

    VkDeviceQueueCreateInfo queueInfo = {};
    bool found = false;
    for (unsigned int i = 0; i < queueFamilyCount; i++)
    {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueInfo.queueFamilyIndex = i;
            found = true;
            break;
        }
    }

    float queuePriorities[1] = {0.0};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    const char* const deviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensionNames;

    CHK_VK(vkCreateDevice(adapter, &deviceInfo, NULL, &device));

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.hinstance = GetModuleHandle(NULL);
    createInfo.hwnd = m_hWnd;
    CHK_VK(vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface));
#else
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.connection = info.connection;
    createInfo.window = info.window;
    CHK_VK(vkCreateXcbSurfaceKHR(info.inst, &createInfo, NULL, &info.surface));
#endif // _WIN32

    BOOL bWindowed = !psDeviceFlags.is(rsFullscreen);

    // Get the list of VkFormats that are supported:
    VkFormat format;
    uint32_t formatCount;
    CHK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &formatCount, NULL));
    std::vector<VkSurfaceFormatKHR> surfFormats(formatCount);
    CHK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &formatCount, surfFormats.data()));
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        assert(formatCount >= 1);
        format = surfFormats[0].format;
    }

    VkSurfaceCapabilitiesKHR surfCapabilities;
    CHK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, surface, &surfCapabilities));

    uint32_t presentModeCount;
    CHK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &presentModeCount, NULL));

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    CHK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &presentModeCount, presentModes.data()));

    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        selectResolution(swapchainExtent.width, swapchainExtent.height, bWindowed);
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        }
        else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        }
        else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfCapabilities.currentExtent;
    }

    // The FIFO present mode is guaranteed by the spec to be supported
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swap chain.
    // We need to acquire only 1 presentable image at at time.
    // Asking for minImageCount images ensures that we can acquire
    // 1 presentable image as long as we present it before attempting
    // to acquire another.
    uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfCapabilities.currentTransform;
    }

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++)
    {
        if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
        {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = surface;
    swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
    swapchain_ci.imageFormat = format;
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = compositeAlpha;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;

    CHK_VK(vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapchain));

    uint32_t imageCount;
    CHK_VK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL));
    swapimages.resize(imageCount);
    CHK_VK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapimages.data()));

    for (uint32_t i = 0; i < imageCount; i++)
    {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.flags = 0;
        color_image_view.image = swapimages[i];
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.format = format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;

        CHK_VK(vkCreateImageView(device, &color_image_view, NULL, &swapviews[i]));
    }

#ifndef _EDITOR
    updateWindowProps(m_hWnd);
    fill_vid_mode_list(this);
#endif

    // Create a command pool to allocate our command buffer from
    // TODO: Move this to a command buffer manager
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    cmdPoolInfo.flags = 0;

    CHK_VK(vkCreateCommandPool(device, &cmdPoolInfo, NULL, &cmdPool));

    // Create the command buffer from the command pool
    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.commandPool = cmdPool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    CHK_VK(vkAllocateCommandBuffers(device, &cmd, &context));
}

void CHW::DestroyDevice()
{
    for (auto it = swapviews.begin(); it != swapviews.end(); it++)
        vkDestroyImageView(device, *it, NULL);
    vkDestroySwapchainKHR(device, swapchain, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);

    DestroyD3D();

#ifndef _EDITOR
    free_vid_mode_list();
#endif
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::Reset(HWND hwnd)
{
    // TODO: VK: Implement swapchain recreation
    updateWindowProps(hwnd);
}

D3DFORMAT CHW::selectDepthStencil(D3DFORMAT fTarget)
{
// R3 hack
#pragma todo("R3 need to specify depth format")
    return D3DFMT_D24S8;
}

void CHW::selectResolution(u32& dwWidth, u32& dwHeight, BOOL bWindowed)
{
    fill_vid_mode_list(this);

    if (bWindowed)
    {
        dwWidth = psCurrentVidMode[0];
        dwHeight = psCurrentVidMode[1];
    }
    else // check
    {
        string64 buff;
        xr_sprintf(buff, sizeof(buff), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]);

        if (_ParseItem(buff, GlobalEnv.vid_mode_token) == u32(-1)) // not found
        { // select safe
            xr_sprintf(buff, sizeof(buff), "vid_mode %s", GlobalEnv.vid_mode_token[0].name);
            Console->Execute(buff);
        }

        dwWidth = psCurrentVidMode[0];
        dwHeight = psCurrentVidMode[1];
    }
}

BOOL CHW::support(D3DFORMAT fmt, DWORD type, DWORD usage)
{
    //  TODO: VK: implement stub for this code.
    VERIFY(!"Implement CHW::support");
    /*
    HRESULT hr      = pD3D->CheckDeviceFormat(DevAdapter,DevT,Caps.fTarget,usage,(D3DRESOURCETYPE)type,fmt);
    if (FAILED(hr)) return FALSE;
    else            return TRUE;
    */
    return TRUE;
}

void CHW::updateWindowProps(HWND m_hWnd)
{
    //  BOOL    bWindowed               = strstr(Core.Params,"-dedicated") ? TRUE : !psDeviceFlags.is   (rsFullscreen);
    BOOL bWindowed = !psDeviceFlags.is(rsFullscreen);

    u32 dwWindowStyle = 0;
    // Set window properties depending on what mode were in.
    if (bWindowed)
    {
        if (m_move_window)
        {
            dwWindowStyle = WS_BORDER | WS_VISIBLE;
            if (!strstr(Core.Params, "-no_dialog_header"))
                dwWindowStyle |= WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
            SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyle);
            // When moving from fullscreen to windowed mode, it is important to
            // adjust the window size after recreating the device rather than
            // beforehand to ensure that you get the window size you want.  For
            // example, when switching from 640x480 fullscreen to windowed with
            // a 1000x600 window on a 1024x768 desktop, it is impossible to set
            // the window size to 1000x600 until after the display mode has
            // changed to 1024x768, because windows cannot be larger than the
            // desktop.

            RECT m_rcWindowBounds;
            RECT DesktopRect;

            GetClientRect(GetDesktopWindow(), &DesktopRect);

            SetRect(&m_rcWindowBounds, (DesktopRect.right - swapchainExtent.width) / 2,
                (DesktopRect.bottom - swapchainExtent.height) / 2, (DesktopRect.right + swapchainExtent.width) / 2,
                (DesktopRect.bottom + swapchainExtent.height) / 2);

            AdjustWindowRect(&m_rcWindowBounds, dwWindowStyle, FALSE);

            SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                (m_rcWindowBounds.right - m_rcWindowBounds.left), (m_rcWindowBounds.bottom - m_rcWindowBounds.top),
                SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_DRAWFRAME);
        }
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyle = (WS_POPUP | WS_VISIBLE));
    }

    ShowCursor(FALSE);
    SetForegroundWindow(m_hWnd);
}

void CHW::initResources(TBuiltInResource &Resources)
{
    Resources.maxLights = 32;
    Resources.maxClipPlanes = 6;
    Resources.maxTextureUnits = 32;
    Resources.maxTextureCoords = 32;
    Resources.maxVertexAttribs = 64;
    Resources.maxVertexUniformComponents = 4096;
    Resources.maxVaryingFloats = 64;
    Resources.maxVertexTextureImageUnits = 32;
    Resources.maxCombinedTextureImageUnits = 80;
    Resources.maxTextureImageUnits = 32;
    Resources.maxFragmentUniformComponents = 4096;
    Resources.maxDrawBuffers = 32;
    Resources.maxVertexUniformVectors = 128;
    Resources.maxVaryingVectors = 8;
    Resources.maxFragmentUniformVectors = 16;
    Resources.maxVertexOutputVectors = 16;
    Resources.maxFragmentInputVectors = 15;
    Resources.minProgramTexelOffset = -8;
    Resources.maxProgramTexelOffset = 7;
    Resources.maxClipDistances = 8;
    Resources.maxComputeWorkGroupCountX = 65535;
    Resources.maxComputeWorkGroupCountY = 65535;
    Resources.maxComputeWorkGroupCountZ = 65535;
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 64;
    Resources.maxComputeUniformComponents = 1024;
    Resources.maxComputeTextureImageUnits = 16;
    Resources.maxComputeImageUniforms = 8;
    Resources.maxComputeAtomicCounters = 8;
    Resources.maxComputeAtomicCounterBuffers = 1;
    Resources.maxVaryingComponents = 60;
    Resources.maxVertexOutputComponents = 64;
    Resources.maxGeometryInputComponents = 64;
    Resources.maxGeometryOutputComponents = 128;
    Resources.maxFragmentInputComponents = 128;
    Resources.maxImageUnits = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    Resources.maxCombinedShaderOutputResources = 8;
    Resources.maxImageSamples = 0;
    Resources.maxVertexImageUniforms = 0;
    Resources.maxTessControlImageUniforms = 0;
    Resources.maxTessEvaluationImageUniforms = 0;
    Resources.maxGeometryImageUniforms = 0;
    Resources.maxFragmentImageUniforms = 8;
    Resources.maxCombinedImageUniforms = 8;
    Resources.maxGeometryTextureImageUnits = 16;
    Resources.maxGeometryOutputVertices = 256;
    Resources.maxGeometryTotalOutputComponents = 1024;
    Resources.maxGeometryUniformComponents = 1024;
    Resources.maxGeometryVaryingComponents = 64;
    Resources.maxTessControlInputComponents = 128;
    Resources.maxTessControlOutputComponents = 128;
    Resources.maxTessControlTextureImageUnits = 16;
    Resources.maxTessControlUniformComponents = 1024;
    Resources.maxTessControlTotalOutputComponents = 4096;
    Resources.maxTessEvaluationInputComponents = 128;
    Resources.maxTessEvaluationOutputComponents = 128;
    Resources.maxTessEvaluationTextureImageUnits = 16;
    Resources.maxTessEvaluationUniformComponents = 1024;
    Resources.maxTessPatchComponents = 120;
    Resources.maxPatchVertices = 32;
    Resources.maxTessGenLevel = 64;
    Resources.maxViewports = 16;
    Resources.maxVertexAtomicCounters = 0;
    Resources.maxTessControlAtomicCounters = 0;
    Resources.maxTessEvaluationAtomicCounters = 0;
    Resources.maxGeometryAtomicCounters = 0;
    Resources.maxFragmentAtomicCounters = 8;
    Resources.maxCombinedAtomicCounters = 8;
    Resources.maxAtomicCounterBindings = 1;
    Resources.maxVertexAtomicCounterBuffers = 0;
    Resources.maxTessControlAtomicCounterBuffers = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers = 0;
    Resources.maxGeometryAtomicCounterBuffers = 0;
    Resources.maxFragmentAtomicCounterBuffers = 1;
    Resources.maxCombinedAtomicCounterBuffers = 1;
    Resources.maxAtomicCounterBufferSize = 16384;
    Resources.maxTransformFeedbackBuffers = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances = 8;
    Resources.maxCombinedClipAndCullDistances = 8;
    Resources.maxSamples = 4;
    Resources.limits.nonInductiveForLoops = 1;
    Resources.limits.whileLoops = 1;
    Resources.limits.doWhileLoops = 1;
    Resources.limits.generalUniformIndexing = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing = 1;
    Resources.limits.generalSamplerIndexing = 1;
    Resources.limits.generalVariableIndexing = 1;
    Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

struct _uniq_mode
{
    _uniq_mode(LPCSTR v) : _val(v) {}
    LPCSTR _val;
    bool operator()(LPCSTR _other) { return !stricmp(_val, _other); }
};

#ifndef _EDITOR

void free_vid_mode_list()
{
    for (int i = 0; GlobalEnv.vid_mode_token[i].name; i++)
    {
        delete GlobalEnv.vid_mode_token[i].name;
    }
    delete GlobalEnv.vid_mode_token;
    GlobalEnv.vid_mode_token = NULL;
}

void fill_vid_mode_list()
{
    if (GlobalEnv.vid_mode_token != NULL)
        return;
    xr_vector<LPCSTR> _tmp;

    DWORD iModeNum = 0;
    DEVMODE dmi;
    ZeroMemory(&dmi, sizeof(dmi));
    dmi.dmSize = sizeof(dmi);

    while (EnumDisplaySettings(nullptr, iModeNum++, &dmi) != 0)
    {
        string32 str;

        if (dmi.dmPelsWidth < 800)
            continue;

        sprintf_s(str, sizeof(str), "%dx%d", dmi.dmPelsWidth, dmi.dmPelsHeight);

        if (_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
            continue;

        _tmp.push_back(NULL);
        _tmp.back() = xr_strdup(str);
    }

    u32 _cnt = _tmp.size() + 1;

    GlobalEnv.vid_mode_token = xr_alloc<xr_token>(_cnt);

    GlobalEnv.vid_mode_token[_cnt - 1].id = -1;
    GlobalEnv.vid_mode_token[_cnt - 1].name = NULL;

#ifdef DEBUG
    Msg("Available video modes[%d]:", _tmp.size());
#endif // DEBUG
    for (u32 i = 0; i < _tmp.size(); ++i)
    {
        GlobalEnv.vid_mode_token[i].id = i;
        GlobalEnv.vid_mode_token[i].name = _tmp[i];
#ifdef DEBUG
        Msg("[%s]", _tmp[i]);
#endif // DEBUG
    }
}

#endif
