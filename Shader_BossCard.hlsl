#include "Engine_Shader_Defines.hlsli"

matrix WorldMatrix, ViewMatrix, ProjMatrix;

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
    float2 vTexCoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    matWV   = mul(WorldMatrix, ViewMatrix);
    matWVP  = mul(matWV, ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexCoord = In.vTexCoord;
    
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
        BaseToonColor = (DissolveColor + float4(0.3f, 0.f, 0.3f, 0.f));
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
