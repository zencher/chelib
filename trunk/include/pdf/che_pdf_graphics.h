#ifndef _CHE_PDF_GRAPHICS_H_
#define _CHE_PDF_GRAPHICS_H_

#include "../che_base.h"
#include "che_pdf_graphicsstate.h"

class IHE_PDF_GraphicsDraw
{
public:
// 	virtual HE_VOID SetFlatness( const HE_FLOAT & flatness );


	virtual HE_VOID MoveTo( HE_FLOAT x, HE_FLOAT y ) = 0;
	virtual HE_VOID LineTo( HE_FLOAT x, HE_FLOAT y ) = 0;
	virtual HE_VOID CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 ) = 0;
	virtual HE_VOID ClosePath() = 0;
	virtual HE_VOID FillPath() = 0;
	virtual HE_VOID StrokePath() = 0;
	virtual HE_VOID FillStrokePath() = 0;

	virtual HE_VOID SetMatrix( const CHE_PDF_Matrix & matrix ) = 0;
	virtual HE_VOID	SetLineWidth( const HE_FLOAT & lineWidth ) = 0;
	virtual HE_VOID	SetMiterLimit( const HE_FLOAT & miterLimit ) = 0;
	virtual HE_VOID	SetLineCap( const PDF_GSTATE_LINECAP & lineCap ) = 0;
	virtual HE_VOID	SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion ) = 0;
	virtual HE_VOID	SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern ) = 0;
	virtual HE_VOID SetFillColor( const HE_DWORD & color ) = 0;
	virtual HE_VOID SetStrokeColor( const HE_DWORD & color ) = 0;
};

#endif