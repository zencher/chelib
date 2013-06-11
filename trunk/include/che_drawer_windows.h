#ifndef _CHE_DRAWER_H_
#define _CHE_DRAWER_H_

#include "che_define.h"
#include "che_graphics.h"

//windows implementation
#include <Windows.h>
#include <GdiPlus.h>

class CHE_GraphicsDrawer
{
public:

	CHE_GraphicsDrawer( HDC hDC, HE_ULONG dibWidth, HE_ULONG dibHeight );
	~CHE_GraphicsDrawer();

	HE_VOID			Resize( HE_ULONG dibWidth, HE_ULONG dibHeight );
	HE_ULONG		GetWidth() const;
	HE_ULONG		GetHeight() const;
	HE_VOID			Clear();
	HDC				GetMemDC() const;
	

	//properties setting
	HE_VOID			SetMatrix( const CHE_Matrix & matrix );
	HE_VOID			SetExtMatrix( const CHE_Matrix & matrix );
	HE_VOID			SetLineWidth( const HE_FLOAT & lineWidth );
	HE_VOID			SetMiterLimit( const HE_FLOAT & miterLimit );
	HE_VOID			SetFillColor( const HE_ULONG & color );
	HE_VOID			SetStrokeColor( const HE_ULONG & color );
	HE_VOID			SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
	HE_VOID			SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
	HE_VOID			SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
	HE_VOID			SetFillMode( GRAPHICS_STATE_FILLMODE mode );

// 	//properties getting
// 	inline CHE_Matrix	GetMatrix() const;
// 	inline CHE_Matrix	GetExtMatrix() const;
 	HE_FLOAT		GetLineWidth() const;
// 	inline HE_FLOAT		GetMiterLimit() const;
// 	inline HE_ULONG		GetFillColor() const;
// 	inline HE_ULONG		GetStrokeColor() const;
	 HE_FLOAT		GetDashPhase() const;
 	 GRAPHICS_STATE_LINECAP		GetLineCap() const;
 	 GRAPHICS_STATE_LINEJOIN	GetLineJion() const;
// 	inline GRAPHICS_STATE_DASHPATTERN	GetLineDash() const;

	//path & clip operations
	 HE_VOID		MoveTo( HE_FLOAT x, HE_FLOAT y );
	 HE_VOID		LineTo( HE_FLOAT x, HE_FLOAT y );
	 HE_VOID		CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 );
	//inline HE_VOID	Rectangle( HE_FLOAT x, HE_FLOAT y, HE_FLOAT width, HE_FLOAT height );
	 HE_VOID		ClosePath();
	 HE_VOID		FillPath();
	 HE_VOID		StrokePath();
	 HE_VOID		FillStrokePath();
	 HE_VOID		ClipPath();
	 HE_VOID		FillClipPath();
	 HE_VOID		StrokeClipPath();
	 HE_VOID		FillStrokeClipPath();
	 HE_VOID		ResetClip();
	
private:
	HDC							m_DC;
	HDC							m_MemDC;
	HBITMAP						m_Bitmap;
	HGDIOBJ						m_OldBitmap;
	HE_ULONG					m_dwWidth;
	HE_ULONG					m_dwHeight;

	Gdiplus::Graphics *			m_pGraphics;
	Gdiplus::Pen *				m_pPen;
	Gdiplus::Brush *			m_pBrush;
	Gdiplus::GraphicsPath		m_path;
	Gdiplus::GraphicsPath		m_pathToDraw;

	HE_FLOAT					mCurX;
	HE_FLOAT					mCurY;
	HE_FLOAT					mBeginX;
	HE_FLOAT					mBeginY;
	HE_FLOAT					mLineWidth;
	GRAPHICS_STATE_LINECAP		mLineCap;
	GRAPHICS_STATE_LINEJOIN		mLineJion;
	HE_FLOAT					mDashPhase;
	CHE_Matrix					mExtMatrix;
};


#endif