#include "../include/che_image.h"
#include "../include/pdf/che_pdf_colorspace.h"

#include "../extlib/libjpeg/jpeglib.h"
#include "../extlib/openjpeg/openjpeg.h"
#include "../extlib/jbig2dec/jbig2.h"

CHE_ImageDescriptor::CHE_ImageDescriptor( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mWidth(0), mHeight(0), mComponent(0), mBitpc(0), mData(NULL), mSize(0) {}

CHE_ImageDescriptor::~CHE_ImageDescriptor()
{
	if ( mData )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( mData );
		mData = NULL;
	}
}

HE_VOID CHE_ImageDescriptor::AllocData( HE_ULONG size )
{
	if ( mData )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( mData );
		mData = NULL;
	}
	mData = GetAllocator()->NewArray<HE_BYTE>( size );
	mSize = size;
}

// #include <jpeglib.h>
// #include <setjmp.h>

struct jpeg_error_mgr_jmp
{
	struct jpeg_error_mgr super;
	jmp_buf env;
	char msg[JMSG_LENGTH_MAX];
};

static void error_exit(j_common_ptr cinfo)
{
	struct jpeg_error_mgr_jmp *err = (struct jpeg_error_mgr_jmp *)cinfo->err;
	cinfo->err->format_message(cinfo, err->msg);
	longjmp(err->env, 1);
}

static void init_source(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static void term_source(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	static unsigned char eoi[2] = { 0xFF, JPEG_EOI };
	struct jpeg_source_mgr *src = cinfo->src;
	src->next_input_byte = eoi;
	src->bytes_in_buffer = 2;
	return 1;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr *src = cinfo->src;
	if (num_bytes > 0)
	{
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
	}
}

HE_BOOL image_decode_jpeg( HE_LPBYTE data, HE_ULONG size, CHE_ImageDescriptor & desRet )
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr_jmp err;
	struct jpeg_source_mgr src;
	unsigned char *row[1], *sp, *dp;
	/*fz_colorspace *colorspace;*/
	unsigned int x;
	int k;

	if (setjmp(err.env))
	{
// 		if (image)
// 			fz_drop_pixmap(ctx, image);
// 		fz_throw(ctx, "jpeg error: %s", err.msg);
	}

 	cinfo.err = jpeg_std_error(&err.super);
 	err.super.error_exit = error_exit;

	jpeg_create_decompress(&cinfo);

	cinfo.src = &src;
	src.init_source = init_source;
	src.fill_input_buffer = fill_input_buffer;
	src.skip_input_data = skip_input_data;
	src.resync_to_restart = jpeg_resync_to_restart;
	src.term_source = term_source;
	src.next_input_byte = data;
	src.bytes_in_buffer = size;

	jpeg_read_header(&cinfo, 1);

	jpeg_start_decompress(&cinfo);

// 	if (cinfo.output_components == 1)
// 		colorspace = fz_device_gray;
// 	else if (cinfo.output_components == 3)
// 		colorspace = fz_device_rgb;
// 	else if (cinfo.output_components == 4)
// 		colorspace = fz_device_cmyk;
// 	else
// 		fz_throw(ctx, "bad number of components in jpeg: %d", cinfo.output_components);

// 	fz_try(ctx)
// 	{
// 		image = fz_new_pixmap(ctx, colorspace, cinfo.output_width, cinfo.output_height);
// 	}
// 	fz_catch(ctx)
// 	{
//		jpeg_finish_decompress(&cinfo);
//		jpeg_destroy_decompress(&cinfo);
// 		fz_throw(ctx, "out of memory");
// 	}

// 	if (cinfo.density_unit == 1)
// 	{
// 		image->xres = cinfo.X_density;
// 		image->yres = cinfo.Y_density;
// 	}
// 	else if (cinfo.density_unit == 2)
// 	{
// 		image->xres = cinfo.X_density * 254 / 100;
// 		image->yres = cinfo.Y_density * 254 / 100;
// 	}
// 
// 	if (image->xres <= 0) image->xres = 72;
// 	if (image->yres <= 0) image->yres = 72;

/*	fz_clear_pixmap(ctx, image);*/

	row[0] = GetDefaultAllocator()->NewArray<unsigned char>( cinfo.output_components * cinfo.output_width );
	//row[0] = fz_malloc(ctx, cinfo.output_components * cinfo.output_width);
	desRet.AllocData( cinfo.output_components * cinfo.output_height * cinfo.output_width );
	dp = desRet.GetData();
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, row, 1);
		sp = row[0];
		for (x = 0; x < cinfo.output_width; x++)
		{
			for (k = 0; k < cinfo.output_components; k++)
				*dp++ = *sp++;
			*dp++ = 255;
		}
	}
	GetDefaultAllocator()->DeleteArray( row[0] );

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return TRUE;
}

static void fz_opj_error_callback(const char *msg, void *client_data)
{
}

static void fz_opj_warning_callback(const char *msg, void *client_data)
{
}

static void fz_opj_info_callback(const char *msg, void *client_data)
{
}

HE_BOOL image_decode_jpx( HE_LPBYTE data, HE_ULONG size, CHE_ImageDescriptor & desRet )
{    
	opj_event_mgr_t     evtmgr;
	opj_dparameters_t   params;
	opj_dinfo_t *       info = NULL;
	opj_cio_t *         cio = NULL;
	opj_image_t *       jpx = NULL;
    CODEC_FORMAT        format;

	unsigned char *p;

	int a, n, w, h, depth, sgnd;
	int x, y, k, v;
    
	/* Check for SOC marker -- if found we have a bare J2K stream */
	if ( data[0] == 0xFF && data[1] == 0x4F )
    {
		format = CODEC_J2K;
    }else{
		format = CODEC_JP2;
    }
    
	memset( &evtmgr, 0, sizeof(evtmgr) );
	evtmgr.error_handler = fz_opj_error_callback;
	evtmgr.warning_handler = fz_opj_warning_callback;
	evtmgr.info_handler = fz_opj_info_callback;
    
	opj_set_default_decoder_parameters( &params );
	
    //if (indexed) ???
	{
        params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
    }
    
	info = opj_create_decompress( format );
    
	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );
    
	cio = opj_cio_open( (opj_common_ptr)info, data, size );
    
	jpx = opj_decode( info, cio );
    
	opj_cio_close( cio );
	opj_destroy_decompress( info );
    
	if ( !jpx )
    {
        //decode error!!!
        return FALSE;
    }
    
	for (k = 1; k < jpx->numcomps; k++)
	{
		if (jpx->comps[k].w != jpx->comps[0].w)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different width");
		}
		if (jpx->comps[k].h != jpx->comps[0].h)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different height");
		}
		if (jpx->comps[k].prec != jpx->comps[0].prec)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different precision");
		}
	}
    
	n = jpx->numcomps;
	w = jpx->comps[0].w;
	h = jpx->comps[0].h;
	depth = jpx->comps[0].prec;
	sgnd = jpx->comps[0].sgnd;

	desRet.AllocData( h * w * n );
	p = desRet.GetData();

	desRet.SetWidth( jpx->comps[0].w );
	desRet.SetHeight( jpx->comps[0].h );
	desRet.SetComponentCount( n );
	desRet.SetBitsPerComponent( depth );
    
	if (jpx->color_space == CLRSPC_SRGB && n == 4) { n = 3; a = 1; }
	else if (jpx->color_space == CLRSPC_SYCC && n == 4) { n = 3; a = 1; }
	else if (n == 2) { n = 1; a = 1; }
	else if (n > 4) { n = 4; a = 1; }
	else { a = 0; }
    
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			for (k = 0; k < n + a; k++)
			{
				v = jpx->comps[k].data[y * w + x];
				if (sgnd)
					v = v + (1 << (depth - 1));
				if (depth > 8)
					v = v >> (depth - 8);
				*p++ = v;
			}
			if (!a)
				*p++ = 255;
		}
	}
 
	opj_image_destroy( jpx );
	
	return TRUE;
}

HE_BOOL image_decode_jbig2( HE_LPBYTE data, HE_ULONG size, CHE_ImageDescriptor & desRet )
{
	Jbig2Ctx *ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL );
	Jbig2GlobalCtx *gctx = NULL;
	Jbig2Image *page = NULL;

// 	if ( mGlobalsParam )
// 	{
// 		jbig2_data_in( ctx, mGlobalsParam, mGlobalsParamLength );
// 		gctx = jbig2_make_global_ctx( ctx );
// 		ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, gctx, NULL, NULL );
// 	}

	if ( !page )
	{
		jbig2_data_in( ctx, data, size );
		jbig2_complete_page( ctx );
		page = jbig2_page_out( ctx );
		//if page = NULL error happened!
	}

	desRet.AllocData( page->height * page->stride );
	desRet.SetWidth( page->width );
	desRet.SetHeight( page->height );
	desRet.SetComponentCount( 1 );
	desRet.SetBitsPerComponent( page->stride * 8 / page->width );

	HE_LPBYTE p = desRet.GetData();
	HE_LPBYTE ep = desRet.GetData() + desRet.GetSize();
	HE_LPBYTE s = page->data;
	HE_INT32 w = page->height * page->stride;
	HE_INT32 x = 0;

	while (p < ep && x < w)
	{
		*p++ = s[x++] ^ 0xff;
	}

	if ( page )
	{
		jbig2_release_page( ctx, page );
	}
	if ( gctx )
	{
		jbig2_global_ctx_free( gctx );
	}
	if ( ctx )
	{
		jbig2_ctx_free( ctx );
	}

	return TRUE;
}

CHE_ImageDecoder::CHE_ImageDecoder( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mDecodeParam(NULL), mParamSize(0) {}

HE_VOID CHE_ImageDecoder::SetDecodeParam( HE_LPBYTE data, HE_ULONG size )
{
	mDecodeParam = data;
	mParamSize = size;
}

HE_BOOL CHE_ImageDecoder::Decode( HE_IMAGE_TYPE type, HE_LPBYTE data, HE_ULONG size, CHE_ImageDescriptor & desRet )
{
	if ( data == NULL || size < 2 )
	{
		return FALSE;
	}

	switch ( type )
	{
	case IMAGE_TYPE_JPEG:
		return image_decode_jpeg( data, size, desRet );
	case IMAGE_TYPE_JPX:
		return image_decode_jpx( data, size, desRet );
	case IMAGE_TYPE_JBIG2:
		return image_decode_jbig2( data, size, desRet );
	case IMAGE_TYPE_TIFF:
	case IMAGE_TYPE_PNG:
		break;
	}
	return FALSE;
}

CHE_Bitmap * image_decode_jpeg_to_bitmap( HE_LPBYTE data, HE_ULONG size )
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_source_mgr src;
	struct jpeg_error_mgr err;
/*	jmp_buf jb;*/

	unsigned char *row[1], *sp, *dp;
	/*fz_colorspace *colorspace;*/
	unsigned int x;
	int k;

	CHE_PDF_ColorSpace * colorspace = NULL;

	cinfo.err = &err;
	jpeg_std_error(cinfo.err);
	cinfo.err->error_exit = error_exit;
	cinfo.client_data = NULL;

	jpeg_create_decompress(&cinfo);

	cinfo.src = &src;
	src.init_source = init_source;
	src.fill_input_buffer = fill_input_buffer;
	src.skip_input_data = skip_input_data;
	src.resync_to_restart = jpeg_resync_to_restart;
	src.term_source = term_source;
	src.next_input_byte = data;
	src.bytes_in_buffer = size;

	jpeg_read_header(&cinfo, 1);

	/* speed up jpeg decoding a bit */
	cinfo.dct_method = JDCT_FASTEST;
	cinfo.do_fancy_upsampling = FALSE;

	jpeg_start_decompress(&cinfo);

	if (cinfo.output_components == 4)
		colorspace = CHE_PDF_ColorSpace::Create( "DeviceCMYK", GetDefaultAllocator() );
 	else if (cinfo.output_components == 3)
 		colorspace = CHE_PDF_ColorSpace::Create( "DeviceRGB", GetDefaultAllocator() );
 	else if (cinfo.output_components == 1)
 		colorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetDefaultAllocator() );
	// 	else
	// 		fz_throw(ctx, "bad number of components in jpeg: %d", cinfo.output_components);

	// 	fz_try(ctx)
	// 	{
	// 		image = fz_new_pixmap(ctx, colorspace, cinfo.output_width, cinfo.output_height);
	// 	}
	// 	fz_catch(ctx)
	// 	{
	//jpeg_finish_decompress(&cinfo);
	//jpeg_destroy_decompress(&cinfo);
	// 		fz_throw(ctx, "out of memory");
	// 	}

	// 	if (cinfo.density_unit == 1)
	// 	{
	// 		image->xres = cinfo.X_density;
	// 		image->yres = cinfo.Y_density;
	// 	}
	// 	else if (cinfo.density_unit == 2)
	// 	{
	// 		image->xres = cinfo.X_density * 254 / 100;
	// 		image->yres = cinfo.Y_density * 254 / 100;
	// 	}
	// 
	// 	if (image->xres <= 0) image->xres = 72;
	// 	if (image->yres <= 0) image->yres = 72;

	/*	fz_clear_pixmap(ctx, image);*/

	row[0] = GetDefaultAllocator()->NewArray<unsigned char>( cinfo.output_components * cinfo.output_width );
	//row[0] = fz_malloc(ctx, cinfo.output_components * cinfo.output_width);

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );

	if ( cinfo.output_components == 1 )
	{
		pBitmap->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_8BPP, BITMAP_DIRECTION_DOWN );
	}else{
		pBitmap->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
	}
	
	if ( colorspace )
	{
		CHE_PDF_Color color;
		HE_ARGB colorARGB = 0xFF000000;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, row, 1);
			sp = row[0];
			for (x = 0; x < cinfo.output_width; x++)
			{
				color.Clear();
				for (k = 0; k < cinfo.output_components; k++)
				{
					color.Push( ( *sp ) / 255.0f );
					sp++;
				}
				colorARGB = colorspace->GetARGBValue( color );
				pBitmap->SetPixelColor( x, cinfo.output_scanline, colorARGB ); 
			}
		}
	}else{
		dp = (HE_LPBYTE)( pBitmap->GetBuffer() );
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, row, 1);
			sp = row[0];
			for (x = 0; x < cinfo.output_width; x++)
			{
				for (k = 0; k < cinfo.output_components; k++)
					*dp++ = *sp++;
				*dp++ = 255;
			}
		}
	}
	
	GetDefaultAllocator()->DeleteArray( row[0] );

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return pBitmap;
}

CHE_Bitmap * image_decode_jpx_to_bitmap( HE_LPBYTE data, HE_ULONG size )
{    
	opj_event_mgr_t     evtmgr;
	opj_dparameters_t   params;
	opj_dinfo_t *       info = NULL;
	opj_cio_t *         cio = NULL;
	opj_image_t *       jpx = NULL;
	CODEC_FORMAT        format;

	int a, n, w, h, depth, sgnd;
	int x, y, k, v;

	/* Check for SOC marker -- if found we have a bare J2K stream */
	if ( data[0] == 0xFF && data[1] == 0x4F )
	{
		format = CODEC_J2K;
	}else{
		format = CODEC_JP2;
	}

	memset( &evtmgr, 0, sizeof(evtmgr) );
	evtmgr.error_handler = fz_opj_error_callback;
	evtmgr.warning_handler = fz_opj_warning_callback;
	evtmgr.info_handler = fz_opj_info_callback;

	opj_set_default_decoder_parameters( &params );

	//if (indexed) ???
	{
		params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
	}

	info = opj_create_decompress( format );

	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );

	cio = opj_cio_open( (opj_common_ptr)info, data, size );

	jpx = opj_decode( info, cio );

	opj_cio_close( cio );
	opj_destroy_decompress( info );

	if ( !jpx )
	{
		//decode error!!!
		return FALSE;
	}

	for (k = 1; k < jpx->numcomps; k++)
	{
		if (jpx->comps[k].w != jpx->comps[0].w)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different width");
		}
		if (jpx->comps[k].h != jpx->comps[0].h)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different height");
		}
		if (jpx->comps[k].prec != jpx->comps[0].prec)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different precision");
		}
	}

	n = jpx->numcomps;
	w = jpx->comps[0].w;
	h = jpx->comps[0].h;
	depth = jpx->comps[0].prec;
	sgnd = jpx->comps[0].sgnd;

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
	pBitmap->Create( w, h, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );

	if (jpx->color_space == CLRSPC_SRGB && n == 4) { n = 3; a = 1; }
	else if (jpx->color_space == CLRSPC_SYCC && n == 4) { n = 3; a = 1; }
	else if (n == 2) { n = 1; a = 1; }
	else if (n > 4) { n = 4; a = 1; }
	else { a = 0; }
	
	HE_ARGB color = 0;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			color = 0;
			for (k = 0; k < n + a ; ++k )
			{
				v = jpx->comps[k].data[y * w + x];
				if (sgnd)
					v = v + (1 << (depth - 1));
				if (depth > 8)
					v = v >> (depth - 8);

				color = (color << 8) + v;
			}
			//if (!a)
			//	*p++ = 255;
			pBitmap->SetPixelColor( x, y, color );
			//pBitmap->ExchangeChannel( BITMAP_CHANNEL_Blue, BITMAP_CHANNEL_Red );
		}
	}

	opj_image_destroy( jpx );

	return pBitmap;
}

CHE_Bitmap * image_decode_jbig2_to_bitmap( HE_LPBYTE data, HE_ULONG size )
{
	Jbig2Ctx *ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL );
	Jbig2GlobalCtx *gctx = NULL;
	Jbig2Image *page = NULL;

	// 	if ( mGlobalsParam )
	// 	{
	// 		jbig2_data_in( ctx, mGlobalsParam, mGlobalsParamLength );
	// 		gctx = jbig2_make_global_ctx( ctx );
	// 		ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, gctx, NULL, NULL );
	// 	}

	if ( !page )
	{
		jbig2_data_in( ctx, data, size );
		jbig2_complete_page( ctx );
		page = jbig2_page_out( ctx );
		//if page = NULL error happened!
	}

	//HE_LPBYTE p = desRet.GetData();
	//HE_LPBYTE ep = desRet.GetData() + desRet.GetSize();
	HE_LPBYTE s = page->data;
	HE_INT32 w = page->height * page->stride;
	HE_INT32 index = 0;

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
	pBitmap->Create( page->width, page->height, BITMAP_DEPTH_1BPP, BITMAP_DIRECTION_DOWN, w, s );

	//for ( HE_INT32 y = 0; y < page->height; ++y )
	//{
	//	for ( HE_INT32 x = 0; x < page->width; ++x )
	//	{
	//		pBitmap->SetPixelColor( x, y, s[y * page->stride + x] ^ 0xff );
	//	}
	//}
// 	while (p < ep && x < w)
// 	{
// 		*p++ = s[x++] ^ 0xff;
// 	}

	if ( page )
	{
		jbig2_release_page( ctx, page );
	}
	if ( gctx )
	{
		jbig2_global_ctx_free( gctx );
	}
	if ( ctx )
	{
		jbig2_ctx_free( ctx );
	}
	return pBitmap;
}

CHE_Bitmap * CHE_ImageDecoder::Decode( HE_IMAGE_TYPE type, HE_LPBYTE data, HE_ULONG size )
{
	if ( data == NULL || size < 2 )
	{
		return NULL;
	}

	switch ( type )
	{
	case IMAGE_TYPE_JPEG:
		return image_decode_jpeg_to_bitmap( data, size );
	case IMAGE_TYPE_JPX:
		return image_decode_jpx_to_bitmap( data, size );
	case IMAGE_TYPE_JBIG2:
		return image_decode_jbig2_to_bitmap( data, size );
	case IMAGE_TYPE_TIFF:
	case IMAGE_TYPE_PNG:
		break;
	}
	return NULL;
}