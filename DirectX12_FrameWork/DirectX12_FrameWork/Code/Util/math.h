#pragma once
#include<DirectXMath.h>

constexpr double PI = 3.1415926535;//‰~ü—¦

/*****************************************************************//**
 * \brief ƒ‰ƒWƒAƒ“‚É•ÏŠ·
 * \param radius “x”–@‚ÌŠp
 * \return •ÏŠ·‚µ‚½Šp
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
	//‰~ü—¦
	static constexpr float PI = 3.14159265358979323846f;
	//‰~ü—¦‚Ì‚Qæ
	static constexpr float PI2 = PI * 2.0f;

	/*****************************************************************//**
	 * \brief Šp“x‚ğDegree‚©‚çRadian‚É•ÏŠ·
	 * 
	 * \param deg DegreeŠp(“x”–@)
	 * \return RadianŠp
	 *********************************************************************/
	constexpr static inline float DegToRad(float deg){
		return deg * (PI / 180.0f);
	}

	/*****************************************************************//**
	 * \brief Šp“x‚ğradian‚©‚çDegree‚É•ÏŠ·
	 * 
	 * \param rad RadianŠp
	 * \return DegreeŠp 
	 *********************************************************************/
	constexpr static inline float RadToDeg(float rad){
		return rad / (PI / 180.0f);
	}
	
	/*****************************************************************//**
	 * \brief T1‚ÆT2‚ÌŠÔ‚ğüŒ`•ÛŠÇ
	 * 
	 * \param rate	•ÏŠ·—¦(0.0`1.0)
	 * \param t0	•âŠ®ŠJn‚Ì’l
	 * \param t1	•âŠ®I—¹‚Ì’l
	 * \return •âŠ®‚³‚ê‚½’l
	 *********************************************************************/
	template<class T> static inline T Lerp(float rate, T t0, T t1){
		T ret;
		ret.Lerp(rate, t0, t1);
		return ret;
	}

	/*****************************************************************//**
	 * \brief T1‚ÆT2‚ÌŠÔ‚ğüŒ`•ÛŠÇ
	 *
	 * \param rate	•ÏŠ·—¦(0.0`1.0)
	 * \param t0	•âŠ®ŠJn‚Ì’l
	 * \param t1	•âŠ®I—¹‚Ì’l
	 * \return •âŠ®‚³‚ê‚½’l
	 *********************************************************************/
	template<> static inline float Lerp(float rate, float t0, float t1){
		return t0 + (t1 - t0) * rate;
	}
};
