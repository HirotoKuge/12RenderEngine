/*****************************************************************//**
 * \file   LambertPS.hlsl
 * \brief  ランバート反射モデル用ピクセルシェーダ
 * 
 * \author Hiroto Kuge
 * \date   November 2022
 *********************************************************************/

//=============================================================================
// Vertex Shader Output Struct
//=============================================================================
struct VSOutput {
	float4  Position : SV_POSITION; // 位置座標
	float2  TexCoord : TEXCOORD;	// テクスチャ座標
	float3  Normal   : NORMAL;		// 法線ベクトル
	float3  WorldPos : WORLD_POS;   // ワールド空間での位置座標
};

//=============================================================================
// Pixel Shader Output Struct (Single Render Target) 
//=============================================================================
struct PSOutput {
	float4 Color : SV_TARGET0;	//出力カラー
};

//=============================================================================
// Constant Values
//=============================================================================
static const float F_PI = 3.1415926535;

//=============================================================================
// Light Buffer Struct
//=============================================================================
cbuffer LightBuffer : register(b1){
	float3 LightPosition  : packoffset(c0);
	float3 LightColor : packoffset(c1);
};

//=============================================================================
// Material Buffer Struct
//=============================================================================
cbuffer MaterialBuffer : register(b2){
	float3 Diffuse : packoffset(c0);
	float Alpha : packoffset(c0.w);
	float3 Specular : packoffset(c1);
	float Shininees : packoffset(c1.w);
}

//=============================================================================
// Texture and Samplers
//=============================================================================
SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);


//-----------------------------------------------------------------------------
// Pixel Shader Main Entry Point
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input){
    PSOutput output = (PSOutput) 0;

    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPosition - input.WorldPos.xyz);

    float NL = saturate(dot(N, L));

    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
    float3 diffuse = Diffuse * (1.0 / F_PI);

    output.Color = float4(LightColor * color.rgb * diffuse * NL, color.a * Alpha);
	
    return output;
}