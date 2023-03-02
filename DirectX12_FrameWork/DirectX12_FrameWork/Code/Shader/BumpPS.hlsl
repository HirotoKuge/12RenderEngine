//=============================================================================
// VSOutput Struct
//=============================================================================
struct VSOutput
{
	float4 Position : SV_POSITION;	// �ʒu���W��
	float2 TexCoord : TEXCOORD;		// �e�N�X�`�����W
	float4 WorldPos : WORLD_POS;	// ���[���h��Ԃł̈ʒu���W
#if 0
	float3x3	TangentBasis	: TANGENT_BASIS;		// �ڐ���Ԃւ̊��ϊ��s��
#else
	float3x3 InvTangentBasis : INV_TANGENT_BASIS; // �ڐ���Ԃւ̊��ϊ��s��̋t�s��ł�.
#endif
};

//=============================================================================
// PSOutput Struct
//=============================================================================
struct PSOutput
{
	float4 Color : SV_TARGET0; // �o�̓J���[
};

//=============================================================================
// Light Constant Buffer
//=============================================================================
cbuffer LightBuffer : register(b1){
	float3 LightPosition : packoffset(c0);	// ���C�g�̈ʒu���W
	float3 LightColor : packoffset(c1);		// ���C�g�J���[
	float3 CameraPosition : packoffset(c2); // �J�����̈ʒu���W
};

//=============================================================================
// Material Constant Buffer
//=============================================================================
cbuffer MaterialBuffer : register(b2){
	float3 Diffuse : packoffset(c0);	// �g�U���˗�
	float Alpha : packoffset(c0.w);		// ���ߓx
	float3 Specular : packoffset(c1);	// ���ʔ��˗�
	float Shininess : packoffset(c1.w); // ���ʔ��ˋ��x
};

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
SamplerState WrapSmp : register(s0);	// �e�N�X�`�����s�[�g
Texture2D ColorMap   : register(t0);	// �J���[�}�b�v
Texture2D NormalMap  : register(t1);	// �@���}�b�v

//-----------------------------------------------------------------------------
// Pixcel Shader Main Entry Point
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput) 0;

#if 0
	/* �ڐ���ԏ�Ń��C�e�B���O����ꍇ */

	// ���C�g�x�N�g��.
	float3 L = normalize(LightPosition - input.WorldPos.xyz);
	L = mul(input.TangentBasis, L);

	// �����x�N�g��.
	float3 V = normalize(CameraPosition - input.WorldPos.xyz);
	V = mul(input.TangentBasis, V);

	// �@���x�N�g��.
	float3 N = NormalMap.Sample(WrapSmp, input.TexCoord).xyz * 2.0 - 1.0;
#else
	/* ���[���h��ԏ�Ń��C�e�B���O����ꍇ */

	// ���C�g�x�N�g��.
	float3 L = normalize(LightPosition - input.WorldPos.xyz);

	// �����x�N�g��.
	float3 V = normalize(CameraPosition - input.WorldPos.xyz);

	// �@���x�N�g��.
	float3 N = NormalMap.Sample(WrapSmp, input.TexCoord).xyz * 2.0 - 1.0;
	N = mul(input.InvTangentBasis, N);
#endif

	// ���˃x�N�g��.
	float3 R = normalize(-reflect(V, N));

	float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
	float3 diffuse = Diffuse * LightColor * saturate(dot(L, N));
	float3 specular = Specular * LightColor * pow(saturate(dot(L, R)), Shininess);

	output.Color = float4(color.rgb * (diffuse + specular), color.a * Alpha);

	return output;
}