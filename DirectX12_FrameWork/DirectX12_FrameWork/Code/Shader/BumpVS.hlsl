
//=============================================================================
// VSInput Struct
//=============================================================================
struct VSInput{
	float3 Position : POSITION; // 頂点座標
	float3 Normal   : NORMAL;   // 法線ベクトル
	float2 TexCoord : TEXCOORD; // UV座標
	float3 Tangent  : TANGENT;  // 接線ベクトル
};

//=============================================================================
// VSOutput Struct
//=============================================================================
struct VSOutput{
	float4 Position : SV_POSITION; // 位置座標
	float2 TexCoord : TEXCOORD;	// UV座標
	float4 WorldPos : WORLD_POS;   // ワールド空間での位置座標
#if 0
	float3x3	TangentBasis	: TANGENT_BASIS;		// 接線空間への基底変換行列
#else
	float3x3 InvTangentBasis : INV_TANGENT_BASIS; // 接線空間への基底変換行列の逆行列
#endif
};

//=============================================================================
// Transform Constant Buffer
//=============================================================================
cbuffer Transform : register(b0){
	float4x4 World : packoffset(c0); // ワールド行列
	float4x4 View : packoffset(c4);  // ビュー行列
	float4x4 Proj : packoffset(c8);  // 射影行列
}

//-----------------------------------------------------------------------------
// Vertex Shader Main Entry Point
//-----------------------------------------------------------------------------
VSOutput main( VSInput input ){
	VSOutput output = (VSOutput) 0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;
	output.WorldPos = worldPos;

	// 基底ベクトル
	float3 N = normalize(mul((float3x3) World, input.Normal));
	float3 T = normalize(mul((float3x3) World, input.Tangent));
	float3 B = normalize(cross(N, T));

#if 0
	/* 接線空間上でライティングする場合 */

	// 基底変換行列.
	output.TangentBasis	= float3x3(T, B, N);

#else
	/* ワールド空間上でライティングする場合 */

	// 基底変換行列の逆行列.
	output.InvTangentBasis = transpose(float3x3(T, B, N));
#endif

	return output;
}