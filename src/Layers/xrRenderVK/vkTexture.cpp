#include "stdafx.h"
#pragma hdrstop

#include <cstdlib>
#include <string>
#include <gli/gli.hpp>

void fix_texture_name(LPSTR fn)
{
    LPSTR _ext = strext(fn);
    if (_ext && (!stricmp(_ext, ".tga") || !stricmp(_ext, ".dds") || !stricmp(_ext, ".bmp") ||
        !stricmp(_ext, ".ogm")))
    {
        *_ext = 0;
    }
}

int get_texture_load_lod(LPCSTR fn)
{
    CInifile::Sect& sect = pSettings->r_section("reduce_lod_texture_list");
    CInifile::SectCIt it_ = sect.Data.begin();
    CInifile::SectCIt it_e_ = sect.Data.end();

    ENGINE_API bool is_enough_address_space_available();
    static bool enough_address_space_available = is_enough_address_space_available();

    CInifile::SectCIt it = it_;
    CInifile::SectCIt it_e = it_e_;

    for (; it != it_e; ++it)
    {
        if (strstr(fn, it->first.c_str()))
        {
            if (psTextureLOD < 1)
            {
                if (enough_address_space_available)
                    return 0;
                else
                    return 1;
            }
            else if (psTextureLOD < 3)
                return 1;
            else
                return 2;
        }
    }

    if (psTextureLOD < 2)
    {
        //if (enough_address_space_available)
        return 0;
        //else
        //    return 1;
    }
    else if (psTextureLOD < 4)
        return 1;
    else
        return 2;
}

u32 calc_texture_size(int lod, u32 mip_cnt, u32 orig_size)
{
    if (1 == mip_cnt)
        return orig_size;

    int _lod = lod;
    float res = float(orig_size);

    while (_lod > 0)
    {
        --_lod;
        res -= res / 1.333f;
    }
    return iFloor(res);
}

const float _BUMPHEIGH = 8.f;
//////////////////////////////////////////////////////////////////////
// Utility pack
//////////////////////////////////////////////////////////////////////
IC u32 GetPowerOf2Plus1(u32 v)
{
    u32 cnt = 0;
    while (v)
    {
        v >>= 1;
        cnt++;
    };
    return cnt;
}
IC void Reduce(int& w, int& h, int& l, int& skip)
{
    while ((l > 1) && skip)
    {
        w /= 2;
        h /= 2;
        l -= 1;

        skip--;
    }
    if (w < 1)
        w = 1;
    if (h < 1)
        h = 1;
}

IC void Reduce(UINT& w, UINT& h, int l, int skip)
{
    while ((l > 1) && skip)
    {
        w /= 2;
        h /= 2;
        l -= 1;

        skip--;
    }
    if (w < 1)
        w = 1;
    if (h < 1)
        h = 1;
}
/*
void TW_Save(ID3DTexture2D* T, LPCSTR name, LPCSTR prefix, LPCSTR postfix)
{
    string256 fn;
    strconcat(sizeof(fn), fn, name, "_", prefix, "-", postfix);
    for (int it = 0; it < int(xr_strlen(fn)); it++)
        if ('\\' == fn[it])
            fn[it] = '_';
    string256 fn2;
    strconcat(sizeof(fn2), fn2, "debug\\", fn, ".dds");
    Log("* debug texture save: ", fn2);
#ifdef USE_DX11
    R_CHK(D3DX11SaveTextureToFile(HW.pContext, T, D3DX11_IFF_DDS, fn2));
#else
    R_CHK(D3DX10SaveTextureToFile(T, D3DX10_IFF_DDS, fn2));
#endif
}
*/
/*
ID3DTexture2D*  TW_LoadTextureFromTexture
(
 ID3DTexture2D*     t_from,
 D3DFORMAT&             t_dest_fmt,
 int                        levels_2_skip,
 u32&                   w,
 u32&                   h
 )
{
    // Calculate levels & dimensions
    ID3DTexture2D*      t_dest          = NULL;
    D3DSURFACE_DESC         t_from_desc0    ;
    R_CHK                   (t_from->GetLevelDesc   (0,&t_from_desc0));
    int levels_exist        = t_from->GetLevelCount();
    int top_width           = t_from_desc0.Width;
    int top_height          = t_from_desc0.Height;
    Reduce                  (top_width,top_height,levels_exist,levels_2_skip);

    // Create HW-surface
    if (D3DX_DEFAULT==t_dest_fmt)   t_dest_fmt = t_from_desc0.Format;
    R_CHK                   (D3DXCreateTexture(
        HW.pDevice,
        top_width,top_height,
        levels_exist,0,t_dest_fmt,
        D3DPOOL_MANAGED,&t_dest
        ));

    // Copy surfaces & destroy temporary
    ID3DTexture2D* T_src= t_from;
    ID3DTexture2D* T_dst= t_dest;

    int     L_src           = T_src->GetLevelCount  ()-1;
    int     L_dst           = T_dst->GetLevelCount  ()-1;
    for (; L_dst>=0; L_src--,L_dst--)
    {
        // Get surfaces
        IDirect3DSurface9       *S_src, *S_dst;
        R_CHK   (T_src->GetSurfaceLevel (L_src,&S_src));
        R_CHK   (T_dst->GetSurfaceLevel (L_dst,&S_dst));

        // Copy
        R_CHK   (D3DXLoadSurfaceFromSurface(S_dst,NULL,NULL,S_src,NULL,NULL,D3DX_FILTER_NONE,0));

        // Release surfaces
        _RELEASE                (S_src);
        _RELEASE                (S_dst);
    }

    // OK
    w                       = top_width;
    h                       = top_height;
    return                  t_dest;
}

template    <class _It>
IC  void    TW_Iterate_1OP
(
 ID3DTexture2D*     t_dst,
 ID3DTexture2D*     t_src,
 const _It              pred
 )
{
    DWORD mips                          = t_dst->GetLevelCount();
    R_ASSERT                            (mips == t_src->GetLevelCount());
    for (DWORD i = 0; i < mips; i++)    {
        D3DLOCKED_RECT              Rsrc,Rdst;
        D3DSURFACE_DESC             desc,descS;

        t_dst->GetLevelDesc         (i, &desc);
        t_src->GetLevelDesc         (i, &descS);
        VERIFY                      (desc.Format==descS.Format);
        VERIFY                      (desc.Format==D3DFMT_A8R8G8B8);
        t_src->LockRect             (i,&Rsrc,0,0);
        t_dst->LockRect             (i,&Rdst,0,0);
        for (u32 y = 0; y < desc.Height; y++)   {
            for (u32 x = 0; x < desc.Width; x++)    {
                DWORD&  pSrc    = *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
                DWORD&  pDst    = *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);
                pDst            = pred(pDst,pSrc);
            }
        }
        t_dst->UnlockRect           (i);
        t_src->UnlockRect           (i);
    }
}
template    <class _It>
IC  void    TW_Iterate_2OP
(
 ID3DTexture2D*     t_dst,
 ID3DTexture2D*     t_src0,
 ID3DTexture2D*     t_src1,
 const _It              pred
 )
{
    DWORD mips                          = t_dst->GetLevelCount();
    R_ASSERT                            (mips == t_src0->GetLevelCount());
    R_ASSERT                            (mips == t_src1->GetLevelCount());
    for (DWORD i = 0; i < mips; i++)    {
        D3DLOCKED_RECT              Rsrc0,Rsrc1,Rdst;
        D3DSURFACE_DESC             desc,descS0,descS1;

        t_dst->GetLevelDesc         (i, &desc);
        t_src0->GetLevelDesc        (i, &descS0);
        t_src1->GetLevelDesc        (i, &descS1);
        VERIFY                      (desc.Format==descS0.Format);
        VERIFY                      (desc.Format==descS1.Format);
        VERIFY                      (desc.Format==D3DFMT_A8R8G8B8);
        t_src0->LockRect            (i,&Rsrc0,  0,0);
        t_src1->LockRect            (i,&Rsrc1,  0,0);
        t_dst->LockRect             (i,&Rdst,   0,0);
        for (u32 y = 0; y < desc.Height; y++)   {
            for (u32 x = 0; x < desc.Width; x++)    {
                DWORD&  pSrc0   = *(((DWORD*)((BYTE*)Rsrc0.pBits + (y * Rsrc0.Pitch)))+x);
                DWORD&  pSrc1   = *(((DWORD*)((BYTE*)Rsrc1.pBits + (y * Rsrc1.Pitch)))+x);
                DWORD&  pDst    = *(((DWORD*)((BYTE*)Rdst.pBits  + (y * Rdst.Pitch)))+x);
                pDst            = pred(pDst,pSrc0,pSrc1);
            }
        }
        t_dst->UnlockRect           (i);
        t_src0->UnlockRect          (i);
        t_src1->UnlockRect          (i);
    }
}

IC u32 it_gloss_rev     (u32 d, u32 s)  {   return  color_rgba  (
    color_get_A(s),     // gloss
    color_get_B(d),
    color_get_G(d),
    color_get_R(d)      );
}
IC u32 it_gloss_rev_base(u32 d, u32 s)  {
    u32     occ     = color_get_A(d)/3;
    u32     def     = 8;
    u32     gloss   = (occ*1+def*3)/4;
    return  color_rgba  (
        gloss,          // gloss
        color_get_B(d),
        color_get_G(d),
        color_get_R(d)
        );
}
IC u32 it_difference    (u32 d, u32 orig, u32 ucomp)    {   return  color_rgba(
    128+(int(color_get_R(orig))-int(color_get_R(ucomp)))*2,     // R-error
    128+(int(color_get_G(orig))-int(color_get_G(ucomp)))*2,     // G-error
    128+(int(color_get_B(orig))-int(color_get_B(ucomp)))*2,     // B-error
    128+(int(color_get_A(orig))-int(color_get_A(ucomp)))*2  );  // A-error
}
IC u32 it_height_rev    (u32 d, u32 s)  {   return  color_rgba  (
    color_get_A(d),                 // diff x
    color_get_B(d),                 // diff y
    color_get_G(d),                 // diff z
    color_get_R(s)  );              // height
}
IC u32 it_height_rev_base(u32 d, u32 s) {   return  color_rgba  (
    color_get_A(d),                 // diff x
    color_get_B(d),                 // diff y
    color_get_G(d),                 // diff z
    (color_get_R(s)+color_get_G(s)+color_get_B(s))/3    );  // height
}
*/

// Create an image memory barrier used to change the layout of an image and put it into an active command buffer
void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange)
{
    // Create an image barrier object
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // Only sets masks for layouts used in this example
    // For a more complete version that can be used with other layouts see vks::tools::setImageLayout

    // Source layouts (old)
    switch (oldImageLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        // Only valid as initial layout, memory contents are not preserved
        // Can be accessed directly, no source dependency required
        imageMemoryBarrier.srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Old layout is transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }

    // Target layouts (new)
    switch (newImageLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Transfer source (copy, blit)
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Transfer destination (copy, blit)
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Shader read (sampler, input attachment)
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    }

    // Put barrier on top of pipeline
    VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmdBuffer,
        srcStageFlags,
        destStageFlags,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}

ID3DBaseTexture* CRender::texture_load(LPCSTR fRName, u32& ret_msize, bool bStaging)
{
    //  Staging control
    static bool bAllowStaging = !strstr(Core.Params, "-no_staging");
    bStaging &= bAllowStaging;

    ID3DBaseTexture* pTexture2D = nullptr;

    // ID3DBaseTexture* pTexture2D = NULL;
    // IDirect3DCubeTexture9*   pTextureCUBE    = NULL;
    string_path fn;
    // u32                      dwWidth,dwHeight;
    u32 img_size = 0;
    int img_loaded_lod = 0;
    // D3DFORMAT                fmt;
    u32 mip_cnt = u32(-1);
    // validation
    R_ASSERT(fRName);
    R_ASSERT(fRName[0]);

    // make file name
    string_path fname;
    xr_strcpy(fname, fRName); //. andy if (strext(fname)) *strext(fname)=0;
    fix_texture_name(fname);
    IReader* S = NULL;
    if (!FS.exist(fn, "$game_textures$", fname, ".dds") && strstr(fname, "_bump"))
    {
        // Msg          ("! auto-generated bump map: %s",fname);
        Msg("! Fallback to default bump map: %s", fname);
        //////////////////
        if (strstr(fname, "_bump#"))
        {
            R_ASSERT2(FS.exist(fn, "$game_textures$", "ed\\ed_dummy_bump#", ".dds"), "ed_dummy_bump#");
            S = FS.r_open(fn);
            R_ASSERT2(S, fn);
            img_size = S->length();
            goto _DDS;
        }
        else if (strstr(fname, "_bump"))
        {
            R_ASSERT2(FS.exist(fn, "$game_textures$", "ed\\ed_dummy_bump", ".dds"), "ed_dummy_bump");
            S = FS.r_open(fn);

            R_ASSERT2(S, fn);

            img_size = S->length();
            goto _DDS;
        }
        //////////////////
    }
    if (FS.exist(fn, "$level$", fname, ".dds"))
        goto _DDS;
    if (FS.exist(fn, "$game_saves$", fname, ".dds"))
        goto _DDS;
    if (FS.exist(fn, "$game_textures$", fname, ".dds"))
        goto _DDS;

#ifdef _EDITOR
    ELog.Msg(mtError, "Can't find texture '%s'", fname);
    return 0;
#else

    Msg("! Can't find texture '%s'", fname);
    R_ASSERT(FS.exist(fn, "$game_textures$", "ed\\ed_not_existing_texture", ".dds"));
    goto _DDS;

//  xrDebug::Fatal(DEBUG_INFO,"Can't find texture '%s'",fname);

#endif

_DDS:
{
    // Load and get header
#ifdef DEBUG
    Msg("* Loaded: %s[%d]", fn, S->length());
#endif // DEBUG
    img_size = S->length();
    R_ASSERT(S);
// R_CHK2                   (D3DXGetImageInfoFromFileInMemory   (S->pointer(),S->length(),&IMG), fn);
    gli::texture tex(gli::load((char*)S->pointer(), img_size));
    // if (IMG.ResourceType == D3DRTYPE_CUBETEXTURE)            goto _DDS_CUBE;

    // Check for LMAP and compress if needed
    strlwr(fn);

    // Load   SYS-MEM-surface, bound to device restrictions
    // ID3DTexture2D*       T_sysmem;
    // R_CHK2(D3DXCreateTextureFromFileInMemoryEx
    //  (
    //  HW.pDevice,S->pointer(),S->length(),
    //  D3DX_DEFAULT,D3DX_DEFAULT,
    //  IMG.MipLevels,0,
    //  IMG.Format,
    //  D3DPOOL_SYSTEMMEM,
    //  D3DX_DEFAULT,
    //  D3DX_DEFAULT,
    //  0,&IMG,0,
    //  &T_sysmem
    //  ), fn);

    // TODO: VK: Implement support for reduced LOD
    //img_loaded_lod = get_texture_load_lod(fn);

    //if (img_loaded_lod)
    //{
    //    Reduce(LoadInfo.Width, LoadInfo.Height, IMG.MipLevels, img_loaded_lod);
    //}

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = (VkFormat)tex.format();
    imageCreateInfo.mipLevels = tex.levels();
    imageCreateInfo.arrayLayers = tex.faces();
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = bStaging ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageCreateInfo.extent = { tex.extent().x, tex.extent().y, 1 };
    imageCreateInfo.flags = gli::is_target_cube(tex.target()) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    CHK_VK(DXTexture::Create(&imageCreateInfo, tex.data(), tex.size(), bStaging, &pTexture2D));

    if (bStaging)
    {
        // Setup buffer copy regions for each face and mip level
        std::vector<VkBufferImageCopy> bufferCopyRegions;
        uint32_t offset = 0;

        for (uint32_t face = 0; face < tex.faces(); face++)
        {
            for (uint32_t level = 0; level < tex.levels(); level++)
            {
                VkBufferImageCopy bufferCopyRegion = {};
                bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferCopyRegion.imageSubresource.mipLevel = level;
                bufferCopyRegion.imageSubresource.baseArrayLayer = face;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex.extent(level).x);
                bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex.extent(level).y);
                bufferCopyRegion.imageExtent.depth = 1;
                bufferCopyRegion.bufferOffset = offset;

                bufferCopyRegions.push_back(bufferCopyRegion);

                offset += static_cast<uint32_t>(tex.size(level));
            }
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(pTexture2D->m_copyCmd, &beginInfo);

        // Image barrier for optimal image

        // The sub resource range describes the regions of the image we will be transition
        VkImageSubresourceRange subresourceRange = {};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        // We will transition on all mip levels and layers
        subresourceRange.levelCount = tex.levels();
        subresourceRange.layerCount = tex.layers();

        // Optimal image will be used as destination for the copy, so we must transfer from our
        // initial undefined image layout to the transfer destination layout
        setImageLayout(
            pTexture2D->m_copyCmd,
            pTexture2D->m_image,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresourceRange);

        // Copy mip levels from staging buffer
        vkCmdCopyBufferToImage(
            pTexture2D->m_copyCmd,
            pTexture2D->m_stagingBuffer,
            pTexture2D->m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            static_cast<uint32_t>(bufferCopyRegions.size()),
            bufferCopyRegions.data());

        // Change texture image layout to shader read after all mip levels have been copied
        setImageLayout(
            pTexture2D->m_copyCmd,
            pTexture2D->m_image,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            subresourceRange);

        vkEndCommandBuffer(pTexture2D->m_copyCmd);
    }
    else
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(pTexture2D->m_copyCmd, &beginInfo);

        // Setup image memory barrier transfer image to shader read layout

        // The sub resource range describes the regions of the image we will be transition
        VkImageSubresourceRange subresourceRange = {};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        // Most implementations won't support more than one mip level for linear tiled images
        subresourceRange.levelCount = tex.levels();
        subresourceRange.layerCount = tex.layers();

        setImageLayout(
            pTexture2D->m_copyCmd,
            pTexture2D->m_image,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_PREINITIALIZED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            subresourceRange);

        vkEndCommandBuffer(pTexture2D->m_copyCmd);
    }

    FS.r_close(S);
    //mip_cnt = tex.levels();
    // OK
    //ret_msize = calc_texture_size(img_loaded_lod, mip_cnt, img_size);
    ret_msize = tex.size();
    return pTexture2D;
}
}
