#include "Engine_Shader_Defines.hlsli"

matrix WorldMatrix, ViewMatrix, ProjMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_NoiseTexture;

float2 DiffuseTexutureOffset;
float2 DiffuseScrollSpeed;

float2 NoiseScrollSpeed = { 0.f, 0.2f };
float  DisrtortionStrength;

float TimeAccumulation;

SamplerState LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), mul(mul(WorldMatrix, ViewMatrix), ProjMatrix));
    Out.vPosition.z = Out.vPosition.w;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent.xyz, 0.f), WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal.xyz, 0.f), WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
};

float3 DesaturateAndLuminance(float3 color, float targetSat)
{
    float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    return lerp(float3(luma, luma, luma), color, targetSat);
}
PS_OUT PS_SKYSPHERE(PS_IN In)
{
    PS_OUT Out;
    
    vector BackGroundColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    //BackGroundColor.rgb *= 0.85f;
    //BackGroundColor.rgb = DesaturateAndLuminance(BackGroundColor.rgb, 0.75f);
    //BackGroundColor.rgb = (BackGroundColor.rgb - 0.5f) * 1.1f + 0.5f;
    //BackGroundColor.rgb = saturate(BackGroundColor.rgb);
    

    float3 linearSky = pow(BackGroundColor.rgb, 2.2f) * 0.8f;
     
    
    Out.vDiffuse = float4(linearSky, BackGroundColor.a); //vector(BackGroundColor.rgb * 1.f, 1.f);
    
    return Out;
}

PS_OUT PS_FLAMESPHERE(PS_IN In)
{
    PS_OUT Out;
    
    float2 SRCNoiseUV = In.vTexcoord + float2(0.05f, 0.08f) * TimeAccumulation;
    float2 DSTNoiseUV = In.vTexcoord + float2(-0.07f, 0.04f) * TimeAccumulation;
    
    float SRCNoiseRColor = g_NoiseTexture.Sample(LinearSampler, SRCNoiseUV).r;
    float DSTNoiseGColor = g_NoiseTexture.Sample(LinearSampler, DSTNoiseUV).g;
    
    float CombinedNoise = (SRCNoiseRColor * DSTNoiseGColor) / 2.f;
    
    float2 Disrtortion;
    Disrtortion.x = (CombinedNoise * 2.f - 1.f) + (sin(TimeAccumulation * 2.0f) * 0.1f);
    Disrtortion.y = (CombinedNoise * 2.f - 1.f) + (cos(TimeAccumulation * 1.5f) * 0.1f);
    
    float2 DiffuseUV = In.vTexcoord + Disrtortion * DisrtortionStrength;
    float4 DiffuseColor = g_DiffuseTexture.Sample(LinearSampler, DiffuseUV);
    
    Out.vDiffuse = DiffuseColor / 2.f;
    Out.vDiffuse.a = 1.f;
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_SKY, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKYSPHERE();
    }
    pass FlameSky
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_SKY, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FLAMESPHERE();
    }
    pass NONDEPTH
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKYSPHERE();
    }
}

