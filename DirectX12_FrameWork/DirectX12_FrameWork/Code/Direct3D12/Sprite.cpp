/*****************************************************************//**
 * \file  Sprite.cpp
 * \brief スプライト
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
// デストラクタ
//=============================================================================
Sprite::~Sprite(){
	
}

//=============================================================================
// 初期化処理
//=============================================================================
void Sprite::Init(const SpriteInitData& initData){
	
	// サイズを格納
	m_size.x = static_cast<float>(initData.width);
	m_size.y = static_cast<float>(initData.height);

	// テクスチャを初期化
	InitTextures(initData);

	// 頂点バッファとインデックスバッファを初期化
	InitVertexBufferAndIndexBuffer(initData);

	// 定数バッファを初期化
	InitConstantBuffer(initData);
	
	// ルートシグネチャを初期化
	m_rootSignature.Init(
		initData.samplerFilter,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	// パイプラインステートを初期化
	InitPipelineState(initData);

	// ディスクリプタヒープを初期化
	InitDescriptorHeap(initData);
}

//=============================================================================
// 更新処理
//=============================================================================
void Sprite::Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot){
	//ピボットを考慮に入れた平行移動行列を作成
	//ピボットは真ん中が0.0, 0.0、左上が-1.0f, -1.0、右下が1.0、1.0になるようにする
	Vector2 localPivot = pivot;
	localPivot.x -= 0.5f;
	localPivot.y -= 0.5f;
	localPivot.x *= -2.0f;
	localPivot.y *= -2.0f;
	//画像のハーフサイズを求める
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
// 描画処理
//=============================================================================
void Sprite::Draw(RenderContext& renderContext){
	
	// カメラを取得
	auto pCamera2D =GraphicsEngine::GetInstance()->GetCamera2D();
	auto pCamera3D =GraphicsEngine::GetInstance()->GetCamera3D();
	
	//現在のビューポートから平行投影行列を計算する 
	D3D12_VIEWPORT viewport = renderContext.GetViewport();

	//TODO:カメラ行列は定数に使用 どうせ変えないので 
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

	//定数バッファを更新 
	//renderContext.UpdateConstantBuffer(m_constantBufferGPU, &m_constantBufferCPU);
	m_constantBufferGPU.CopyToVRAM(&m_constantBufferCPU);
	if (m_userExpandConstantBufferCPU != nullptr) {
		//renderContext.UpdateConstantBuffer(m_userExpandConstantBufferGPU, m_userExpandConstantBufferCPU);
		m_userExpandConstantBufferGPU.CopyToVRAM(m_userExpandConstantBufferCPU);
	}
	//ルートシグネチャを設定 
	renderContext.SetRootSignature(m_rootSignature);
	
	//パイプラインステートを設定 
	renderContext.SetPipelineState(m_pipelineState);
	
	//頂点バッファを設定 
	renderContext.SetVertexBuffer(m_vertexBuffer);
	
	//インデックスバッファを設定 
	renderContext.SetIndexBuffer(m_indexBuffer);
	
	//プリミティブトポロジーを設定する 
	renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//ディスクリプタヒープを設定する 
	renderContext.SetDescriptorHeap(m_descriptorHeap);
	
	//描画
	renderContext.DrawIndexed(m_indexBuffer.GetCount());
}




#pragma region InitMethod
//=============================================================================
// テクスチャの初期化
//=============================================================================
void Sprite::InitTextures(const SpriteInitData& initData) {

	uint32_t texNo = 0;
	// スプライトで使用するテクスチャを準備する
	if (initData.ddsFilePath[0] != nullptr) {
		// ddsファイルのパスが指定されてるのなら、ddsファイルからテクスチャを作成する
		int texNo = 0;
		while (initData.ddsFilePath[texNo] && texNo < MAX_TEXTURE) {
			m_textures[texNo].InitFromDDSFile(initData.ddsFilePath[texNo]);
			texNo++;
		}
		m_numTexture = texNo;
	}
	else if (initData.pTextures[0] != nullptr) {
		// 外部テクスチャ(GBufferなど)を指定されている
		int texNo = 0;
		while (initData.pTextures[texNo] != nullptr) {
			m_pTextureExternals[texNo].reset(initData.pTextures[texNo]);
			texNo++;
		}
		m_numTexture = texNo;
	}
	else {
		//テクスチャが指定されてない 
		MessageBoxA(nullptr, "使用するテクスチャの情報を設定してください", "エラー", MB_OK);
		std::abort();
	}
}

//=============================================================================
// 頂点バッファと定数バッファの初期化
//=============================================================================
void Sprite::InitVertexBufferAndIndexBuffer(const SpriteInitData& initData) {

	float halfW = m_size.x * 0.5f;
	float halfH = m_size.y * 0.5f;

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();

	//　スプライト用のリソース
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

	// 頂点バッファ初期化
	m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]));
	m_vertexBuffer.Copy(vertices);

	// インデックスバッファ初期化
	m_indexBuffer.Init(sizeof(indices), sizeof(indices[0]));
	m_indexBuffer.Copy(indices);
}

//=============================================================================
// パイプラインステートの初期化
//=============================================================================
void Sprite::InitPipelineState(const SpriteInitData& initData) {
	
	// 頂点レイアウトを定義する 
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout						= { inputElementDescs, _countof(inputElementDescs) };
	desc.RasterizerState					= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);	// ラスタライザーはデフォルト
	desc.RasterizerState.CullMode			= D3D12_CULL_MODE_NONE;						// カリングはなし
	
	desc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);		// ブレンドステートもデフォルト
	if (initData.alphaBlendMode == AlphaBlendMode_Trans) {
		//半透明合成のブレンドステートを作成する
		desc.BlendState.RenderTarget[0].BlendEnable = true;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	}
	else if (initData.alphaBlendMode == AlphaBlendMode_Add) {
		//加算合成
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
	desc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// 三角形を描画
	
	// RTVフォーマットを設定
	for (auto& format : initData.colorBufferFormats) {
		if (format == DXGI_FORMAT_UNKNOWN) {
			break;
		}
		desc.RTVFormats[desc.NumRenderTargets] = format;
		desc.NumRenderTargets++;
	}

	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;				// サンプラーは1

	// パイプラインステートオブジェクトの生成
	// TODO:シェーダーの扱い方を考える
	// クラスのが扱いやすそうだなとは思う
	// 後はシェーダーファイルもなんか使いやすいのにしたい
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
// 定数バッファの初期化
//=============================================================================
void Sprite::InitConstantBuffer(const SpriteInitData& initData) {
	//定数バッファの初期化
	m_constantBufferGPU.Init(sizeof(m_constantBufferCPU), nullptr);
	//ユーザー拡張の定数バッファが指定されている
	if (initData.pExpandConstantBuffer != nullptr) {
		m_userExpandConstantBufferCPU = initData.pExpandConstantBuffer;
		m_userExpandConstantBufferGPU.Init(
			initData.expandConstantBufferSize,
			initData.pExpandConstantBuffer
		);
	}
}

//=============================================================================
// ディスクリプタヒープを初期化
//=============================================================================
void Sprite::InitDescriptorHeap(const SpriteInitData& initData) {
	if (m_pTextureExternals[0] != nullptr) {
		//外部のテクスチャが指定されている 
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
		//拡張シェーダーリソースビュー
		m_descriptorHeap.RegistShaderResource(
			EXPAND_SRV_REG_START_NO,
			*initData.pExpandShaderResoruceView
		);
	}
	m_descriptorHeap.RegistConstantBuffer(0, m_constantBufferGPU);
	if (m_userExpandConstantBufferCPU != nullptr) {
		//ユーザー拡張の定数バッファはb1に関連付けする
		m_descriptorHeap.RegistConstantBuffer(1, m_userExpandConstantBufferGPU);
	}
	m_descriptorHeap.Commit();
}

#pragma endregion

