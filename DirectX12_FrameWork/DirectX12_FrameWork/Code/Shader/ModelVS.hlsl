//=============================================================================
// VertexShader Input 
//=============================================================================
struct VSInput{
    float4 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 biNormal : BINORMAL;
};

//=============================================================================
// VertexShader Output
//=============================================================================
struct VSOutput{
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT; // 接ベクトル
    float3 biNormal : BINORMAL; // 従ベクトル

};

//=============================================================================
// Model ConstantBuffer BufferRegister0
//=============================================================================
cbuffer ModelCb : register(b0){
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

//=============================================================================
// ModelVS 
//=============================================================================
VSOutput main(VSInput vsIn){
    VSOutput psIn;

    psIn.pos = mul(mWorld, vsIn.pos); // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
    psIn.normal = normalize(mul(mWorld, vsIn.normal));

    // 接ベクトルと従ベクトルをワールド空間に変換する
    psIn.tangent = normalize(mul(mWorld, vsIn.tangent));
    psIn.biNormal = normalize(mul(mWorld, vsIn.biNormal));
    
    psIn.uv = vsIn.uv;

    return psIn;
}