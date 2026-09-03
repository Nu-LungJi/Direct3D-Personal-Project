#include "Engine_Shader_Defines.hlsli"

matrix WorldMatrix, LocalMatrix, ViewMatrix, ProjMatrix;

float g_fBrightness = 0.85f;
vector g_vLightDir = vector(-1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 0.9f, 0.9f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(0.25f, 0.25f, 0.25f, 0.25f);

Texture2D g_DiffuseTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_EmissiveTexture : register(t2);
Texture2D g_LightDiffuseTexture;

vector g_vMtrlAmbient = vector(0.8f, 0.8f, 0.8f, 1.f);
vector g_vMtrlSpecular = vector(0.3f, 0.3f, 0.3f, 1.f);

vector g_vCamPosition;

float4 g_EmissiveDiffuse = { 0.56f, 0.f, 1.f, 1.f };
float g_EmissiveIntensity = 0.1f;

float ShadowBorderLine = 0.2f;
float ShadowScattering = 0.015f;
float ShadowIntensity = 1.1f;
float BrightIntensity = 1.2f;

float DissolveAmount = 0.f;
float DissolveEdgeWidth = { 0.03f };
float4 DissolveColor = { 1.f, 1.f, 1.f, 1.f }; //= { 0.255f, 0.157f, 0.941f, 0.f };

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition    : POSITION;
    float2 vTexCoord    : TEXCOORD0;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    row_major matrix TransformMatrix : WORLD;
};

struct VS_OUT
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float4 vTangent     : TANGENT;
    float4 vBinormal    : BINORMAL;
    float2 vTexCoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 LocalPos      = mul(float4(In.vPosition, 1.f), LocalMatrix);
    float4 WorldPos      = mul(LocalPos, In.TransformMatrix);
    
    Out.vPosition       = mul(mul(WorldPos, ViewMatrix), ProjMatrix);
                         
    Out.vWorldPos       = float4(In.vPosition, 1.f);
    Out.vProjPos         = Out.vPosition;   
                         
    Out.vTexCoord        = In.vTexCoord;
    
    matrix CombinedWorld = mul(LocalMatrix, In.TransformMatrix);
    Out.vNormal         = normalize(mul(float4(In.vNormal.xyz, 0.f)  , CombinedWorld));
    Out.vTangent        = normalize(mul(float4(In.vTangent.xyz, 0.f) , CombinedWorld));
    Out.vBinormal       = normalize(mul(float4(In.vBinormal.xyz, 0.f), CombinedWorld));
    
    return Out;
}
struct PS_IN
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float4 vTangent     : TANGENT;
    float4 vBinormal    : BINORMAL;
    float2 vTexCoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};
struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal  : SV_TARGET1;
    vector vDepth   : SV_TARGET2;
    vector vPick    : SV_TARGET3;
};
float3 AdjustSaturation(float3 color, float saturation)
{
    float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    return lerp(float3(luma, luma, luma), color, saturation);
}
float3 AdjustContrast(float3 color, float contrast)
{
    return (color - 0.5f) * contrast + 0.5f;
}
float3 SoftClamp(float3 color, float whitePoint)
{
    float3 num = color * (1.0f + (color / (whitePoint * whitePoint)));
    float3 den = 1.0f + color;
    return num / den;
}
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
        
    vector vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    float4 fLightColor = (g_vLightDiffuse * g_fBrightness) * vMtrlDiffuse * saturate(vShade) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 TBNMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
    vNormal = vector(mul(vNormal.xyz, TBNMatrix), 0.f);
    
    vector vLightDiffuse = g_LightDiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    
    // Toon Shading
    float NDL = dot(normalize(vNormal.xyz), normalize(-g_vLightDir.xyz));
    float ShadowInstance = smoothstep(ShadowBorderLine, ShadowBorderLine + ShadowScattering, max(NDL, 0));
    ShadowInstance = lerp(ShadowIntensity, BrightIntensity, ShadowInstance);
    
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    float3 FinalColor = fLightColor * ShadowInstance + FinalEmissive;
    
    FinalColor = AdjustContrast(FinalColor, 1.2f);
    FinalColor = AdjustSaturation(FinalColor, 1.5f);
    
    Out.vDiffuse.xyz = saturate(FinalColor);
    Out.vDiffuse.a = 1.f;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPick = vector(In.vWorldPos.xyz, 1.f);
    
    return Out;
}
PS_OUT PS_NONDESATURATE(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
        
    vector vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    float4 fLightColor = (g_vLightDiffuse * g_fBrightness) * vMtrlDiffuse * saturate(vShade) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 TBNMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
    vNormal = vector(mul(vNormal.xyz, TBNMatrix), 0.f);
    
    vector vLightDiffuse = g_LightDiffuseTexture.Sample(LinearSampler, In.vTexCoord);
    
    // Toon Shading
    float NDL = dot(normalize(vNormal.xyz), normalize(-g_vLightDir.xyz));
    float ShadowInstance = smoothstep(ShadowBorderLine, ShadowBorderLine + ShadowScattering, max(NDL, 0));
    ShadowInstance = lerp(ShadowIntensity, BrightIntensity, ShadowInstance);
    
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    Out.vDiffuse.xyz = fLightColor * ShadowInstance + FinalEmissive;
    Out.vDiffuse.a = 1.f;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPick = vector(In.vWorldPos.xyz, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
    pass PS_NONDESATURATE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONDESATURATE();
    }
}