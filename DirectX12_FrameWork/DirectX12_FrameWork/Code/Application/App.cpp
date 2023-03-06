/*****************************************************************//**
 * \file   App.cpp
 * \brief  �A�v���P�[�V�����N���X
 * 
 * \author Hiroto Kuge
 * \date   November 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "App.h"

//-----------------------------------------------------------------------------
// �X�^�e�B�b�N�@�����o�[
//-----------------------------------------------------------------------------
const TCHAR* Application::WINDOW_TITLE = L"DirectX12";
const TCHAR* Application::WINDOW_CLASS_NAME = L"win32app";

const uint32_t Application::WINDOW_STYLE_WINDOWED = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
const uint32_t Application::WINDOW_EX_STYLE_WINDOWED = (0);
const uint32_t Application::WINDOW_STYLE_FULL_SCREEN = (WS_VISIBLE | WS_POPUP);
const uint32_t Application::WINDOW_EX_STYLE_FULL_SCREEN = (0);

const uint32_t Application::CLIENT_WIDTH = 1280;
const uint32_t Application::CLIENT_HEIGHT = 720;

uint32_t Application::SYSTEM_WIDTH = 0;
uint32_t Application::SYSTEM_HEIGHT = 0;

const float	Application::FPS = 60;


// �K���ɒu�����f�B���N�V���i�����C�g
struct DirectionalLight
{
	Matrix viewProjInvMatrix;
	Vector3  color;
	float pad0;         // �p�f�B���O
	Vector3  direction;
	float pad1;         // �p�f�B���O
	Vector3 eyePos;     // ���_
	float specPow;      // �X�y�L�����̍i��
};


// ���_�\����
struct SimpleVertex
{
	float pos[3];       // ���_���W
	float color[3];     // ���_�J���[
};

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);

/**************************************************//**
 * ��brief�@�R���X�g���N�^
 ******************************************************/
Application::Application() :mSystemCounter(0) {}

/**************************************************//**
 * ��brief�@�f�X�g���N�^
 ******************************************************/
Application::~Application() {
	Dispose();
}

/**************************************************//**
 * ��brief �C���X�^���X�擾(singleton)
 * \return �C���X�^���X�A�h���X
 ******************************************************/
Application* Application::Instance() {
	static Application instance;
	return &instance;
}

/**************************************************//**
 * ��brief �V�X�e���̈�̕��ƍ����̏�����
 ******************************************************/
void Application::InitSystemWH() {

}

/**************************************************//**
 * ��brief	����������
 * \param hInstance �C���X�^���X�n���h��
 * \return ����/���s
 ******************************************************/
bool Application::Init(HINSTANCE hInstance) {
	// �������[���[�N�����o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//�E�B���h�E�N���X�̃C���X�^���X�擾
	m_pWindow = Window::GetInstance();

	//�E�B���h�E�N���X�o�^
	m_pWindow->RegisterClass(hInstance, WINDOW_CLASS_NAME, CS_OWNDC);

	//�E�B���h�E���Z�b�g
	m_pWindow->SetWindow(hInstance,
		WINDOW_STYLE_WINDOWED,
		WINDOW_EX_STYLE_WINDOWED,
		NULL,
		CLIENT_WIDTH + SYSTEM_WIDTH,
		CLIENT_HEIGHT + SYSTEM_HEIGHT,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		false);

	//�E�B���h�E�n���h���擾
	mHWnd = m_pWindow->GetHandle();

	//�C���X�^���X�n���h���i�[
	mHInst = hInstance;

	
	//DirectX12������
	GraphicsEngine::GetInstance()->Init(mHWnd,CLIENT_WIDTH,CLIENT_HEIGHT);
	
	//Test:�J����������
	GraphicsEngine::GetInstance()->GetCamera3D()->SetPosition({ 0.0f, 120.0f, 300.0f });
	GraphicsEngine::GetInstance()->GetCamera3D()->SetTarget({ 0.0f, 120.0f, 0.0f });
	

	

	return true;
}

/**************************************************//**
 * ��brief �I������
 ******************************************************/
void Application::Dispose() {
	//// �W���o�̓N���[�Y
	//fclose(m_fp);
	//// �R���\�[���J��
	//::FreeConsole();
}

/**************************************************//**
 * ��brief�@���C�����[�v
 * \return ���b�Z�[�WID
 ******************************************************/
unsigned long Application::MainLoop(){
	//���b�Z�[�WID�i�[�p
	MSG	msg;
	ZeroMemory(&msg, sizeof(msg));

	clock_t current_time = 0;	//�A�v���P�[�V�����J�n����̌��݂̌o�ߎ���
	clock_t last_time = 0;		//�A�v���P�[�V�����J�n����̑O��t���[���̊J�n����

	//TEST �e�평��������

	// ���[�g�V�O�l�`�����쐬
	RootSignature rootSignature;
	InitRootSignature(rootSignature);

	// �V�F�[�_�[�����[�h
	Shader vs, ps;
	vs.LoadVS("Assets/shader/sample.fx", "VSMain");
	ps.LoadPS("Assets/shader/sample.fx", "PSMain");
	
	// �p�C�v���C���X�e�[�g���쐬
	PipelineState pipelineState;
	InitPipelineState(pipelineState, rootSignature, vs, ps);

	// �O�p�`�̒��_�o�b�t�@���쐬
	// ���_�z����`
	SimpleVertex vertices[] = {
		{
			{-0.5f, -0.5f, 0.0f},
			{ 1.0f, 0.0f, 0.0f }
		},
		{
			{ 0.0f, 0.5f, 0.0f },
			{ 0.0f, 1.0f, 0.0f }
		},
		{
			{ 0.5f, -0.5f, 0.0f },
			{ 0.0f, 0.0f, 1.0f }
		}
	};

	VertexBuffer triangleVB;
	triangleVB.Init(sizeof(vertices), sizeof(vertices[0]));
	triangleVB.Copy(vertices);

	// �O�p�`�̃C���f�b�N�X�o�b�t�@���쐬
	// �C���f�b�N�X�z��
	uint16_t indices[] = {
		0,1,2
	};
	IndexBuffer triangleIB;
	triangleIB.Init(sizeof(indices), 2);
	triangleIB.Copy(indices);

	auto& renderContext = GraphicsEngine::GetInstance()->GetRenderContext();

	while (m_pWindow->ExecMessage()) {
		
		// �Q�[���̍X�V�����F�Q�[���w
		//!{
				/* �����ɃQ�[���̍X�V�������L�q���Ă�������*/
		
		//!} 
		// �`��I�u�W�F�N�g�̃��X�g�𐶐��F�C���^�[�t�F�[�X�w
		//!{
				/* �����ɃC���^�[�t�F�[�X�w�̏������L�q���Ă�������*/
		//!} 

		// �`��̎��ۂ̏����𔭍s�F�����_�[�w
		//!{
		GraphicsEngine::GetInstance()->BeginRender();
		
		// ���[�g�V�O�l�`����ݒ�
		renderContext.SetRootSignature(rootSignature);
		// �p�C�v���C���X�e�[�g��ݒ�
		renderContext.SetPipelineState(pipelineState);
		// �v���~�e�B�u�̃g�|���W�[��ݒ�
		renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// ���_�o�b�t�@��ݒ�
		renderContext.SetVertexBuffer(triangleVB);
		// �C���f�b�N�X�o�b�t�@��ݒ�
		renderContext.SetIndexBuffer(triangleIB);
		// �h���[�R�[��
		renderContext.DrawIndexed(3);
	
		
		GraphicsEngine::GetInstance()->EndRender();

		//!} 

	}

	//�Q�[���̏I������



	return m_pWindow->GetMessage();
}

/**************************************************//**
 * ��brief	�E�B���h�E�n���h���擾
 * \return �E�B���h�E�n���h��
 ******************************************************/
HWND Application::GetHWnd() { return mHWnd; }

/**************************************************//**
 * ��brief �C���X�^���X�n���h���擾
 * \return �C���X�^���X�n���h��
 ******************************************************/
HINSTANCE Application::GetHInst() { return mHInst; }



// ���[�g�V�O�l�`���̏�����
void InitRootSignature(RootSignature& rs)
{
	rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

// �p�C�v���C���X�e�[�g�̏�����
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps)
{
	// ���_���C�A�E�g���`����
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// �p�C�v���C���X�e�[�g���쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rs.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps.GetCompiledBlob());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineState.Init(psoDesc);
}
