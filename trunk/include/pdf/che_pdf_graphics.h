#ifndef _CHE_PDF_GRAPHICS_H_
#define _CHE_PDF_GRAPHICS_H_

#include "../che_base.h"
#include "../che_graphics.h"

// enum PDF_PATH_TYPE
// {
// 	PATH_NOOP			= 0x00,
// 	PATH_STROKE			= 0x01,
// 	PATH_FILL			= 0x02,
// 	PATH_FILLSTROKE		= 0x03,
// 	PATH_NOOP_CLIP		= 0x04,
// 	PATH_STROKE_CLIP	= 0x05,
// 	PATH_FILL_CLIP		= 0x06,
// 	PATH_FILLSTROKE_CLIP= 0x07
// };

// enum PDF_FILL_MODE
// {
// 	FILL_MODE_NOZERO	= 0x00,
// 	FILL_MODE_EVERODD	= 0x01
// };

// enum PDF_ORDER_TYPE
// {
// 	ORDER_UNKNOWN		= 0x00,
// 	ORDER_INIT_STATE	= 0x01,
// 
// 	//General graphics state
// 	ORDER_LINE_WIDTH	= 0x02,
// 	ORDER_LINE_CAP		= 0x03,
// 	ORDER_LINE_JION		= 0x04,
// 	ORDER_MITER_LIMIT	= 0x05,
// 	ORDER_DASH_PATTERN	= 0x06,
// 	ORDER_INTENT		= 0x07,
// 	ORDER_FALTNESS		= 0x08,
// 	ORDER_SET_STATE		= 0x09,
// 
// 	//Special graphics state
// 	ORDER_PUSH_STATE	= 0x0A,
// 	ORDER_POP_STATE		= 0x0B,
// 	ORDER_MATRIX		= 0x0C,
// 
// 	//Color
// 	ORDER_STROKE_COLOR	= 0x0D,
// 	ORDER_FILL_COLOR	= 0x0E
// };

class IHE_PDF_DrawGraphics : public CHE_Object
{
public:
	IHE_PDF_DrawGraphics( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_PDF_DrawGraphics() {};

	virtual HE_VOID SetMatrixToDevice( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f ) = 0;

	virtual HE_FLOAT GetX( HE_FLOAT x, HE_FLOAT y ) = 0;

	virtual HE_FLOAT GetY( HE_FLOAT x, HE_FLOAT y ) = 0;

// 	virtual HE_VOID DrawPath( CHE_PDF_PathObject * pPath ) = 0;
// 
// 	virtual HE_VOID DrawText( CHE_PDF_TextObject * pText ) = 0;
// 
// 	virtual HE_VOID Execute( CHE_PDF_OrderObject * pOrder ) = 0;
};

#endif