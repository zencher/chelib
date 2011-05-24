#include "../../include/pdf/che_pdf_renderer.h"
#include "../../include/pdf/che_pdf_parser.h"

HE_VOID	CHE_PDF_Renderer::Render( CHE_PDF_Page * page, IHE_PDF_DrawGraphics * pIHE_draw )
{
	if ( page == NULL || pIHE_draw == NULL )
	{
		return;
	}

	CHE_DynBuffer buf( 1024, 1024, GetAllocator() );
	if ( page->GetPageContent( buf ) == FALSE )
	{
		return;
	}
	IHE_Read * pIHE_Read = HE_CreateMemBufRead( buf.GetData(), buf.GetByteCount(), GetAllocator() );
	if ( pIHE_Read == NULL )
	{
		return;
	}
	CHE_PDF_SyntaxParser sParser( GetAllocator() );
	sParser.InitParser( pIHE_Read );

	CHE_PtrStack OpdStack( GetAllocator() );
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	HE_BOOL bOpd = TRUE;
	CHE_PDF_Object * pTmpNode = NULL;

	//graph state
	HE_LONG cpx = 0, cpy = 0;
	HE_LONG ctma = 1, ctmb = 0, ctmc = 0, ctmd = 1, ctme = 0, ctmf = 0;
	HE_LONG tmda = 1, tmdb = 0, tmdc = 0, tmdd = -1, tmde = 0, tmdf = (HE_LONG)page->GetPageHeight();
	CHE_Path * pPath = NULL;

	while ( sParser.GetWord( wordDes ) == TRUE )
	{
		bOpd = TRUE;
		switch ( wordDes.type )
		{
		case PDFPARSER_WORD_INTEGER:
			pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_FLOAT:
			pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_NAME:
			pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_ARRAY_B:
			sParser.SetPos( wordDes.offset );
			pTmpNode = sParser.GetArray();
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_DICT_B:
			sParser.SetPos( wordDes.offset );
			pTmpNode = sParser.GetDictionary();
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_STRING:
			pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		default:
			bOpd = FALSE;
			break;
		}
		if ( bOpd == TRUE )
		{
			continue;
		}
		if ( wordDes.str == "m" )
		{
			if ( pPath )
			{
				delete pPath;
			}
			pPath = new CHE_Path( GetAllocator() ) ;
			OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
			cpy = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
			pTmpNode->Release();
			OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
			cpx = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
			pTmpNode->Release();
		}else if ( wordDes.str == "l" )
		{
			OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
			HE_LONG endY = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
			pTmpNode->Release();
			OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
			HE_LONG endX = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
			pTmpNode->Release();
			if ( pPath )
			{
				HE_LONG X1 = ctma * endX + ctmc * endY + ctme;
				HE_LONG Y1 = ctmb * endX + ctmd * endY + ctmf;
				X1 = tmda * X1 + tmdc * Y1 + tmde;
				Y1 = tmdb * X1 + tmdd * Y1 + tmdf;
				HE_LONG X0 = ctma * cpx + ctmc * cpy + ctme;
				HE_LONG Y0 = ctmb * cpx + ctmd * cpy + ctmf;
				X0 = tmda * X0 + tmdc * Y0 + tmde;
				Y0 = tmdb * X0 + tmdd * Y0 + tmdf;
				CHE_Line line( X0, Y0, X1, Y1, GetAllocator() );
				pPath->AddLine( line );
			}
		}else if ( wordDes.str == "s" )
		{
			if ( pPath )
			{
				pIHE_draw->StrokePath( *pPath );
			}
		}
	}
}
