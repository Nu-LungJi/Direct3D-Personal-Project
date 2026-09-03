#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D EffectTexture;
Texture2D DissolveTexture;
Texture2D NoiseTexture;

float2  EffectScrollSpeed       = { 0.f, 0.3f };
float   EffectAlpha             = { 1.f };

float3  EffectEmissiveColor     = { 0.f, 0.f, 0.f };
float   EffectEmissiveIntensity = { 0.f };

float2  DissolveScrollSpeed = { 0.f, 0.f };

float2  NoiseScrollSpeed = { 0.f, 0.f };

float   DissolveStrength = { 0.f };
float   DissolveEdgeWidth = { 0.f };
float3  DissolveEdgeColor = { 1.f, 0.6f, 0.6f };

float   TimeProgress = { 0.f };
float   TimeAccumulation = { 0.f };

float2  EffectTextureOffset = { 0.f, 0.f };
float2  DissolveTextureOffset = { 0.f, 0.f };

float   EffectFadeOutValue = { 0.2f };


sampler LinearSampler = sampler_state
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};
sampler NormalSampler = sampler_state
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
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

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

PS_OUT PS_NORMALEFFECT(PS_IN In)
{
    PS_OUT Out;

    // Dissolve Texture Sampling
    float2  NoiseUV         = In.vTexcoord + DissolveTextureOffset + DissolveScrollSpeed * TimeAccumulation;
    float4  NoiseColor      = DissolveTexture.Sample(LinearSampler, NoiseUV);
    float2  NoiseUVoffset   = (NoiseColor.rg - 0.5f) * DissolveStrength;
    
    // Effect Texture Sampling
    float2  EffectUV        = In.vTexcoord + NoiseUVoffset + EffectTextureOffset + EffectScrollSpeed * TimeAccumulation;
    float4  EffectColor     = EffectTexture.Sample(LinearSampler, EffectUV);
    
    // Dissolve Noise Strength
    float   ScaledNoise     = NoiseColor.r * DissolveStrength;
    
    // Dissolve FadeOut
    float   Progress = pow(TimeProgress, 1.5f);
    float   DissolveMask = NoiseColor.r - Progress;
    
    // Effect FadeOut
    float   EffectFadeLeft      = smoothstep(0.0f, EffectFadeOutValue, In.vTexcoord.x);
    float   EffectFadeRight     = smoothstep(1.0f, 1.0f - EffectFadeOutValue, In.vTexcoord.x);
    float   EffectFadeBottom    = smoothstep(0.0f, EffectFadeOutValue, In.vTexcoord.y);
    float   EffectFadeTop       = smoothstep(1.0f, 1.0f - EffectFadeOutValue, In.vTexcoord.y);
    
    float EffectFade = EffectFadeLeft * EffectFadeRight * EffectFadeTop * EffectFadeBottom;
    
    //clip(DissolveMask);
    float Softness = 0.15f + (Progress * 0.35f);
    float SmoothDissolve = smoothstep(Progress, Progress + Softness, NoiseColor.r);
    /// * --  Additional Effect -- * ///
       
    // Effect Emissive
    float3  EffectEmissive  = EffectEmissiveColor * EffectEmissiveIntensity;
    
     // Edge Glow
    float DissolveEdge  = smoothstep(0.f, DissolveEdgeWidth, DissolveMask) * smoothstep(DissolveEdgeWidth * 2.0f, DissolveEdgeWidth, DissolveMask);
    float3 EdgeGlow     = EffectColor.rgb * DissolveEdgeColor * DissolveEdge * SmoothDissolve;
    
    float3 BaseColor    = EffectColor.rgb * EffectEmissiveColor * EffectEmissiveIntensity;
    float3 FinalColor   = lerp(BaseColor, BaseColor + EdgeGlow, DissolveEdge) * 5.f;
    
    Out.vColor.xyz = (FinalColor.xyz + EffectEmissive + EdgeGlow) * EffectFade;
    Out.vColor.a   = EffectColor.a * EffectFade * pow(EffectAlpha, 3.f) * SmoothDissolve;

    return  Out;
}
PS_OUT PS_REPEATEFFECT(PS_IN In) {
    PS_OUT Out;
    
    // Dissolve Texture Sampling
    float2 NoiseUV = In.vTexcoord + DissolveTextureOffset + DissolveScrollSpeed;
    float4 NoiseColor = DissolveTexture.Sample(NormalSampler, NoiseUV);
    float2 NoiseUVoffset = (NoiseColor.rg - 0.5f) * DissolveStrength;
        
        // Effect Texture Sampling
    float2 EffectUV = In.vTexcoord + NoiseUVoffset + EffectTextureOffset + EffectScrollSpeed;
    float4 EffectColor = EffectTexture.Sample(NormalSampler, EffectUV);
        
        // Dissolve Noise Strength
    float ScaledNoise = NoiseColor.r * 0.5f;
        
        // Dissolve FadeOut
    float Progress = 0.5f; //pow(TimeProgress, 1.5f);
    float DissolveMask = NoiseColor.r - Progress;
        
        // Effect FadeOut
    float EffectFadeLeft = smoothstep(0.0f, EffectFadeOutValue, In.vTexcoord.x);
    float EffectFadeRight = smoothstep(1.0f, 1.0f - EffectFadeOutValue, In.vTexcoord.x);
    float EffectFadeBottom = smoothstep(0.0f, EffectFadeOutValue, In.vTexcoord.y);
    float EffectFadeTop = smoothstep(1.0f, 1.0f - EffectFadeOutValue, In.vTexcoord.y);
        
    float EffectFade = EffectFadeLeft * EffectFadeRight * EffectFadeTop * EffectFadeBottom;
        
        //clip(DissolveMask);
    float Softness = 0.15f + (Progress * 0.35f);
    float SmoothDissolve = smoothstep(Progress, Progress + Softness, NoiseColor.r);
        /// * --  Additional Effect -- * ///
        
        // Effect Emissive
    float3 EffectEmissive = EffectEmissiveColor * EffectEmissiveIntensity;
        
         // Edge Glow
    float DissolveEdge = smoothstep(0.f, DissolveEdgeWidth, DissolveMask) * smoothstep(DissolveEdgeWidth * 2.0f, DissolveEdgeWidth, DissolveMask);
    float3 EdgeGlow = EffectColor.rgb * DissolveEdgeColor * DissolveEdge * SmoothDissolve;
        
    float3 BaseColor = EffectColor.rgb * EffectEmissiveColor * EffectEmissiveIntensity;
    float3 FinalColor = lerp(BaseColor, BaseColor + EdgeGlow, DissolveEdge) * 5.f;
    
    Out.vColor.xyz = (FinalColor.xyz + EffectEmissive + EdgeGlow) * EffectFade;
    Out.vColor.a = EffectColor.a * EffectFade * pow(EffectAlpha, 3.f) * SmoothDissolve;

    return Out;
}
PS_OUT PS_RADIALEFFECT(PS_IN In) {
    PS_OUT Out;
    
    float2 SRCNoiseUV = In.vTexcoord + float2(0.05f, 0.08f) * TimeAccumulation;
    float2 DSTNoiseUV = In.vTexcoord + float2(-0.07f, 0.04f) * TimeAccumulation;
    
    float SRCNoiseRColor = NoiseTexture.Sample(LinearSampler, SRCNoiseUV).r;
    float DSTNoiseGColor = NoiseTexture.Sample(LinearSampler, DSTNoiseUV).g;
    
    float CombinedNoise = (SRCNoiseRColor * DSTNoiseGColor) / 2.f;
    
    float2 Disrtortion;
    Disrtortion.x = (CombinedNoise * 2.f - 1.f) + (sin(TimeAccumulation * 2.0f) * 0.1f);
    Disrtortion.y = (CombinedNoise * 2.f - 1.f) + (cos(TimeAccumulation * 1.5f) * 0.1f);
    
    float2 DiffuseUV = In.vTexcoord + Disrtortion * 0.02f;
    float4 DiffuseColor = EffectTexture.Sample(LinearSampler, DiffuseUV);
    
    Out.vColor = DiffuseColor;
    
    return Out;
}
technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_AlphaDepth, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NORMALEFFECT();
    }
    pass Repeat
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_AlphaDepth, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_REPEATEFFECT();
    }
    pass Radial
    {
        SetRasterizerState(RS_Cull_NONE);
        SetDepthStencilState(DSS_AlphaDepth, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIALEFFECT();
    }
}