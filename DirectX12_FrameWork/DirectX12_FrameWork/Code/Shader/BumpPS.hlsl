//=============================================================================
// VSOutput Struct
//=============================================================================
struct VSOutput
{
	float4 Position : SV_POSITION;	// 位置座標で
	float2 TexCoord : TEXCOORD;		// テクスチャ座標
	float4 WorldPos : WORLD_POS;	// ワールド空間での位置座標
#if 0
	float3x3	TangentBasis	: TANGENT_BASIS;		// 接線空間への基底変換行列
#else
	float3x3 InvTangentBasis : INV_TANGENT_BASIS; // 接線空間への基底変換行列の逆行列です.
#endif
};

//=============================================================================
// PSOutput Struct
//=============================================================================
struct PSOutput
{
	float4 Color : SV_TARGET0; // 出力カラー
};

//=============================================================================
// Light Constant Buffer
//=============================================================================
cbuffer LightBuffer : register(b1){
	float3 LightPosition : packoffset(c0);	// ライトの位置座標
	float3 LightColor : packoffset(c1);		// ライトカラー
	float3 CameraPosition : packoffset(c2); // カメラの位置座標
};

//=============================================================================
// Material Constant Buffer
//=============================================================================
cbuffer MaterialBuffer : register(b2){
	float3 Diffuse : packoffset(c0);	// 拡散反射率
	float Alpha : packoffset(c0.w);		// 透過度
	float3 Specular : packoffset(c1);	// 鏡面反射率
	float Shininess : packoffset(c1.w); // 鏡面反射強度
};

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
SamplerState WrapSmp : register(s0);	// テクスチャリピート
Texture2D ColorMap   : register(t0);	// カラーマップ
Texture2D NormalMap  : register(t1);	// 法線マップ

//-----------------------------------------------------------------------------
// Pixcel Shader Main Entry Point
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput) 0;

#if 0
	/* 接線空間上でライティングする場合 */

	// ライトベクトル.
	float3 L = normalize(LightPosition - input.WorldPos.xyz);
	L = mul(input.TangentBasis, L);

	// 視線ベクトル.
	float3 V = normalize(CameraPosition - input.WorldPos.xyz);
	V = mul(input.TangentBasis, V);

	// 法線ベクトル.
	float3 N = NormalMap.Sample(WrapSmp, input.TexCoord).xyz * 2.0 - 1.0;
#else
	/* ワールド空間上でライティングする場合 */

	// ライトベクトル.
	float3 L = normalize(LightPosition - input.WorldPos.xyz);

	// 視線ベクトル.
	float3 V = normalize(CameraPosition - input.WorldPos.xyz);

	// 法線ベクトル.
	float3 N = NormalMap.Sample(WrapSmp, input.TexCoord).xyz * 2.0 - 1.0;
	N = mul(input.InvTangentBasis, N);
#endif

	// 反射ベクトル.
	float3 R = normalize(-reflect(V, N));

	float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
	float3 diffuse = Diffuse * LightColor * saturate(dot(L, N));
	float3 specular = Specular * LightColor * pow(saturate(dot(L, R)), Shininess);

	output.Color = float4(color.rgb * (diffuse + specular), color.a * Alpha);

	return output;
}