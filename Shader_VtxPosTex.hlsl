#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

/*삼각형리스트로 그린다 -> 인덱스버퍼에 있는 인덱스 세개에 해당하는 정점 세개롤 뽑아온다. */ 
/* 각각 vs_main에 집어넣어서 함수 세개를 각각 동시에 호출하여 정점 쉐이더를 시작한다. */
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

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 

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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;    

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    
    vector vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        
    Out.vColor = vColor;
    
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