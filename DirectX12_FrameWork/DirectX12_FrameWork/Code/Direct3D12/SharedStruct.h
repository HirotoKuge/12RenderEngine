#pragma once

//=============================================================================
// unnamed namespace
//=============================================================================
namespace {

	//-----------------------------------------------------------------------------
	// SRGB�t�H�[�}�b�g�ɕϊ�
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
// �ϊ��s��
//=============================================================================
struct alignas(256) Transform{
	DirectX::XMMATRIX World;	// ���[���h�s��
	DirectX::XMMATRIX View;	 // �r���[�s��
	DirectX::XMMATRIX Proj;	 // ���e�s��
};

//=============================================================================
// ���b�V���p���_�N���X
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
// ���b�V�����\�[�X
//=============================================================================
struct ResMesh {
	std::vector<MeshVertex> Vertices;   // ���_�f�[�^
	std::vector<uint32_t> Indices;		// �C���f�b�N�X�f�[�^
	uint32_t matetialID;				// �}�e���A���ԍ�
};

//=============================================================================
// �X�v���C�g�p���_�N���X
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
// �X�v���C�g���\�[�X
//=============================================================================
struct ResSprite {
	std::vector<SpriteVertex> Vertices;
	std::vector<uint32_t> Indices;
};

//=============================================================================
// �}�e���A�����\�[�X
//=============================================================================
struct ResMaterial{
	DirectX::XMFLOAT3   Diffuse;			// �g�U���ː���
	DirectX::XMFLOAT3   Specular;			// ���ʔ��ː���
	float				Alpha;				// ���ߐ���
	float				Shininess;			// ���ʔ��ˋ��x
	std::wstring		DiffuseMapPath;		// �f�B�t���[�Y�}�b�v�t�@�C���p�X
	std::wstring		SpecularMapPath;	// �X�y�L�����[�}�b�v�t�@�C���p�X
	std::wstring		ShininessMapPath;   // �V���C�l�X�}�b�v�t�@�C���p�X
	std::wstring		NormalMapPath;		// �@���}�b�v�t�@�C���p�X
};


struct ResMaterialPBR {
	float Matallic = 0.f;
	float Roughness = 0.f;

	std::wstring AlbedMapFileName = L"";		// �f�B�t���[�Y�}�b�v�t�@�C���p�X
	std::wstring MatalnessMapFileName = L"";	// ���^���l�X�}�b�v�t�@�C���p�X
	std::wstring RoughnessMapFileName = L"";	// ���t�l�X�}�b�v�t�@�C���p�X
	std::wstring NormalMapFileName = L"";		// �@���}�b�v�t�@�C���p�X
	std::wstring HeightMapFileName = L"";		// �n�C�g�}�b�v�p�X	
}; 
