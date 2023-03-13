/*****************************************************************//**
 * \file   Vector.h
 * \brief  ベクタークラス
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Includes
//=============================================================================
#include <DirectXMath.h>
#include "math.h"

//=============================================================================
// Forword Decolations
//=============================================================================
class Matrix;


//=============================================================================
// 二次元ベクトルクラス
//=============================================================================
class Vector2 {
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
	static const Vector2 Zero;
	
	/*****************************************************************//**
	 * \brief 無名共用体
	 *********************************************************************/
	union {
		DirectX::XMFLOAT2 vec;
		struct { float x, y; };
		float v[2];
	};

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief コンストラクタ
	 * 
	 * \memo  違う型の値を同一視しない  
	 *********************************************************************/
	explicit Vector2(){
		x = 0.0f;
		y = 0.0f;
	}
	
	/*****************************************************************//**
	 * \brief 引数付きコンストラクタ：初期化
	 *  
	 * \param x x要素の初期値 
	 * \param y y要素の初期値 
	 *********************************************************************/
	Vector2(float x, float y){
		this->x = x;
		this->y = y;
	}

	/*****************************************************************//**
	 * \brief 代入演算子
	 *  
	 * \param _v 計算対象
	 * \return 計算結果(自身のポインタ)
	 *********************************************************************/
	Vector2& operator=(const Vector2& _v)
	{
		vec = _v.vec;
		return *this;
	}

	/*****************************************************************//**
	 * \brief 値をセット
	 *  
	 * \param _x x要素
	 * \param _y y要素
	 *********************************************************************/
	void Set(float _x, float _y){
		vec.x = _x;
		vec.y = _y;
	}
	
	/*****************************************************************//**
	 * \brief 線形補完
	 *  
	 * \param t	 補間率
	 * \param v0 補間開始のベクトル
	 * \param v1 補間終了のベクトル
	 *********************************************************************/
	void Lerp(float t, const Vector2& v0, const Vector2& v1){
		x = v0.x + (v1.x - v0.x) * t;
		y = v0.y + (v1.y - v0.y) * t;
	}
	
	/*****************************************************************//**
	 * \brief 正規化
	 *********************************************************************/
	void Normalize(){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat2(&vec);
		xmv = DirectX::XMVector2Normalize(xmv);
		DirectX::XMStoreFloat2(&vec, xmv);
	}
	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
};


//=============================================================================
/// 3次元ベクトルクラス 
//=============================================================================
class Vector3 {
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
	//-----------------------------------------------------------------------------	
	//!{
			/* Nothing */
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
public:
	union {
		DirectX::XMFLOAT3 vec;
		float v[3];
		struct { float x, y, z; };
	};
	static const Vector3 Zero;
	static const Vector3 Right;
	static const Vector3 Left;
	static const Vector3 Up;
	static const Vector3 Down;
	static const Vector3 Front;
	static const Vector3 Back;
	static const Vector3 AxisX;
	static const Vector3 AxisY;
	static const Vector3 AxisZ;
	static const Vector3 One;
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
public:
	/*****************************************************************//**
	 * \brief XMVECTORへの暗黙の変換
	 *********************************************************************/
	operator DirectX::XMVECTOR() const {
		return DirectX::XMLoadFloat3(&vec);
	}

	/*****************************************************************//**
	 * \brief 代入演算子
	 *********************************************************************/
	Vector3& operator=(const Vector3& _v)
	{
		vec = _v.vec;
		return *this;
	}

	/*****************************************************************//**
	 * \brief コンストラクタ
	 *
	 * \memo x,y,z全ての要素を0で初期化
	 *********************************************************************/
	explicit Vector3() {
		x = y = z = 0.0f;
	}
	
	/*****************************************************************//**
	 * \brief コンストラクタ
	 * \param x 初期化値	
	 * \param y 初期化値
	 * \param z 初期化値
	 *********************************************************************/
	Vector3(float x, float y, float z){
		Set(x, y, z);
	}
	
	/*****************************************************************//**
	 * \brief 線形補完
	 * \param t  補完率
	 * \param v0 補完開始ベクトル
	 * \param v1 補完終了ベクトル
	 *********************************************************************/
	void Lerp(float t, const Vector3& v0, const Vector3& v1){
		DirectX::XMVECTOR _v = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&v0.vec),
			DirectX::XMLoadFloat3(&v1.vec),
			t);
		DirectX::XMStoreFloat3(&vec, _v);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルのコピー
	 * \param dst コピー元
	 *********************************************************************/
	template<class TVector>
	void CopyTo(TVector& dst) const{
		dst.x = x;
		dst.y = y;
		dst.z = z;
	}

	/*****************************************************************//**
	 * \brief ベクトルの設定
	 * \param _x
	 * \param _y
	 * \param _z
	 *********************************************************************/
	void Set(float _x, float _y, float _z){
		vec.x = _x;
		vec.y = _y;
		vec.z = _z;
	}
	template<class TVector>
	void Set(TVector& _v){
		Set(_v.x, _v.y, _v.z);
	}

	
	
	/*****************************************************************//**
	 * \brief ベクトルの加算
	 * 
	 * \param v1 加算されるベクトル
	 *********************************************************************/
	void Add(const Vector3& _v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの加算
	 * 
	 * \param v0 加算するベクトル
	 * \param v1 加算されるベクトル
	 *********************************************************************/
	void Add(const Vector3& v0, const Vector3& v1){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの減算
	 * 
	 * \param _v 減算されるベクトル
	 *********************************************************************/
	void Subtract(const Vector3& _v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの減算
	 * \param v0 引かれるベクトル
	 * \param v1 引くベクトル
	 *********************************************************************/
	void Subtract(const Vector3& v0, const Vector3& v1){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}

	
	/*****************************************************************//**
	 * \brief 内積を計算
	 * 
	 * \param _v
	 * \return 
	 *********************************************************************/
	float Dot(const Vector3& _v) const{
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
		return DirectX::XMVector3Dot(xmv0, xmv1).m128_f32[0];
	}

	/// <summary>
	/// 外積
	/// </summary>
	/// <remarks>
	/// Vector3 v ;
	/// v.x = this->y * _v.z - this->z * _v.y;
	/// v.y = thiz->z * _v.x - this->x * _v.z;
	/// v.z = this->x * _v.y - this->y * _v.x;
	/// this = v;
	/// </remarks>
	
	/*****************************************************************//**
	 * \brief 外積を計算
	 * 
	 * \param _v
	 *********************************************************************/
	void Cross(const Vector3& _v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}
	/// <summary>
	/// 外積
	/// </summary>
	/// <remarks>
	/// this->x = v0.y * v1.z - v0.z * v1.y;
	/// this->y = v0.z * v1.x - v0.x * v1.z;
	/// this->z = v0.x * v1.y - v0.y * v1.x; 
	/// </remarks>
	
	/*****************************************************************//**
	 * \brief 外積を計算
	 * 
	 * \param v0
	 * \param v1
	 *********************************************************************/
	void Cross(const Vector3& v0, const Vector3& v1){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
		DirectX::XMStoreFloat3(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの長さを取得
	 * 
	 * \return 
	 *********************************************************************/
	float Length() const{
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
		return DirectX::XMVector3Length(xmv).m128_f32[0];
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの長さの２乗を取得
	 * \return 
	 *********************************************************************/
	float LengthSq() const{
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
		return DirectX::XMVector3LengthSq(xmv).m128_f32[0];
	}


	/*****************************************************************//**
	 * \brief ベクトルをスカラーで拡大
	 * 
	 * \param s
	 * \memo sを各要素にかけてる
	 *********************************************************************/
	void Scale(float s){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
		xmv = DirectX::XMVectorScale(xmv, s);
		DirectX::XMStoreFloat3(&vec, xmv);
	}

	/*****************************************************************//**
	 * \brief ベクトルを正規化
	 *********************************************************************/
	void Normalize(){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
		xmv = DirectX::XMVector3Normalize(xmv);
		DirectX::XMStoreFloat3(&vec, xmv);
	}

	/*****************************************************************//**
	 * \brief ベクトルをスカラーで除算
	 * 
	 * \param d
	 * \memo dで各要素を割ってる
	 *********************************************************************/
	void Div(float d){
		float scale = 1.0f / d;
		Scale(scale);
	}

	
	/*****************************************************************//**
	 * \brief 最大値を設定
	 * 
	 * \param v 要素ごとに最大値が入ったVector3
	 * \memo 内部では要素nについてstd::max(v.n,this->n)の処理を行っている
	 *********************************************************************/
	void Max(const Vector3& v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v.vec);
		DirectX::XMStoreFloat3(&vec, DirectX::XMVectorMax(xmv0, xmv1));
	}

	/*****************************************************************//**
	 * \brief 最小値を設定
	 *
	 * \param v 要素ごとに最小値が入ったVector3
	 * \memo 内部では要素nについてstd::min(v.n,this->n)の処理を行っている
	 *********************************************************************/
	void Min(const Vector3& v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v.vec);
		DirectX::XMStoreFloat3(&vec, DirectX::XMVectorMin(xmv0, xmv1));
	}

	/*****************************************************************//**
	 * \brief 加算代入演算子
	 * 
	 * \param _v 加算値
	 * \return 
	 *********************************************************************/
	const Vector3& operator+=(const Vector3& _v){
		Add(_v);
		return *this;
	}
	
	/*****************************************************************//**
	 * \brief 乗算代入演算子
	 * 
	 * \param s 倍率
	 * \return 
	 *********************************************************************/
	const Vector3& operator*=(float s){
		Scale(s);
		return *this;
	}
	
	/*****************************************************************//**
	 * \brief 減算代入演算子
	 * \param _v 減算値
	 * \return 
	 *********************************************************************/
	const Vector3& operator-=(const Vector3& _v){
		Subtract(_v);
		return *this;
	}
	
	/*****************************************************************//**
	 * \brief 除算代入演算子
	 * 
	 * \param s 割る値
	 * \return 
	 *********************************************************************/
	const Vector3& operator/=(const float s){
		Div(s);
		return *this;
	}
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
			/* Nothing */
	//!} 
};


//=============================================================================
// 4次元ベクトル
//=============================================================================
class Vector4 {
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
	//-----------------------------------------------------------------------------	
	//!{
			/* Nothing */
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{

	union {
		DirectX::XMFLOAT4 vec;
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float v[4];
	};
	static const Vector4 White;
	static const Vector4 Black;
	static const Vector4 Yellow;
	static const Vector4 Gray;

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
public:
	
	/*****************************************************************//**
	 * \brief XMVECTOR型への暗黙の型変換
	 *********************************************************************/
	operator DirectX::XMVECTOR() const{
		return DirectX::XMLoadFloat4(&vec);
	}
	
	/*****************************************************************//**
	 * \brief 初期化
	 *********************************************************************/
	explicit Vector4(){
		x = y = z = 0.0f;
		w = 1.0f;
	}
	
	
	/*****************************************************************//**
	 * \brief コピー
	 * 
	 * \param dst	
	 *********************************************************************/
	template<class TVector4>
	void  CopyTo(TVector4& dst) const{
		dst.x = x;
		dst.y = y;
		dst.z = z;
		dst.w = w;
	}

	/*****************************************************************//**
	 * \brief 代入演算子
	 *********************************************************************/
	Vector4& operator=(const Vector4& _v){
		vec = _v.vec;
		return *this;
	}

	/*****************************************************************//**
	 * \brief コンストラクタ
	 * 
	 * \param x X成分
	 * \param y	Y成分
	 * \param z	Z成分
	 * \param w	W成分
	 *********************************************************************/
	Vector4(float x, float y, float z, float w){
		Set(x, y, z, w);
	}
	

	/*****************************************************************//**
	 * \brief 引数付きコンストラクタ
	 * 
	 * \param v 3次元ベクトル
	 * \memo wには1.0を格納
	 *********************************************************************/
	Vector4(const Vector3& v){
		Set(v);
	}

	/*****************************************************************//**
	 * \brief ベクトルの各要素を設定
	 * \param _x X成分
	 * \param _y Y成分
	 * \param _z Z成分
	 * \param _w W成分
	 *********************************************************************/
	void Set(float _x, float _y, float _z, float _w){
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;
	}
	
	/*****************************************************************//**
	 * \brief ベクトルを正規化
	 *********************************************************************/
	void Normalize(){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
		xmv = DirectX::XMVector4Normalize(xmv);
		DirectX::XMStoreFloat4(&vec, xmv);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルを設定
	 * 
	 * \param _v 設定元
	 *********************************************************************/
	void Set(const Vector4& _v){
		*this = _v;
	}

	/*****************************************************************//**
	 * \brief 3次元ベクトルを設定
	 * 
	 * \param _v
	 * \memo W成分には1.0が設定されます
	 *********************************************************************/
	void Set(const Vector3& _v){
		this->x = _v.x;
		this->y = _v.y;
		this->z = _v.z;
		this->w = 1.0f;
	}
	
	/*****************************************************************//**
	 * \brief ベクトルを加算
	 * 
	 * \param _v
	 *********************************************************************/
	void Add(const Vector4& _v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
		DirectX::XMStoreFloat4(&vec, xmvr);
	}

	/*****************************************************************//**
	 * \brief ベクトルを加算
	 * 
	 * \param v0
	 * \param v1
	 * \memo this = v0+v1
	 *********************************************************************/
	void Add(const Vector4& v0, const Vector4& v1){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
		DirectX::XMStoreFloat4(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの減算
	 * 
	 * \param _v
	 *********************************************************************/
	void Subtract(const Vector4& _v){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
		DirectX::XMStoreFloat4(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの減算
	 * 
	 * \param v0
	 * \param v1
	 * \memo this = v0 - v1
	 *********************************************************************/
	void Subtract(const Vector4& v0, const Vector4& v1){
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
		DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
		DirectX::XMStoreFloat4(&vec, xmvr);
	}
	
	/*****************************************************************//**
	 * \brief 内積を計算
	 * 
	 * \param _v
	 * \return 内積
	 *********************************************************************/
	float Dot(const Vector4& _v) const{
		DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
		DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
		return DirectX::XMVector4Dot(xmv0, xmv1).m128_f32[0];
	}
	
	/*****************************************************************//**
	 * \brief ベクトルの長さを取得
	 * 
	 * \return 長さ
	 *********************************************************************/
	float Length(){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
		return DirectX::XMVector4Length(xmv).m128_f32[0];
	}

	/*****************************************************************//**
	 * \brief ベクトルの長さの２乗を取得
	 * 
	 * \return 長さの２乗
	 *********************************************************************/
	float LengthSq(){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
		return DirectX::XMVector4LengthSq(xmv).m128_f32[0];
	}
	
	/*****************************************************************//**
	 * \brief ベクトルをスカラーで拡大
	 * 
	 * \param s 拡大率
	 *********************************************************************/
	void Scale(float s){
		DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
		xmv = DirectX::XMVectorScale(xmv, s);
		DirectX::XMStoreFloat4(&vec, xmv);
	}
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	//!} 
};

//=============================================================================
// 定数
//=============================================================================
const Vector2 g_vec2Zero = { 0.0f, 0.0f };
const Vector3 g_vec3Zero = { 0.0f,  0.0f,  0.0f };
const Vector3 g_vec3Right = { 1.0f,  0.0f,  0.0f };
const Vector3 g_vec3Left = { -1.0f,  0.0f,  0.0f };
const Vector3 g_vec3Up = { 0.0f,  1.0f,  0.0f };
const Vector3 g_vec3Down = { 0.0f, -1.0f,  0.0f };
const Vector3 g_vec3Front = { 0.0f,   0.0f,  1.0f };
const Vector3 g_vec3Back = { 0.0f,   0.0f, -1.0f };
const Vector3 g_vec3AxisX = { 1.0f,  0.0f,  0.0f };
const Vector3 g_vec3AxisY = { 0.0f,  1.0f,  0.0f };
const Vector3 g_vec3AxisZ = { 0.0f,  0.0f,  1.0f };
const Vector3 g_vec3One = { 1.0f, 1.0f, 1.0f };

const Vector4 g_vec4White = { 1.0f, 1.0f, 1.0f, 1.0f };
const Vector4 g_vec4Black = { 0.0f, 0.0f, 0.0f, 1.0f };
const Vector4 g_vec4Gray = { 0.5f, 0.5f, 0.5f, 1.0f };
const Vector4 g_vec4Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

//=============================================================================
//　クォータニオン 
//=============================================================================
class Quaternion : public Vector4 {
public:
	static const Quaternion Identity; // 単位クォータニオン
	Quaternion()
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}
	
	/*****************************************************************//**
	 * \brief コンストラクタ
	 * \param x X成分　
	 * \param y	Y成分
	 * \param z	Z成分
	 * \param w	W成分
	 *********************************************************************/
	Quaternion(float x, float y, float z, float w) :
		Vector4(x, y, z, w)
	{}
	
	/*****************************************************************//**
	 * \brief X軸周りの回転クォータニオンを作成
	 * 
	 * \param angle 角度(Radian)
	 *********************************************************************/
	void SetRotationX(float angle){
		SetRotation(g_vec3AxisX, angle);
	}
	
	/*****************************************************************//**
	 * \brief X軸周りの回転クォータニオンを作成
	 * 
	 * \param angle 角度(Degree)
	 *********************************************************************/
	void SetRotationDegX(float angle){
		SetRotationDeg(g_vec3AxisX, angle);
	}

	/*****************************************************************//**
	 * \brief Y軸周りの回転クォータニオンを作成
	 *
	 * \param angle 角度(Radian)
	 *********************************************************************/
	void SetRotationY(float angle){
		SetRotation(g_vec3AxisY, angle);
	}

	/*****************************************************************//**
	 * \brief Y軸周りの回転クォータニオンを作成
	 *
	 * \param angle 角度(Degree)
	 *********************************************************************/
	void SetRotationDegY(float angle){
		SetRotationDeg(g_vec3AxisY, angle);
	}

	/*****************************************************************//**
	 * \brief Z軸周りの回転クォータニオンを作成
	 *
	 * \param angle 角度(Radian)
	 *********************************************************************/
	void SetRotationZ(float angle){
		SetRotation(g_vec3AxisZ, angle);
	}

	/*****************************************************************//**
	 * \brief Z軸周りの回転クォータニオンを作成
	 *
	 * \param angle 角度(Radian)
	 *********************************************************************/
	void SetRotationDegZ(float angle){
		SetRotationDeg(g_vec3AxisZ, angle);
	}

	/*****************************************************************//**
	 * \brief 任意の軸周りの回転クォータニオンを作成
	 * 
	 * \param axis 回転軸
	 * \param angle 回転角度(Radian)
	 *********************************************************************/
	void SetRotation(const Vector3& axis, float angle){
		float s;
		float halfAngle = angle * 0.5f;
		s = sinf(halfAngle);
		w = cosf(halfAngle);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}

	/*****************************************************************//**
	 * \brief 任意の軸周りの回転クォータニオンを作成
	 * 
	 * \param axis	回転軸
	 * \param angle	回転角度(Degree)
	 *********************************************************************/
	void SetRotationDeg(const Vector3& axis, float angle){
		float s;
		float halfAngle = ToRad(angle) * 0.5f;
		s = sinf(halfAngle);
		w = cosf(halfAngle);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}
	
	/*****************************************************************//**
	 * \brief 行列からクォータニオンを作成
	 * 
	 * \param m
	 *********************************************************************/
	void SetRotation(const Matrix& m);
	
	/*****************************************************************//**
	 * \brief FromベクトルからToベクトルへ回転させる回転クォータニオンを作成
	 * 
	 * \param from
	 * \param to
	 *********************************************************************/
	void SetRotation(Vector3 from, Vector3 to);
	
	/*****************************************************************//**
	 * \brief 球面線形保管
	 * 
	 * \param t	 補完率
	 * \param q1 開始
	 * \param q2 終了
	 *********************************************************************/
	void Slerp(float t, Quaternion q1, Quaternion q2){
		DirectX::XMVECTOR xmv = DirectX::XMQuaternionSlerp(
			DirectX::XMLoadFloat4(&q1.vec),
			DirectX::XMLoadFloat4(&q2.vec),
			t
		);
		DirectX::XMStoreFloat4(&vec, xmv);
	}
	
	/*****************************************************************//**
	 * \brief Y軸での回転を加算
	 * 
	 * \param angle 加算する回転角度(Radian)
	 *********************************************************************/
	void AddRotationY(float angle){
		Quaternion addRot;
		addRot.SetRotation(Vector3::AxisY, angle);
		*this *= addRot;
	}
	
	/*****************************************************************//**
	 * \brief クォータニオン同士の乗算
	 * 
	 * \param rot 
	 * \memo this*=rot
	 *********************************************************************/
	void Multiply(const Quaternion& rot){
		float pw, px, py, pz;
		float qw, qx, qy, qz;

		qw = w; qx = x; qy = y; qz = z;
		pw = rot.w; px = rot.x; py = rot.y; pz = rot.z;

		w = pw * qw - px * qx - py * qy - pz * qz;
		x = pw * qx + px * qw + py * qz - pz * qy;
		y = pw * qy - px * qz + py * qw + pz * qx;
		z = pw * qz + px * qy - py * qx + pz * qw;

	}
	
	/*****************************************************************//**
	 * \brief クォータニオン同士の乗算
	 * 
	 * \param rot0
	 * \param rot1
	 * \memo this = rot0 * rot1
	 *********************************************************************/
	void Multiply(const Quaternion& rot0, const Quaternion& rot1){
		float pw, px, py, pz;
		float qw, qx, qy, qz;

		qw = rot0.w; qx = rot0.x; qy = rot0.y; qz = rot0.z;
		pw = rot1.w; px = rot1.x; py = rot1.y; pz = rot1.z;

		w = pw * qw - px * qx - py * qy - pz * qz;
		x = pw * qx + px * qw + py * qz - pz * qy;
		y = pw * qy - px * qz + py * qw + pz * qx;
		z = pw * qz + px * qy - py * qx + pz * qw;
	}
	
	/*****************************************************************//**
	 * \brief クオータニオンの代入演算子
	 * 
	 * \param rot0
	 * \return 
	 *********************************************************************/
	const Quaternion& operator*=(const Quaternion& rot0){
		Multiply(rot0, *this);
		return *this;
	}
	
	/*****************************************************************//**
	 * \brief ベクトルにクォータニオンを適応する
	 * \param _v
	 *********************************************************************/
	void Apply(Vector4& _v) const{
		DirectX::XMVECTOR xmv = DirectX::XMVector3Rotate(_v, *this);
		DirectX::XMStoreFloat4(&_v.vec, xmv);
	}

	/*****************************************************************//**
	 * \brief ベクトルにクォータニオンを適応する
	 * \param _v
	 *********************************************************************/
	void Apply(Vector3& _v) const{
		DirectX::XMVECTOR xmv = DirectX::XMVector3Rotate(_v, *this);
		DirectX::XMStoreFloat3(&_v.vec, xmv);
	}
};

//　整数型のベクトルクラス
__declspec(align(16)) class Vector4i {
public:
	union {
		struct { int x, y, z, w; };
		int v[4];
	};
};


/*****************************************************************//**
 * \brief ベクトル同士の加算
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline Vector3 operator+(const Vector3& v0, const Vector3& v1){
	Vector3 result;
	result.Add(v0, v1);
	return result;
}

/*****************************************************************//**
 * \brief ベクトルとスカラーの乗算
 * \param v
 * \param s
 * \return 
 *********************************************************************/
static inline Vector3 operator*(const Vector3& v, float s){
	Vector3 result;
	result = v;
	result.Scale(s);
	return result;
}

/*****************************************************************//**
 * \brief ベクトルとスカラーの除算
 * 
 * \param v
 * \param s
 * \return 
 *********************************************************************/
static inline Vector3 operator/(const Vector3& v, float s){
	Vector3 result;
	result = v;
	result.Div(s);
	return result;
}

/*****************************************************************//**
 * \brief ベクトル同士の減算
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline Vector3 operator-(const Vector3& v0, const Vector3& v1){
	Vector3 result;
	result.Subtract(v0, v1);
	return result;
}

/*****************************************************************//**
 * \brief 外積を計算
 * 
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline Vector3 Cross(const Vector3& v0, const Vector3& v1){
	Vector3 result;
	result.Cross(v0, v1);
	return result;
}

/*****************************************************************//**
 * \brief 内積を計算
 * 
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline float Dot(const Vector3& v0, const Vector3& v1){
	return v0.Dot(v1);
}

/*****************************************************************//**
 * \brief ベクトル同士の加算
 * 
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline Vector4 operator+(const Vector4& v0, const Vector4& v1){
	Vector4 result;
	result.Add(v0, v1);
	return result;
}

/*****************************************************************//**
 * \brief ベクトルとスカラーの乗算
 * 
 * \param v
 * \param s
 * \return 
 *********************************************************************/
static inline Vector4 operator*(const Vector4& v, float s){
	Vector4 result;
	result = v;
	result.Scale(s);
	return result;
}

/*****************************************************************//**
 * \brief ベクトル同士の減算
 * 
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline Vector4 operator-(const Vector4& v0, const Vector4& v1){
	Vector4 result;
	result.Subtract(v0, v1);
	return result;
}

/*****************************************************************//**
 * \brief 内積を計算
 * 
 * \param v0
 * \param v1
 * \return 
 *********************************************************************/
static inline float Dot(const Vector4& v0, const Vector4& v1){
	return v0.Dot(v1);
}

/*****************************************************************//**
 * \brief クォータニオン同士の乗算
 * 
 * \param q1
 * \param q2
 * \return 
 *********************************************************************/
static inline Quaternion operator*(const Quaternion& q1, const Quaternion q2){
	Quaternion qRet;
	qRet.Multiply(q2, q1);
	return qRet;
}

const Quaternion g_quatIdentity = { 0.0f,  0.0f, 0.0f, 1.0f };
