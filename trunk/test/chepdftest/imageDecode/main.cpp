#include <cstdio>

#include "../../../include/che_bitmap.h"
#include "../../../include/pdf/che_pdf_file.h"
#include "../../../include/pdf/che_pdf_colorspace.h"

int main( int argc, char **argv )
{
	IHE_Read * pFileRead = HE_CreateFileRead( argv[1] );
	if ( pFileRead == NULL )
	{
		return FALSE;
	}

	CHE_PDF_File file;
	file.Open( pFileRead );

	printf( "open file %s\n", argv[1] );

	PDF_RefInfo refInfo;
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr;
	CHE_PDF_StreamPtr stmPtr;
	CHE_PDF_Color color;
	CHE_PDF_ColorSpacePtr csPtr;
	HE_ULONG width = 0;
	HE_ULONG height = 0;
	HE_ULONG bpc = 8;
	HE_ULONG index = 0;

	unsigned long count = file.GetXRefTable()->GetMaxObjNum();
	for ( unsigned long i = 0; i < count; ++i )
	{
		refInfo.objNum = i;
		refInfo.genNum = 0;
		objPtr = file.GetObject( refInfo );
		if ( objPtr && objPtr->GetType() == OBJ_TYPE_STREAM )
		{
			stmPtr = objPtr->GetStreamPtr();
			dictPtr = stmPtr->GetDictPtr();
			objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
			if ( objPtr && objPtr->GetNamePtr()->GetString() == "XObject" )
			{
				objPtr = dictPtr->GetElement( "Subtype", OBJ_TYPE_NAME );
				if ( objPtr && objPtr->GetNamePtr()->GetString() == "Image" )
				{
					objPtr = dictPtr->GetElement( "ColorSpace" );
					if ( objPtr )
					{
						csPtr = CHE_PDF_ColorSpace::Create( objPtr );
					}else{
						csPtr = CHE_PDF_ColorSpace::CreateDeviceGray();
					}
					if ( csPtr )
					{
						objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
						if ( objPtr )
						{
							width = objPtr->GetNumberPtr()->GetInteger();
								
							objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								height = objPtr->GetNumberPtr()->GetInteger();
								
								objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
								if ( objPtr )
								{
									bpc = objPtr->GetNumberPtr()->GetInteger();
								}

								printf( "object index %d is a image\n", i );
								printf( "image width %d, image height %d\n", width, height );

								CHE_PDF_StreamAcc stmAcc;
								stmAcc.Attach( stmPtr );

								HE_LPBYTE pByte = stmAcc.GetData();
								HE_ULONG byteSize = stmAcc.GetSize();
								HE_ULONG offset = 0;
								CHE_Bitmap bitmap;
								HE_ARGB argb = 0;
								HE_ULONG cscc = csPtr->GetComponentCount();
								bitmap.Create( width, height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );

								if ( bpc == 8 )
								{
									for ( unsigned int y = 0; y < height; ++y )
									{
										for ( unsigned int x = 0; x < width; ++x )
										{
											for ( unsigned int f = 0; f < cscc; ++f )
											{
												color.Push( *(pByte + offset) / 255.0 );
												++offset;
											}
											argb = csPtr->GetARGBValue( color );
											color.Clear();
											bitmap.SetPixelColor( x, y, argb );
										}
									}
								}else
								{
									unsigned int byteIndex = 0;
									unsigned int byteCount = width * bpc / 8;
									BYTE byte = 0;
									float v = 0;
									if ( width * bpc % 8 != 0 )
									{
										++byteCount;
									}
									for ( unsigned int y = 0; y < height; ++y )
									{
										byteIndex = y * width * bpc / 8;
										for ( unsigned int m = 0; m < byteCount; ++m )
										{
											byte = *(pByte + byteIndex + m);
											for ( unsigned int n = 0; n < (8/bpc); ++n )
											{
												v = (byte >> ((8-bpc)-n)) & (2^bpc - 1);
												color.Push( v );
												argb = csPtr->GetARGBValue( color );
												color.Clear();
												bitmap.SetPixelColor( m * 8 + n, y, argb );
											}
										}
									}
								}
								char tmpStr[128];
								sprintf( tmpStr, "D:\\%d.bmp", index++ );
								bitmap.Save( tmpStr );

								printf( "out put image %d.bmp\n", index-1 );
							}
						}
					}
				}
			}
		}
	}

	file.Close();
	
	HE_DestoryIHERead( pFileRead );
	pFileRead = NULL;

	return 0;
}