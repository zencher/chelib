#ifndef _CHE_PDF_MATRIX_H_
#define _CHE_PDF_MATRIX_H_
#include "../che_define.h"
#include <cmath>
#include <cfloat>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class CHE_PDF_Point
{
public:
	CHE_PDF_Point() : x(0), y(0) {}
	CHE_PDF_Point( HE_FLOAT vx, HE_FLOAT vy ) : x(vx), y(vy) {}

	HE_FLOAT	x;
	HE_FLOAT	y;
};

class CHE_PDF_Rect
{
public:
	CHE_PDF_Rect() : left(0), bottom(0), width(0), height(0) {}
	CHE_PDF_Rect( HE_FLOAT vleft, HE_FLOAT vbottom, HE_FLOAT vwidth, HE_FLOAT vheight )
		: left(vleft), bottom(vbottom), width(vwidth), height(vheight) {}

	HE_BOOL	IsEmpty() const
	{
		return ( fabs(width) < FLT_EPSILON || fabs(height) < FLT_EPSILON );
	}

	HE_VOID	Union( const CHE_PDF_Rect & rect )
	{
		if ( &rect != this )
		{
			if ( IsEmpty() )
			{
				*this = rect;
				return;
			}
			if ( !rect.IsEmpty() )
			{
				CHE_PDF_Rect tmpRect;
				tmpRect.left = ( left <= rect.left ) ? left : rect.left;
				tmpRect.bottom = ( bottom <= rect.bottom ) ? bottom : rect.bottom;
				tmpRect.width = ( left + width >= rect.left + rect.width ) ? left + width : rect.left + rect.width;
				tmpRect.width -= tmpRect.left;
				tmpRect.height = ( bottom + height >= rect.bottom + rect.height ) ? bottom + height : rect.bottom + rect.height;
				tmpRect.height -= tmpRect.bottom;
				*this = tmpRect;
			}
		}
	}

	bool operator == ( const CHE_PDF_Rect & rect ) const
	{
		if (	fabsf( left - rect.left ) <= FLT_EPSILON &&
				fabsf( bottom - rect.bottom ) <= FLT_EPSILON &&
				fabsf( width - rect.width ) <= FLT_EPSILON &&
				fabsf( height - rect.height ) <= FLT_EPSILON )
		{
			return true;
		}
		return false;
	}

	bool operator != ( const CHE_PDF_Rect & rect ) const
	{
		return ! operator==( rect );
	}

	HE_FLOAT	left;
	HE_FLOAT	bottom;
	HE_FLOAT	width;
	HE_FLOAT	height;
};


#define HE_MAX4(a,b,c,d) HE_max(HE_max(a,b), HE_max(c,d))
#define HE_MIN4(a,b,c,d) HE_min(HE_min(a,b), HE_min(c,d))
static inline float HE_min(float a, float b)
{
	return (a < b ? a : b);
}
static inline float HE_max(float a, float b)
{
	return (a > b ? a : b);
}

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

	CHE_PDF_Point Transform( const CHE_PDF_Point & point ) const
	{
		CHE_PDF_Point pointRet;
		pointRet.x = point.x * a + point.y * c + e;
		pointRet.y = point.x * b + point.y * d + f;
		return pointRet;
	}

	CHE_PDF_Rect Transform( const CHE_PDF_Rect & rect ) const
	{
		CHE_PDF_Point s, t, u, v;	
		CHE_PDF_Rect retRect;

		s.x = rect.left;
		s.y = rect.bottom;
		t.x = rect.left;
		t.y = rect.bottom + rect.height;
		u.x = rect.left + rect.width;
		u.y = rect.bottom + rect.height;
		v.x = rect.left + rect.width;
		v.y = rect.bottom;
		s = Transform( s );
		t = Transform( t );
		u = Transform( u );
		v = Transform( v );
		
		retRect.left = HE_MIN4(s.x, t.x, u.x, v.x);
		retRect.bottom = HE_MIN4(s.y, t.y, u.y, v.y);
		retRect.width = HE_MAX4(s.x, t.x, u.x, v.x) - HE_MIN4(s.x, t.x, u.x, v.x);
		retRect.height = HE_MAX4(s.y, t.y, u.y, v.y) - HE_MIN4(s.y, t.y, u.y, v.y);
		return retRect;
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
			return true;
		}
		return false;
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