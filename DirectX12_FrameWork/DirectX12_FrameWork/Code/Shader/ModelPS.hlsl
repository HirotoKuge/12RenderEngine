//=============================================================================
// PixcelShader Input Struct
//=============================================================================
struct PSInput{
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT;  // 接ベクトル
    float3 biNormal : BINORMAL; // 従ベクトル
};

//=============================================================================
// PixcelShader Output Struct
//=============================================================================
struct PSOutPut{
    float4 albedo   : SV_Target0; // アルベド
    float4 normal   : SV_Target1; // 法線
    float4 depth    : SV_Target2; // 深度値
};

//=============================================================================
// Textures
//=============================================================================
// モデルテクスチャ
Texture2D<float4> g_texture : register(t0);
// 法線マップにアクセスするための変数
Texture2D<float4> g_normalMap : register(t1);
// スペキュラマップにアクセスするための変数
Texture2D<float4> g_specularMap : register(t2);


//=============================================================================
// TEST:Material ConstantBuffer BufferRgister0　
//=============================================================================
cbuffer MaterialCb : register(b1){
    float Matallic;
    float Roughness;
}


//=============================================================================
// Sampler State
//=============================================================================
sampler g_sampler : register(s0);


//=============================================================================
// ModelPS main Entry Point
//=============================================================================
PSOutPut main(PSInput psIn){
    PSOutPut psOut;

    // アルベドカラーを出力
    psOut.albedo = g_texture.Sample(g_sampler, psIn.uv);

    // 法線マップからタンジェントスペースの法線をサンプリングする
    float3 localNormal = g_normalMap.Sample(g_sampler, psIn.uv).xyz;
    
    // タンジェントスペースの法線を0～1の範囲から-1～1の範囲に復元する
    localNormal = (localNormal - 0.5f) * 2.0f;

    // タンジェントスペースの法線をワールドスペースに変換する
    float3 normal = psIn.tangent * localNormal.x + psIn.biNormal * localNormal.y + psIn.normal * localNormal.z;


    // 法線を出力
    // 出力は0～1に丸められてしまうのでマイナスの値が失われてしまう
    // なので-1～1を0～1に変換する
    // (-1 ～ 1) ÷ 2.0       = (-0.5 ～ 0.5)
    // (-0.5 ～ 0.5) + 0.5  = ( 0.0 ～ 1.0)
    psOut.normal.xyz = (normal / 2.0f) + 0.5f;

    // スペキュラ強度をpsOut.normal.wに代入
    psOut.normal.w = Matallic;

    // ピクセルシェーダーから深度値を出力
    psOut.depth.x = psIn.pos.z;
    
    // yzwは未使用
    psOut.depth.yzw = 0.0f;
    
    return psOut;
}