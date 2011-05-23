#include <cstdio>

#include <windows.h>

#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_text.h"
#include "../../include/pdf/che_pdf_pages.h"

char * pFile = NULL;

DWORD WINAPI WordThreadProc( LPVOID lpParameter )
{
	CHE_HeapAllocator allocator;
	CHE_Allocator * pAllocator = &allocator;
	if ( pFile == NULL )
	{
		return -1;
	}
	int tid = (int)lpParameter;
	printf( "thread %d is working!\n", tid );

	IHE_Read * pFileRead = HE_CreateFileRead( pFile, FILEREAD_MODE_DEFAULT, 4096, pAllocator );
	//IHE_Read * pFileRead = HE_CreateFileRead( pFile, FILEREAD_MODE_MEMCOPY, 4096, pAllocator );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return 0;
	}
	try{
		CHE_PDF_Document doc( pAllocator );
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
			CHE_DynBuffer buffer( 4096*10, 4096*10, pAllocator );
			pTmpPage->GetPageContent( buffer );
			CHE_PDF_TextExtractor textExtractor( pAllocator );
			CHE_DynWideByteBuffer buf( 4096*10, 4096*10, pAllocator );
			HE_DWORD lcount = textExtractor.Extract( pTmpPage, buf );
			if ( pTmpPage )
			{
				doc.GetAllocator()->Delete( pTmpPage );
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

DWORD WINAPI WordThreadProc1( LPVOID lpParameter )
{
	CHE_HeapAllocator allocator;
	int * pInt = NULL;
	for ( HE_DWORD i = 0; i < 10*1024*1024; i++ )
	{
		pInt = allocator.NewArray<int>( 1024 );
		allocator.DeleteArray( pInt );
		pInt = allocator.New<int>();
		allocator.Delete( pInt );
	}
	return 0;
}

DWORD WINAPI WordThreadProc2( LPVOID lpParameter )
{
	int * pInt = NULL;
	for ( HE_DWORD i = 0; i < 10*1024*1024; i++ )
	{
		pInt = new int[1024];
		delete [] pInt;
		pInt = new int;
		delete pInt;
	}
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
