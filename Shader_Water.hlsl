#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D GroundColor : register(t0);
Texture2D GroundDepthValue : register(t1);

matrix WaterWorldMatrix;


cbuffer WaterConstantData : register(b0) {
    matrix InvViewProj;
    float4 ScreenParam;
    float3 CamPosition;
    float GamePlayTime;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : POSITION1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    vector WaterPosition = mul(vector(In.vPosition, 1.f), WaterWorldMatrix);
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = vPosition;
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    return Out;
}