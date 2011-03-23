#include <stdio.h>
#include "../../include/pdf/che_pdf_parser.h"

#include <windows.h>

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}

	IHE_Read * pFileRead = HE_CreateFileRead( argv[1] );
	//IHE_FileRead * pFileRead = HE_CreateFileMemRead( argv[1] );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return 0;
	}else{
		printf( "%s \n",argv[1] );
		printf( "file size : %d\n", pFileRead->GetSize() );
	}

	CHE_PDF_Parser parser;
	parser.StartParse( pFileRead );

	printf( "pdf version : %d\n", parser.GetPDFVersion() );
	printf( "xref : %d\n", parser.GetStartxrefOffset( 1024 ) );
	unsigned int tickBegin = GetTickCount();
	unsigned int tickEnd = 0;
	printf( "parse xref table start : %d\n", tickBegin );
	parser.ParseXRef();
	tickEnd = GetTickCount();
	printf( "parse xref table stop : %d\n", tickEnd );
	printf( "time spend : %d\n", tickEnd - tickBegin );
	printf( "page count : %d\n\n", parser.GetPageCount() );

	parser.CloseParser();
	pFileRead->Release();
	return 0;
}