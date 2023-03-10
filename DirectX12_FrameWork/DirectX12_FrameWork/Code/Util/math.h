#pragma once
#include<DirectXMath.h>

constexpr double PI = 3.1415926535;//円周率

/*****************************************************************//**
 * \brief ラジアンに変換
 * \param radius 度数法の角
 * \return 変換した角
 *********************************************************************/
inline float ToRad(float radius) {
	float rad = radius * PI / 180;

	return rad;
}


//=============================================================================
// MathClass
//=============================================================================
class Math {
public:
	//円周率
	static constexpr float PI = 3.14159265358979323846f;
	//円周率の２乗
	static constexpr float PI2 = PI * 2.0f;

	/*****************************************************************//**
	 * \brief 角度をDegreeからRadianに変換
	 * 
	 * \param deg Degree角(度数法)
	 * \return Radian角
	 *********************************************************************/
	constexpr static inline float DegToRad(float deg){
		return deg * (PI / 180.0f);
	}

	/*****************************************************************//**
	 * \brief 角度をradianからDegreeに変換
	 * 
	 * \param rad Radian角
	 * \return Degree角 
	 *********************************************************************/
	constexpr static inline float RadToDeg(float rad){
		return rad / (PI / 180.0f);
	}
	
	/*****************************************************************//**
	 * \brief T1とT2の間を線形保管
	 * 
	 * \param rate	変換率(0.0〜1.0)
	 * \param t0	補完開始の値
	 * \param t1	補完終了の値
	 * \return 補完された値
	 *********************************************************************/
	template<class T> static inline T Lerp(float rate, T t0, T t1){
		T ret;
		ret.Lerp(rate, t0, t1);
		return ret;
	}

	/*****************************************************************//**
	 * \brief T1とT2の間を線形保管
	 *
	 * \param rate	変換率(0.0〜1.0)
	 * \param t0	補完開始の値
	 * \param t1	補完終了の値
	 * \return 補完された値
	 *********************************************************************/
	template<> static inline float Lerp(float rate, float t0, float t1){
		return t0 + (t1 - t0) * rate;
	}
};
