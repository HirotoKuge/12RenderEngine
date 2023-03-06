//=============================================================================
// VertexShader Input Struct
//=============================================================================
struct VSInput{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

//=============================================================================
// VertexShader Output Struct
//=============================================================================
struct VSOutput{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

//=============================================================================
// ConstantBuffer register0
//=============================================================================
cbuffer cb : register(b0){
    float4x4 mvp;
    float4 mulColor;
};


//=============================================================================
// DefferdSprite VSShader
//=============================================================================
VSOutput main(VSInput In){
    
    VSOutput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}