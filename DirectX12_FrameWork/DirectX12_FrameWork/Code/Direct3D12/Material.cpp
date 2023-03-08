/*****************************************************************//**
 * \file   Material.cpp
 * \brief  �}�e���A���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "Material.h"

//=============================================================================
// static member
//=============================================================================
const wchar_t* Material::DummyTag = L""; // �_�~�[�e�N�X�`���p

//=============================================================================
// �}�e���A���̏�����
//=============================================================================
bool Material::Init(
	const ResMaterialPBR& resMaterial, 
	const wchar_t* p_vsFilePath, 
	const wchar_t* p_psFilePath, 
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat, 
	uint32_t numSrv, 
	uint32_t numCbv, 
	uint32_t offsetInDescriptorsFromTableStartCB, 
	uint32_t offsetInDescriptorsFromTableStartSRV, 
	D3D12_FILTER samplerFilter
){

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	auto pCommandQueue =GraphicsEngine::GetInstance()->GetCommandQueue();


	// �e�N�X�`����������
	InitTexture(pDevice,pCommandQueue,resMaterial);

	// �萔�o�b�t�@���쐬
	PBR_Param param;
	param.Metallic = resMaterial.Matallic;
	param.Roughness = resMaterial.Roughness;
	m_materialCB.Init(sizeof(PBR_Param),&param);

	//���[�g�V�O�l�`����������
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[2];
	
	//�f�t�H���g�̃T���v��-
	samplerDescArray[0].Filter = samplerFilter;
	samplerDescArray[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].MipLODBias = 0;
	samplerDescArray[0].MaxAnisotropy = 0;
	samplerDescArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDescArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDescArray[0].MinLOD = 0.0f;
	samplerDescArray[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDescArray[0].ShaderRegister = 0;
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	//�V���h�E�}�b�v�p�̃T���v��-
	samplerDescArray[1] = samplerDescArray[0];
	
	//��r�Ώۂ̒l����������΂O�A�傫����΂P��Ԃ���r�֐���ݒ肷��
	samplerDescArray[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[1].MaxAnisotropy = 1;
	samplerDescArray[1].ShaderRegister = 1;

	m_rootSignature.Init(
		samplerDescArray,
		2,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);

	return true;
}

//=============================================================================
// �`��O����
//=============================================================================
void Material::BeginRender(RenderContext& rc){
	rc.SetRootSignature(m_rootSignature);
	rc.SetPipelineState(m_pipelineState);
}

//=============================================================================
// �p�C�v���C���X�e�[�g�̏�����
//=============================================================================
void Material::InitPipelineState(const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats){
	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	
	desc.InputLayout		= MeshVertex::InputLayout;
	desc.pRootSignature		= m_rootSignature.Get();
	desc.VS					= D3D12_SHADER_BYTECODE(m_pVSShader->GetCompiledBlob());
	desc.PS					= D3D12_SHADER_BYTECODE(m_pPSShader->GetCompiledBlob());
	
	desc.RasterizerState			= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);	// ���X�^���C�U�[�̓f�t�H���g
	desc.RasterizerState.CullMode	= D3D12_CULL_MODE_NONE;				// �J�����O�͂Ȃ�
	desc.BlendState					= CD3DX12_BLEND_DESC(D3D12_DEFAULT);				// �u�����h�X�e�[�g���f�t�H���g
	
	desc.DepthStencilState					= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// �[�x�X�e���V���̓f�t�H���g���g��
	desc.DepthStencilState.DepthEnable		= TRUE;
	desc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
	desc.DepthStencilState.DepthFunc		= D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.StencilEnable	= FALSE;
	
	desc.SampleMask = UINT_MAX;
	
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;// �O�p�`��`��

	int numRenderTarget = 0;
	for (auto& format : colorBufferFormats) {
		if (format == DXGI_FORMAT_UNKNOWN) {
			//�t�H�[�}�b�g���w�肳��Ă��Ȃ��ꏊ��������I���
			break;
		}
		desc.RTVFormats[numRenderTarget] = colorBufferFormats[numRenderTarget];
		numRenderTarget++;
	}
	desc.NumRenderTargets = numRenderTarget;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;

	m_pipelineState.Init(desc);

}

//=============================================================================
// �V�F�[�_�[��������
//=============================================================================
void Material::InitShader(const wchar_t* pVSShaderPath, const wchar_t* pPSShaderPath){
	m_pVSShader = new Shader();
	m_pVSShader->LoadVScso(pVSShaderPath);

	m_pPSShader = new Shader();
	m_pPSShader->LoadPScso(pPSShaderPath);

}
void Material::InitShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* vsSkinEntriyPointFunc,
	const char* psEntryPointFunc
){
	//�V�F�[�_�[�����[�h����
	m_pVSShader->LoadVS("Assets/shader/sample.fx", "VSMain");
	m_pPSShader->LoadPS("Assets/shader/sample.fx", "PSMain");
}

//=============================================================================
// �e�N�X�`��������
//=============================================================================
void Material::InitTexture(
	ID3D12Device5* pDevice,
	ID3D12CommandQueue* pCommandQueue,
	const ResMaterialPBR& resMaterial){


	//�A���x�h�}�b�v
	{
		// �Y���t�@�C�������݂��Ă���Ȃ�
		if (resMaterial.AlbedMapFileName != L"") {
			// �e�N�X�`�������[�h����
			// TODO:.dds�ȊO�ɂ��Ή���������
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO]->
				InitFromDDSFile(resMaterial.AlbedMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO] = nullptr;
		}
	}	
	// ���^���l�X�}�b�v
	{
		// �Y���t�@�C�������݂��Ă���Ȃ�
		if (resMaterial.MatalnessMapFileName != L"") {
			// �e�N�X�`�������[�h����
			// TODO:.dds�ȊO�ɂ��Ή���������
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC]->
				InitFromDDSFile(resMaterial.MatalnessMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC] = nullptr;
		}
	}
	// ���t�l�X�}�b�v
	{
		// �Y���t�@�C�������݂��Ă���Ȃ�
		if (resMaterial.RoughnessMapFileName != L"") {
			// �e�N�X�`�������[�h����
			// TODO:.dds�ȊO�ɂ��Ή���������
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS]->
				InitFromDDSFile(resMaterial.RoughnessMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS ] = nullptr;
		}
	}
	// �m�[�}���}�b�v
	{
		// �Y���t�@�C�������݂��Ă���Ȃ�
		if (resMaterial.NormalMapFileName != L"") {
			// �e�N�X�`�������[�h����
			// TODO:.dds�ȊO�ɂ��Ή���������
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL]->
				InitFromDDSFile(resMaterial.NormalMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL] = nullptr;
		}
	}
	// �n�C�g�}�b�v
	{
		// �Y���t�@�C�������݂��Ă���Ȃ�
		if (resMaterial.HeightMapFileName != L"") {
			// �e�N�X�`�������[�h����
			// TODO:.dds�ȊO�ɂ��Ή���������
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT]->
				InitFromDDSFile(resMaterial.HeightMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT] = nullptr;
		}
	}
	
}
