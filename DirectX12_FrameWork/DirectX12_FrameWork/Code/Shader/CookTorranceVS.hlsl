//=============================================================================
// VSInput Struct
//=============================================================================
struct VSInput{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 Tangent  : TANGENT;
};

//=============================================================================
// VSOutput Struct
//=============================================================================
struct VSOutput{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD;
	float3 Normal   : NORMAL;
	float3 WorldPos : WORLD_POS;
};

//=============================================================================
// Transform Constat Buffer
//=============================================================================
cbuffer Transform : register(b0){
	float4x4 World	: packoffset(c0); // ワールド行列
	float4x4 View	: packoffset(c4); // ビュー行列
	float4x4 Proj	: packoffset(c8); // 射影行列
}


//-----------------------------------------------------------------------------
// Main Entry Point
//-----------------------------------------------------------------------------
VSOutput main(VSInput input){
	
	VSOutput output = (VSOutput) 0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;
	output.Normal = normalize(mul((float3x3) World, input.Normal));
	output.WorldPos = worldPos.xyz;

	return output;
	
}