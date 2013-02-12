#ifndef _CHE_PDF_GRAPHICS_H_
#define _CHE_PDF_GRAPHICS_H_

#include "../che_base.h"
#include "che_pdf_graphicsstate.h"

class IHE_PDF_GraphicsDraw /*: public CHE_Object*/
{
public:
// 	virtual HE_VOID SetMatrix( const CHE_PDF_Matrix & matirx );
// 	virtual HE_VOID SetFlatness( const HE_FLOAT & flatness );
// 	virtual HE_BOOL SetFillColor( const HE_ARGB & color );
// 	virtual HE_BOOL SetStrokeColor( const HE_ARGB & color );
// 	virtual HE_VOID	SetLineWidth( const HE_FLOAT & lineWidth );
// 	virtual HE_VOID	SetMiterLimit( const HE_FLOAT & miterLimit );
// 	virtual HE_VOID	SetLineCap( const PDF_GSTATE_LINECAP & lineCap );
// 	virtual HE_VOID	SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion );
// 	virtual HE_VOID	SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern );

	virtual HE_VOID MoveTo( HE_FLOAT x, HE_FLOAT y ) = 0;
	virtual HE_VOID LineTo( HE_FLOAT x, HE_FLOAT y ) = 0;
	virtual HE_VOID CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 ) = 0;
	virtual HE_VOID ClosePath() = 0;
	virtual HE_VOID FillPath() = 0;

};

#endif