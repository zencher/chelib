#include <cstdio>
#include <memory.h>

#include "../../include/che_bitmap.h"

#include "../../extlib/agg/include/agg_basics.h"
#include "../../extlib/agg/include/agg_rendering_buffer.h"
#include "../../extlib/agg/include/agg_rasterizer_scanline_aa.h"
#include "../../extlib/agg/include/agg_scanline_u.h"
#include "../../extlib/agg/include/agg_renderer_scanline.h"
#include "../../extlib/agg/include/agg_conv_stroke.h"
#include "../../extlib/agg/include/agg_conv_dash.h"
#include "../../extlib/agg/include/agg_pixfmt_rgb.h"
#include "../../extlib/agg/include/agg_path_storage.h"
#include "../../extlib/agg/include/agg_curves.h"


int main()
{
	CHE_Bitmap bitmap;
	bitmap.Create( 800, 800, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
	bitmap.Fill( 0xFFFF8800 );

	agg::rendering_buffer					buf( bitmap.GetBuffer(), 800, 800, bitmap.Pitch() );
	agg::pixfmt_rgb24						pixfmt( buf );
	agg::renderer_base<agg::pixfmt_rgb24>	render( pixfmt );
	agg::scanline_u8						sl;
	agg::rasterizer_scanline_aa<>			raster;

	raster.reset();

	agg::path_storage psFill;

	psFill.remove_all();
	psFill.move_to( 100, 100 );
	psFill.line_to( 0, 0 );
	psFill.line_to( 500, 100 );
	psFill.line_to( 100, 600 );

	agg::trans_affine mtx( 1, 0, 0, 1, 100, 50 );
	psFill.transform_all_paths( mtx );
	raster.add_path( psFill );
	agg::render_scanlines_aa_solid( raster, sl, render, agg::rgba8( 0, 0, 0, 255 ) );

 	agg::conv_stroke<agg::path_storage> psStroke( psFill );
 	psStroke.width(10);
 	raster.reset();
 	raster.add_path(psStroke);
 	agg::render_scanlines_aa_solid( raster, sl, render, agg::rgba8( 0, 0, 255, 255 ) );


 	agg::curve4_div curve;
 	curve.init( 100, 100, 500, 100, 500, 800, 300, 200 );
 	psFill.remove_all();
 	psFill.concat_path( curve );

	raster.add_path( psFill );

 	agg::render_scanlines_aa_solid( raster, sl, render, agg::rgba8( 255, 255, 255, 255 ) );

	agg::conv_stroke<agg::path_storage> a(psFill);
	a.width( 10 );
	raster.reset();
	raster.add_path(a);

	agg::render_scanlines_aa_solid( raster, sl, render, agg::rgba8( 255, 0, 255, 255 ) );
	
	agg::conv_dash<agg::path_storage> dash(psFill);
	dash.add_dash( 10, 20 );
	agg::conv_stroke<agg::conv_dash<agg::path_storage>> d(dash);
	d.width( 10 );
	raster.reset();
	raster.add_path( d );

	agg::render_scanlines_aa_solid( raster, sl, render, agg::rgba8( 255, 0, 0, 255 ) );

	bitmap.Save( "D:\\aagtest.bmp" );
}