/*****************************************************************//**
 * \file   ShaderResourse.h
 * \brief  シェーダーリソースの基底クラス
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once
#include <d3dx12.h>

 //=============================================================================
 // ShaderResourse Class
 //=============================================================================
class ShaderResource {
public:
	virtual ~ShaderResource()
	{
	}
	
	/*****************************************************************//**
	 * \brief シェーダーリソースビューに登録
	 *  
	 * \param descriptorHandle CPUディスクリプタハンドル
	 * \param bufferNo		   バッファ番号
	 *********************************************************************/
	virtual void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) = 0;

};