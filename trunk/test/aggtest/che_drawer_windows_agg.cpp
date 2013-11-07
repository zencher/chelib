#include "../include/che_drawer_windows_agg.h"

#include "../extlib/agg/include/agg_vcgen_stroke.cpp"

CHE_GraphicsDrawer::CHE_GraphicsDrawer( CHE_Bitmap * pBitmap )
	: mPixFmtRgb24( mRenBuf ), mPixFmtRgba32( mRenBuf )
{
	mStrokeAlpha = 1;
	mFillAlpha = 1;
	mCurX = 0;
	mCurY = 0;
	mBeginX = 0;
	mBeginY = 0;
	mLineWidth = 1;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mpCanvas = pBitmap;

	mRenBuf.attach( mpCanvas->GetBuffer(), mpCanvas->Width(), mpCanvas->Height(), mpCanvas->Pitch() );
	
	if ( mpCanvas->Depth() == BITMAP_DEPTH_24BPP )
	{
		//mPixFmtRgb24.attach( mRenBuf );
		mRender24.attach( mPixFmtRgb24 );
	}else if ( mpCanvas->Depth() == BITMAP_DEPTH_32BPP )
	{
		//mPixFmtRgba32.attach( mRenBuf );
		mRender32.attach( mPixFmtRgba32 );
	}
}

CHE_GraphicsDrawer::~CHE_GraphicsDrawer()
{
	
}

HE_VOID	CHE_GraphicsDrawer::Resize( HE_ULONG width, HE_ULONG height )
{
	mpCanvas->Clean();
	mpCanvas->Create( width, height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
	mRenBuf.attach( mpCanvas->GetBuffer(), mpCanvas->Width(), mpCanvas->Height(), mpCanvas->Pitch() );
}

HE_VOID	CHE_GraphicsDrawer::Clear()
{
	//mpCanvas->Clean();
	mStrokeAlpha = 1;
	mFillAlpha = 1;
	mCurX = 0;
	mCurY = 0;
	mBeginX = 0;
	mBeginY = 0;
	mLineWidth = 1;
	mMiterLimit = 10;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mLineDash.dashPhase = 0;
	mLineDash.dashArray.clear();
}

HE_VOID CHE_GraphicsDrawer::MoveTo( HE_FLOAT x, HE_FLOAT y )
{
	mPath.move_to( x, y );
	mBeginX = mCurX = x;
	mBeginY = mCurY = y;
}

HE_VOID CHE_GraphicsDrawer::LineTo( HE_FLOAT x, HE_FLOAT y )
{
	mPath.line_to( x, y );
	mCurX = x;
	mCurY = y;
}

HE_VOID CHE_GraphicsDrawer::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
	mPath.curve4( x1, y1, x2, y2, x3, y3 );
	mCurX = x3;
	mCurY = y3;
}

HE_VOID CHE_GraphicsDrawer::ClosePath()
{
	mPath.close_polygon();
	mCurX = mBeginX;
	mCurY = mBeginY;
}

HE_VOID CHE_GraphicsDrawer::FillPath()
{
	agg::scanline_u8 sl;
	if ( mFillMode == FillMode_EvenOdd )
	{
		mRaster.filling_rule( agg::fill_even_odd );
	}else{
		mRaster.filling_rule( agg::fill_non_zero );
	}
	mRaster.add_path( mPath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID CHE_GraphicsDrawer::StrokePath()
{
	agg::scanline_u8 sl;
	agg::conv_stroke<agg::path_storage> strokePath( mPath );
	strokePath.width( mLineWidth );
	//strokePath.miter_limit( mMiterLimit );

	switch( mLineCap )
	{
	case LineCap_Butt:
		strokePath.line_cap( agg::butt_cap );
		break;
	case LineCap_Round:
		strokePath.line_cap( agg::round_cap );
		break;
	case LineCap_Square:
		strokePath.line_cap( agg::square_cap );
		break;
	default:break;
	}

	switch( mLineJion )
	{
	case LineJoin_Miter:
		strokePath.line_join( agg::miter_join );
		break;
	case LineJoin_Round:
		strokePath.line_join( agg::round_join );
		break;
	case LineJoin_Bevel:
		strokePath.line_join( agg::bevel_join );
		break;
	default:break;
	}

	if ( mLineDash.dashArray.size() > 0 )
	{

	}

	mRaster.add_path( strokePath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID CHE_GraphicsDrawer::FillStrokePath()
{
	agg::scanline_u8 sl;
	mRaster.add_path( mPath );
	if ( mFillMode == FillMode_EvenOdd )
	{
		mRaster.filling_rule( agg::fill_even_odd );
	}else{
		mRaster.filling_rule( agg::fill_non_zero );
	}
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();

	agg::conv_stroke<agg::path_storage> strokePath( mPath );
	strokePath.width( mLineWidth );
	strokePath.miter_limit( mMiterLimit );

	switch( mLineCap )
	{
	case LineCap_Butt:
		strokePath.line_cap( agg::butt_cap );
		break;
	case LineCap_Round:
		strokePath.line_cap( agg::round_cap );
		break;
	case LineCap_Square:
		strokePath.line_cap( agg::square_cap );
		break;
	default:break;
	}

	switch( mLineJion )
	{
	case LineJoin_Miter:
		strokePath.line_join( agg::miter_join );
		break;
	case LineJoin_Round:
		strokePath.line_join( agg::round_join );
		break;
	case LineJoin_Bevel:
		strokePath.line_join( agg::bevel_join );
		break;
	default:break;
	}

	if ( mLineDash.dashArray.size() > 0 )
	{

	}

	mRaster.add_path( strokePath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID CHE_GraphicsDrawer::ClipPath()
{
// 	if ( m_path.GetPointCount() > 0 )
// 	{
// 		m_pathToDraw.AddPath( &m_path, false );
// 		m_path.Reset();
// 	}
// 	if ( m_pathToDraw.GetPointCount() > 0 )
// 	{
// 		m_pGraphics->SetClip( &m_pathToDraw, Gdiplus::CombineModeIntersect );
// 		m_pathToDraw.Reset();
// 	}
	mPath.remove_all();
}

HE_VOID	CHE_GraphicsDrawer::FillClipPath()
{
	agg::scanline_u8 sl;
	if ( mFillMode == FillMode_EvenOdd )
	{
		mRaster.filling_rule( agg::fill_even_odd );
	}else{
		mRaster.filling_rule( agg::fill_non_zero );
	}
	mRaster.add_path( mPath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID	CHE_GraphicsDrawer::StrokeClipPath()
{
	agg::scanline_u8 sl;
	agg::conv_stroke<agg::path_storage> strokePath( mPath );
	strokePath.width( mLineWidth );
	strokePath.miter_limit( mMiterLimit );

	switch( mLineCap )
	{
	case LineCap_Butt:
		strokePath.line_cap( agg::butt_cap );
		break;
	case LineCap_Round:
		strokePath.line_cap( agg::round_cap );
		break;
	case LineCap_Square:
		strokePath.line_cap( agg::square_cap );
		break;
	default:break;
	}

	switch( mLineJion )
	{
	case LineJoin_Miter:
		strokePath.line_join( agg::miter_join );
		break;
	case LineJoin_Round:
		strokePath.line_join( agg::round_join );
		break;
	case LineJoin_Bevel:
		strokePath.line_join( agg::bevel_join );
		break;
	default:break;
	}

	if ( mLineDash.dashArray.size() > 0 )
	{

	}

	mRaster.add_path( strokePath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID	CHE_GraphicsDrawer::FillStrokeClipPath()
{
	agg::scanline_u8 sl;
	if ( mFillMode == FillMode_EvenOdd )
	{
		mRaster.filling_rule( agg::fill_even_odd );
	}else{
		mRaster.filling_rule( agg::fill_non_zero );
	}
	mRaster.add_path( mPath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();

	agg::conv_stroke<agg::path_storage> strokePath( mPath );
	strokePath.width( mLineWidth );
	strokePath.miter_limit( mMiterLimit );

	switch( mLineCap )
	{
	case LineCap_Butt:
		strokePath.line_cap( agg::butt_cap );
		break;
	case LineCap_Round:
		strokePath.line_cap( agg::round_cap );
		break;
	case LineCap_Square:
		strokePath.line_cap( agg::square_cap );
		break;
	default:break;
	}

	switch( mLineJion )
	{
	case LineJoin_Miter:
		strokePath.line_join( agg::miter_join );
		break;
	case LineJoin_Round:
		strokePath.line_join( agg::round_join );
		break;
	case LineJoin_Bevel:
		strokePath.line_join( agg::bevel_join );
		break;
	default:break;
	}

	if ( mLineDash.dashArray.size() > 0 )
	{

	}

	mRaster.add_path( strokePath );
	agg::render_scanlines_aa_solid( mRaster, sl, mRender24, agg::rgba8( 0, 0, 0, 255 ) );
	mRaster.reset();
	mPath.remove_all();
}

HE_VOID	CHE_GraphicsDrawer::ResetClip()
{
	//m_pGraphics->ResetClip();
	mPath.remove_all();
}

HE_VOID CHE_GraphicsDrawer::SetMatrix( const CHE_Matrix & matrix )
{
	CHE_Matrix t = matrix;
	t.Concat( mExtMatrix );

	agg::trans_affine mtx( t.a, t.b, t.c, t.d, t.e, t.f );
	mPath.transform_all_paths( mtx );
}

HE_VOID CHE_GraphicsDrawer::SetExtMatrix( const CHE_Matrix & matrix )
{
	mExtMatrix = matrix;
}

HE_VOID CHE_GraphicsDrawer::SetLineWidth( const HE_FLOAT & lineWidth )
{
	mLineWidth = lineWidth;
}

HE_VOID CHE_GraphicsDrawer::SetMiterLimit( const HE_FLOAT & miterLimit )
{
	mMiterLimit = miterLimit;
}

HE_VOID CHE_GraphicsDrawer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{	
	mLineCap = lineCap;
}

HE_VOID CHE_GraphicsDrawer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
	mLineJion = lineJion;
}

HE_VOID CHE_GraphicsDrawer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
	mLineDash = dashPattern;
}

HE_VOID CHE_GraphicsDrawer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
	mFillMode = mode;
}

HE_VOID CHE_GraphicsDrawer::SetFillAlpha( HE_FLOAT & alpha )
{
	mFillAlpha = alpha;
}

HE_VOID	CHE_GraphicsDrawer::SetStrokeAlpha( HE_FLOAT & alpha )
{
	mStrokeAlpha = alpha;
}

HE_VOID CHE_GraphicsDrawer::SetFillColor( const HE_ULONG & color )
{
}

HE_VOID CHE_GraphicsDrawer::SetStrokeColor( const HE_ULONG & color )
{
}

HE_VOID	CHE_GraphicsDrawer::DrawBitmap( CHE_Bitmap * pBitmap )
{
}