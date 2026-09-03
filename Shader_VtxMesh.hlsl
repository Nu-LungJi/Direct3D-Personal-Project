#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fBrightness = 1.15f;
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 0.9f, 0.9f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(0.25f, 0.25f, 0.25f, 0.25f);

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
texture2D g_LightDiffuseTexture;

vector g_vMtrlAmbient = vector(0.8f, 0.8f, 0.8f, 1.f);
vector g_vMtrlSpecular = vector(0.3f, 0.3f, 0.3f, 1.f);

vector g_vCamPosition;

float4 g_EmissiveDiffuse;//= { 1.f, 1.f, 1.f, 1.f };
float g_EmissiveIntensity;//= 0.1f;

float ShadowBorderLine = 0.2f;
float ShadowScattering = 0.015f;
float ShadowIntensity = 1.1f;
float BrightIntensity = 1.3f;

float DissolveAmount = 0.02f;
float DissolveEdgeWidth = { 0.03f };
float4 DissolveColor = { 1.f, 1.f, 1.f, 1.f }; //= { 0.255f, 0.157f, 0.941f, 0.f };

Texture2D g_NoiseTexture;

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
    float3 vNormal : NORMAL;            
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

/* 포지션시멘틱에 해당하는 변수의 w로 모든 성분을 나눈다 */
/* 뷰포트로 변환한다.(윈도우좌표로 변환) */
/* 래스터라이즈 : 리턴된 정점정보를 기반으로하여 사이를 선형보간한 픽셀을 만들어준다.  */

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
struct PS_DISSOLVE_OUT
{
    float4 vDiffuse : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;       
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
        
    vector vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    float4 fLightColor = (g_vLightDiffuse * g_fBrightness) * vMtrlDiffuse * saturate(vShade) +
        (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    vector vLightDiffuse = g_LightDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    // Toon Shading
    float NDL = dot(normalize(In.vNormal), normalize(-g_vLightDir));
    float ShadowInstance = smoothstep(ShadowBorderLine, ShadowBorderLine + ShadowScattering, max(NDL, 0));
    ShadowInstance = lerp(ShadowIntensity, BrightIntensity, ShadowInstance);
    
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    Out.vDiffuse = fLightColor * ShadowInstance + float4(FinalEmissive.xyz, 1.f);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPick = vector(In.vWorldPos.xyz, 1.f);
    
    return Out;
}
struct PS_OUT_SHADOW
{
    vector vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_NONPICK(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
       discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
   
    Out.vDiffuse = float4(vMtrlDiffuse.xyz, 1.f);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);    
    
    return Out;
}
PS_DISSOLVE_OUT PS_DISSOLVE(PS_IN In)
{
    PS_DISSOLVE_OUT Out;
    
    float4 DiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector NoiseColor = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    // Emissive
    vector vEmissiveDiffuse = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float3 FinalEmissive = vEmissiveDiffuse * g_EmissiveDiffuse.xyz * g_EmissiveIntensity;
    
    float NDL = dot(normalize(In.vNormal), normalize(-g_vLightDir));
    
    float3 FinalColor = DiffuseColor;
    
    float DissolveFactor = NoiseColor.r - DissolveAmount;
    clip(DissolveFactor);
    if (DissolveFactor < DissolveEdgeWidth)
    {
        FinalColor = DissolveColor * 3.f;
    }
    
    Out.vDiffuse.xyz = FinalColor + FinalEmissive.xyz;
    Out.vDiffuse.a = 1.f;
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

    pass NonComputeZ
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONPICK();
    }
    pass DissolveMesh
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
}