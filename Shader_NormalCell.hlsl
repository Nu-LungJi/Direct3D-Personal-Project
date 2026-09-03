#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor = vector(0.64f, 0.286f, 0.643f, 0.7f);

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;   
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    
    vPosition.z -= 0.03f;
    vPosition = mul(vPosition, g_ProjMatrix);  
    
    Out.vPosition = vPosition;
    return Out;        
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;    
       
    
    Out.vColor = vector(g_vColor.xyz, 0.25f);
    
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_AlphaDepth, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}