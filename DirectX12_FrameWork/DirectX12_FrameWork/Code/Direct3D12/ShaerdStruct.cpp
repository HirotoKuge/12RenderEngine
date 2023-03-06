#include "../stdafx.h"
#include "SharedStruct.h"

//=============================================================================
// Constant Values
//=============================================================================
const D3D12_INPUT_ELEMENT_DESC MeshVertex::InputElements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	  0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

const D3D12_INPUT_LAYOUT_DESC MeshVertex::InputLayout = { MeshVertex::InputElements, MeshVertex::InputElementCount };

const D3D12_INPUT_ELEMENT_DESC SpriteVertex::InputElements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }};

const D3D12_INPUT_LAYOUT_DESC SpriteVertex::InputLayout = { SpriteVertex::InputElements, SpriteVertex::InputElementCount };

