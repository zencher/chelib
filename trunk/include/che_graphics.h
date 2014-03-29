#ifndef _CHE_GRAPHICS_H_
#define _CHE_GRAPHICS_H_

#include "che_define.h"
#include <vector>
#include <cfloat>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum GRAPHICS_STATE_LINECAP
{
	LineCap_Butt = 0,
	LineCap_Round = 1,
	LineCap_Square = 2
};

enum GRAPHICS_STATE_LINEJOIN
{
	LineJoin_Miter = 0,
	LineJoin_Round = 1,
	LineJoin_Bevel = 2
};

struct GRAPHICS_STATE_DASHPATTERN
{
	GRAPHICS_STATE_DASHPATTERN()
	{
		dashPhase = 0;
	}
	HE_FLOAT dashPhase;
	std::vector<float> dashArray;
};

enum GRAPHICS_STATE_RENDERINTENTS
{
	RI_AbsoluteColorimetric = 0,
	RI_RelativeColorimetric = 1,
	RI_Saturation = 2,
	RI_Perceptual = 3
};

enum GRAPHICS_STATE_TEXTRENDERMODE
{
	TextRenderMode_Fill = 0,
	TextRenderMode_Stroke = 1,
	TextRenderMode_FillStroke = 2,
	TextRenderMode_Invisible = 3,
	TextRenderMode_FillClip = 4,
	TextRenderMode_StrokeClip = 5,
	TextRenderMode_FillStrokeClip = 6,
	TextRenderMode_Clip = 7
};

enum GRAPHICS_STATE_BLENDMODE
{
	BlendMode_Normal		= 0,
	BlendMode_Multiply		= 1,
	BlendMode_Screen		= 2,
	BlendMode_Overlay		= 3,
	BlendMode_Darken		= 4,
	BlendMode_Lighten		= 5,
	BlendMode_ColorDodge	= 6,
	BlendMode_ColorBurn		= 7,
	BlendMode_HardLight		= 8,
	BlendMode_SoftLight		= 9,
	BlendMode_Difference	= 10,
	BlendMode_Exclusion		= 11
};

enum GRAPHICS_STATE_FILLMODE
{
	FillMode_Nonzero = 0,
	FillMode_EvenOdd = 1
};


class CHE_Point
{
public:
	CHE_Point() : x(0), y(0) {}
	CHE_Point( HE_FLOAT vx, HE_FLOAT vy ) : x(vx), y(vy) {}

	HE_FLOAT	x;
	HE_FLOAT	y;
};

class CHE_Rect
{
public:
	CHE_Rect() : left(0), bottom(0), width(0), height(0) {}
	CHE_Rect( HE_FLOAT vleft, HE_FLOAT vbottom, HE_FLOAT vwidth, HE_FLOAT vheight )
		: left(vleft), bottom(vbottom), width(vwidth), height(vheight) {}

	HE_BOOL	IsEmpty() const
	{
		return ( fabs(width) < FLT_EPSILON || fabs(height) < FLT_EPSILON );
	}

	HE_BOOL IsUnion( const CHE_Rect & rect )
	{
		CHE_Point p1;
		CHE_Point p2;
		p1.x = left + width / 2;
		p1.y = bottom + height / 2;
		p2.x = rect.left + rect.width / 2;
		p2.y = rect.bottom + rect.height / 2;
		HE_FLOAT xDis = fabsf( p1.x - p2.x );
		HE_FLOAT yDis = fabsf( p1.y - p2.y );
		if ( ( xDis < ( rect.width + width ) / 2 ) && ( yDis < ( rect.height + height ) / 2 ) )
		{
			return TRUE;
		}
		return FALSE;
	}

	HE_VOID	Union( const CHE_Rect & rect )
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
				CHE_Rect tmpRect;
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

	bool operator == ( const CHE_Rect & rect ) const
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

	bool operator != ( const CHE_Rect & rect ) const
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

class CHE_Matrix
{
public:
	CHE_Matrix() : a(1), b(0), c(0), d(1), e(0), f(0) {}
	CHE_Matrix( float va, float vb, float vc, float vd, float ve, float vf )
		: a(va), b(vb), c(vc), d(vd), e(ve), f(vf) {}

	static CHE_Matrix TranslateMatrix( HE_FLOAT tx, HE_FLOAT ty )
	{
		CHE_Matrix matrixRet;
		matrixRet.e = tx;
		matrixRet.f = ty;
		return matrixRet;
	}

	static CHE_Matrix RotateMatrix( HE_FLOAT theta )
	{
		float s;
		float c;
		CHE_Matrix matrixRet;

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

	static CHE_Matrix ScaleMatrix( HE_FLOAT sx, HE_FLOAT sy )
	{
		CHE_Matrix matrixRet;
		matrixRet.a = sx;
		matrixRet.d = sy;
		return matrixRet;
	}

	static CHE_Matrix SkewMatrix( HE_FLOAT a, HE_FLOAT b )
	{
		CHE_Matrix matrixRet;
		matrixRet.b = tanf(a);
		matrixRet.c = tanf(b);
		return matrixRet;
	}

	void Concat( const CHE_Matrix & matrix )
	{
		CHE_Matrix tmpMatrix = *this;
		a = tmpMatrix.a * matrix.a +tmpMatrix. b * matrix.c;
		b = tmpMatrix.a * matrix.b + tmpMatrix.b * matrix.d;
		c = tmpMatrix.c * matrix.a + tmpMatrix.d * matrix.c;
		d = tmpMatrix.c * matrix.b + tmpMatrix.d * matrix.d;
		e = tmpMatrix.e * matrix.a + tmpMatrix.f * matrix.c + matrix.e;
		f = tmpMatrix.e * matrix.b + tmpMatrix.f * matrix.d + matrix.f;
	}

	void Invert( const CHE_Matrix & matirx )
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

	CHE_Point Transform( const CHE_Point & point ) const
	{
		CHE_Point pointRet;
		pointRet.x = point.x * a + point.y * c + e;
		pointRet.y = point.x * b + point.y * d + f;
		return pointRet;
	}

	CHE_Rect Transform( const CHE_Rect & rect ) const
	{
		CHE_Point s, t, u, v;	
		CHE_Rect retRect;

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

	bool operator == ( const CHE_Matrix & matrix ) const
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

	bool operator != ( const CHE_Matrix & matrix ) const
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