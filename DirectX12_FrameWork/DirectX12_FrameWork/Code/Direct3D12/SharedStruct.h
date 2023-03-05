#pragma once

//=============================================================================
// unnamed namespace
//=============================================================================
namespace {

	//-----------------------------------------------------------------------------
	// SRGBフォーマットに変換
	//-----------------------------------------------------------------------------
	DXGI_FORMAT ConvertToSRGB(DXGI_FORMAT format)
	{
		DXGI_FORMAT result = format;
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		{ result = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC1_UNORM:
		{ result = DXGI_FORMAT_BC1_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC2_UNORM:
		{ result = DXGI_FORMAT_BC2_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC3_UNORM:
		{ result = DXGI_FORMAT_BC3_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
		{ result = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
		{ result = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; }
		break;

		case DXGI_FORMAT_BC7_UNORM:
		{ result = DXGI_FORMAT_BC7_UNORM_SRGB; }
		break;

		default:
			break;
		}

		return result;
	}

} // namespace 

//=============================================================================
// 変換行列
//=============================================================================
struct alignas(256) Transform{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;	 // ビュー行列
	DirectX::XMMATRIX Proj;	 // 投影行列
};

//=============================================================================
// メッシュ用頂点クラス
//=============================================================================
class MeshVertex {
public:
	DirectX::XMFLOAT3   Position;
	DirectX::XMFLOAT3   Normal;
	DirectX::XMFLOAT2   TexCoord;
	DirectX::XMFLOAT3   Tangent;
	
	MeshVertex() = default;
	
	MeshVertex(
		DirectX::XMFLOAT3 const& position,
		DirectX::XMFLOAT3 const& normal,
		DirectX::XMFLOAT2 const& texcoord,
		DirectX::XMFLOAT3 const& tangent)
		: Position(position)
		, Normal(normal)
		, TexCoord(texcoord)
		, Tangent(tangent)
	{ /* DO_NOTHING */
	}
	
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	
private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

//=============================================================================
// メッシュリソース
//=============================================================================
struct ResMesh {
	std::vector<MeshVertex> Vertices;   // 頂点データ
	std::vector<uint32_t> Indices;		// インデックスデータ
	uint32_t matetialID;				// マテリアル番号
};

//=============================================================================
// スプライト用頂点クラス
//=============================================================================
class SpriteVertex {
public:
	DirectX::XMFLOAT3   Position;
	DirectX::XMFLOAT2   TexCoord;

	SpriteVertex() = default;

	SpriteVertex(
		DirectX::XMFLOAT3 const& position,
		DirectX::XMFLOAT2 const& texcoord)
		: Position(position)
		, TexCoord(texcoord)
	{ /* DO_NOTHING */}

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};


//=============================================================================
// スプライトリソース
//=============================================================================
struct ResSprite {
	std::vector<SpriteVertex> Vertices;
	std::vector<uint32_t> Indices;
};

//=============================================================================
// マテリアルリソース
//=============================================================================
struct ResMaterial{
	DirectX::XMFLOAT3   Diffuse;			// 拡散反射成分
	DirectX::XMFLOAT3   Specular;			// 鏡面反射成分
	float				Alpha;				// 透過成分
	float				Shininess;			// 鏡面反射強度
	std::wstring		DiffuseMapPath;		// ディフューズマップファイルパス
	std::wstring		SpecularMapPath;	// スペキュラーマップファイルパス
	std::wstring		ShininessMapPath;   // シャイネスマップファイルパス
	std::wstring		NormalMapPath;		// 法線マップファイルパス
};


struct ResMaterialPBR {
	float Matallic = 0.f;
	float Roughness = 0.f;

	std::wstring AlbedMapFileName = L"";		// ディフューズマップファイルパス
	std::wstring MatalnessMapFileName = L"";	// メタルネスマップファイルパス
	std::wstring RoughnessMapFileName = L"";	// ラフネスマップファイルパス
	std::wstring NormalMapFileName = L"";		// 法線マップファイルパス
	std::wstring HeightMapFileName = L"";		// ハイトマップパス	
}; 
