#include "../../include/che_dynBuffer.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/che_bitmap.h"
#include <stdio.h>
#include <memory.h>

int main()
{
	//CHE_DynBuffer dynBuffer;

	//CHE_PDF_RLEFileter filter;
	//CHE_PDF_FlateFilter filter;
	//CHE_PDF_ASCII85Filter filter;
	//CHE_PDF_LZWFilter lzwfilter;
	//CHE_PDF_HexFilter filter;
	//HE_BYTE	tmpByte[18] = { 120, 156, 50, 52, 50, 54, 49, 77, 76, 74, 78, 73, 5, 0, 0, 0, 255, 255    /*'t', 'O', 'A', '2', '#', '~', '>'*//*'0', 'e', 't', 'O', 'A', '2', '.', '9', 's', '(', 'A', '7', 'Y', '~', '>'*/ };
	//HE_BYTE	tmpByte[10] = { '1', '1', '3', '4', '5' , 'a', 'a', 'c', 'c', 'e' };
	//HE_BYTE buffer[30];
	//HE_BYTE buffer2[20];

	/*IHE_FileRead * pFileRead = HE_CreateFileRead( "c:\\1.txt" );
	HE_LPBYTE pTmp = new HE_BYTE[pFileRead->GetSize()];
	pFileRead->ReadBlock( pTmp, 0, pFileRead->GetSize() );
	filter.Decode( pTmp, pFileRead->GetSize(), dynBuffer );

	CHE_DynBuffer dynBuffer2;

	HE_LPBYTE pTmp2 = new HE_BYTE[dynBuffer.GetByteCount()];
	dynBuffer.Read( pTmp2, dynBuffer.GetByteCount() );
	lzwfilter.Decode( pTmp2, dynBuffer.GetByteCount(), dynBuffer2 );
	HE_LPBYTE pTmp3 = new HE_BYTE[dynBuffer2.GetByteCount()];
	dynBuffer2.Read( pTmp3, dynBuffer2.GetByteCount() );

	FILE * pFile = fopen( "c:\\1d.txt", "wb+" );
	fwrite( pTmp3, 1, dynBuffer2.GetByteCount(), pFile );
	fclose( pFile );*/

	



	//filter.Encode( tmpByte, 10, dynBuffer );

	//HE_BYTE * pTmp = new HE_BYTE[dynBuffer.GetByteCount()];
	//dynBuffer.Read( pTmp, dynBuffer.GetByteCount() );

	//dynBuffer.Clear();
	//filter.Decode( pTmp, 13, dynBuffer );

	//IHE_FileRead * file = HE_CreateFileRead( "c:\\das.txt" );

// 	FILE * pFile = fopen( "c:\\d.pdf", "rb" );
// 	if ( pFile )
// 	{
// 		CHE_DynBuffer bf( 7 * 1024 * 1024, 1024 * 1024 ) ;
// 		fseek( pFile, 0x12e, SEEK_SET );
// 		HE_BYTE * byte= new HE_BYTE[173405];
// 		fread( byte, 1, 173405, pFile );
// 		filter.Decode( byte, 173405, bf );
// 		HE_BYTE * p = new HE_BYTE[bf.GetByteCount()];
// 		bf.Read( p, bf.GetByteCount() );
	//	FILE * pF = fopen( "C:\\adsf.txt", "wb" );
	//	fwrite( p, 1, bf.GetByteCount(), pF );
	//	fclose( pF );
		

// 		CHE_Bitmap bitmap;
// 		bitmap.Create( 1920, 1280, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN, bf.GetByteCount(), p );
// 		bitmap.ExchangeChannel( BITMAP_CHANNEL_Blue, BITMAP_CHANNEL_Red );
// 		bitmap.Save( "c:\\sd.bmp" );
// 
// 		return 0;
// 
// 	}

	/*if ( file )
	{
		HE_BYTE * pBtye = new HE_BYTE[ file->GetSize() ];
		file->ReadBlock( pBtye, 0, file->GetSize() );
		if ( pBtye )
		{
			filter.Encode( pBtye, file->GetSize(), dynBuffer );
			HE_BYTE * pEnByte = new HE_BYTE[dynBuffer.GetByteCount()];
			dynBuffer.Read( pEnByte, dynBuffer.GetByteCount() );
			FILE * pFile = fopen( "c:\\2.txt", "wb" );
			if ( pFile )
			{
				fwrite( pEnByte, 1, dynBuffer.GetByteCount(), pFile );
				fclose( pFile );
				pFile = NULL;
			}
			return 0;
			HE_DWORD size = dynBuffer.GetByteCount();
			CHE_DynBuffer dynBuffer2;
			filter.Decode( pEnByte, size, dynBuffer2 );
			HE_BYTE * pDeByte = new HE_BYTE[dynBuffer2.GetByteCount()];
			dynBuffer2.Read( pDeByte, dynBuffer2.GetByteCount() );

			pFile = fopen( "c:\\3.txt", "wb" );
			if ( pFile )
			{
				fwrite( pDeByte, 1, dynBuffer2.GetByteCount(), pFile );
				fclose( pFile );
				pFile = NULL;
			}
		}
	}*/

	

	//memset( buffer, 0, 20 );
	//memset( buffer2, 0, 20 );
	//filter.Encode( tmpByte, 10, dynBuffer );
	//filter.Decode( tmpByte, 18, dynBuffer );
	//filter.Decode( buffer, 0, buffer2, 20 );




	IHE_FileRead * pFileRead = HE_CreateFileRead( "c:\\pdf.pdf" );
	if ( pFileRead )
	{
		CHE_PDF_Parser parser;
		parser.StartParse( pFileRead );
		HE_DWORD version = parser.GetPDFVersion();
		HE_DWORD xrefoffset = parser.GetStartxrefOffset( 512 );
		HE_BOOL bRet = parser.GetXRefTable();
		//CHE_PDF_IndirectObject * pObj = parser.GetIndirectObject( 1 );
		//CHE_PDF_Dictionary* pDict = (CHE_PDF_Dictionary*)(pObj->GetDict()->GetElement( CHE_ByteString("OCProperties") ));
		//CHE_PDF_Array* pArr = (CHE_PDF_Array*)(pDict->GetElement( CHE_ByteString("OCGs") ) );
		//CHE_PDF_Dictionary* pDict2 = (CHE_PDF_Dictionary*)(pDict->GetElement( CHE_ByteString("D") ) );
		//pDict->Release();
		//int x = 0;
		HE_DWORD pageCount = parser.GetPageCount();
		
// 		parser.CloseParser();
// 
// 		CHE_PDF_SyntaxParser syntaxParser;
// 		syntaxParser.InitParser( pFileRead );
// 		CHE_ByteString str;
// 		HE_BYTE	byteType;
// 		while ( syntaxParser.GetPos() < syntaxParser.GetFileSize() )
// 		{
// 			str = syntaxParser.GetWord();
// 			byteType = syntaxParser.GetType();
// 		//	syntaxParser.NextLine();
// 		//	syntaxParser.NextLine();
// 			int x = 0;
// 		}

	}
	return 0;
}