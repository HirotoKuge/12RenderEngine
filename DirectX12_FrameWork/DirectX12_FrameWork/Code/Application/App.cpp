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

Model* pActor;

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
		GraphicsEngine::GetInstance()->BeginRender();
		GraphicsEngine::GetInstance()->EndRender();


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
