/*****************************************************************//**
 * \file   UnorderAccessResrouce.h
 * \brief  アンオーダードアクセスリソース
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once
#include "ShaderResourse.h"


class UnorderAccessResrouce : public ShaderResource {
public:
	virtual ~UnorderAccessResrouce() {};
	
	/*****************************************************************//**
	 * \brief UAVに登録
	 *  
	 * \param CPUディスクリプタハンドル
	 * \param bufferNo バッファ番号
	 *********************************************************************/
	virtual void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE, int bufferNo) = 0;
};
