//-----------------------------------------------------------------------------
// Constant Values
//-----------------------------------------------------------------------------
static const float F_PI = 3.141596535f;

//=============================================================================
// VSOutput structure
//=============================================================================
struct VSOutput{
	float4 Position : SV_POSITION; // �ʒu���W
	float2 TexCoord : TEXCOORD; // �e�N�X�`�����W
	float3 Normal : NORMAL; // �@���x�N�g��
	float3 WorldPos : WORLD_POS; // ���[���h��Ԃ̈ʒu���W
};

//=============================================================================
// PSOutput structure
//=============================================================================
struct PSOutput{
	float4 Color : SV_TARGET0; // �o�̓J���[
};

//=============================================================================
// LightBuffer 
//=============================================================================
cbuffer LightBuffer : register(b1){
	float3 LightPosition : packoffset(c0);	// ���C�g�ʒu
	float3 LightColor : packoffset(c1);		// ���C�g�J���[
	float3 CameraPosition : packoffset(c2); // �J�����ʒu
};

//=============================================================================
// MaterialBuffer
//=============================================================================
cbuffer MaterialBuffer : register(b2){
	float3 BaseColor : packoffset(c0);	// ��{�F
	float Alpha : packoffset(c1.w);		// ���ߓx
	float Roughness : packoffset(c1);	// �ʂ̑e��(�͈͂�[0, 1])
	float Metallic : packoffset(c1.y);	// ���^���b�N
};	

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);

//-----------------------------------------------------------------------------
// Schlick�ɂ��t���l�����̋ߎ���
//-----------------------------------------------------------------------------
float3 SchlickFresnel(float3 specular, float VH){
	return specular + (1.0f - specular) * pow((1.0f - VH), 5.0f);
}

//-----------------------------------------------------------------------------
// Beckmann���z�֐�
//-----------------------------------------------------------------------------
float D_Beckmann(float m, float NH){
	float c2 = NH * NH;
	float c4 = c2 * c2;
	float m2 = m * m;

	return (1.0f / (m2 * c4)) * exp((-1.0f / m2) * (1.0f - c2) / c2);
}

//-----------------------------------------------------------------------------
// V-cavity�ɂ��V���h�E�C���O-�}�X�L���O�֐�
//-----------------------------------------------------------------------------
float G2_Vcavity(float NH, float NV, float NL, float VH)
{
	return min(1.0f, min(2.0f * NH * NV / VH, 2.0f * NH * NL / VH));
}

//-----------------------------------------------------------------------------
// Main Entry Point
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput) 0;

	float3 N = normalize(input.Normal);
	float3 L = normalize(LightPosition - input.WorldPos);
	float3 V = normalize(CameraPosition - input.WorldPos);
	float3 R = normalize(-V + 2.0f * dot(N, V) * N);
	float3 H = normalize(V + L);

	float NH = saturate(dot(N, H));
	float NV = saturate(dot(N, V));
	float NL = saturate(dot(N, L));
	float VH = saturate(dot(V, H));

	float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
	float3 Kd = BaseColor * (1.0f - Metallic);
	float3 diffuse = Kd * (1.0 / F_PI);

	float3 Ks = BaseColor * Metallic;
	float a = Roughness * Roughness;
	float D = D_Beckmann(a, NH);
	float G2 = G2_Vcavity(NH, NV, NL, VH);
	float3 Fr = SchlickFresnel(Ks, NL);

	float3 specular = (D * G2 * Fr) / (4.0f * NV * NL);

	output.Color = float4(LightColor * color.rgb * (diffuse + specular) * NL, color.a * Alpha);
	return output;
}