/*****************************************************************//**
 * \file   Matrix.h
 * \brief  行列クラス
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/
#pragma once

//=============================================================================
// 行列クラス
//=============================================================================
class Matrix {
public:

	union {
		DirectX::XMFLOAT4X4 mat;
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		Vector4 v[4];
		float m[4][4];
	};
	//単位行列
	static const Matrix Identity;
public:

	/*****************************************************************//**
	 * \brief DirectX::XMMATRIX型への暗黙の型変換
	 *
	 * \return 変換結果
	 *********************************************************************/
	operator DirectX::XMMATRIX() const {
		return DirectX::XMLoadFloat4x4(&mat);
	}

	/*****************************************************************//**
	 * \brief コンストラクタ
	 *
	 * \memo 単位行列として初期化
	 *********************************************************************/
	Matrix() {
		mat._11 = 1.0f;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._14 = 0.0f;

		mat._21 = 0.0f;
		mat._22 = 1.0f;
		mat._23 = 0.0f;
		mat._24 = 0.0f;

		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;
		mat._34 = 0.0f;

		mat._41 = 0.0f;
		mat._42 = 0.0f;
		mat._43 = 0.0f;
		mat._44 = 1.0f;
	}
	Matrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) :
		mat(m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{

	}

	/*****************************************************************//**
	 * \brief コンストラクタ
	 * \param m コピー元
	 *********************************************************************/
	Matrix(const DirectX::XMFLOAT4X4& m)
	{
		mat = m;
	}

	/*****************************************************************//**
	 * \brief 代入演算子
	 * \param _m
	 * \return
	 *********************************************************************/
	Matrix& operator=(const Matrix& _m) {
		mat = _m.mat;
		return *this;
	}

	/*****************************************************************//**
	 * \brief ベクトルと3x3行列を乗算
	 *
	 * \param vOut 乗算されるベクトル
	 *********************************************************************/
	void Apply3x3(Vector3& vOut) const {
		Vector3 vTmp = vOut;
		vOut.x = vTmp.x * m[0][0] + vTmp.y * m[1][0] + vTmp.z * m[2][0];
		vOut.y = vTmp.x * m[0][1] + vTmp.y * m[1][1] + vTmp.z * m[2][1];
		vOut.z = vTmp.x * m[0][2] + vTmp.y * m[1][2] + vTmp.z * m[2][2];
	}


	/*****************************************************************//**
	 * \brief ベクトルに行列を乗算
	 *
	 * \param vOut 乗算されるベクトル
	 *********************************************************************/
	void Apply(Vector3& vOut) const {
		DirectX::XMStoreFloat3(
			&vOut.vec,
			DirectX::XMVector3Transform(vOut, *this)
		);
	}

	/*****************************************************************//**
	 * \brief ベクトルに行列を乗算
	 *
	 * \param vOut 乗算されるベクトル
	 *********************************************************************/
	void Apply(Vector4& vOut) const
	{
		DirectX::XMStoreFloat4(
			&vOut.vec,
			DirectX::XMVector4Transform(vOut, *this)
		);
	}

	/*****************************************************************//**
	 * \brief 平行移動行列を作成
	 *
	 * \param trans 移動量
	 *********************************************************************/
	void MakeTranslation(const Vector3& trans) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixTranslationFromVector(trans)
		);
	}
	void MakeTranslation(float x, float y, float z) {
		MakeTranslation(Vector3(x, y, z));
	}

	/*****************************************************************//**
	 * \brief Y軸周りの回転行列を作成
	 *
	 * \param angle 角度(radian)
	 *********************************************************************/
	void MakeRotationY(float angle) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixRotationY(angle)
		);
	}

	/*****************************************************************//**
	 * \brief Z軸周りの回転行列を作成
	 *
	 * \param angle 角度(radian)
	 *********************************************************************/
	void MakeRotationZ(float angle)
	{
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixRotationZ(angle)
		);
	}

	/*****************************************************************//**
	 * \brief X軸周りの回転行列を作成
	 *
	 * \param angle 回転角度
	 *********************************************************************/
	void MakeRotationX(float angle) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixRotationX(angle)
		);
	}

	/*****************************************************************//**
	 * \brief クォータニオンから回転行列を居作成
	 *
	 * \param q クオータニオン
	 *********************************************************************/
	void MakeRotationFromQuaternion(const Quaternion& q) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixRotationQuaternion(q)
		);
	}

	/*****************************************************************//**
	 * \brief 任意の軸周りの回転行列を作成
	 *
	 * \param axis	回転軸
	 * \param angle	回転角度
	 *********************************************************************/
	void MakeRotationAxis(const Vector3& axis, float angle) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixRotationAxis(axis, angle)
		);
	}

	/*****************************************************************//**
	 * \brief 拡大行列を作成
	 *
	 * \param scale 拡大率
	 *********************************************************************/
	void MakeScaling(const Vector3& scale) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixScalingFromVector(scale)
		);
	}

	/*****************************************************************//**
	 * \brief プロジェクション変換行列を作成
	 *
	 * \param viewAngle	画角
	 * \param aspect	アスペクト比
	 * \param fNear		ニアクリップ
	 * \param fFar		ファークリップ
	 *********************************************************************/
	void MakeProjectionMatrix(
		float viewAngle,
		float aspect,
		float fNear,
		float fFar
	) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixPerspectiveFovLH(viewAngle, aspect, fNear, fFar)
		);
	}

	/*****************************************************************//**
	 * \brief プロジェクション変換行列を作成
	 *
	 * \param w		ビューボリュームの幅
	 * \param h		ビューボリュームの高さ
	 * \param fNear	ニアクリップ
	 * \param fFar	ファークリップ
	 *********************************************************************/
	void MakeOrthoProjectionMatrix(float w, float h, float fNear, float fFar) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixOrthographicLH(w, h, fNear, fFar)
		);
	}

	/*****************************************************************//**
	 * \brief カメラ行列を作成
	 *
	 * \param position	カメラ視点
	 * \param target	注視点
	 * \param up		カメラの上方向ベクトル
	 *********************************************************************/
	void MakeLookAt(const Vector3& position, const Vector3& target, const Vector3& up) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixLookAtLH(position, target, up)
		);
	}

	/*****************************************************************//**
	 * \brief 行列同士の乗算
	 *
	 * \param m0
	 * \param m1
	 *********************************************************************/
	void Multiply(const Matrix& m0, const Matrix& m1) {
		DirectX::XMFLOAT4X4 lm;
		DirectX::XMStoreFloat4x4(
			&lm,
			DirectX::XMMatrixMultiply(m0, m1)
		);
		mat = lm;
	}

	/*****************************************************************//**
	 * \brief 代入演算子
	 * \param _m
	 * \return
	 *********************************************************************/
	const Matrix& operator*=(const Matrix& _m) {
		Multiply(*this, _m);
		return *this;
	}

	/*****************************************************************//**
	 * \brief 逆行列を計算
	 *
	 * \param _m 元の行列
	 *********************************************************************/
	void Inverse(const Matrix& _m) {
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixInverse(NULL, _m)
		);
	}

	/*****************************************************************//**
	 * \brief 自身の逆行列を作成
	 *********************************************************************/
	void Inverse() {
		Inverse(*this);
	}
	
	/*****************************************************************//**
	 * \brief 転置行列を計算
	 *********************************************************************/
	void Transpose(){
		DirectX::XMStoreFloat4x4(
			&mat,
			DirectX::XMMatrixTranspose(*this)
		);
	}

};

/*****************************************************************//**
 * \brief 乗算
 * \param m1
 * \param m2
 * \return 
 *********************************************************************/
static inline Matrix operator*(const Matrix& m1, const Matrix m2){
	Matrix mRet;
	mRet.Multiply(m1, m2);
	return mRet;
}

// 単位行列
const Matrix g_matIdentity = {
		1.0f, 0.0f, 0.0f, 0.0f ,
		0.0f, 1.0f, 0.0f, 0.0f ,
		0.0f, 0.0f, 1.0f, 0.0f ,
		0.0f, 0.0f, 0.0f, 1.0f
};

