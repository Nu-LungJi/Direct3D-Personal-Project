#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;

vector g_vCamPosition;

float g_DepthMultiplier = 1.f;
float g_PortalAlpha = 0.f;

float g_Time;

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
    float2 vTexcoord : TEXCOORD0;
    float3 ViewDirTS : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    float3 vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
    
    // 3. 월드 공간의 시선 벡터 계산 (카메라 위치 - 정점 위치)
    float3 vWorldViewDir = normalize(g_vCamPosition.xyz - vWorldPos);
    
    // 4. 정점의 T, B, N 벡터를 월드 공간으로 변환
    // (g_WorldMatrix의 회전 성분만 필요하므로 float3x3 변환)
    float3 vNormalW = normalize(mul(In.vNormal, (float3x3) g_WorldMatrix));
    float3 vTangentW = normalize(mul(In.vTangent, (float3x3) g_WorldMatrix));
    float3 vBinormalW = normalize(mul(In.vBinormal, (float3x3) g_WorldMatrix));
    
    // 5. 월드 시선 벡터를 Tangent Space로 변환하기 위한 TBN 변환 (행벡터 방식 변환)
    Out.ViewDirTS.x = dot(vWorldViewDir, vTangentW);
    Out.ViewDirTS.y = dot(vWorldViewDir, vBinormalW);
    Out.ViewDirTS.z = dot(vWorldViewDir, vNormalW);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 ViewDirTS : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In) : SV_TARGET
{
    PS_OUT Out;
    float3 viewDir = normalize(In.ViewDirTS);
    
    float2 parallaxOffset = (viewDir.xy / max(viewDir.z, 0.001f)) * g_DepthMultiplier;
    
    float2 fakeInteriorUV = In.vTexcoord - parallaxOffset;
    float cosTheta = cos(g_Time * 0.2f);
    float sinTheta = sin(g_Time * 0.2f);
    float2 center = float2(0.5f, 0.5f);
    
    float2 rotatedUV = fakeInteriorUV - center;
    rotatedUV = float2(
        rotatedUV.x * cosTheta - rotatedUV.y * sinTheta,
        rotatedUV.x * sinTheta + rotatedUV.y * cosTheta
    );
    rotatedUV += center;
    
    Out.vDiffuse.xyz = g_DiffuseTexture.Sample(LinearSampler, rotatedUV) * 2.f;
    Out.vDiffuse.a = g_PortalAlpha;
    
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
}