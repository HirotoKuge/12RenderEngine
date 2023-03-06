/*****************************************************************//**
 * \file  Sprite.cpp
 * \brief �X�v���C�g
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
 
//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "Sprite.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


namespace {
	struct SimpleVertex {
		Vector4 pos;
		Vector2 tex;
	};
}

const Vector2 Sprite::DEFAULT_PIVOT = { 0.5f, 0.5f };

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Sprite::~Sprite(){
	
}

//=============================================================================
// ����������
//=============================================================================
void Sprite::Init(const SpriteInitData& initData){
	
	// �T�C�Y���i�[
	m_size.x = static_cast<float>(initData.width);
	m_size.y = static_cast<float>(initData.height);

	// �e�N�X�`����������
	InitTextures(initData);

	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��������
	InitVertexBufferAndIndexBuffer(initData);

	// �萔�o�b�t�@��������
	InitConstantBuffer(initData);
	
	// ���[�g�V�O�l�`����������
	m_rootSignature.Init(
		initData.samplerFilter,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	// �p�C�v���C���X�e�[�g��������
	InitPipelineState(initData);

	// �f�B�X�N���v�^�q�[�v��������
	InitDescriptorHeap(initData);
}

//=============================================================================
// �X�V����
//=============================================================================
void Sprite::Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot){
	//�s�{�b�g���l���ɓ��ꂽ���s�ړ��s����쐬
	//�s�{�b�g�͐^�񒆂�0.0, 0.0�A���オ-1.0f, -1.0�A�E����1.0�A1.0�ɂȂ�悤�ɂ���
	Vector2 localPivot = pivot;
	localPivot.x -= 0.5f;
	localPivot.y -= 0.5f;
	localPivot.x *= -2.0f;
	localPivot.y *= -2.0f;
	//�摜�̃n�[�t�T�C�Y�����߂�
	Vector2 halfSize = m_size;
	halfSize.x *= 0.5f;
	halfSize.y *= 0.5f;
	Matrix mPivotTrans;

	mPivotTrans.MakeTranslation(
		{ halfSize.x * localPivot.x, halfSize.y * localPivot.y, 0.0f }
	);
	Matrix mTrans, mRot, mScale;
	mTrans.MakeTranslation(pos);
	mRot.MakeRotationFromQuaternion(rot);
	mScale.MakeScaling(scale);
	m_worldMtx = mPivotTrans * mScale;
	m_worldMtx = m_worldMtx * mRot;
	m_worldMtx = m_worldMtx * mTrans;
}

//============================================================================
// �`�揈��
//=============================================================================
void Sprite::Draw(RenderContext& renderContext){
	
	// �J�������擾
	auto pCamera2D =GraphicsEngine::GetInstance()->GetCamera2D();
	auto pCamera3D =GraphicsEngine::GetInstance()->GetCamera3D();
	
	//���݂̃r���[�|�[�g���畽�s���e�s����v�Z���� 
	D3D12_VIEWPORT viewport = renderContext.GetViewport();

	//TODO:�J�����s��͒萔�Ɏg�p �ǂ����ς��Ȃ��̂� 
	Matrix viewMatrix = pCamera2D->GetViewMatrix();
	Matrix projMatrix;
	projMatrix.MakeOrthoProjectionMatrix(viewport.Width, viewport.Height, 0.1f, 1.0f);

	m_constantBufferCPU.mvp = m_worldMtx * viewMatrix * projMatrix;
	m_constantBufferCPU.mulColor.x = 1.0f;
	m_constantBufferCPU.mulColor.y = 1.0f;
	m_constantBufferCPU.mulColor.z = 1.0f;
	m_constantBufferCPU.mulColor.w = 1.0f;
	m_constantBufferCPU.screenParam.x = pCamera3D->GetNear();
	m_constantBufferCPU.screenParam.y = pCamera3D->GetFar();
	m_constantBufferCPU.screenParam.z = FRAME_BUFFER_W;
	m_constantBufferCPU.screenParam.w = FRAME_BUFFER_H;

	//�萔�o�b�t�@���X�V 
	//renderContext.UpdateConstantBuffer(m_constantBufferGPU, &m_constantBufferCPU);
	m_constantBufferGPU.CopyToVRAM(&m_constantBufferCPU);
	if (m_userExpandConstantBufferCPU != nullptr) {
		//renderContext.UpdateConstantBuffer(m_userExpandConstantBufferGPU, m_userExpandConstantBufferCPU);
		m_userExpandConstantBufferGPU.CopyToVRAM(m_userExpandConstantBufferCPU);
	}
	//���[�g�V�O�l�`����ݒ� 
	renderContext.SetRootSignature(m_rootSignature);
	
	//�p�C�v���C���X�e�[�g��ݒ� 
	renderContext.SetPipelineState(m_pipelineState);
	
	//���_�o�b�t�@��ݒ� 
	renderContext.SetVertexBuffer(m_vertexBuffer);
	
	//�C���f�b�N�X�o�b�t�@��ݒ� 
	renderContext.SetIndexBuffer(m_indexBuffer);
	
	//�v���~�e�B�u�g�|���W�[��ݒ肷�� 
	renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//�f�B�X�N���v�^�q�[�v��ݒ肷�� 
	renderContext.SetDescriptorHeap(m_descriptorHeap);
	
	//�`��
	renderContext.DrawIndexed(m_indexBuffer.GetCount());
}




#pragma region InitMethod
//=============================================================================
// �e�N�X�`���̏�����
//=============================================================================
void Sprite::InitTextures(const SpriteInitData& initData) {

	uint32_t texNo = 0;
	// �X�v���C�g�Ŏg�p����e�N�X�`������������
	if (initData.ddsFilePath[0] != nullptr) {
		// dds�t�@�C���̃p�X���w�肳��Ă�̂Ȃ�Adds�t�@�C������e�N�X�`�����쐬����
		int texNo = 0;
		while (initData.ddsFilePath[texNo] && texNo < MAX_TEXTURE) {
			m_textures[texNo].InitFromDDSFile(initData.ddsFilePath[texNo]);
			texNo++;
		}
		m_numTexture = texNo;
	}
	else if (initData.pTextures[0] != nullptr) {
		// �O���e�N�X�`��(GBuffer�Ȃ�)���w�肳��Ă���
		int texNo = 0;
		while (initData.pTextures[texNo] != nullptr) {
			m_pTextureExternals[texNo].reset(initData.pTextures[texNo]);
			texNo++;
		}
		m_numTexture = texNo;
	}
	else {
		//�e�N�X�`�����w�肳��ĂȂ� 
		MessageBoxA(nullptr, "�g�p����e�N�X�`���̏���ݒ肵�Ă�������", "�G���[", MB_OK);
		std::abort();
	}
}

//=============================================================================
// ���_�o�b�t�@�ƒ萔�o�b�t�@�̏�����
//=============================================================================
void Sprite::InitVertexBufferAndIndexBuffer(const SpriteInitData& initData) {

	float halfW = m_size.x * 0.5f;
	float halfH = m_size.y * 0.5f;

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();

	//�@�X�v���C�g�p�̃��\�[�X
	SimpleVertex vertices[] =
	{
		{
			Vector4(-halfW, -halfH, 0.0f, 1.0f),
			Vector2(0.0f, 1.0f),
		},
		{
			Vector4(halfW, -halfH, 0.0f, 1.0f),
			Vector2(1.0f, 1.0f),
		},
		{
			Vector4(-halfW, halfH, 0.0f, 1.0f),
			Vector2(0.0f, 0.0f)
		},
		{
			Vector4(halfW, halfH, 0.0f, 1.0f),
			Vector2(1.0f, 0.0f)
		}

	};

	unsigned short indices[] = { 0,1,2,3 };

	// ���_�o�b�t�@������
	m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]));
	m_vertexBuffer.Copy(vertices);

	// �C���f�b�N�X�o�b�t�@������
	m_indexBuffer.Init(sizeof(indices), sizeof(indices[0]));
	m_indexBuffer.Copy(indices);
}

//=============================================================================
// �p�C�v���C���X�e�[�g�̏�����
//=============================================================================
void Sprite::InitPipelineState(const SpriteInitData& initData) {
	
	// ���_���C�A�E�g���`���� 
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout						= { inputElementDescs, _countof(inputElementDescs) };
	desc.RasterizerState					= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);	// ���X�^���C�U�[�̓f�t�H���g
	desc.RasterizerState.CullMode			= D3D12_CULL_MODE_NONE;						// �J�����O�͂Ȃ�
	
	desc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);		// �u�����h�X�e�[�g���f�t�H���g
	if (initData.alphaBlendMode == AlphaBlendMode_Trans) {
		//�����������̃u�����h�X�e�[�g���쐬����
		desc.BlendState.RenderTarget[0].BlendEnable = true;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	}
	else if (initData.alphaBlendMode == AlphaBlendMode_Add) {
		//���Z����
		desc.BlendState.RenderTarget[0].BlendEnable = true;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	}

	desc.DepthStencilState.DepthFunc		= D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.DepthEnable		= FALSE;
	desc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.StencilEnable	= FALSE;
	desc.SampleMask							= UINT_MAX;
	desc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`��`��
	
	// RTV�t�H�[�}�b�g��ݒ�
	for (auto& format : initData.colorBufferFormats) {
		if (format == DXGI_FORMAT_UNKNOWN) {
			break;
		}
		desc.RTVFormats[desc.NumRenderTargets] = format;
		desc.NumRenderTargets++;
	}

	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;				// �T���v���[��1

	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	// TODO:�V�F�[�_�[�̈��������l����
	// �N���X�̂������₷�������ȂƂ͎v��
	// ��̓V�F�[�_�[�t�@�C�����Ȃ񂩎g���₷���̂ɂ�����
#ifdef _DEBUG
	m_pipelineState.Init(
		desc,
		L"Assets/Shader/DefferdSpriteVS.cso",
		L"Assets/Shader/DefferdSpritePS.cso",
		m_rootSignature.Get());

#else
	m_pPipelineState.Init(
		pDevice,
		desc,
		L"Assets/Shader/DefferdSpritePS.cso",
		L"Assets/Shader/DefferdSpritePS.cso",
		m_pRootSignature->Get());

#endif // _DEBUG

}

//=============================================================================
// �萔�o�b�t�@�̏�����
//=============================================================================
void Sprite::InitConstantBuffer(const SpriteInitData& initData) {
	//�萔�o�b�t�@�̏�����
	m_constantBufferGPU.Init(sizeof(m_constantBufferCPU), nullptr);
	//���[�U�[�g���̒萔�o�b�t�@���w�肳��Ă���
	if (initData.pExpandConstantBuffer != nullptr) {
		m_userExpandConstantBufferCPU = initData.pExpandConstantBuffer;
		m_userExpandConstantBufferGPU.Init(
			initData.expandConstantBufferSize,
			initData.pExpandConstantBuffer
		);
	}
}

//=============================================================================
// �f�B�X�N���v�^�q�[�v��������
//=============================================================================
void Sprite::InitDescriptorHeap(const SpriteInitData& initData) {
	if (m_pTextureExternals[0] != nullptr) {
		//�O���̃e�N�X�`�����w�肳��Ă��� 
		for (int texNo = 0; texNo < m_numTexture; texNo++) {
			m_descriptorHeap.RegistShaderResource(texNo, *m_pTextureExternals[texNo]);
		}
	}
	else {
		for (int texNo = 0; texNo < m_numTexture; texNo++) {
			m_descriptorHeap.RegistShaderResource(texNo, m_textures[texNo]);
		}
	}
	if (initData.pExpandShaderResoruceView != nullptr) {
		//�g���V�F�[�_�[���\�[�X�r���[
		m_descriptorHeap.RegistShaderResource(
			EXPAND_SRV_REG_START_NO,
			*initData.pExpandShaderResoruceView
		);
	}
	m_descriptorHeap.RegistConstantBuffer(0, m_constantBufferGPU);
	if (m_userExpandConstantBufferCPU != nullptr) {
		//���[�U�[�g���̒萔�o�b�t�@��b1�Ɋ֘A�t������
		m_descriptorHeap.RegistConstantBuffer(1, m_userExpandConstantBufferGPU);
	}
	m_descriptorHeap.Commit();
}

#pragma endregion

