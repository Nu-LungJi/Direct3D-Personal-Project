#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInverse, g_ProjMatrixInverse;
Texture2D g_Texture;

Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_NormalTexture;
Texture2D g_DepthTexture;
Texture2D g_SpecularTexture;

vector g_vLightPos;
float g_fLightRange;
vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
vector g_vMtrlAmbient = 1.f;//vector(0.8f, 0.8f, 0.8f, 1.f);
vector g_vMtrlSpecular = 1.f; //vector(0.3f, 0.3f, 0.3f, 1.f);

vector g_vCamPosition;

Texture2D g_CombinedTexture : register(t0);
Texture2D g_NonLightTexture : register(t1);
Texture2D g_BlurVTexture : register(t2);
Texture2D g_BlurHTexture : register(t3);

float2      g_vTexelOffset;
float       g_fThreshold = 0.8f;
float g_fBloomIntensity;

static const float Weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
static const float Offsets[5] = { 0.0, 1.3846153, 3.2307692, 5.0769230, 6.9230769 };

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
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/* 투영변환 -> W나누기 */ 
/* 뷰포트로 변환해준다 */ 
/* 래스터라이즈 : 픽셀의 정보가 생성된다. */ 
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    vector vBackBuffer : SV_TARGET0;
    
};

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vNormal = vector(0.f, 0.f, -1.f, 0.f);
    Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) +
        (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 2차원 투영공간상의 위치 */ 
    /* 0 -> -1 */
    /* 1 -> 1 */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    /* 0 -> 1 */
    /* 1 -> -1 */
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰스페이스 상의 위치 */ 
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vNormal = vector(0.f, 0.f, -1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 2차원 투영공간상의 위치 */ 
    /* 0 -> -1 */
    /* 1 -> 1 */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    /* 0 -> 1 */
    /* 1 -> -1 */
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰스페이스 상의 위치 */ 
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    Out.vShade = (g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) +
        (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f) * fAtt;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vNonLight = g_NonLightTexture.Sample(LinearSampler, In.vTexcoord);
   
    Out.vBackBuffer = vDiffuse * vShade + vSpecular + vNonLight;
    Out.vBackBuffer.a = 1.f;
    
    return Out;
}

float3 ExtractBrightColor(float3 color)
{
    float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));
    if (luminance < g_fThreshold)
    {
        return float3(0.f, 0.f, 0.f);
    }
    return color;
}
PS_OUT_BACKBUFFER PS_BlurHorizontal_WithBright(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 s_vTexelOffset = float2(1.0f / 640.0f, 0.0f);
    
    float3 centerColor = g_CombinedTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float3 result = ExtractBrightColor(centerColor) * Weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = s_vTexelOffset * Offsets[i];
        
        float3 colorPlus = g_CombinedTexture.Sample(LinearSampler, In.vTexcoord + offset).rgb;
        float3 colorMinus = g_CombinedTexture.Sample(LinearSampler, In.vTexcoord - offset).rgb;
        
        result += ExtractBrightColor(colorPlus) * Weights[i];
        result += ExtractBrightColor(colorMinus) * Weights[i];
    }
    
    Out.vBackBuffer = float4(result, 1.f);
    
    return Out;
}
PS_OUT_BACKBUFFER PS_BlurVertical(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 s_vTexelOffset = float2(0.0f, 1.0f / 360.0f);
    
    float3 result = g_BlurHTexture.Sample(LinearSampler, In.vTexcoord).rgb * Weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = s_vTexelOffset * Offsets[i];
        
        result += g_BlurHTexture.Sample(LinearSampler, In.vTexcoord + offset).rgb * Weights[i];
        result += g_BlurHTexture.Sample(LinearSampler, In.vTexcoord - offset).rgb * Weights[i];
    }
    
    Out.vBackBuffer = float4(result, 1.f);
    
    return Out;
}
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

PS_OUT_BACKBUFFER PS_Composite(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    float4 sceneColor = g_CombinedTexture.Sample(LinearSampler, In.vTexcoord);
    float4 bloomColor = g_BlurVTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNonLight = g_NonLightTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 FakeAlpha = max(vNonLight.r, max(vNonLight.g, vNonLight.b));
    FakeAlpha = saturate(FakeAlpha);
    float3 WorldColor = lerp(sceneColor, vNonLight, FakeAlpha);
    float3 finalColor = WorldColor + (bloomColor.rgb * g_fBloomIntensity);

    finalColor = ACESFilm(finalColor);
   
    Out.vBackBuffer = float4(pow(finalColor, 1.f / 2.2f), 1.f);
    //Out.vBackBuffer = float4(finalColor, 1.f);
    return Out;
}
PS_OUT_BACKBUFFER PS_PureBrightPass(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float3 CombinedTexColor = g_CombinedTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    
    Out.vBackBuffer = float4(ExtractBrightColor(CombinedTexColor), 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
    pass BrightPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BlurHorizontal_WithBright();
    }
    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BlurVertical();
    }
    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Composite();
    }
    pass PureBright
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PureBrightPass();
    }
}