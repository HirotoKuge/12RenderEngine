/*****************************************************************//**
 * \file   stdafx.h
 * \brief  プリコンパイル済ヘッダー
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#pragma comment( lib, "xinput.lib")
#pragma comment( lib, "d3d12.lib") 
#pragma comment( lib, "dxgi.lib") 
#pragma comment(lib, "dxcompiler.lib")
#pragma comment( lib, "DirectXTK12.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3dcompiler.lib" )


#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <vector>
#include <list>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d12.h>
#include <map>
#include <functional>
#include <algorithm>
#include <DirectXMath.h>
#include <Xinput.h>
#include <array>
#include <dxcapi.h>
#include <atlbase.h>
#include <dxgi.h>
#include <dxgitype.h>
#include <dxgiformat.h>

#include "../DirectXTK/Inc/CommonStates.h"
#include "../DirectXTK/Inc/SpriteBatch.h"
#include "../DirectXTK/Inc/SpriteFont.h"

#include "d3dx12.h"

const int MAX_RENDERING_TARGET = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;	//レンダリングターゲットの最大数

using namespace Microsoft::WRL;

#include "Util/math.h"
#include "Util/Vector.h"
#include "Util/Matrix.h"
#include "Util/XMFLOATUtil.h"
#include "Util/FileUtil.h"
#include "Util/Logger.h"

#include "Direct3D12/MDLoader.h"

#include "Direct3D12/VertexBuffer.h"
#include "Direct3D12/IndexBuffer.h"
#include "Direct3D12/ShaderResourse.h"
#include "Direct3D12/UnorderAccessResrouce.h"
#include "Direct3D12/GPUBuffer.h"
#include "Direct3D12/Texture.h"
#include "Direct3D12/PipelineState.h"
#include "Direct3D12/ConstantBuffer.h"
#include "Direct3D12/RootSignature.h"
#include "Direct3D12/RootSignature.h"
#include "Direct3D12/GraphicsEngine.h"
#include "Direct3D12/DescriptorHeap.h"
#include "Direct3D12/RenderTarget.h"
#include "Direct3D12/SharedStruct.h"
#include "Direct3D12/Shader.h"

#include "Direct3D12/RenderContext_inline.h"
#include "Direct3D12/DescriptorHeap_inline.h"

#include "Direct3D12/Sprite.h"
#include "Direct3D12/Model.h"


const UINT FRAME_BUFFER_W = 1280;	//フレームバッファの幅
const UINT FRAME_BUFFER_H = 720;	//フレームバッファの高さ
