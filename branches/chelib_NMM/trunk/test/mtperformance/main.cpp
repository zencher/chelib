#include <cstdio>

#include <windows.h>

#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_pages.h"
#include "../../include/pdf/che_pdf_text.h"

char * pFile = NULL;

DWORD WINAPI WordThreadProc( LPVOID lpParameter )
{
	if ( pFile == NULL )
	{
		return -1;
	}
	int tid = (int)lpParameter;
	printf( "thread %d is working!\n", tid );

	IHE_Read * pFileRead = HE_CreateFileRead( pFile );
	//IHE_Read * pFileRead = HE_CreateFileRead( pFile, FILEREAD_MODE_MEMCOPY );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return 0;
	}
	try{
		CHE_PDF_Document doc;
		doc.Load( pFileRead );
		HE_DWORD lPageCount = doc.GetPageCount();
		CHE_PDF_Page * pTmpPage = NULL;
		for ( HE_DWORD i = 0; i < doc.GetPageCount(); i++ )
		{
			printf( "%d\n", i );
			pTmpPage = doc.GetPage( i );
			if ( pTmpPage == NULL )
			{
				continue;
			}
			CHE_DynBuffer buffer( 4096*10, 4096*10 );
			pTmpPage->GetPageContent( buffer );
			CHE_PDF_TextExtractor textExtractor;
			CHE_DynWideByteBuffer buf( 4096*10, 4096*10 );
			HE_DWORD lcount = textExtractor.Extract( pTmpPage, buf );
			//pTmpPage->GetPageHeight();
			//pTmpPage->GetPageWidth();
			if ( pTmpPage )
			{
				delete pTmpPage;
			}
		}
		doc.Unload();
		pFileRead->Release();
		HE_DestoryIHERead( pFileRead );
		pFileRead = NULL;
	}catch(...)
	{
		printf( "thread %d : Error! Crash!\n", tid );
		return -1;
	}

	printf( "thread %d is over!\n", tid );

	return 0;
}

int main( int argc, char **argv )
{
	if ( argc < 3 )
	{
		printf( "no param!\n" );
		return 0;
	}else{
		printf( "%s \n",argv[1] );
	}
	int ithreadCount = atoi(argv[2]);
	pFile = argv[1];


	HANDLE * phThread = new HANDLE[ithreadCount];
	for ( int i = 0; i < ithreadCount; i++ )
	{
		phThread[i] = CreateThread( NULL, 0, WordThreadProc, (LPVOID)i, 0, 0 );
	}
	DWORD lBeginTick = GetTickCount();

	WaitForMultipleObjects( ithreadCount, phThread, TRUE, INFINITE );

	DWORD lEndTick = GetTickCount();

	DWORD lSpend = lEndTick - lBeginTick;
	lSpend = lSpend;

	printf( "Total spend %d ms.\n", lSpend );

	return 0;
}
