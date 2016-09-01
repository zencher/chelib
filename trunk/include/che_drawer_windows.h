#ifndef _CHE_DRAWER_H_
#define _CHE_DRAWER_H_

#include "che_define.h"
#include "che_graphics.h"
#include "che_bitmap.h"

//windows implementation
#include <Windows.h>
#include <GdiPlus.h>

enum ImageTypeSupport
{
	IMAGE_BMP,
	IMAGE_GIF,
	IMAGE_TIFF,
	IMAGE_PNG,
	IMAGE_JPG
};

class CHE_GraphicsDrawer
{
public:
	CHE_GraphicsDrawer( HDC hDC, size_t dibWidth, size_t dibHeight );
	~CHE_GraphicsDrawer();

	void			Resize( size_t dibWidth, size_t dibHeight );
	size_t		GetWidth() const;
	size_t		GetHeight() const;
	void			Clear();
	HDC				GetMemDC() const;

	bool			GetBitmap( CHE_Bitmap & bitmap );

	//properties setting
	void			SetMatrix( const CHE_Matrix & matrix );
	void			SetExtMatrix( const CHE_Matrix & matrix );
	void			SetLineWidth( const FLOAT & lineWidth );
	void			SetMiterLimit( const FLOAT & miterLimit );
	void			SetFillColor( const size_t & color );
	void			SetStrokeColor( const size_t & color );
	void			SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
	void			SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
	void			SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
	void			SetFillMode( GRAPHICS_STATE_FILLMODE mode );
	void			SetFillAlpha( FLOAT & alpha );
	void			SetStrokeAlpha( FLOAT & alpha );

// 	//properties getting
// 	inline CHE_Matrix	GetMatrix() const;
// 	inline CHE_Matrix	GetExtMatrix() const;
 	FLOAT		GetLineWidth() const;
// 	inline FLOAT		GetMiterLimit() const;
// 	inline size_t		GetFillColor() const;
// 	inline size_t		GetStrokeColor() const;
	 FLOAT		GetDashPhase() const;
 	 GRAPHICS_STATE_LINECAP		GetLineCap() const;
 	 GRAPHICS_STATE_LINEJOIN	GetLineJion() const;
// 	inline GRAPHICS_STATE_DASHPATTERN	GetLineDash() const;

	//path & clip operations
	 void		MoveTo( FLOAT x, FLOAT y );
	 void		LineTo( FLOAT x, FLOAT y );
	 void		CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 );
	//inline void	Rectangle( FLOAT x, FLOAT y, FLOAT width, FLOAT height );
	 void		ClosePath();
	 void		FillPath();
	 void		StrokePath();
	 void		FillStrokePath();
	 void		ClipPath();
	 void		FillClipPath();
	 void		StrokeClipPath();
	 void		FillStrokeClipPath();
	 void		ResetClip();

	 //void		DrawBitmap( size_t width, size_t height, BYTE bpc, PBYTE pData, size_t size );
	 void		DrawImage( ImageTypeSupport imgType, PBYTE data, size_t size );
	 void		DrawBitmap( CHE_Bitmap * pBitmap );
	
private:
	HDC							m_DC;
	HDC							m_MemDC;
	HBITMAP						m_Bitmap;
	HGDIOBJ						m_OldBitmap;
	size_t					m_dwWidth;
	size_t					m_dwHeight;

	Gdiplus::Graphics *			m_pGraphics;
	Gdiplus::Pen *				m_pPen;
	Gdiplus::Brush *			m_pBrush;
	Gdiplus::GraphicsPath		m_path;
	Gdiplus::GraphicsPath		m_pathToDraw;

	FLOAT					mStrokeAlpha;
	FLOAT					mFillAlpha;
	FLOAT					mCurX;
	FLOAT					mCurY;
	FLOAT					mBeginX;
	FLOAT					mBeginY;
	FLOAT					mLineWidth;
	GRAPHICS_STATE_LINECAP		mLineCap;
	GRAPHICS_STATE_LINEJOIN		mLineJion;
	FLOAT					mDashPhase;
	CHE_Matrix					mExtMatrix;
};


#endif