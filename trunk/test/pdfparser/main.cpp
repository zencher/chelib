#include <cstdio>

#include "../../include/pdf/CHE_PDF_Parser.h"

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

	CHE_PDF_Parser parser;
	parser.StartParse( pFileRead );
	if ( parser.GetPDFVersion() == 0 )
	{
		parser.CloseParser();
		printf( "not a pdf file!\n" );
		return 0;
	}
	printf( "pdf version : %d\n", parser.GetPDFVersion() );
	printf( "xref : %d\n", parser.GetStartxrefOffset( 1024 ) );
	parser.ParseXRef();
	printf( "page count : %d\n\n", parser.GetPageCount() );

	CHE_PDF_IndirectObject * pInObj = parser.GetIndirectObject( 104 );
	if ( pInObj )
	{
 		CHE_PDF_Stream * pObj = pInObj->GetStream();
 		IHE_Write * pIWrite = HE_CreateFileWrite( "c:\\ToUnicode.txt" );
 		CHE_PDF_StreamAcc strAcc;
 		strAcc.Attach( pObj );
 		pIWrite->WriteBlock( (HE_LPVOID)strAcc.GetData(), strAcc.GetSize() );
 		pIWrite->Release();
 		strAcc.Detach();
	}   

	parser.CloseParser();
	HE_DestoryIHERead( pFileRead );

	return 0;
}