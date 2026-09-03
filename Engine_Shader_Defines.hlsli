SamplerState DefaultSampler : register(s0);


/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
};


RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_NONE
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

/*
BOOL DepthEnable;
D3D11_DEPTH_WRITE_MASK DepthWriteMask;
D3D11_COMPARISON_FUNC DepthFunc;
BOOL StencilEnable;
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_ZDisable
{
    DepthEnable = false;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
};
DepthStencilState DSS_AlphaDepth
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
};
DepthStencilState DSS_SKY
{
    DepthEnable = false;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
};
DepthStencilState DSS_OCEAN
{
    DepthEnable = true;
    DepthWriteMask = true;
    DepthFunc = less_equal;
};
/*
BOOL BlendEnable;
D3D11_BLEND SrcBlend;
D3D11_BLEND DestBlend;
D3D11_BLEND_OP BlendOp;
D3D11_BLEND SrcBlendAlpha;
D3D11_BLEND DestBlendAlpha;
D3D11_BLEND_OP BlendOpAlpha;
UINT8 RenderTargetWriteMask;
*/

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_Additive
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = One;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Zero;
    BlendOpAlpha = Add;

};
BlendState BS_PremultipliedAlpha
{
    BlendEnable[0] = true;
    SrcBlend = One;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;

};
BlendState BS_BloomComposite
{
    BlendEnable[0] = true;
    SrcBlend[0] = src_alpha;
    DestBlend[0] = inv_src_alpha;
    BlendOp[0] = add;
};