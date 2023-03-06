
//=============================================================================
// PixcelShader Input Struct
//=============================================================================
struct PSInput{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

//=============================================================================
// User Expand ConstantBuffer register1
//=============================================================================
cbuffer ExpandCbParam : register(b1){
    Matrix viewProjInvMatrix;   // �r���[�v���W�F�N�V�����s��̋t�s��
    float3 ligColor;            // ���C�g�̃J���[
    float3 ligDirection;        // ���C�g�̕���
    float3 eyePos;              // ���_
};


//=============================================================================
// Textures
//=============================================================================
Texture2D<float4> albedoTexture : register(t0); // �A���x�h
Texture2D<float4> normalTexture : register(t1); // �@��
Texture2D<float4> depthTexture : register(t2);  // �[�x

//=============================================================================
// Sampler State
//=============================================================================
sampler Sampler : register(s0);


//=============================================================================
// DefferdSprite PSShader
//=============================================================================
float4 main(PSInput In) : SV_TARGET0{
    // G-Buffer�̓��e���g���ă��C�e�B���O
    // �A���x�h�J���[���T���v�����O
    float4 albedo = albedoTexture.Sample(Sampler, In.uv);
    // �@�����T���v�����O
    float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
    
    // �[�x��񂩂烏�[���h���W���v�Z����
    float4 worldPos;

    // �܂����K���X�N���[�����W�n�ł̍��W���v�Z����
    // z���W�͐[�x�e�N�X�`��������������Ă���
    worldPos.z = depthTexture.Sample(Sampler, In.uv).r;
    
    // xy���W��UV���W����v�Z����
    worldPos.xy = In.uv * float2(2.0f, -2.0f) - 1.0f;
    worldPos.w = 1.0f;
    
    // �r���[�v���W�F�N�V�����s��̋t�s�����Z���āA���[���h���W�ɖ߂�
    worldPos = mul(viewProjInvMatrix, worldPos);
    worldPos.xyz /= worldPos.w;
    normal = (normal * 2.0f) - 1.0f;
    
    // �g�U���ˌ����v�Z
    float3 lig = 0.0f;
    float t = max(0.0f, dot(normal, ligDirection) * -1.0f);
    lig = ligColor * t;

    // �X�y�L�������˂��v�Z
    float3 toEye = normalize(eyePos - worldPos.xyz);
    float3 r = reflect(ligDirection, normal);
    t = max(0.0f, dot(toEye, r));
    t = pow(t, 5.0f);
    
    // ������₷�����邽�߂�50�{���Ă���
    float3 specColor = ligColor * t * 50.0f;

    // �X�y�L�������x��@���e�N�X�`����w�v�f����T���v�����O����
    float specPower = normalTexture.Sample(Sampler, In.uv).w;

    // �X�y�L�������x���X�y�L�������C�g�ɏ�Z����
    specColor *= specPower;

    // ���ˌ��ɃX�y�L�����J���[�𑫂��Z����
    lig += specColor;
    
    // ���� �K��
    lig += 0.7f;
    
    float4 finalColor =albedo;
    finalColor.xyz *= lig;
    //return finalColor;
    
    
    return float4(1.0, 0.3, 0.4, 1.0);
    
}