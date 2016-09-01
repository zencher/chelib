#include "../../include/pdf/che_pdf_image.h"

CHE_PDF_ReferencePtr CHE_PDF_Image::InsertImageToFile(	CHE_PDF_File * pFile, PDF_IMAGE_TYPE type, size_t bpc,
														size_t width, size_t height, PBYTE pData, size_t size,
														CHE_PDF_ReferencePtr mask )
{
	CHE_PDF_ReferencePtr refPtrRet;

	if ( pFile == nullptr )
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

	dictPtr->SetName( "Type", "XObject" );
	dictPtr->SetName( "Subtype", "Image" );
	dictPtr->SetInteger( "Width", (int32)width );
	dictPtr->SetInteger( "Height", (int32)height );
	dictPtr->SetInteger( "BitsPerComponent", (int32)bpc );
	dictPtr->SetName( "ColorSpace", "DeviceRGB" );

	if ( mask )
	{
		dictPtr->SetReference( "SoftMask", mask->GetRefNum(), mask->GetGenNum(), pFile );
	}
	switch ( type )
	{
	case IMAGE_JPEG:
		dictPtr->SetName( "Filter", "DCTDecode" );
		streamPtr->SetRawData( pData, size );
		break;
	case IMAGE_BMP:
		streamPtr->SetRawData( pData, size, STREAM_FILTER_FLATE );
	default:
		break;
	}

	return refPtrRet;
}