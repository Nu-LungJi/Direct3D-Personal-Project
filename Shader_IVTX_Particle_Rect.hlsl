#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;    
    float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;   
    
    float4x4 TransformationMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), TransformationMatrix);
    
    Out.vPosition = mul(vPosition, mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;        
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;    

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    
    vector      vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);  
    
    if(vColor.a <= 0.1f)
        discard;
    
    vColor.a = saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    vColor.rgb = vColor.rgb + (1.f - vColor.a);
    
    Out.vColor = vColor;    
    
    return Out;
}



technique11 DefaultTechnique
{
    pass Default
    { 
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);   
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();       
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}