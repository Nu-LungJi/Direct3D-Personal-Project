#include "Engine_Shader_Defines.hlsli"

matrix g_ViewMatrix, g_ProjMatrix, g_WorldMatrix;

Texture2D NormalTextureA;
Texture2D NormalTextureB;
Texture2D ReflectionTexture;
Texture2D RefractionTexture;
Texture2D DepthTexture;

cbuffer OceanPixelParameter : register(b5)
{
    float2 ScreenResolution;
    float NearZ;
    float FarZ;
    
    float PS_TimeAccumulation;
    float3 CameraPosition;
    
    float3 LightDirection;
    float Padding03;
    
    float2 WaterTransparency;
    float ShoreRange;
    float Padding04;
    
    float3 HorizontalExtinction;
    float Padding05;
    
    float3 ShoreDiffuse;
    float Padding06;
    
    float3 SurfaceColor;
    float Padding07;
    
    float3 DepthColor;
    float Padding08;
};

cbuffer OceanVertexParameter : register(b6)
{
    float3 OceanWave_DirectionA;
    float OceanWave_SteepnessA;
    
    float OceanWave_LengthA;
    float OceanWave_HeightA;
    float2 Padding00;
    
    float3 OceanWave_DirectionB;
    float OceanWave_SteepnessB;

    float OceanWave_LengthB;
    float OceanWave_HeightB;
    float2 Padding01;

    float  VS_TimeAccumulation;
    float3 Padding02;
}

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPos : POSITION;
    float2 vTexCoord : TEXCOORD0;
    float3 vNormal   : NORMAL;
    float3 vTangent  : TANGENT;
    float vLinearDepth : TEXCOORD1;
};                   
void Compute_GerstnerWave(float3 _Position, float3 _Direction, float _Steepness, float _Length, float _Height, float _TimeAccumulation
, inout float3 _Offset, inout float3 _Tangent, inout float3 _BiNormal)
{
    float k = 2.f * 3.141592f / _Length;
    float c = sqrt(9.8f / k);
    
    float2 d = normalize(_Direction.xz);
    float DotProduct = dot(d, _Position.xz);
    
    float phase = k * (DotProduct - c * _TimeAccumulation);
    
    _Offset.x = (_Steepness / k) * d.x * cos(phase);
    _Offset.z = (_Steepness / k) * d.y * cos(phase);
    _Offset.y = _Height * sin(phase);
    
    _Tangent.x += -d.x * d.x * (_Steepness * sin(phase));
    _Tangent.z += -d.x * d.y * (_Steepness * sin(phase));
    _Tangent.y += d.x * (_Height * k * cos(phase));
    
    _BiNormal.x += -d.x * d.y * (_Steepness * sin(phase));
    _BiNormal.z += -d.y * d.y * (_Steepness * sin(phase));
    _BiNormal.y += d.y * (_Height * k * cos(phase));
}
float LinearizeDepth(float _NDCDepth, float _NearPlane, float _FarPlane)
{
    return (2.f * _NearPlane * _FarPlane) / (_FarPlane + _NearPlane - _NDCDepth * (_FarPlane - _NearPlane));
}
float3 DepthRefraction(float2 waterTransparency, float2 waterDepthValues, float shoreRange, float3 horizontalExtinction,
	float3 refractionColor, float3 shoreColor, float3 surfaceColor, float3 depthColor)
{
    float waterClarity = waterTransparency.x;
    float visibility = waterTransparency.y;
    float waterDepth = waterDepthValues.x;
    float viewWaterDepth = waterDepthValues.y;

    float accDepth = viewWaterDepth * waterClarity; // accumulated water depth
    float accDepthExp = saturate(accDepth / (2.5 * visibility));
    accDepthExp *= (1.0 - accDepthExp) * accDepthExp * accDepthExp + 1; // out cubic

    surfaceColor = lerp(shoreColor, surfaceColor, saturate(waterDepth / shoreRange));
    float3 waterColor = lerp(surfaceColor, depthColor, saturate(waterDepth / horizontalExtinction));

    refractionColor = lerp(refractionColor, surfaceColor * waterColor, saturate(accDepth / visibility));
    refractionColor = lerp(refractionColor, depthColor, accDepthExp);
    refractionColor = lerp(refractionColor, depthColor * waterColor, saturate(waterDepth / horizontalExtinction));
    return refractionColor;
}
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 OriginalPosition = In.vPosition;
    
    float3 TotalOffset  = { 0.f, 0.f, 0.f };
    float3 Tangent      = { 1.f, 0.f, 0.f };
    float3 BiNormal     = { 0.f, 0.f, 1.f };
    
    Compute_GerstnerWave(OriginalPosition, OceanWave_DirectionA, OceanWave_SteepnessA, OceanWave_LengthA, OceanWave_HeightA, VS_TimeAccumulation, TotalOffset, Tangent, BiNormal);
    Compute_GerstnerWave(OriginalPosition, OceanWave_DirectionB, OceanWave_SteepnessB, OceanWave_LengthB, OceanWave_HeightB, VS_TimeAccumulation, TotalOffset, Tangent, BiNormal);
    
    float3 FinalPosition = OriginalPosition + TotalOffset;
    
    float4x4 matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    float4 Position = mul(float4(FinalPosition, 1.f), g_WorldMatrix);
    Position = mul(Position, g_ViewMatrix);
    Out.vLinearDepth = Position.z;
    
    Out.vPosition = mul(Position, g_ProjMatrix);
    Out.vWorldPos = mul(float4(FinalPosition, 1.f), g_WorldMatrix).xyz;
    Out.vTexCoord = In.vTexCoord;
    mul(float4(Out.vWorldPos, 1.f), g_ViewMatrix).z;
    
    float3 ComputeNormal = normalize(cross(BiNormal, Tangent));
    Out.vNormal     = normalize(mul(ComputeNormal, (float3x3)g_WorldMatrix));
    Out.vTangent    = normalize(mul(Tangent, (float3x3) g_WorldMatrix));
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPos : POSITION;
    float2 vTexCoord : TEXCOORD0;
    float3 vNormal   : NORMAL;
    float3 vTangent  : TANGENT;
    float vLinearDepth : TEXCOORD1;
};
struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    /// Scroll = Distortion
    float2 ScreenUV = In.vPosition.xy / ScreenResolution;
    
    float2 ScrollUVA = In.vTexCoord * 12.f + float2(0.01f, 0.02f) * PS_TimeAccumulation;
    float2 ScrollUVB = In.vTexCoord * 16.f + float2(-0.015f, 0.01f) * PS_TimeAccumulation;
    
    float3 NormalA = NormalTextureA.Sample(LinearSampler, ScrollUVA).xyz * 2.0f - 1.0f;
    float3 NormalB = NormalTextureB.Sample(LinearSampler, ScrollUVB).xyz * 2.0f - 1.0f;
    
    float3 WorldNormal = normalize(NormalA + NormalB + In.vNormal);
    float2 Distortion = WorldNormal.xz * 0.005f;
    
    float2 FinalRefractUV = saturate(ScreenUV + Distortion);
    float2 FinalReflectUV = saturate(ScreenUV + Distortion);
    
    // Reflection / Refraction
    float3 RefractionColor = RefractionTexture.Sample(LinearSampler, FinalRefractUV).rgb;
    float3 ReflectionColor = ReflectionTexture.Sample(LinearSampler, FinalRefractUV).rgb;
    
    // Reflection
    float3 ReflectDirection = reflect(-normalize(CameraPosition - In.vWorldPos), WorldNormal);
    
    float2 ReflectUV = ReflectDirection.xy * 0.5f + 0.5f;
    float3 ReflectedColor = ReflectionColor * 0.3f;
    
    // OceanDepth
    float RawFloorDepth = DepthTexture.Sample(LinearSampler, FinalRefractUV).r;
    float FloorDepth = LinearizeDepth(RawFloorDepth, NearZ, FarZ);
    
    float3 WorldViewDirection = In.vWorldPos - CameraPosition;
    float3 CameraLook = normalize(float3(g_ViewMatrix._13, g_ViewMatrix._23, g_ViewMatrix._33));
    
    float SurfaceDepth = abs(dot(WorldViewDirection, CameraLook));
    float VerticalDepth = max(0.f, FloorDepth - SurfaceDepth);
    float3 RayLength = length(WorldViewDirection / max(SurfaceDepth, 0.001f)) * 3.f;
    
    float WaterDepth = VerticalDepth * RayLength;
    
    //float SurfaceDepth = In.vLinearDepth;
    //float WaterDepth = max(0.f, FloorDepth - SurfaceDepth);
    
    float3 ViewDirection = normalize(CameraPosition - In.vWorldPos);
    float  ViewWaterDepth = WaterDepth / max(ViewDirection.y, 0.001f);
    float2 WaterDepthValues = float2(WaterDepth, ViewWaterDepth);
    
    float DepthFactor = saturate(WaterDepth / 10.f);
    DepthFactor = smoothstep(0.f, 1.f, DepthFactor);
    
    float3 WaterBaseColor = DepthRefraction(WaterTransparency, WaterDepthValues, ShoreRange, HorizontalExtinction,
                                    RefractionColor, lerp(ShoreDiffuse, DepthColor, DepthFactor), SurfaceColor, DepthColor);
    
    // Fresnel
    float Fresnel = saturate(abs(dot(WorldNormal, ViewDirection)));
    Fresnel = pow(1.f - Fresnel, 5.f);
    
    float3 NormalizedLightDirection = normalize(LightDirection);
    float3 HalfVec = normalize(NormalizedLightDirection + ViewDirection);
    float  Specular = pow(max(dot(WorldNormal, HalfVec), 0.f), 64.f);
    float3 SpecularDiffuse = float3(1.f, 1.f, 0.95f) * Specular * 2.5f;
    
    //float3 FinalColor = lerp(WaterBaseColor, ReflectedColor, Fresnel * 0.4f) + SpecularDiffuse;
    float3 FinalColor = WaterBaseColor + SpecularDiffuse;
   
    Out.vColor = float4(FinalColor, 1.f);
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