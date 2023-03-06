//=============================================================================
// VertexShader Input 
//=============================================================================
struct VSInput{
    float4 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 biNormal : BINORMAL;
};

//=============================================================================
// VertexShader Output
//=============================================================================
struct VSOutput{
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float3 tangent  : TANGENT; // �ڃx�N�g��
    float3 biNormal : BINORMAL; // �]�x�N�g��

};

//=============================================================================
// Model ConstantBuffer BufferRegister0
//=============================================================================
cbuffer ModelCb : register(b0){
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

//=============================================================================
// ModelVS 
//=============================================================================
VSOutput main(VSInput vsIn){
    VSOutput psIn;

    psIn.pos = mul(mWorld, vsIn.pos); // ���f���̒��_�����[���h���W�n�ɕϊ�
    psIn.pos = mul(mView, psIn.pos); // ���[���h���W�n����J�������W�n�ɕϊ�
    psIn.pos = mul(mProj, psIn.pos); // �J�������W�n����X�N���[�����W�n�ɕϊ�
    psIn.normal = normalize(mul(mWorld, vsIn.normal));

    // �ڃx�N�g���Ə]�x�N�g�������[���h��Ԃɕϊ�����
    psIn.tangent = normalize(mul(mWorld, vsIn.tangent));
    psIn.biNormal = normalize(mul(mWorld, vsIn.biNormal));
    
    psIn.uv = vsIn.uv;

    return psIn;
}