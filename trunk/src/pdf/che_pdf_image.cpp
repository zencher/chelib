#include "../../include/pdf/che_pdf_image.h"

CHE_PDF_ReferencePtr CHE_PDF_Image::InsertImageToFile(	CHE_PDF_File * pFile, PDF_IMAGE_TYPE type, HE_ULONG bpc,
														HE_ULONG width, HE_ULONG height, HE_LPBYTE pData, HE_ULONG size,
														CHE_PDF_ReferencePtr mask )
{
	CHE_PDF_ReferencePtr refPtrRet;

	if ( pFile == NULL )
	{
		return refPtrRet;
	}

	PDF_RefInfo refInfo;
	CHE_PDF_DictionaryPtr dictPtr;
	CHE_PDF_StreamPtr streamPtr;

	refInfo = pFile->CreateStreamObject( streamPtr );

	if ( ! streamPtr )
	{
		return refPtrRet;
	}

	refPtrRet = CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, pFile );

	refPtrRet->SetRefInfo( refInfo );

	dictPtr = streamPtr->GetDictPtr();

	dictPtr->SetAtName( "Type", "XObject" );
	dictPtr->SetAtName( "Subtype", "Image" );
	dictPtr->SetAtInteger( "Width", (HE_INT32)width );
	dictPtr->SetAtInteger( "Height", (HE_INT32)height );
	dictPtr->SetAtInteger( "BitsPerComponent", (HE_INT32)bpc );
	dictPtr->SetAtName( "ColorSpace", "DeviceRGB" );

	if ( mask )
	{
		dictPtr->SetAtReference( "SoftMask", mask->GetRefNum(), mask->GetGenNum(), pFile );
	}
	switch ( type )
	{
	case IMAGE_JPEG:
		dictPtr->SetAtName( "Filter", "DCTDecode" );
		streamPtr->SetRawData( pData, size );
		break;
	case IMAGE_BMP:
		streamPtr->SetRawData( pData, size, STREAM_FILTER_FLATE );
	default:
		break;
	}

	return refPtrRet;
}