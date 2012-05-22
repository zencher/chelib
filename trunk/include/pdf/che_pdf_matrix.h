#ifndef _CHE_PDF_MATRIX_H_
#define _CHE_PDF_MATRIX_H_
#include "../che_define.h"
#include <cmath>
#include <cfloat>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class CHE_PDF_Matrix
{
public:
	CHE_PDF_Matrix() : a(1), b(0), c(0), d(1), e(0), f(0) {}
	CHE_PDF_Matrix( float va, float vb, float vc, float vd, float ve, float vf )
		: a(va), b(vb), c(vc), d(vd), e(ve), f(vf) {}

	static CHE_PDF_Matrix TranslateMatrix( HE_FLOAT tx, HE_FLOAT ty )
	{
		CHE_PDF_Matrix matrixRet;
		matrixRet.e = tx;
		matrixRet.f = ty;
		return matrixRet;
	}

	static CHE_PDF_Matrix RotateMatrix( HE_FLOAT theta )
	{
		float s;
		float c;
		CHE_PDF_Matrix matrixRet;

		while ( theta < 0 )
			theta += 360;
		while ( theta >= 360 )
			theta -= 360;

		if ( fabsf(0 - theta) < FLT_EPSILON )
		{
			s = 0;
			c = 1;
		}
		else if ( fabsf(90.0f - theta) < FLT_EPSILON )
		{
			s = 1;
			c = 0;
		}
		else if ( fabsf(180.0f - theta) < FLT_EPSILON )
		{
			s = 0;
			c = -1;
		}
		else if ( fabsf(270.0f - theta) < FLT_EPSILON )
		{
			s = -1;
			c = 0;
		}
		else
		{
			s = sinf(theta * (float)M_PI / 180);
			c = cosf(theta * (float)M_PI / 180);
		}

		matrixRet.a = c; matrixRet.b = s;
		matrixRet.c = -s; matrixRet.d = c;
		matrixRet.e = 0; matrixRet.f = 0;
		return matrixRet;
	}

	static CHE_PDF_Matrix ScaleMatrix( HE_FLOAT sx, HE_FLOAT sy )
	{
		CHE_PDF_Matrix matrixRet;
		matrixRet.a = sx;
		matrixRet.d = sy;
		return matrixRet;
	}

	static CHE_PDF_Matrix SkewMatrix( HE_FLOAT a, HE_FLOAT b )
	{
		CHE_PDF_Matrix matrixRet;
		matrixRet.b = tanf(a);
		matrixRet.c = tanf(b);
		return matrixRet;
	}

	void Concat( const CHE_PDF_Matrix & matrix )
	{
		CHE_PDF_Matrix tmpMatrix = *this;
		a = tmpMatrix.a * matrix.a +tmpMatrix. b * matrix.c;
		b = tmpMatrix.a * matrix.b + tmpMatrix.b * matrix.d;
		c = tmpMatrix.c * matrix.a + tmpMatrix.d * matrix.c;
		d = tmpMatrix.c * matrix.b + tmpMatrix.d * matrix.d;
		e = tmpMatrix.e * matrix.a + tmpMatrix.f * matrix.c + matrix.e;
		f = tmpMatrix.e * matrix.b + tmpMatrix.f * matrix.d + matrix.f;
	}

	void Invert( const CHE_PDF_Matrix & matirx )
	{
		float rdet = matirx.AbsValue();
		a = matirx.d * rdet;
		b = - matirx.b * rdet;
		c = - matirx.c * rdet;
		d = matirx.a * rdet;
		e = - matirx.e * a - matirx.f * c;
		f = - matirx.e * b - matirx.f * d;
	}

	HE_FLOAT AbsValue() const
	{
		HE_FLOAT val = 1 / ( a * d - b * c );
		return val;
	}

	bool operator == ( const CHE_PDF_Matrix & matrix ) const
	{
		if ( fabsf( a - matrix.a ) <= FLT_EPSILON &&
			 fabsf( b - matrix.b ) <= FLT_EPSILON &&
			 fabsf( c - matrix.c ) <= FLT_EPSILON &&
			 fabsf( d - matrix.d ) <= FLT_EPSILON && 
			 fabsf( e - matrix.e ) <= FLT_EPSILON &&
			 fabsf( f - matrix.f ) <= FLT_EPSILON )
		{
			return TRUE;
		}
		return FALSE;
	}

	bool operator != ( const CHE_PDF_Matrix & matrix ) const
	{
		return ! operator==( matrix );
	}

	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
};

#endif