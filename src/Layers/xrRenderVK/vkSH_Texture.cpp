#include "stdafx.h"
#pragma hdrstop

#include "Layers/xrRender/ResourceManager.h"

#ifndef _EDITOR
#include "xrEngine/Render.h"
#endif

#include "xrEngine/tntQAVI.h"
#include "xrEngine/xrTheora_Surface.h"
#include "StateManager/dx10ShaderResourceStateCache.h"

#define PRIORITY_HIGH 12
#define PRIORITY_NORMAL 8
#define PRIORITY_LOW 4

void resptrcode_texture::create(LPCSTR _name) { _set(RImplementation.Resources->_CreateTexture(_name)); }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
    pSurface = NULL;
    m_pSRView = NULL;
    pAVI = NULL;
    pTheora = NULL;
    desc_cache = 0;
    seqMSPF = 0;
    flags.MemoryUsage = 0;
    flags.bLoaded = false;
    flags.bUser = false;
    flags.seqCycles = FALSE;
    flags.bLoadedAsStaging = FALSE;
    m_material = 1.0f;
    bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_load);
}

CTexture::~CTexture()
{
    Unload();

    // release external reference
    RImplementation.Resources->_DeleteTexture(this);
}

void CTexture::surface_set(ID3DBaseTexture* surf)
{
    if (surf)
        surf->AddRef();
    _RELEASE(pSurface);
    _RELEASE(m_pSRView);

    pSurface = surf;

    if (pSurface)
    {
        desc_update();
        CHK_VK(DXView::Create(pSurface, &m_pSRView));
    }
}

ID3DBaseTexture* CTexture::surface_get()
{
    if (flags.bLoadedAsStaging)
        ProcessStaging();

    if (pSurface)
        pSurface->AddRef();
    return pSurface;
}

void CTexture::PostLoad()
{
    if (pTheora)
        bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_theora);
    else if (pAVI)
        bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_avi);
    else if (!seqDATA.empty())
        bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_seq);
    else
        bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_normal);
}

void CTexture::apply_load(u32 dwStage)
{
    if (!flags.bLoaded)
        Load();
    else
        PostLoad();
    bind(dwStage);
}

void CTexture::ProcessStaging()
{
    VERIFY(pSurface);
    VERIFY(flags.bLoadedAsStaging);

    /*
    for( int i=0; i<iNumSubresources; ++i)
    {
        HW.pDevice->CopySubresourceRegion(
            pTargetSurface,
            i,
            0,
            0,
            0,
            pSurface,
            i,
            0
            );
    }
    */

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &pSurface->m_copyCmd;

    CHK_VK(vkQueueSubmit(HW.queue, 1, &submitInfo, VK_NULL_HANDLE));
    CHK_VK(vkQueueWaitIdle(HW.queue));

    flags.bLoadedAsStaging = FALSE;

    // Destroy the staging buffer
    vkFreeMemory(HW.device, pSurface->m_stagingMemory, nullptr);
    pSurface->m_stagingMemory = VK_NULL_HANDLE;
    vkDestroyBuffer(HW.device, pSurface->m_stagingBuffer, nullptr);
    pSurface->m_stagingBuffer = VK_NULL_HANDLE;
}

void CTexture::Apply(u32 dwStage)
{
    if (flags.bLoadedAsStaging)
        ProcessStaging();

    // if( !RImplementation.o.dx10_msaa )
    //   VERIFY( !((!pSurface)^(!m_pSRView)) );	//	Both present or both missing
    // else
    //{
    // if( ((!pSurface)^(!m_pSRView)) )
    //   return;
    //}

    if (dwStage < rstVertex) //	Pixel shader stage resources
    {
        // HW.pDevice->PSSetShaderResources(dwStage, 1, &m_pSRView);
        SRVSManager.SetPSResource(dwStage, m_pSRView);
    }
    else if (dwStage < rstGeometry) //	Vertex shader stage resources
    {
        // HW.pDevice->VSSetShaderResources(dwStage-rstVertex, 1, &m_pSRView);
        SRVSManager.SetVSResource(dwStage - rstVertex, m_pSRView);
    }
    else if (dwStage < rstHull) //	Geometry shader stage resources
    {
        // HW.pDevice->GSSetShaderResources(dwStage-rstGeometry, 1, &m_pSRView);
        SRVSManager.SetGSResource(dwStage - rstGeometry, m_pSRView);
    }
#ifdef USE_DX11
    else if (dwStage < rstDomain) //	Geometry shader stage resources
    {
        SRVSManager.SetHSResource(dwStage - rstHull, m_pSRView);
    }
    else if (dwStage < rstCompute) //	Geometry shader stage resources
    {
        SRVSManager.SetDSResource(dwStage - rstDomain, m_pSRView);
    }
    else if (dwStage < rstInvalid) //	Geometry shader stage resources
    {
        SRVSManager.SetCSResource(dwStage - rstCompute, m_pSRView);
    }
#endif
    else
        VERIFY("Invalid stage");
}

void CTexture::apply_theora(u32 dwStage)
{
    if (pTheora->Update(m_play_time != 0xFFFFFFFF ? m_play_time : Device.dwTimeContinual))
    {
        R_ASSERT(VK_IMAGE_TYPE_2D == pSurface->m_createInfo.imageType);
        // Get sub resource layout
        // Mip map count, array layer, etc.
        VkImageSubresource subRes = {};
        subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = pTheora->Width(true);
        rect.bottom = pTheora->Height(true);

        u32 _w = pTheora->Width(false);

        // Get sub resources layout 
        // Includes row pitch, size offsets, etc.
        VkSubresourceLayout subResLayout;
        vkGetImageSubresourceLayout(HW.device, pSurface->m_image, &subRes, &subResLayout);
        R_ASSERT(subResLayout.rowPitch == int(pTheora->Width(false) * 4));

// R_CHK				(T2D->LockRect(0,&R,&rect,0));
        void *data;
        CHK_VK(vkMapMemory(HW.device, pSurface->m_memory, 0, VK_WHOLE_SIZE, 0, &data));
        // R_ASSERT			(R.Pitch == int(pTheora->Width(false)*4));
        int _pos = 0;
        pTheora->DecompressFrame((u32*)data, _w - rect.right, _pos);
        VERIFY(u32(_pos) == rect.bottom * _w);
// R_CHK				(T2D->UnlockRect(0));
        vkUnmapMemory(HW.device, pSurface->m_memory);
    }
    Apply(dwStage);
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
};
void CTexture::apply_avi(u32 dwStage)
{
    if (pAVI->NeedUpdate())
    {
        R_ASSERT(VK_IMAGE_TYPE_2D == pSurface->m_createInfo.imageType);
        // Get sub resource layout
        // Mip map count, array layer, etc.
        VkImageSubresource subRes = {};
        subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        // Get sub resources layout 
        // Includes row pitch, size offsets, etc.
        VkSubresourceLayout subResLayout;
        vkGetImageSubresourceLayout(HW.device, pSurface->m_image, &subRes, &subResLayout);
        R_ASSERT(subResLayout.rowPitch == int(pAVI->m_dwWidth * 4));

// AVI
// R_CHK	(T2D->LockRect(0,&R,NULL,0));
        void *data;
        CHK_VK(vkMapMemory(HW.device, pSurface->m_memory, 0, VK_WHOLE_SIZE, 0, &data));
        BYTE* ptr;
        pAVI->GetFrame(&ptr);
        CopyMemory(data, ptr, pAVI->m_dwWidth * pAVI->m_dwHeight * 4);
// R_CHK	(T2D->UnlockRect(0));
        vkUnmapMemory(HW.device, pSurface->m_memory);
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
};
void CTexture::apply_seq(u32 dwStage)
{
    // SEQ
    u32 frame = Device.dwTimeContinual / seqMSPF; // Device.dwTimeGlobal
    u32 frame_data = seqDATA.size();
    if (flags.seqCycles)
    {
        u32 frame_id = frame % (frame_data * 2);
        if (frame_id >= frame_data)
            frame_id = (frame_data - 1) - (frame_id % frame_data);
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }
    else
    {
        u32 frame_id = frame % frame_data;
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
};
void CTexture::apply_normal(u32 dwStage)
{
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
};

void CTexture::Preload()
{
    m_bumpmap = RImplementation.Resources->m_textures_description.GetBumpName(cName);
    m_material = RImplementation.Resources->m_textures_description.GetMaterial(cName);
}

void CTexture::Load()
{
    flags.bLoaded = true;
    desc_cache = 0;
    if (pSurface)
        return;

    flags.bUser = false;
    flags.MemoryUsage = 0;
    if (0 == stricmp(*cName, "$null"))
        return;
    if (0 != strstr(*cName, "$user$"))
    {
        flags.bUser = true;
        return;
    }

    Preload();

    // Check for OGM
    string_path fn;
    if (FS.exist(fn, "$game_textures$", *cName, ".ogm"))
    {
        // AVI
        pTheora = new CTheoraSurface();
        m_play_time = 0xFFFFFFFF;

        if (!pTheora->Load(fn))
        {
            xr_delete(pTheora);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.MemoryUsage = pTheora->Width(true) * pTheora->Height(true) * 4;
            pTheora->Play(TRUE, Device.dwTimeContinual);

            // Now create texture
            u32 _w = pTheora->Width(false);
            u32 _h = pTheora->Height(false);

            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
            imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            imageCreateInfo.extent = { _w, _h, 1 };
            imageCreateInfo.flags = 0;
            VkResult hrr = DXTexture::Create(&imageCreateInfo, nullptr, 0, false, &pSurface);

            if (hrr != VK_SUCCESS)
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pTheora);
                pSurface = 0;
                m_pSRView = 0;
            }
            else
            {
                CHK_VK(DXView::Create(pSurface, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, "$game_textures$", *cName, ".avi"))
    {
        // AVI
        pAVI = new CAviPlayerCustom();

        if (!pAVI->Load(fn))
        {
            xr_delete(pAVI);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.MemoryUsage = pAVI->m_dwWidth * pAVI->m_dwHeight * 4;

            // Now create texture
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
            imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            imageCreateInfo.extent = { pAVI->m_dwWidth, pAVI->m_dwHeight, 1 };
            imageCreateInfo.flags = 0;
            VkResult hrr = DXTexture::Create(&imageCreateInfo, nullptr, 0, false, &pSurface);

            if (hrr != VK_SUCCESS)
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pAVI);
                pSurface = 0;
                m_pSRView = 0;
            }
            else
            {
                CHK_VK(DXView::Create(pSurface, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, "$game_textures$", *cName, ".seq"))
    {
        // Sequence
        string256 buffer;
        IReader* _fs = FS.r_open(fn);

        flags.seqCycles = FALSE;
        _fs->r_string(buffer, sizeof(buffer));
        if (0 == stricmp(buffer, "cycled"))
        {
            flags.seqCycles = TRUE;
            _fs->r_string(buffer, sizeof(buffer));
        }
        u32 fps = atoi(buffer);
        seqMSPF = 1000 / fps;

        while (!_fs->eof())
        {
            _fs->r_string(buffer, sizeof(buffer));
            _Trim(buffer);
            if (buffer[0])
            {
                // Load another texture
                u32 mem = 0;
                pSurface = ::RImplementation.texture_load(buffer, mem);
                if (pSurface)
                {
                    // pSurface->SetPriority	(PRIORITY_LOW);
                    seqDATA.push_back(pSurface);
                    m_seqSRView.push_back(0);
                    CHK_VK(DXView::Create(seqDATA.back(), &m_seqSRView.back()));
                    flags.MemoryUsage += mem;
                }
            }
        }
        pSurface = 0;
        FS.r_close(_fs);
    }
    else
    {
        // Normal texture
        u32 mem = 0;
        // pSurface = ::RImplementation.texture_load	(*cName,mem);
        pSurface = ::RImplementation.texture_load(*cName, mem, true);

        if (pSurface->m_stagingBuffer)
            flags.bLoadedAsStaging = TRUE;

        // Calc memory usage and preload into vid-mem
        if (pSurface)
        {
            // pSurface->SetPriority	(PRIORITY_NORMAL);
            flags.MemoryUsage = mem;
        }
    }

    if (pSurface && !m_pSRView)
        CHK_VK(DXView::Create(pSurface, &m_pSRView));
    PostLoad();
}

void CTexture::Unload()
{
#ifdef DEBUG
    string_path msg_buff;
    xr_sprintf(msg_buff, sizeof(msg_buff), "* Unloading texture [%s] pSurface RefCount=", cName.c_str());
#endif // DEBUG

    //.	if (flags.bLoaded)		Msg		("* Unloaded: %s",cName.c_str());

    flags.bLoaded = FALSE;
    flags.bLoadedAsStaging = FALSE;
    if (!seqDATA.empty())
    {
        for (u32 I = 0; I < seqDATA.size(); I++)
        {
            _RELEASE(seqDATA[I]);
            _RELEASE(m_seqSRView[I]);
        }
        seqDATA.clear();
        m_seqSRView.clear();
        pSurface = 0;
        m_pSRView = 0;
    }

#ifdef DEBUG
    _SHOW_REF(msg_buff, pSurface);
#endif // DEBUG
    _RELEASE(pSurface);
    _RELEASE(m_pSRView);

    xr_delete(pAVI);
    xr_delete(pTheora);

    bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_load);
}

void CTexture::desc_update()
{
    desc_cache = pSurface;
    if (pSurface)
    {
        desc = pSurface->m_createInfo;
    }
}

void CTexture::video_Play(BOOL looped, u32 _time)
{
    if (pTheora)
        pTheora->Play(looped, (_time != 0xFFFFFFFF) ? (m_play_time = _time) : Device.dwTimeContinual);
}

void CTexture::video_Pause(BOOL state)
{
    if (pTheora)
        pTheora->Pause(state);
}

void CTexture::video_Stop()
{
    if (pTheora)
        pTheora->Stop();
}

BOOL CTexture::video_IsPlaying() { return (pTheora) ? pTheora->IsPlaying() : FALSE; }
