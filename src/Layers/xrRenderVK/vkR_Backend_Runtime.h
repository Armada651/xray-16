#ifndef vkR_Backend_Runtime_included
#define vkR_Backend_Runtime_included
#pragma once

IC void CBackend::set_xform(u32 ID, const Fmatrix& M)
{
    stat.xforms++;
    //  TODO: VK: Implement CBackend::set_xform
    // VERIFY(!"Implement CBackend::set_xform");
}

IC void CBackend::set_RT(ID3DRenderTargetView* RT, u32 ID)
{
    if (RT != pRT[ID])
    {
        PGO(Msg("PGO:setRT"));
        stat.target_rt++;
        pRT[ID] = RT;
        //  TODO: VK: Implement CBackend::set_RT
        VERIFY(!"Implement CBackend::set_RT");

        m_bChangedRTorZB = true;
    }
}

IC void CBackend::set_ZB(ID3DDepthStencilView* ZB)
{
    if (ZB != pZB)
    {
        PGO(Msg("PGO:setZB"));
        stat.target_zb++;
        pZB = ZB;
        //  TODO: VK: Implement CBackend::set_ZB
        VERIFY(!"Implement CBackend::set_ZB");

        m_bChangedRTorZB = true;
    }
}

ICF void CBackend::set_Format(SDeclaration* _decl)
{
    if (decl != _decl)
    {
        PGO(Msg("PGO:v_format:%x", _decl));
#ifdef DEBUG
        stat.decl++;
#endif
        decl = _decl;
    }
}

ICF void CBackend::set_PS(ID3DPixelShader* _ps, LPCSTR _n)
{
    if (ps != _ps)
    {
        PGO(Msg("PGO:Pshader:%x", _ps));
        stat.ps++;
        ps = _ps;
        //  TODO: VK: Implement CBackend::set_PS
        VERIFY(!"Implement CBackend::set_PS");

#ifdef DEBUG
        ps_name = _n;
#endif
    }
}

ICF void CBackend::set_GS(ID3DGeometryShader* _gs, LPCSTR _n)
{
    if (gs != _gs)
    {
        PGO(Msg("PGO:Gshader:%x", _ps));
        //  TODO: DX10: Get statistics for G Shader change
        // stat.gs          ++;
        gs = _gs;
        //  TODO: VK: Implement CBackend::set_GS
        VERIFY(!"Implement CBackend::set_GS");

#ifdef DEBUG
        gs_name = _n;
#endif
    }
}

#ifdef USE_DX11
ICF void CBackend::set_HS(ID3D11HullShader* _hs, LPCSTR _n)
{
    if (hs != _hs)
    {
        PGO(Msg("PGO:Hshader:%x", _ps));
        //  TODO: DX10: Get statistics for H Shader change
        // stat.hs          ++;
        hs = _hs;
        //  TODO: VK: Implement CBackend::set_HS
        VERIFY(!"Implement CBackend::set_HS");

#ifdef DEBUG
        hs_name = _n;
#endif
    }
}

ICF void CBackend::set_DS(ID3D11DomainShader* _ds, LPCSTR _n)
{
    if (ds != _ds)
    {
        PGO(Msg("PGO:Dshader:%x", _ps));
        //  TODO: DX10: Get statistics for D Shader change
        // stat.ds          ++;
        ds = _ds;
        //  TODO: VK: Implement CBackend::set_DS
        VERIFY(!"Implement CBackend::set_DS");

#ifdef DEBUG
        ds_name = _n;
#endif
    }
}

ICF void CBackend::set_CS(ID3D11ComputeShader* _cs, LPCSTR _n)
{
    if (cs != _cs)
    {
        PGO(Msg("PGO:Cshader:%x", _ps));
        //  TODO: DX10: Get statistics for D Shader change
        // stat.cs          ++;
        cs = _cs;
        //  TODO: VK: Implement CBackend::set_CS
        VERIFY(!"Implement CBackend::set_CS");

#ifdef DEBUG
        cs_name = _n;
#endif
    }
}

ICF bool CBackend::is_TessEnabled() { return HW.features.tessellationShader && (ds != 0 || hs != 0); }
#endif

ICF void CBackend::set_VS(ID3DVertexShader* _vs, LPCSTR _n)
{
    if (vs != _vs)
    {
        PGO(Msg("PGO:Vshader:%x", _vs));
        stat.vs++;
        vs = _vs;
        //  TODO: VK: Implement CBackend::set_VS
        VERIFY(!"Implement CBackend::set_VS");

#ifdef DEBUG
        vs_name = _n;
#endif
    }
}

ICF void CBackend::set_Vertices(ID3DVertexBuffer* _vb, u32 _vb_stride)
{
    if ((vb != _vb) || (vb_stride != _vb_stride))
    {
        PGO(Msg("PGO:VB:%x,%d", _vb, _vb_stride));
#ifdef DEBUG
        stat.vb++;
#endif
        vb = _vb;
        vb_stride = _vb_stride;
        // CHK_DX           (HW.pDevice->SetStreamSource(0,vb,0,vb_stride));
        // UINT StreamNumber,
        // IDirect3DVertexBuffer9 * pStreamData,
        // UINT OffsetInBytes,
        // UINT Stride

        // UINT StartSlot,
        // UINT NumBuffers,
        // ID3DxxBuffer *const *ppVertexBuffers,
        // const UINT *pStrides,
        // const UINT *pOffsets
        VkDeviceSize iOffset = 0;
        vkCmdBindVertexBuffers(HW.context, 0, 1, &_vb->m_buffer, &iOffset);
    }
}

ICF void CBackend::set_Indices(ID3DIndexBuffer* _ib)
{
    if (ib != _ib)
    {
        PGO(Msg("PGO:IB:%x", _ib));
#ifdef DEBUG
        stat.ib++;
#endif
        ib = _ib;
        vkCmdBindIndexBuffer(HW.context, _ib->m_buffer, 0, VK_INDEX_TYPE_UINT16);
    }
}

IC D3D_PRIMITIVE_TOPOLOGY TranslateTopology(D3DPRIMITIVETYPE T)
{
    static D3D_PRIMITIVE_TOPOLOGY translateTable[] = {
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, // None
        D3D_PRIMITIVE_TOPOLOGY_POINTLIST, // D3DPT_POINTLIST = 1,
        D3D_PRIMITIVE_TOPOLOGY_LINELIST, // D3DPT_LINELIST = 2,
        D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, // D3DPT_LINESTRIP = 3,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, // D3DPT_TRIANGLELIST = 4,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // D3DPT_TRIANGLESTRIP = 5,
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, // D3DPT_TRIANGLEFAN = 6,
    };

    VERIFY(T < sizeof(translateTable) / sizeof(translateTable[0]));
    VERIFY(T >= 0);

    D3D_PRIMITIVE_TOPOLOGY result = translateTable[T];

    VERIFY(result != D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);

    return result;
}

IC u32 GetIndexCount(D3DPRIMITIVETYPE T, u32 iPrimitiveCount)
{
    switch (T)
    {
    case D3DPT_POINTLIST: return iPrimitiveCount;
    case D3DPT_LINELIST: return iPrimitiveCount * 2;
    case D3DPT_LINESTRIP: return iPrimitiveCount + 1;
    case D3DPT_TRIANGLELIST: return iPrimitiveCount * 3;
    case D3DPT_TRIANGLESTRIP: return iPrimitiveCount + 2;
    default: NODEFAULT;
#ifdef DEBUG
        return 0;
#endif // #ifdef DEBUG
    }
}

IC void CBackend::ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology) { VERIFY(!"Not implemented."); }
#ifdef USE_DX11
IC void CBackend::Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
    stat.calls++;

    SRVSManager.Apply();
    StateManager.Apply();
    //  State manager may alter constants
    constants.flush();
    vkCmdDispatch(HW.context, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}
#endif

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC)
{
    // VERIFY(vs);
    // HW.pDevice->VSSetShader(vs);
    // HW.pDevice->GSSetShader(0);

    D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    u32 iIndexCount = GetIndexCount(T, PC);

//!!! HACK !!!
#ifdef USE_DX11
    if (hs != 0 || ds != 0)
    {
        R_ASSERT(Topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    }
#endif

    stat.calls++;
    stat.verts += countV;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);

    // CHK_DX(HW.pDevice->DrawIndexedPrimitive(T,baseV, startV, countV,startI,PC));
    // D3DPRIMITIVETYPE Type,
    // INT BaseVertexIndex,
    // UINT MinIndex,
    // UINT NumVertices,
    // UINT StartIndex,
    // UINT PriResmitiveCount

    // UINT IndexCount,
    // UINT StartIndexLocation,
    // INT BaseVertexLocation
    SRVSManager.Apply();
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    //  State manager may alter constants
    constants.flush();
    //  Msg("DrawIndexed: Start");
    //  Msg("iIndexCount=%d, startI=%d, baseV=%d", iIndexCount, startI, baseV);
    vkCmdDrawIndexed(HW.context, iIndexCount, 1, startI, baseV, 0);
    //  Msg("DrawIndexed: End\n");

    PGO(Msg("PGO:DIP:%dv/%df", countV, PC));
}

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC)
{
    //  TODO: DX10: Remove triangle fan usage from the engine
    if (T == D3DPT_TRIANGLEFAN)
        return;

    // VERIFY(vs);
    // HW.pDevice->VSSetShader(vs);

    D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    u32 iVertexCount = GetIndexCount(T, PC);

    stat.calls++;
    stat.verts += 3 * PC;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);
    SRVSManager.Apply();
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    //  State manager may alter constants
    constants.flush();
    //  Msg("Draw: Start");
    //  Msg("iVertexCount=%d, startV=%d", iVertexCount, startV);
    // CHK_DX               (HW.pDevice->DrawPrimitive(T, startV, PC));
    vkCmdDraw(HW.context, iVertexCount, 1, startV, 0);
    //  Msg("Draw: End\n");
    PGO(Msg("PGO:DIP:%dv/%df", 3 * PC, PC));
}

IC void CBackend::set_Geometry(SGeometry* _geom)
{
    set_Format(&*_geom->dcl);

    set_Vertices(_geom->vb, _geom->vb_stride);
    set_Indices(_geom->ib);
}

IC void CBackend::set_Scissor(Irect* R)
{
    if (R)
    {
        // CHK_DX       (HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE));
        StateManager.EnableScissoring();
        VkRect2D clip = {{R->left, R->top}, {R->width(), R->height()}};
        vkCmdSetScissor(HW.context, 0, 1, &clip);
    }
    else
    {
        // CHK_DX       (HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));
        StateManager.EnableScissoring(FALSE);
        vkCmdSetScissor(HW.context, 0, 0, nullptr);
    }
}

IC void CBackend::set_Stencil(
    u32 _enable, u32 _func, u32 _ref, u32 _mask, u32 _writemask, u32 _fail, u32 _pass, u32 _zfail)
{
    StateManager.SetStencil(_enable, _func, _ref, _mask, _writemask, _fail, _pass, _zfail);
    // Simple filter
    // if (stencil_enable       != _enable)     { stencil_enable=_enable;       CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILENABLE,     _enable             )); }
    // if (!stencil_enable)                 return;
    // if (stencil_func     != _func)       { stencil_func=_func;           CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILFUNC,
    // _func                )); }
    // if (stencil_ref          != _ref)        { stencil_ref=_ref;             CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILREF,
    // _ref
    // )); }
    // if (stencil_mask     != _mask)       { stencil_mask=_mask;           CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILMASK,
    // _mask                )); }
    // if (stencil_writemask    != _writemask)  { stencil_writemask=_writemask; CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILWRITEMASK,  _writemask          )); }
    // if (stencil_fail     != _fail)       { stencil_fail=_fail;           CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILFAIL,
    // _fail                )); }
    // if (stencil_pass     != _pass)       { stencil_pass=_pass;           CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILPASS,
    // _pass                )); }
    // if (stencil_zfail        != _zfail)      { stencil_zfail=_zfail;         CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_STENCILZFAIL,
    // _zfail               )); }
}

IC void CBackend::set_Z(u32 _enable)
{
    StateManager.SetDepthEnable(_enable);
    // if (z_enable != _enable)
    //{
    //  z_enable=_enable;
    //  CHK_DX(HW.pDevice->SetRenderState   ( D3DRS_ZENABLE, _enable ));
    //}
}

IC void CBackend::set_ZFunc(u32 _func)
{
    StateManager.SetDepthFunc(_func);
    // if (z_func!=_func)
    //{
    //  z_func = _func;
    //  CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZFUNC, _func));
    //}
}

IC void CBackend::set_AlphaRef(u32 _value)
{
    //  TODO: DX10: Implement rasterizer state update to support alpha ref
    VERIFY(!"Not implemented.");
    // if (alpha_ref != _value)
    //{
    //  alpha_ref = _value;
    //  CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,_value));
    //}
}

IC void CBackend::set_ColorWriteEnable(u32 _mask)
{
    StateManager.SetColorWriteEnable(_mask);
    // if (colorwrite_mask      != _mask)       {
    //  colorwrite_mask=_mask;
    //  CHK_DX(HW.pDevice->SetRenderState   ( D3DRS_COLORWRITEENABLE,   _mask   ));
    //  CHK_DX(HW.pDevice->SetRenderState   ( D3DRS_COLORWRITEENABLE1,  _mask   ));
    //  CHK_DX(HW.pDevice->SetRenderState   ( D3DRS_COLORWRITEENABLE2,  _mask   ));
    //  CHK_DX(HW.pDevice->SetRenderState   ( D3DRS_COLORWRITEENABLE3,  _mask   ));
    //}
}
ICF void CBackend::set_CullMode(u32 _mode)
{
    StateManager.SetCullMode(_mode);
    // if (cull_mode        != _mode)       { cull_mode = _mode;            CHK_DX(HW.pDevice->SetRenderState   (
    // D3DRS_CULLMODE,
    // _mode
    // )); }
}

IC void CBackend::ApplyVertexLayout() { VERIFY(!"Not implemented."); }
ICF void CBackend::set_VS(ref_vs& _vs)
{
    m_pInputSignature = _vs->signature->signature;
    set_VS(_vs->vs, _vs->cName.c_str());
}

ICF void CBackend::set_VS(SVS* _vs)
{
    m_pInputSignature = _vs->signature->signature;
    set_VS(_vs->vs, _vs->cName.c_str());
}

IC bool CBackend::CBuffersNeedUpdate(
    ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax)
{
    bool bRes = false;
    int i = 0;
    while ((i < MaxCBuffers) && (buf1[i] == buf2[i]))
        ++i;

    uiMin = i;

    for (; i < MaxCBuffers; ++i)
    {
        if (buf1[i] != buf2[i])
        {
            bRes = true;
            uiMax = i;
        }
    }

    return bRes;
}

IC void CBackend::set_Constants(R_constant_table* C)
{
    // caching
    if (ctable == C)
        return;
    ctable = C;
    xforms.unmap();
    hemi.unmap();
    tree.unmap();
#ifdef USE_DX11
    LOD.unmap();
#endif
    StateManager.UnmapConstants();
    if (0 == C)
        return;

    PGO(Msg("PGO:c-table"));

    //  Setup constant tables
    VERIFY(!"Not implemented.");

    // process constant-loaders
    R_constant_table::c_table::iterator it = C->table.begin();
    R_constant_table::c_table::iterator end = C->table.end();
    for (; it != end; it++)
    {
        R_constant* Cs = &**it;
        VERIFY(Cs);
        if (Cs && Cs->handler)
            Cs->handler->setup(Cs);
    }
}

ICF void CBackend::ApplyRTandZB() { VERIFY(!"Not implemented."); }
IC void CBackend::get_ConstantDirect(shared_str& n, u32 DataSize, void** pVData, void** pGData, void** pPData)
{
    ref_constant C = get_c(n);

    if (C)
        constants.access_direct(&*C, DataSize, pVData, pGData, pPData);
    else
    {
        if (pVData)
            *pVData = 0;
        if (pGData)
            *pGData = 0;
        if (pPData)
            *pPData = 0;
    }
}

#endif //   vkR_Backend_Runtime_included
