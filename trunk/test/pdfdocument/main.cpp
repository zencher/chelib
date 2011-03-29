#include <stdio.h>
#include "../../include/pdf/che_pdf_pages.h"
#include "../../include/che_dynBuffer.h"
#include "../../include/pdf/che_pdf_renderer.h"

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
		printf( "file size : %d\n", pFileRead->GetSize() );
	}
	
	CHE_PDF_Document doc;
	doc.Load( pFileRead );
	HE_DWORD lPageCount = doc.GetPageCount();
	printf( "page count : %d\n", lPageCount );
	CHE_PDF_Page * pTmpPage = NULL;
	CHE_PDF_Renderer renderer;
	//char tmpStr[1024];
	//sprintf( tmpStr, "%s.txt", argv[1] );
	//IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );
	HE_BOOL	bEncrypt = doc.IsEncrypted();
	if ( bEncrypt )
	{
		//if ( pWrite )
		//{
		//	pWrite->WriteBlock( "Encrypted", 9 );
		//	pWrite->Release();
		//}
		return 0;
	}
	for ( HE_DWORD i = 0; i < lPageCount; i++ )
	{
		if ( i == 228 )
		{
			int x = 0;
		}
		pTmpPage = doc.GetPage( i );
		CHE_DynBuffer buffer;
		renderer.GetPageContent( *pTmpPage, buffer );
		HE_BYTE * pData = new HE_BYTE[buffer.GetByteCount()];
		buffer.Read( pData,  buffer.GetByteCount() );
		//if ( pWrite )
		//{
		//	pWrite->WriteBlock( pData, buffer.GetByteCount() );
		//}
// 		if ( pTmpPage )
// 		{
// 			printf( "page index : %d ", i+1 );
// 			printf( "page width : %.2f ", pTmpPage->GetPageWidth() );
// 			printf( "page height : %.2f\n", pTmpPage->GetPageHeight() );
// 		}
	}
//	pWrite->Release();
	return 0;
}