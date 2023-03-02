/*****************************************************************//**
 * \file   LambertVS.hlsl
 * \brief  �����o�[�g���˃��f���p���_�V�F�[�_
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
	float4  Position : SV_POSITION; // �ʒu���W
	float2  TexCoord : TEXCOORD;	// �e�N�X�`�����W
	float3  Normal   : NORMAL;		// �@���x�N�g��
	float3  WorldPos : WORLD_POS;   // ���[���h��Ԃł̈ʒu���W
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