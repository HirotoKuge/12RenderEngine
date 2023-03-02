/*****************************************************************//**
 * \file   Matrix.cpp
 * \brief  matrix
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/

#include "Matrix.h"

const Matrix Matrix::Identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);