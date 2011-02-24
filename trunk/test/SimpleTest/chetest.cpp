#include "../../include/pdf/che_pdf_parser.h"
#include <stdio.h>

int main()
{
	IHE_FileRead * pFileRead = HE_CreateFileRead( "c:\\pdf3.pdf" );
	if ( pFileRead )
	{
		CHE_PDF_Parser parser;
		parser.StartParse( pFileRead );
		HE_DWORD version = parser.GetPDFVersion();
		HE_DWORD xrefoffset = parser.GetStartxrefOffset( 4096 );
		HE_BOOL bRet = parser.GetXRefTable();
		parser.CloseParser();

		//CHE_PDF_SyntaxParser syntaxParser;
		//syntaxParser.InitParser( pFileRead );
		//CHE_ByteString str;
		//while ( syntaxParser.GetPos() < syntaxParser.GetFileSize() )
		//{
		//	str = syntaxParser.GetWord();
		//	//syntaxParser.NextLine();
		//	//syntaxParser.NextLine();
		//	int x = 0;
		//}

	}
	return 0;
}