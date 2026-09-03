#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector  LightDirection      = vector(1.f, -1.f, 1.f, 0.f);
vector  LightColor_Diffuse  = vector(1.f, 1.f, 1.f, 1.f);
vector  LightColor_Ambient  = vector(1.f, 1.f, 1.f, 1.f);
vector  LightColor_Specular = vector(1.f, 1.f, 1.f, 1.f);

Texture2D g_DiffuseTexture;
vector  Material_Ambient    = vector(1.f, 1.f, 1.f, 1.f);
vector  Material_Specular   = vector(1.f, 1.f, 1.f, 1.f);

vector CameraPosition;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;            
    float2 TexCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 WorldPos : TEXCOORD1;
    //float4 ProjPos : TEXCOORD2;
};

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.Position = mul(float4(In.Position, 1.f), matWVP);
    Out.Normal  = mul(float4(In.Normal, 0.f), g_WorldMatrix);
    Out.TexCoord = In.TexCoord;
    Out.WorldPos = mul(float4(In.Position, 1.f), g_WorldMatrix);
    
    return Out;
}

/* 포지션시멘틱에 해당하는 변수의 w로 모든 성분을 나눈다 */
/* 뷰포트로 변환한다.(윈도우좌표로 변환) */
/* 래스터라이즈 : 리턴된 정점정보를 기반으로하여 사이를 선형보간한 픽셀을 만들어준다.  */

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 WorldPos : TEXCOORD1;
    //float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector Color : SV_TARGET0;   
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.TexCoord * 1.f);
    
    vector vShade = max(dot(normalize(LightDirection) * -1.f, normalize(In.Normal)), 0.f) +
        (LightColor_Ambient * Material_Ambient);
    
    vector vReflect = reflect(normalize(LightDirection), normalize(In.Normal));
    vector vLook = In.WorldPos - CameraPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 500.f);
    
    Out.Color = LightColor_Diffuse * vMtrlDiffuse * saturate(vShade) +
        (LightColor_Specular * Material_Specular) * fSpecular;
    
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