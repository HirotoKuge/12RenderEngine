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
#include "../Direct3D12/TrianglePolygon.h"


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

// �֐��錾
void InitRootSignature(RootSignature& rs);


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

	// �萔�o�b�t�@���쐬
	ConstantBuffer cb;
	cb.Init(sizeof(Matrix));

	// �O�p�`�|���S�����`
	TrianglePolygon triangle;
	triangle.Init(rootSignature);

	// �O�p�`�|���S����UV���W��ݒ�
	triangle.SetUVCoord(
		0,
		0.0f,
		1.0f
	);

	triangle.SetUVCoord(
		1,
		0.5f,
		0.0f
	);

	triangle.SetUVCoord(
		2,
		1.0f,
		1.0f
	);

	// �e�N�X�`�������[�h
	Texture tex;
	tex.InitFromDDSFile(L"assets/image/default.DDS");

	// �f�B�X�N���v�^�q�[�v���쐬
	DescriptorHeap ds;
	ds.RegistConstantBuffer(0, cb); // �f�B�X�N���v�^�q�[�v�ɒ萔�o�b�t�@��o�^

	// �e�N�X�`�����f�B�X�N���v�^�q�[�v�ɓo�^
	ds.RegistShaderResource(0, tex);
		
	ds.Commit(); //�f�B�X�N���v�^�q�[�v�ւ̓o�^���m��
	
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

		// ���[���h�s����쐬
		Matrix mWorld;

		// ���[���h�s����O���t�B�b�N�������ɃR�s�[
		cb.CopyToVRAM(mWorld);

		//�f�B�X�N���v�^�q�[�v��ݒ�
		renderContext.SetDescriptorHeap(ds);

		//�O�p�`���h���[
		triangle.Draw(renderContext);
		
		GraphicsEngine::GetInstance()->EndRender();

		//!} 

	}

	//�Q�[���̏I������

	int a = 0;

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
void InitRootSignature(RootSignature& rs){
	rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}
