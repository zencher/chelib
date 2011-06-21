#ifndef _CHE_PDF_PAGEOBJS_H_
#define _CHE_PDF_PAGEOBJS_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "../che_bitmap.h"
#include "../che_graphics.h"
#include "che_pdf_objects.h"


#define CONTENTOBJ_INVALID			0x0000
#define CONTENTOBJ_TEXT				0x0001
#define CONTENTOBJ_PATH				0x0003
#define	CONTENTOBJ_ORDER			0x0004

#define COLORSAPCE_DEVICE			0x0000
#define COLORSPACE_CIEBASE			0x0001
#define COLORSPACE_SPECIAL			0x0002

#define COLOR_DEVICE_GRAY			0x0000
#define COLOR_DEVICE_RGB			0x0001
#define COLOR_DEVICE_CMYK			0x0002

#define COLOR_CIEBASE_CALCRAY		0x0100
#define COLOR_CIEBASE_CALRGB		0x0101
#define COLOR_CIEBASE_CALLAB		0x0102
#define COLOR_CIEBASE_ICCBASED		0x0103

#define COLOR_SPECIAL_PATTERN		0x0201
#define COLOR_SPECIAL_INDEXED		0x0202
#define COLOR_SPECIAL_SEPARATION	0x0203
#define COLOR_SPECAIL_DEVICEN		0x0204


class CHE_PDF_Color : public CHE_Object
{
public:
	CHE_PDF_Color( HE_BYTE colorSpace = COLORSAPCE_DEVICE, HE_BYTE colorType = COLOR_DEVICE_RGB, HE_DWORD value = 0xFF000000, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
	{
		m_colorSpace = colorSpace;
		m_colorType = colorType;
		m_value = value;
	}

	HE_VOID SetColorSpace( HE_BYTE colorSpace ) { m_colorSpace = colorSpace; }

	HE_VOID SetColorType( HE_BYTE colorType ) { m_colorType = colorType; }

	HE_VOID SetValue( HE_DWORD value ) { m_value = value; }

	HE_BYTE GetColorSpace() { return m_colorSpace; }

	HE_BYTE GetColorType() { return m_colorType; }

	HE_DWORD GetValue() { return m_value; }

	HE_ARGB GetARGB() { return m_value; }

private:
	HE_BYTE m_colorSpace;
	HE_BYTE	m_colorType;
	HE_DWORD m_value;
};


// class CHE_PDF_GraphState : public CHE_Object
// {
// public:
// 	CHE_PDF_GraphState( CHE_Allocator * pAllocator = NULL )
// 		: CHE_Object( pAllocator ), 
// 		m_StrokeColor(	COLORSAPCE_DEVICE, COLOR_DEVICE_RGB, 0xFF000000, pAllocator ),
// 		m_FillColor(	COLORSAPCE_DEVICE, COLOR_DEVICE_RGB, 0xFF000000, pAllocator )
// 	{
// 		m_MatrixA = 1;
// 		m_MatrixB = 0;
// 		m_MatrixC = 0;
// 		m_MatrixD = 1;
// 		m_MatrixE = 0;
// 		m_MatrixF = 0;
// 		m_LineWidth = 1;
// 		m_LineCap = 0;
// 		m_LineJoin = 0;
// 		m_MiterLimit = 10;
// 		m_DashArraySize = 0;
// 		m_DashArray = NULL;
// 		m_DashPhase = 0;
// 	}
// 
// 	~CHE_PDF_GraphState()
// 	{
// 		if ( m_DashArray )
// 		{
// 			GetAllocator()->DeleteArray<HE_FLOAT>( m_DashArray );
// 			m_DashArray = NULL;
// 		}
// 	}
// 
// 	CHE_PDF_GraphState * Clone()
// 	{
// 		CHE_PDF_GraphState * pTmp = GetAllocator()->New<CHE_PDF_GraphState>( GetAllocator() );
// 		pTmp->SetMatrix( m_MatrixA, m_MatrixB, m_MatrixC, m_MatrixD, m_MatrixE, m_MatrixF );
// 		pTmp->SetLineWidth( m_LineWidth );
// 		pTmp->SetLineCap( m_LineCap );
// 		pTmp->SetLineJoin( m_LineJoin );
// 		pTmp->SetMiterLimit( m_MiterLimit );
// 		if ( m_DashArraySize > 0 && m_DashArray != NULL )
// 		{
// 			pTmp->SetDashArray( m_DashArraySize, m_DashArray, m_DashPhase );
// 		}
// 		pTmp->GetStrokeColor()->SetColorSpace( GetStrokeColor()->GetColorSpace() );
// 		pTmp->GetStrokeColor()->SetColorType( GetStrokeColor()->GetColorType() );
// 		pTmp->GetStrokeColor()->SetValue( GetStrokeColor()->GetValue() );
// 		pTmp->GetFillColor()->SetColorSpace( GetFillColor()->GetColorSpace() );
// 		pTmp->GetFillColor()->SetColorType( GetFillColor()->GetColorType() );
// 		pTmp->GetFillColor()->SetValue( GetFillColor()->GetValue() );
// 		return pTmp;
// 	}
// 
// 	HE_FLOAT	GetMatrixA() { return m_MatrixA; }
// 	HE_FLOAT	GetMatrixB() { return m_MatrixB; }
// 	HE_FLOAT	GetMatrixC() { return m_MatrixC; }
// 	HE_FLOAT	GetMatrixD() { return m_MatrixD; }
// 	HE_FLOAT	GetMatrixE() { return m_MatrixE; }
// 	HE_FLOAT	GetMatrixF() { return m_MatrixF; }
// 
// 	HE_VOID		SetMatrix( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
// 	{
// 		m_MatrixA = a;
// 		m_MatrixB = b;
// 		m_MatrixC = c;
// 		m_MatrixD = d;
// 		m_MatrixE = e;
// 		m_MatrixF = f;
// 	}
// 
// 	HE_VOID		MultiplyMatrix( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
// 	{
// 		HE_FLOAT aa = 0.0, bb = 0.0, cc = 0.0, dd =0.0, ee = 0.0, ff = 0.0;
// 		aa = m_MatrixA * a + m_MatrixB * c;
// 		bb = m_MatrixA * b + m_MatrixB * d;
// 		cc = m_MatrixC * a + m_MatrixD * c;
// 		dd = m_MatrixC * b + m_MatrixD * d;
// 		ee = m_MatrixE * a + m_MatrixF * c + e;
// 		ff = m_MatrixE * b + m_MatrixF * d + f;
// 		m_MatrixA = aa;
// 		m_MatrixB = bb;
// 		m_MatrixC = cc;
// 		m_MatrixD = dd;
// 		m_MatrixE = ee;
// 		m_MatrixF = ff;
// 	}
// 
// 	HE_FLOAT	GetLineWidth() { return m_LineWidth; }
// 	HE_VOID		SetLineWidth( HE_FLOAT width ) { m_LineWidth = width; }
// 
// 	HE_BYTE		GetLineCap() { return m_LineCap; }
// 	HE_VOID		SetLineCap( HE_BYTE cap ) { m_LineCap = cap; }
// 
// 	HE_BYTE		GetLineJoin() { return m_LineJoin; }
// 	HE_VOID		SetLineJoin( HE_BYTE join ) { m_LineJoin = join; }
// 
// 	HE_FLOAT	GetMiterLimit() { return m_MiterLimit; }
// 	HE_VOID		SetMiterLimit( HE_FLOAT miterLimit ) { m_MiterLimit = miterLimit;  }
// 
// 	HE_DWORD	GetDashArraySize() { return m_DashArraySize; }
// 	HE_FLOAT*	GetDashArray() { return m_DashArray; }
// 	HE_FLOAT	GetDashPhase() { return m_DashPhase; }
// 
// 	HE_VOID		SetDashArray( HE_DWORD size, HE_FLOAT* pArray, HE_FLOAT dashPhase )
// 	{
// 		if ( m_DashArray != NULL )
// 		{
// 			GetAllocator()->DeleteArray<HE_FLOAT>( m_DashArray );
// 			m_DashArray = NULL;
// 			m_DashArraySize = 0;
// 		}
// 		if ( size != 0 && pArray != NULL )
// 		{
// 			m_DashArraySize = size;
// 			m_DashArray = GetAllocator()->NewArray<HE_FLOAT>( size );
// 			memcpy( m_DashArray, pArray, m_DashArraySize * sizeof( HE_FLOAT ) );
// 		}
// 		m_DashPhase = dashPhase;
// 	}
// 
// 	CHE_PDF_Color * GetStrokeColor() { return &m_StrokeColor; }
// 	CHE_PDF_Color * GetFillColor() { return &m_FillColor; }
// 
// private:
// 	CHE_PDF_GraphState( CHE_PDF_GraphState & graphstate )
// 		: CHE_Object( graphstate.GetAllocator() ),
// 		m_StrokeColor(	graphstate.GetStrokeColor()->GetColorSpace(), graphstate.GetStrokeColor()->GetColorType(),
// 						graphstate.GetStrokeColor()->GetValue(), graphstate.GetAllocator() ),
// 		m_FillColor(	graphstate.GetFillColor()->GetColorSpace(), graphstate.GetFillColor()->GetColorType(),
// 						graphstate.GetFillColor()->GetValue(), graphstate.GetAllocator() )
// 	{
// 		m_MatrixA = graphstate.m_MatrixA;
// 		m_MatrixB = graphstate.m_MatrixB;
// 		m_MatrixC = graphstate.m_MatrixC;
// 		m_MatrixD = graphstate.m_MatrixD;
// 		m_MatrixE = graphstate.m_MatrixE;
// 		m_MatrixF = graphstate.m_MatrixF;
// 
// 		m_LineWidth = graphstate.m_LineWidth;
// 		m_LineCap = graphstate.m_LineCap;
// 		m_LineJoin = graphstate.m_LineJoin;
// 		
// 		if ( graphstate.m_DashArraySize > 0 && graphstate.m_DashArray != NULL  )
// 		{
// 			SetDashArray( graphstate.m_DashArraySize, graphstate.m_DashArray, graphstate.m_DashPhase );
// 		}
// 
// 	}
// 
// 	HE_FLOAT	m_MatrixA;
// 	HE_FLOAT	m_MatrixB;
// 	HE_FLOAT	m_MatrixC;
// 	HE_FLOAT	m_MatrixD;
// 	HE_FLOAT	m_MatrixE;
// 	HE_FLOAT	m_MatrixF;
// 	HE_FLOAT	m_LineWidth;
// 	HE_BYTE		m_LineCap;		//0, 1, 2
// 	HE_BYTE		m_LineJoin;		//0, 1, 2
// 	HE_FLOAT	m_MiterLimit;	//def: 10.0
// 	HE_DWORD	m_DashArraySize;
// 	HE_FLOAT*	m_DashArray;
// 	HE_FLOAT	m_DashPhase;
// // 	HE_DWORD	m_Intent;
// // 	HE_DWORD	m_Flatness;
// 
// 	//color
// 	CHE_PDF_Color	m_StrokeColor;
// 	CHE_PDF_Color	m_FillColor;
// };

class CHE_PDF_TextObject;
class CHE_PDF_PathObject;
class CHE_PDF_OrderObject;

class CHE_PDF_ContentObject : public CHE_Object
{
public:
	CHE_PDF_ContentObject( HE_BYTE type = CONTENTOBJ_INVALID, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
	{ m_Type = type; }

	virtual ~CHE_PDF_ContentObject() {};

	HE_BYTE GetType() { return m_Type; }

	HE_VOID Release();

private:
	HE_BYTE m_Type;
};


class CHE_PDF_TextObjectItem;

class CHE_PDF_TextObject : public CHE_PDF_ContentObject
{
public:
	static CHE_PDF_TextObject* Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_TextObject>(  pAllocator );
		}else{
			return new CHE_PDF_TextObject( NULL );
		}
	}

	HE_BOOL AppendItem( CHE_PDF_TextObjectItem * pItem );

	CHE_PDF_TextObjectItem * GetItem( HE_DWORD index )
	{
		return (CHE_PDF_TextObjectItem*)m_arrTextItem.GetItem( index );
	}

	HE_DWORD GetItemCount() { return m_arrTextItem.GetCount(); }

	HE_VOID Clear();

private:
	CHE_PDF_TextObject( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_ContentObject( CONTENTOBJ_TEXT, pAllocator ), m_arrTextItem( pAllocator ) {}
	~CHE_PDF_TextObject() { Clear(); }

	CHE_PtrArray	m_arrTextItem;

	friend class CHE_PDF_ContentObject;
	friend class CHE_Allocator;
};

class CHE_PDF_TextObjectItem : public CHE_Object
{
public:
	CHE_PDF_TextObjectItem(  CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), m_str( pAllocator )
	{
		m_fPosiX = 0;
		m_fPosiY = 0;
		m_fCharSpace = 0;
		m_fWordSpace = 0;
		m_dwScale = 100;
		m_fLeading = 0;
		m_dwFontObjNum = 0;
		m_dwSize = 0;
		m_byteRender = 0;
		m_dwRise = 0;
		m_bKnockout = FALSE;
		m_fMatrixA = 1;
		m_fMatrixB = 0;
		m_fMatrixC = 0;
		m_fMatrixD = 1;
		m_fMatrixE = 0;
		m_fMatrixF = 0;
		m_pText = NULL;	
	}
	~CHE_PDF_TextObjectItem();

	HE_VOID SetPosi( HE_FLOAT fX, HE_FLOAT fY ) { m_fPosiX = fX; m_fPosiY = fY; }
	HE_VOID SetCharSpace( HE_FLOAT cs ) { m_fCharSpace = cs; }
	HE_VOID SetWordSpace( HE_FLOAT ws ) { m_fWordSpace = ws; }
	HE_VOID SetScale( HE_DWORD scale ) { m_dwScale = scale; }
	HE_VOID SetLeading( HE_FLOAT leading ) { m_fLeading = leading; }
	HE_VOID SetFontObj( HE_DWORD objNum ) { m_dwFontObjNum = objNum; }
	HE_VOID SetSize( HE_DWORD size ) { m_dwSize = size; }
	HE_VOID SetRender( HE_BYTE render ) { m_byteRender = render; }
	HE_VOID SetRise( HE_LONG rise ) { m_dwRise = rise; }
	HE_VOID SetKnockout( HE_BOOL knockout ) { m_bKnockout = knockout; }
	HE_VOID SetMatrix( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
	{
		m_fMatrixA = a; m_fMatrixB = b; m_fMatrixC = c; m_fMatrixD = d; m_fMatrixE = e; m_fMatrixF = f;
	}
	HE_VOID SetText( CHE_PDF_Object * pObj ) { m_pText = pObj; }

	HE_VOID SetString( CHE_WideString & str ) { m_str = str; }

	HE_FLOAT	GetPosiX() { return m_fPosiX; }
	HE_FLOAT	GetPosiY() { return m_fPosiY; }
	HE_FLOAT	GetCharSpace() { return m_fCharSpace; }
	HE_FLOAT	GetWordSpace() { return m_fWordSpace; }
	HE_DWORD	GetScale() { return m_dwScale; }
	HE_FLOAT	GetLeading() { return m_fLeading; }
	HE_DWORD	GetFontObjNum() { return m_dwFontObjNum; }
	HE_DWORD	GetSize() { return m_dwSize; }
	HE_BYTE		GetRender() { return m_byteRender; }
	HE_LONG		GetRise() { return m_dwRise; }
	HE_BOOL		GetKnockout() { return m_bKnockout; }
	HE_FLOAT	GetMatrixA() { return m_fMatrixA; }
	HE_FLOAT	GetMatrixB() { return m_fMatrixB; }
	HE_FLOAT	GetMatrixC() { return m_fMatrixC; }
	HE_FLOAT	GetMatrixD() { return m_fMatrixD; }
	HE_FLOAT	GetMatrixE() { return m_fMatrixE; }
	HE_FLOAT	GetMatrixF() { return m_fMatrixF; }
	CHE_WideString	*	GetString() { return &m_str; }

private:
	HE_FLOAT	m_fPosiX;
	HE_FLOAT	m_fPosiY;
	HE_FLOAT	m_fCharSpace;	//def: 0
	HE_FLOAT	m_fWordSpace;	//def: 0
	HE_DWORD	m_dwScale;		//def: 100
	HE_FLOAT	m_fLeading;		//def: 0
	HE_DWORD	m_dwFontObjNum;
	HE_DWORD	m_dwSize;
	HE_BYTE		m_byteRender;	//0 - 7
	HE_LONG		m_dwRise;		//0
	HE_BOOL		m_bKnockout;	//def: false
	HE_FLOAT	m_fMatrixA;		//def: 1
	HE_FLOAT	m_fMatrixB;		//def: 0
	HE_FLOAT	m_fMatrixC;		//def: 0
	HE_FLOAT	m_fMatrixD;		//def: 1
	HE_FLOAT	m_fMatrixE;		//def: 0
	HE_FLOAT	m_fMatrixF;		//def: 0
	CHE_PDF_Object *	m_pText;	//string or array
	CHE_WideString m_str;

	friend class CHE_PDF_TextObject;
};

#define PATH_OPERATOR_NOOP			0x00
#define PATH_OPERATOR_STROKE		0x01
#define PATH_OPERATOR_FILL			0x10
#define PATH_OPERATOR_FILLSTROKE	0x11

#define PATH_FILL_MODE_NOZERO		0x00
#define PATH_FILL_MODE_EVERODD		0x01

class CHE_PDF_PathObject : public CHE_PDF_ContentObject
{
public:
	static CHE_PDF_PathObject* Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_PathObject>( pAllocator );
		}else{
			return new CHE_PDF_PathObject( NULL );
		}
	}

	HE_VOID SetOperator( HE_BYTE opt ) { m_byteOperator = opt; }
	HE_VOID SetFillMode( HE_BYTE mode ) { m_byteMode = mode; }

	HE_BYTE GetOperator() { return m_byteOperator; }
	HE_BYTE GetFillMode() { return m_byteMode; }

	HE_VOID	SetClip( HE_BOOL bClip ) { m_bClip = bClip; }
	HE_BOOL	GetClip() { return m_bClip; }

	HE_VOID	SetClipFlag( HE_BYTE clipFlag ) { m_byteClipFlag = clipFlag; }
	HE_BYTE	GetClipFlag() { return m_byteClipFlag; }

	HE_BOOL AppendItem( CHE_GraphicsObject * pItem );

	CHE_GraphicsObject * GetItem( HE_DWORD index )
	{
		return (CHE_GraphicsObject*)m_arrPathItem.GetItem( index );
	}

	HE_DWORD GetItemCount() { return m_arrPathItem.GetCount(); }

	HE_VOID Clear();

private:
	CHE_PDF_PathObject( CHE_Allocator * pAllocator = NULL ) 
		: CHE_PDF_ContentObject( CONTENTOBJ_PATH, pAllocator ), m_arrPathItem( pAllocator )
	{ m_byteMode = PATH_FILL_MODE_NOZERO; m_byteOperator = PATH_OPERATOR_STROKE; m_bClip = FALSE; m_byteClipFlag = 0; }
	~CHE_PDF_PathObject() { Clear(); }

	CHE_PtrArray	m_arrPathItem;
	HE_BYTE			m_byteOperator;
	HE_BYTE			m_byteMode;

	HE_BOOL			m_bClip;
	HE_BYTE			m_byteClipFlag;

	friend class CHE_PDF_ContentObject;
	friend class CHE_Allocator;
};


//init
#define ORDER_INITGRAPHSTATE	0x0001

//General graphics state
#define ORDER_LINE_WIDTH		0x0002
#define ORDER_LINE_CAP			0x0003
#define ORDER_LINE_JION			0x0004
#define ORDER_MITER_LIMIT		0x0005
#define ORDER_DASH_PATTERN		0x0006
#define ORDER_INTENT			0x0007
#define ORDER_FALTNESS			0x0008
#define ORDER_GRAPHSTATE		0x0009

//Special graphics state
#define ORDER_PUSH_STATE		0x000A
#define ORDER_POP_STATE			0x000B
#define ORDER_MATRIX			0x000C

//Color
#define ORDER_STROKE_COLOR		0x000D
#define ORDER_FILL_COLOR		0x000E

class CHE_PDF_OrderParam : public CHE_Object
{
public:
	CHE_PDF_OrderParam( HE_BYTE type = NULL, CHE_Allocator * pAllocator = NULL ) 
		:CHE_Object( pAllocator )
	{ m_type = type; }

	~CHE_PDF_OrderParam() {};

	HE_BYTE GetType() { m_type; }
	HE_VOID	Release();

private:
	HE_BYTE m_type;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_LineWidth : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_LineWidth( HE_FLOAT fLineWidth, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_LINE_WIDTH, pAllocator )
	{ m_fLineWidth = fLineWidth; }

	HE_VOID	SetLineWidth( HE_FLOAT fLineWidth ) { m_fLineWidth = fLineWidth; } 
	HE_FLOAT GetLineWidth() { return m_fLineWidth; }

private:
	HE_FLOAT m_fLineWidth;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_LineCap : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_LineCap( HE_BYTE lineCap, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_LINE_CAP, pAllocator )
	{ m_lineCap = lineCap; }

	HE_VOID SetLineCap( HE_BYTE lineCap ) { m_lineCap = lineCap; }
	HE_BYTE GetLineCap() { return m_lineCap; }

private:
	HE_BYTE m_lineCap;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_LineJoin : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_LineJoin( HE_BYTE lineJoin, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_LINE_JION, pAllocator )
	{ m_lineJoin = lineJoin; }

	HE_VOID SetLineJoin( HE_BYTE lineCap ) { m_lineJoin = lineCap; }
	HE_BYTE GetLineJoin() { return m_lineJoin; }
private:
	HE_BYTE m_lineJoin;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_MiterLimit : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_MiterLimit( HE_FLOAT miterLimit, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_MITER_LIMIT, pAllocator )
	{ m_fMiterLimit = miterLimit; }

	HE_VOID	SetMiterLimit( HE_FLOAT fMiterLimit ) { m_fMiterLimit = fMiterLimit; } 
	HE_FLOAT GetMiterLimit() { return m_fMiterLimit; }

private:
	HE_FLOAT m_fMiterLimit;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_DashPattern : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_DashPattern( HE_FLOAT * pDashArray, HE_DWORD dwArraySize, HE_FLOAT DashPhase, CHE_Allocator * pAllocator )
		: CHE_PDF_OrderParam( ORDER_DASH_PATTERN, pAllocator )
	{
		m_fDashPhase = DashPhase;
		if ( pDashArray == NULL )
		{
			m_pDashArray = NULL;
			m_dwArraySize = 0;
		}else{
			m_pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwArraySize );
			for ( HE_DWORD i = 0; i < dwArraySize; i++ )
			{
				m_pDashArray[i] = pDashArray[i];
			}
			m_dwArraySize = dwArraySize;
		}
	}

	HE_VOID		SetDashArray( HE_FLOAT * pDashArray, HE_DWORD dwArraySize )
	{
		if ( m_pDashArray != NULL )
		{
			GetAllocator()->DeleteArray( m_pDashArray );
		}
		if ( pDashArray == NULL )
		{
			m_pDashArray = NULL;
			m_dwArraySize = 0;
		}else{
			m_pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwArraySize );
			for ( HE_DWORD i = 0; i < dwArraySize; i++ )
			{
				m_pDashArray[i] = pDashArray[i];
			}
			m_dwArraySize = dwArraySize;
		}
	}
	HE_FLOAT *	GetDashArray() { return m_pDashArray; }
	HE_DWORD	GetArraySize() { return m_dwArraySize; }
	HE_DWORD	SetDashPhase( HE_FLOAT dashPhase ) { m_fDashPhase = dashPhase; }
	HE_FLOAT	GetDashPhase() { return m_fDashPhase; }

private:
	HE_FLOAT *	m_pDashArray;
	HE_FLOAT	m_fDashPhase;
	HE_DWORD	m_dwArraySize;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_Flatness : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_Flatness( HE_BYTE flatness, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_FALTNESS, pAllocator )
	{
		m_flatness = flatness;
	}

	HE_VOID		SetFlatness( HE_BYTE flatness ) { m_flatness = flatness; }
	HE_BYTE		GetFlatness() { return m_flatness; }

private:
	HE_BYTE		m_flatness;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_Intent : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_Intent( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_INTENT, pAllocator ) {}

private:
	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_GraphState : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_GraphState( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_GRAPHSTATE, pAllocator ), m_array( pAllocator ) {}

	HE_DWORD GetCount() { return m_array.GetCount(); }

	HE_VOID Append( CHE_PDF_OrderParam * pOrderParam )
	{
		if ( pOrderParam )
		{
			m_array.Append( (HE_LPVOID)pOrderParam ); 
		}
	}

	CHE_PDF_OrderParam * GetItem( HE_DWORD index ) { return (CHE_PDF_OrderParam *)m_array.GetItem( index ); }

private:
	CHE_PtrArray m_array;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_PushState : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_PushState( CHE_Allocator * pAllocator )
		: CHE_PDF_OrderParam( ORDER_PUSH_STATE, pAllocator ) {}
private:
	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_PopState : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_PopState( CHE_Allocator * pAllocator )
		: CHE_PDF_OrderParam( ORDER_POP_STATE, pAllocator ) {}
private:

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_Matrix : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_Matrix( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_MATRIX, pAllocator )
	{
		m_fA = 1; m_fB = 0; m_fC = 0; m_fD = 1; m_fE = 0; m_fF = 0;
	}

	CHE_PDF_OrderParam_Matrix(	HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, 
								HE_FLOAT e, HE_FLOAT f, CHE_Allocator * pAllocator = NULL )
								: CHE_PDF_OrderParam( ORDER_MATRIX, pAllocator )
	{
		m_fA = a; m_fB = b; m_fC = c; m_fD = d; m_fE = e; m_fF = f;
	}

	HE_VOID SetMatrix(	HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, 
						HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
	{
		m_fA = a; m_fB = b; m_fC = c; m_fD = d; m_fE = e; m_fF = f;
	}

	HE_FLOAT GetMatrixA() { return m_fA; }
	HE_FLOAT GetMatrixB() { return m_fB; }
	HE_FLOAT GetMatrixC() { return m_fC; }
	HE_FLOAT GetMatrixD() { return m_fD; }
	HE_FLOAT GetMatrixE() { return m_fE; }
	HE_FLOAT GetMatrixF() { return m_fF; }

private:
	HE_FLOAT m_fA;
	HE_FLOAT m_fB;
	HE_FLOAT m_fC;
	HE_FLOAT m_fD;
	HE_FLOAT m_fE;
	HE_FLOAT m_fF;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_StrokeColor : public CHE_PDF_OrderParam
{
public:
	CHE_PDF_OrderParam_StrokeColor( CHE_PDF_Color color, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_STROKE_COLOR, pAllocator ) { m_color = color; }

	HE_VOID SetColor( CHE_PDF_Color color ) { m_color = color; }
	CHE_PDF_Color GetColor() { return m_color; }

private:
	CHE_PDF_Color m_color;

	friend CHE_Allocator;
};

class CHE_PDF_OrderParam_FillColor : public CHE_PDF_OrderParam
{
	CHE_PDF_OrderParam_FillColor( CHE_PDF_Color color, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_OrderParam( ORDER_STROKE_COLOR, pAllocator ) { m_color = color; }

	HE_VOID SetColor( CHE_PDF_Color color ) { m_color = color; }
	CHE_PDF_Color GetColor() { return m_color; }

private:
	CHE_PDF_Color m_color;

	friend CHE_Allocator;
};

class CHE_PDF_OrderObject : public CHE_PDF_ContentObject
{
public:
	static CHE_PDF_OrderObject* Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_OrderObject>( pAllocator );
		}else{
			return new CHE_PDF_OrderObject( NULL );
		}
	}

	HE_VOID SetOrder( HE_BYTE order ) { m_order = order; }
	HE_BYTE	GetOrder() { return m_order; }

	HE_VOID	SetParam( CHE_PDF_OrderParam * pParam ) { m_pParam = pParam; }
	CHE_PDF_OrderParam * GetParam() { return m_pParam; } 

private:
	CHE_PDF_OrderObject( CHE_Allocator * pAllocator = NULL ) 
		: CHE_PDF_ContentObject( CONTENTOBJ_ORDER, pAllocator )
	{ m_order = ORDER_INITGRAPHSTATE; m_pParam = NULL; }
	
	~CHE_PDF_OrderObject() {}

	HE_BYTE			m_order;
	CHE_PDF_OrderParam * m_pParam;

	friend class CHE_PDF_ContentObject;
	friend class CHE_Allocator;
};

#endif