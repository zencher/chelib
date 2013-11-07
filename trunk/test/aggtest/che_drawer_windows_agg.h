#ifndef _CHE_DRAWER_H_
#define _CHE_DRAWER_H_

#include "che_define.h"
#include "che_graphics.h"
#include "che_bitmap.h"

#include "../extlib/agg/include/agg_basics.h"
#include "../extlib/agg/include/agg_rendering_buffer.h"
#include "../extlib/agg/include/agg_rasterizer_scanline_aa.h"
#include "../extlib/agg/include/agg_scanline_u.h"
#include "../extlib/agg/include/agg_renderer_scanline.h"
#include "../extlib/agg/include/agg_conv_stroke.h"
#include "../extlib/agg/include/agg_pixfmt_rgb.h"
#include "../extlib/agg/include/agg_path_storage.h"
#include "../extlib/agg/include/agg_curves.h"


class CHE_GraphicsDrawer
{
public:
	CHE_GraphicsDrawer( CHE_Bitmap * pBitmap );
	~CHE_GraphicsDrawer();

	HE_VOID			Resize( HE_ULONG width, HE_ULONG height );
	HE_VOID			Clear();

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
	HE_VOID			SetFillAlpha( HE_FLOAT & alpha );
	HE_VOID			SetStrokeAlpha( HE_FLOAT & alpha );

// 	//properties getting
// 	inline CHE_Matrix	GetMatrix() const;
// 	inline CHE_Matrix	GetExtMatrix() const;
//  	HE_FLOAT		GetLineWidth() const;
// 	inline HE_FLOAT		GetMiterLimit() const;
// 	inline HE_ULONG		GetFillColor() const;
// 	inline HE_ULONG		GetStrokeColor() const;
// 	 HE_FLOAT		GetDashPhase() const;
//  	 GRAPHICS_STATE_LINECAP		GetLineCap() const;
//  	 GRAPHICS_STATE_LINEJOIN	GetLineJion() const;
// 	inline GRAPHICS_STATE_DASHPATTERN	GetLineDash() const;

	//path & clip operations
	 HE_VOID		MoveTo( HE_FLOAT x, HE_FLOAT y );
	 HE_VOID		LineTo( HE_FLOAT x, HE_FLOAT y );
	 HE_VOID		CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 );
	 HE_VOID		ClosePath();

	 HE_VOID		DrawBitmap( CHE_Bitmap * pBitmap );

	 HE_VOID		FillPath();
	 HE_VOID		StrokePath();
	 HE_VOID		FillStrokePath();
	 HE_VOID		ClipPath();
	 HE_VOID		FillClipPath();
	 HE_VOID		StrokeClipPath();
	 HE_VOID		FillStrokeClipPath();
	 HE_VOID		ResetClip();

private:
	HE_FLOAT								mStrokeAlpha;
	HE_FLOAT								mFillAlpha;
	HE_FLOAT								mCurX;
	HE_FLOAT								mCurY;
	HE_FLOAT								mBeginX;
	HE_FLOAT								mBeginY;
	HE_FLOAT								mLineWidth;
	HE_FLOAT								mMiterLimit;
	GRAPHICS_STATE_LINECAP					mLineCap;
	GRAPHICS_STATE_LINEJOIN					mLineJion;
	GRAPHICS_STATE_DASHPATTERN				mLineDash;
	GRAPHICS_STATE_FILLMODE					mFillMode;
	CHE_Matrix								mExtMatrix;
	CHE_Bitmap *							mpCanvas;

	agg::rendering_buffer					mRenBuf;
	agg::pixfmt_rgb24						mPixFmtRgb24;
	agg::pixfmt_rgba32						mPixFmtRgba32;
	agg::renderer_base<agg::pixfmt_rgb24>	mRender24;
	agg::renderer_base<agg::pixfmt_rgba32>	mRender32;
	agg::rasterizer_scanline_aa<>			mRaster;

	agg::path_storage						mPath;
};


#endif