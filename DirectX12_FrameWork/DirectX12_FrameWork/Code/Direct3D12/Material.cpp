/*****************************************************************//**
 * \file   Material.cpp
 * \brief  �}�e���A���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Material.h"
#include <DirectXMath.h>
#include "GraphicsEngine.h"
#include "../Util/Logger.h"
#include "../Util/FileUtil.h"




//-----------------------------------------------------------------------------
// static member.
//-----------------------------------------------------------------------------
const wchar_t* Material::DummyTag = L""; // �_�~�[�e�N�X�`���p


//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
Material::~Material()
{}


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

	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto pCommandQueue = GraphicsEngine::GetInstance()->GetCommandQueue();


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


	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout				= MeshVertex::InputLayout;
	desc.RasterizerState			= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		// ���X�^���C�U�[�̓f�t�H���g
	desc.RasterizerState.CullMode	= D3D12_CULL_MODE_NONE;							// �J�����O�͂Ȃ�
	desc.BlendState					= CD3DX12_BLEND_DESC(D3D12_DEFAULT);			// �u�����h�X�e�[�g���f�t�H���g
	desc.DepthStencilState			= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// �[�x�X�e���V���̓f�t�H���g���g��
	desc.SampleMask					= UINT_MAX;
	desc.PrimitiveTopologyType		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// �O�p�`��`��
	
	int numRenderTarget = 0;
	for (auto& format : colorBufferFormat) {
		if (format == DXGI_FORMAT_UNKNOWN) {
			//�t�H�[�}�b�g���w�肳��Ă��Ȃ��ꏊ��������I���
			break;
		}
		desc.RTVFormats[numRenderTarget] = colorBufferFormat[numRenderTarget];
		numRenderTarget++;
	}
	desc.NumRenderTargets	= numRenderTarget;
	desc.DSVFormat			= DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count	= 1;


#ifdef _DEBUG
	auto isSuccessed = m_pipelineState.Init(
		desc,
		p_vsFilePath,
		p_psFilePath,
		m_rootSignature.Get());

	if (isSuccessed == false) {
		ELOG("Error : Failed Init PipelineState Class");
		return false;
	}
#else
	auto isSuccessed = m_pipelineState.Init(
		pDevice,
		desc,
		p_vsFilePath,
		p_psFilePath,
		m_rootSignature.Get());

	if (isSuccessed == false) {
		ELOG("Error : Failed Init PipelineState Class");
		return false;
	}

#endif // _DEBUG
	

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
// �e�N�X�`��������
//=============================================================================
void Material::InitTexture(
	ID3D12Device5* pDevice,
	ID3D12CommandQueue* pCommandQueue,
	const ResMaterialPBR& resMaterial){


	//�A���x�h�}�b�v�B
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

	
	
}