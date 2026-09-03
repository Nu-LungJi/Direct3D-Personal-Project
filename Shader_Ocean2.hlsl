#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 OceanWaveDataA;
float4 OceanWaveDataB;

float TimeAccumulation;
float3 CameraPosition;

float2 ScreenResolution = { 1280.f, 720.f };
float NearZ = 0.01f;
float FarZ = 1000.f;

Texture2D RampTexture;
Texture2D FoamTexture;
Texture2D NoiseTexture;
Texture2D DepthTexture;
Texture2D ReflectionTexture;

matrix ReflectViewMatrix;

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
    float3 vNormal : NORMAL;
    float2 vTexCoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD7;
};
void Compute_GerstnerWave(float4 _WaveData, float3 _Position, inout float3 _Offset, inout float3 _Tangent, inout float3 _BiNormal)
{
    float2 Direction = normalize(_WaveData.xy);
    float Steepness = _WaveData.z;
    float WaveLength = _WaveData.w;
    
    float k = 2.f * 3.141592f;
    float c = sqrt(9.8f / k);
    
    float Phase = k * (dot(Direction, _Position.xz) - c * TimeAccumulation);
    float a = Steepness / k;
    
    _Offset.x += Direction.x * (a * cos(Phase));
    _Offset.y += a * sin(Phase);
    _Offset.z += Direction.y * (a * cos(Phase));
    
    _Tangent += float3(-Direction.x * Direction.x * (Steepness * sin(Phase)), Direction.x * (Steepness * cos(Phase)), -Direction.x * Direction.y * (Steepness * sin(Phase)));
    _BiNormal += float3(-Direction.x * Direction.y * (Steepness * sin(Phase)), Direction.y * (Steepness * cos(Phase)), -Direction.y * Direction.y * (Steepness * sin(Phase)));
}
float LinearizeDepth(float _NDCDepth, float _NearPlane, float _FarPlane)
{
    return (_NearPlane * _FarPlane) / (_FarPlane - _NDCDepth * (_FarPlane - _NearPlane));
}
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 OriginalPosition = In.vPosition;
    
    float3 TotalOffset = { 0.f, 0.f, 0.f };
    float3 Tangent = { 1.f, 0.f, 0.f };
    float3 BiNormal = { 0.f, 0.f, 1.f };
    
    Compute_GerstnerWave(OceanWaveDataA, OriginalPosition, TotalOffset, Tangent, BiNormal);
    Compute_GerstnerWave(OceanWaveDataB, OriginalPosition, TotalOffset, Tangent, BiNormal);
    
    float3 FinalPosition = OriginalPosition + TotalOffset;
    
    float4x4 matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(FinalPosition, 1.f), matWVP).xyzw;
    Out.vWorldPos = mul(float4(FinalPosition, 1.f), g_WorldMatrix).xyz;
    Out.vTexCoord = In.vTexCoord;
    Out.vNormal = normalize(cross(BiNormal, Tangent));
    Out.vNormal = float3(0.f, 0.4f, 0.f);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPos : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexCoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD7;
};
struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float DistnaceFromCenter = length(In.vWorldPos.xz);
    float MaxGradiantDistnace = 150.f;
    
    float DepthDiffuseUV = saturate(DistnaceFromCenter / MaxGradiantDistnace);
    
    // Foam //
    float2 VirtualUV = In.vWorldPos.xz * 0.35f;
    float2 FoamUVA = VirtualUV * 1.0f + float2(0.06f, 0.03f) * TimeAccumulation;
    float2 FoamUVB = VirtualUV * 1.3f + float2(0.04f, 0.06f) * TimeAccumulation;
    
    float FoamA = FoamTexture.Sample(LinearSampler, FoamUVA).r;
    float FoamB = FoamTexture.Sample(LinearSampler, FoamUVB).r;
    float CombinedFoam = FoamA * FoamB;
    
    float StylizedCaustics = pow(CombinedFoam, 1.5f);
    float3 CausticsLight = float3(1.f, 1.f, 1.f) * StylizedCaustics * 0.6f;
    float3 CausticsShadow = float3(0.f, 0.05f, 0.1f) * (1.f - StylizedCaustics) * 0.2f;
    
    float3 ViewDirection = normalize(CameraPosition - In.vWorldPos);
    float3 FlattenNormal = float3(0.f, 1.f, 0.f);
    
    float3 OceanDiffuse_Near = float3(0.027f, 0.227f, 0.458f) * 0.4f;
    float3 OceanDiffuse_Far  = float3(0.027f, 0.227f, 0.458f) * 1.0f;
    
    // Noise
    float2  MicroUV = In.vWorldPos.xz * 1.5f + float2(0.05f, -0.05f) * TimeAccumulation;
    float   MicroNoise = NoiseTexture.Sample(LinearSampler, MicroUV).r * 0.25f;
    
    // Specular
    float3 LightDirection = normalize(float3(0.9f, 0.4f, -0.4f));
    
    float3 HalfDirection = normalize(LightDirection + ViewDirection);
    float3 OceanNormal = normalize(In.vNormal);
    
    OceanNormal.xz *= 0.15f;
    float NDL = saturate(dot(normalize(OceanNormal), HalfDirection));
    float3 Specular = float3(1.f, 1.f, 1.f) * pow(NDL, 12.f) * 0.3f;
    
    // Reflection
    float3 ReflectCameraPos = ReflectViewMatrix[3].xyz;
    
    float3 NoiseNormalOffset = float3(CombinedFoam * 2.f - 1.f, 0.f, CombinedFoam * 2.f - 1.f);
    float3 Normal = normalize(float3(0.f, 1.f, 0.f) + NoiseNormalOffset * 0.05f);
    
    float4 ReflectClipPos = mul(float4(In.vWorldPos, 1.f), ReflectViewMatrix);
    ReflectClipPos = mul(ReflectClipPos, g_ProjMatrix);
    
    float2 PureProjUV = ReflectClipPos.xy / ReflectClipPos.w;
    float2 ScreenUV = float2(PureProjUV.x * 0.5f + 0.5f, -PureProjUV.y * 0.5f + 0.5f);
    
    //float CameraHeightDistance = abs(CameraPosition.y - In.vWorldPos.y);
    ScreenUV.y += ViewDirection.y * 0.05f;
    //ScreenUV.y = min(saturate(ScreenUV.y), 0.495f);
    
    float2 Distort = float2(CombinedFoam * 2.f - 1.f, 0.f) * 0.012f;
    ScreenUV = saturate(ScreenUV + Distort);
    
    float3 ReflectionDiffuse = ReflectionTexture.Sample(LinearSampler, ScreenUV).rgb * 0.8f;
    
    float3 ReflectionViewDirection = reflect(ReflectCameraPos, In.vWorldPos);
    float3 ReflectionDirection = reflect(-ReflectionViewDirection, Normal);
    
    // Fresnel
    float Fresnel = saturate(1.f - dot(float3(0.f, 1.f, 0.f), ViewDirection));
    Fresnel = pow(Fresnel, 4.f);
    
    //DepthFade
    float2 ScreenSpaceUV = In.vProjPos.xy / In.vProjPos.w;
    ScreenSpaceUV = float2(ScreenSpaceUV.x * 0.5f + 0.5f, -ScreenSpaceUV.y * 0.5f + 0.5f);
    float RawDepth = DepthTexture.Sample(LinearSampler, ScreenSpaceUV).r;
    float SceneLinearDepth = LinearizeDepth(RawDepth, 0.01f, 1000.f);
    float WaterLinearDepth = In.vProjPos.w;
    float WaterDepth = SceneLinearDepth - WaterLinearDepth;
    
    float DepthFade = saturate(WaterDepth / 2.f);
    DepthFade = max(DepthFade, 0.15f);
    
    float3 FinalColor = OceanDiffuse_Near + (CausticsLight - CausticsShadow) * 1.5f;
    FinalColor = lerp(FinalColor, OceanDiffuse_Far, Fresnel) + (float3(1.f, 1.f, 1.f) * MicroNoise * 0.025f);
    FinalColor = lerp(FinalColor, ReflectionDiffuse, Fresnel) + Specular;
    
    Out.vColor.rgb = FinalColor;
    Out.vColor.a = 1.f;
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}