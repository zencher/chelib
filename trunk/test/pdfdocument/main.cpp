#include <cstdio>
#include "../../include/pdf/che_pdf_pages.h"
#include "../../include/che_dynbuffer.h"
#include "../../include/pdf/che_pdf_renderer.h"
#include "../../include/pdf/che_pdf_text.h"

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}
	
	IHE_Read * pFileRead = HE_CreateFileRead( argv[1] );
	//IHE_Read * pFileRead = HE_CreateFileRead( argv[1], FILEREAD_MODE_MEMCOPY );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return 0;
	}else{
		printf( "%s \n",argv[1] );
		printf( "file size : %ld\n", pFileRead->GetSize() );
	}

	try{
		CHE_PDF_Document doc;
		doc.Load( pFileRead );
		HE_DWORD lPageCount = doc.GetPageCount();
		printf( "page count : %ld\n", lPageCount );
		CHE_PDF_Page * pTmpPage = NULL;
		char tmpStr[1024];
		sprintf( tmpStr, "%s.text.txt", argv[1] );
		IHE_Write * pWriteText = HE_CreateFileWrite( tmpStr );
		HE_BYTE pTmp[2] = { 0xFF, 0xFE }; //UTF-16LE BOM
		if ( pWriteText )
		{
			pWriteText->WriteBlock( pTmp, 2 );
		}
		sprintf( tmpStr, "%s.content.txt", argv[1] );
		IHE_Write * pWriteContent = HE_CreateFileWrite( tmpStr );
		for ( HE_DWORD i = 0; i < doc.GetPageCount(); i++ )
		{
			pTmpPage = doc.GetPage( i );
			if ( pTmpPage == NULL )
			{
				printf( "page index : %ld Error, Can not get the page!\n", i+1 );
				continue;
			}
			CHE_DynBuffer buffer;
			pTmpPage->GetPageContent( buffer );
			HE_BYTE * pData = new HE_BYTE[buffer.GetByteCount()];
			buffer.Read( pData,  buffer.GetByteCount() );
			if ( pWriteContent )
			{
				pWriteContent->WriteBlock( pData, buffer.GetByteCount() );
			}
			CHE_PDF_TextExtractor textExtractor;
			CHE_DynWideByteBuffer buf(4096, 4096);
			HE_DWORD lcount = textExtractor.Extract( pTmpPage, buf );
			if ( pWriteText )
			{
				if ( sizeof(HE_WCHAR) == 2 )
				{
					pWriteText->WriteBlock( (HE_BYTE*)buf.GetData(), buf.GetByteCount() * 2 );
				}else{
					HE_CHAR ch;
					for( HE_DWORD i = 0; i < buf.GetByteCount(); i++ )
					{
						ch = buf.GetData()[i] & 0xFF;
						pWriteText->WriteByte( ch );
						ch = (buf.GetData()[i] & 0xFF00) >> 8;
						pWriteText->WriteByte( ch );	
					}
				}
			}
			if ( pTmpPage )
			{
				printf( "page index : %ld ", i+1 );
				printf( "page width : %.2f ", pTmpPage->GetPageWidth() );
				printf( "page height : %.2f\n", pTmpPage->GetPageHeight() );
				delete pTmpPage;
			}
		}
		doc.Unload();
		pWriteContent->Release();
		pWriteText->Release();
		pFileRead->Release();
		HE_DestoryIHEWrite( pWriteContent );
		HE_DestoryIHEWrite( pWriteText );
		HE_DestoryIHERead( pFileRead );
		pWriteContent = NULL;
		pWriteText = NULL;
		pFileRead = NULL;
	}catch(...)
	{
		printf( "Error! Crash!\n" );
	}
	return 0;
}
