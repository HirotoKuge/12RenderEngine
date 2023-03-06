//=============================================================================
// PixcelShader Input Struct
//=============================================================================
struct PSInput{
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT;  // �ڃx�N�g��
    float3 biNormal : BINORMAL; // �]�x�N�g��
};

//=============================================================================
// PixcelShader Output Struct
//=============================================================================
struct PSOutPut{
    float4 albedo   : SV_Target0; // �A���x�h
    float4 normal   : SV_Target1; // �@��
    float4 depth    : SV_Target2; // �[�x�l
};

//=============================================================================
// Textures
//=============================================================================
// ���f���e�N�X�`��
Texture2D<float4> g_texture : register(t0);
// �@���}�b�v�ɃA�N�Z�X���邽�߂̕ϐ�
Texture2D<float4> g_normalMap : register(t1);
// �X�y�L�����}�b�v�ɃA�N�Z�X���邽�߂̕ϐ�
Texture2D<float4> g_specularMap : register(t2);


//=============================================================================
// TEST:Material ConstantBuffer BufferRgister0�@
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

    // �A���x�h�J���[���o��
    psOut.albedo = g_texture.Sample(g_sampler, psIn.uv);

    // �@���}�b�v����^���W�F���g�X�y�[�X�̖@�����T���v�����O����
    float3 localNormal = g_normalMap.Sample(g_sampler, psIn.uv).xyz;
    
    // �^���W�F���g�X�y�[�X�̖@����0�`1�͈̔͂���-1�`1�͈̔͂ɕ�������
    localNormal = (localNormal - 0.5f) * 2.0f;

    // �^���W�F���g�X�y�[�X�̖@�������[���h�X�y�[�X�ɕϊ�����
    float3 normal = psIn.tangent * localNormal.x + psIn.biNormal * localNormal.y + psIn.normal * localNormal.z;


    // �@�����o��
    // �o�͂�0�`1�Ɋۂ߂��Ă��܂��̂Ń}�C�i�X�̒l�������Ă��܂�
    // �Ȃ̂�-1�`1��0�`1�ɕϊ�����
    // (-1 �` 1) �� 2.0       = (-0.5 �` 0.5)
    // (-0.5 �` 0.5) + 0.5  = ( 0.0 �` 1.0)
    psOut.normal.xyz = (normal / 2.0f) + 0.5f;

    // �X�y�L�������x��psOut.normal.w�ɑ��
    psOut.normal.w = Matallic;

    // �s�N�Z���V�F�[�_�[����[�x�l���o��
    psOut.depth.x = psIn.pos.z;
    
    // yzw�͖��g�p
    psOut.depth.yzw = 0.0f;
    
    return psOut;
}