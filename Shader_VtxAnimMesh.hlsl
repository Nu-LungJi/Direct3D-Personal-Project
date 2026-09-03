#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

float  g_fBrightness    = 1.2f;
vector g_vLightDiffuse  = vector(1.f, 0.8f, 0.8f, 1.f);
vector g_vLightAmbient  = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(0.25f, 0.25f, 0.25f, 0.25f);

float3  g_fRimLightDiffuse = float3(15.f, 0.f, 0.f);
float   g_fRimLightIntensity = 5.f;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

Texture2D g_NoiseTexture;

Texture2D g_EmissiveTexture;
float4  g_EmissiveDiffuse = { 0.08f, 0.1f, 0.84f, 1.f };
float   g_EmissiveIntensity = 1.f;

Texture2D g_MetallicTexture;

Texture2D g_LightDiffuseTexture;

Texture2D g_MatCapTexture;
float       g_fMatCapIntensity = 0.3f;

vector g_vMtrlAmbient = vector(0.8f, 0.8f, 0.8f, 1.f);
vector g_vMtrlSpecular = vector(0.3f, 0.3f, 0.3f, 1.f);

vector g_vCamPosition;

float ShadowBorderLine = 0.2f;
float ShadowScattering = 0.015f;
float ShadowIntensity = 1.1f;
float BrightIntensity = 1.3f;

float DissolveAmount = 0.f;
float DissolveEdgeWidth = { 0.03f };
float4 DissolveColor = { 1.f, 1.f, 1.f, 1.f }; //= { 0.255f, 0.157f, 0.941f, 0.f };

/* 이 메시에게 영향을 주는 뼈들의 행렬 */
float4x4 g_BoneMatrices[512];

sampler LinearSampler = sampler_state
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
    uint4  vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
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
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    float4x4 BoneMatrix =
        g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal   = mul(float4(In.vNormal, 0.f), BoneMatrix);
   
    Out.vPosition   = mul(vPosition, mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    Out.vNormal     = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent    = normalize(mul(float4(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal   = normalize(mul(float4(In.vBinormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord   = In.vTexcoord;
    Out.vWorldPos   = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos    = Out.vPosition;
    
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
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vPick : SV_TARGET3;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vector vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    vector vInvLook = normalize(g_vCamPosition - In.vWorldPos);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    float4 fLightColor = (g_vLightDiffuse * g_fBrightness) * vMtrlDiffuse * saturate(vShade) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    vector vLightDiffuse = g_LightDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    // Emissive
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    float NDL = dot(normalize(In.vNormal), normalize(-g_vLightDir));
    
    // RimLight 
    float RimFactor = 1.f - saturate(dot(normalize(In.vNormal), vInvLook));
    RimFactor = pow(RimFactor, g_fRimLightIntensity);

    float3 FinalRim = RimFactor * g_fRimLightDiffuse;
    
    // Toon Shading
    float ShadowInstance = smoothstep(ShadowBorderLine, ShadowBorderLine + ShadowScattering, max(NDL, 0));
    ShadowInstance = lerp(ShadowIntensity, BrightIntensity, ShadowInstance);
       
    // MatCap
    float3 vNormalVS = mul(normalize(vNormal.xyz), (float3x3) g_ViewMatrix);
    float2 MatCapUV = vNormalVS.xy * 0.5f + 0.5f;
    MatCapUV.y = 1.0f - MatCapUV.y;
    
    float3 vMatCap = g_MatCapTexture.Sample(LinearSampler, MatCapUV).rgb;
    float3 SoftLightMtrl = (1.0f - 2.0f * vMatCap) * (vMtrlDiffuse.rgb * vMtrlDiffuse.rgb) + (2.0f * vMatCap * vMtrlDiffuse.rgb);
    
    float3 BaseToonColor = lerp(vMtrlDiffuse.rgb, SoftLightMtrl, g_fMatCapIntensity) * fLightColor.rgb * ShadowInstance;
    
    Out.vDiffuse.xyz = BaseToonColor + float4(FinalEmissive.xyz, 1.f) + float4(FinalRim.xyz, RimFactor);
    Out.vDiffuse.a = 1.f; //fLightColor.a;
    Out.vNormal = vector(vNormal.xyz * 1.f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPick = vector(In.vWorldPos.xyz, 1.f);
    
    return Out;
}
PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out;
    
    float4 DiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector NoiseColor = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    vector vInvLook = normalize(g_vCamPosition - In.vWorldPos);
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    float4 fLightColor = (g_vLightDiffuse * g_fBrightness) * DiffuseColor * saturate(vShade) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    vector vLightDiffuse = g_LightDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    // Emissive
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    float NDL = dot(normalize(In.vNormal), normalize(-g_vLightDir));
    
    float ShadowInstance = smoothstep(ShadowBorderLine, ShadowBorderLine + ShadowScattering, max(NDL, 0));
    ShadowInstance = lerp(ShadowIntensity, BrightIntensity, ShadowInstance);
       
    // MatCap
    float3 vNormalVS = mul(normalize(vNormal.xyz), (float3x3) g_ViewMatrix);
    float2 MatCapUV = vNormalVS.xy * 0.5f + 0.5f;
    MatCapUV.y = 1.0f - MatCapUV.y;
    
    float3 vMatCap = g_MatCapTexture.Sample(LinearSampler, MatCapUV).rgb;
    float3 SoftLightMtrl = (1.0f - 2.0f * vMatCap) * (DiffuseColor.rgb * DiffuseColor.rgb) + (2.0f * vMatCap * DiffuseColor.rgb);
    
    float3 BaseToonColor = lerp(DiffuseColor.rgb, SoftLightMtrl, g_fMatCapIntensity) * fLightColor.rgb * ShadowInstance;
    
    float DissolveFactor = NoiseColor.r - DissolveAmount;
    clip(DissolveFactor);
    if (DissolveFactor < DissolveEdgeWidth)
    {
        BaseToonColor = (DissolveColor * 3.f);
    }
    
    Out.vDiffuse.xyz = BaseToonColor + float4(FinalEmissive.xyz, 1.f);
    Out.vDiffuse.a = 1.f; //fLightColor.a;
    Out.vNormal = vector(vNormal.xyz * 1.f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPick = vector(In.vWorldPos.xyz, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
}

