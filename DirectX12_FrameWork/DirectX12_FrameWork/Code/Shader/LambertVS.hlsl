/*****************************************************************//**
 * \file   LambertVS.hlsl
 * \brief  ランバート反射モデル用頂点シェーダ
 * 
 * \author Hiroto Kuge
 * \date   November 2022
 *********************************************************************/


//=============================================================================
// Input Struct
//=============================================================================
struct VSInput {
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
	float2 TexCoord	: TEXCOORD;
	float3 Tangent	: TANGENT;
};

//=============================================================================
// Output Struct
//=============================================================================
struct VSOutput{
	float4  Position : SV_POSITION; // 位置座標
	float2  TexCoord : TEXCOORD;	// テクスチャ座標
	float3  Normal   : NORMAL;		// 法線ベクトル
	float3  WorldPos : WORLD_POS;   // ワールド空間での位置座標
};
	
//=============================================================================
// Transform Constant Buffer
//=============================================================================
cbuffer Transform : register(b0) {
	float4x4 World	: packoffset(c0);
	float4x4 View	: packoffset(c4);
	float4x4 Proj	: packoffset(c8);
}

//-----------------------------------------------------------------------------
// Vertex Shader Main Entry Point
//-----------------------------------------------------------------------------
VSOutput main(VSInput input){
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);

	float4 worldPos	= mul(World, localPos);
	float4 viewPos	= mul(View, worldPos);
	float4 projPos	= mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;
	output.WorldPos = worldPos;
	output.Normal	= normalize(mul((float3x3)World, input.Normal));

	return output;
}