#include "../../include/pdf/che_pdf_text.h"
#include "../../include/pdf/che_pdf_resource.h"
#include "../../include/che_datastructure.h"

HE_DWORD CHE_PDF_TextExtractor::Extract( CHE_PDF_Page * page, HE_WCHAR* buffer, HE_DWORD size )
{
	if ( page == NULL || buffer == NULL || size == 0 )
	{
		return 0;
	}
	CHE_DynBuffer contentBuf;
	if ( !page->GetPageContent( contentBuf ) )
	{
		return 0;
	}
	if ( contentBuf.GetByteCount() < 2 )
	{
		return 0;
	}
	
	//用于维护页面的字体对应的文字编码器的MAP
	CHE_ByteStringToPtrMap pMapStrToFontCharMgr;

	CHE_PDF_FontCharCodeMgr * pCurFontCharCodeMgr = NULL;

	//获取页面的字体资源
	CHE_PDF_Dictionary * pFontDict = page->GetPageDictionary();
	if ( pFontDict == NULL )
	{
		return 0;
	}
	pFontDict = (CHE_PDF_Dictionary*)pFontDict->GetElement( CHE_ByteString("Resources") );
	if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInObj = page->GetDocument()->GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
		pFontDict = pInObj->GetDict();
	}else if ( pFontDict == NULL || pFontDict->GetType() != PDFOBJ_DICTIONARY )
	{
		return 0;
	}
	pFontDict = (CHE_PDF_Dictionary*)pFontDict->GetElement( CHE_ByteString("Font") );
	if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInObj = page->GetDocument()->GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
		pFontDict = pInObj->GetDict();
	}else if ( pFontDict == NULL || pFontDict->GetType() != PDFOBJ_DICTIONARY )
	{
		return 0;
	}

	IHE_Read* pContentRead = HE_CreateMemBufRead( contentBuf.GetData(), contentBuf.GetByteCount() );
	if (  pContentRead == NULL )
	{
		return 0;
	}
	CHE_PDF_SyntaxParser sParser;
	if ( sParser.InitParser( pContentRead ) == FALSE )
	{
		pContentRead->Release();
		return FALSE;
	}

	HE_DWORD lCharCount = 0;

	CHE_PtrStack OpdStack;
	PDFPARSER_WORD_DES wordDes;
	CHE_PDF_Object * pTmpNode = NULL;
	while( sParser.GetWord( wordDes ) == TRUE )
	{
		if ( wordDes.str != "BT" )
			 continue;

		HE_BOOL	bOpd = TRUE;
		while ( sParser.GetWord( wordDes ) == TRUE )
		{
			bOpd = TRUE;
			if ( wordDes.str == "ET" )
			{
				break;
			}
			switch ( wordDes.type )
			{
			case PDFPARSER_WORD_INTEGER:
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToInteger(wordDes.str) );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_FLOAT:
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToFloat(wordDes.str) );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_NAME:
				pTmpNode = CHE_PDF_Name::Create( wordDes.str );
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
				pTmpNode = CHE_PDF_String::Create( wordDes.str );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_UNKNOWN:
				{
					if ( wordDes.str == "null" )
					{
						pTmpNode = CHE_PDF_Null::Create();
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "false" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( FALSE );
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "true" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( TRUE );
						OpdStack.Push( pTmpNode );
						break;
					}
				}
			default:
				bOpd = FALSE;
				break;
			}
			if ( bOpd == TRUE )
			{
				continue;
			}

			//处理指令
			if ( wordDes.str == "Tf" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				pTmpNode->Release();
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );

				pCurFontCharCodeMgr = (CHE_PDF_FontCharCodeMgr*)pMapStrToFontCharMgr.GetItem( ((CHE_PDF_Name*)pTmpNode)->GetString() );
				if ( pCurFontCharCodeMgr == NULL )
				{
					CHE_PDF_Object *pTmpObj = pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() );
					if ( pTmpObj == NULL )
					{
						pCurFontCharCodeMgr = new CHE_PDF_FontCharCodeMgr( NULL );
					}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
					{
						CHE_PDF_IndirectObject * pInObj = page->GetDocument()->GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml() );
						pTmpObj = pInObj->GetDict();
						pCurFontCharCodeMgr = new CHE_PDF_FontCharCodeMgr( (CHE_PDF_Dictionary*)pTmpObj );
					}else if ( pTmpObj->GetType() == PDFOBJ_DICTIONARY )
					{
						pCurFontCharCodeMgr = new CHE_PDF_FontCharCodeMgr( (CHE_PDF_Dictionary*)pTmpObj );
					}
					pMapStrToFontCharMgr.Append( ((CHE_PDF_Name*)pTmpObj)->GetString(), (HE_LPVOID)pCurFontCharCodeMgr );
				}
				continue;
			}else if ( wordDes.str == "Tj" || wordDes.str == "'" || wordDes.str == "\"" ) 
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
				for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
				{
					buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( str[i] );
					lCharCount++;
					if ( lCharCount == size )
					{
						return lCharCount;
					}
				}
			}else if ( wordDes.str == "TJ" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				CHE_PDF_Array * pArray = (CHE_PDF_Array*)pTmpNode;
				for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
				{
					pTmpNode = pArray->GetElement( i );
					if ( pTmpNode->GetType() != PDFOBJ_STRING )
					{
						continue;
					}
					CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
					for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
					{
						buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( str[i] );
						lCharCount++;
						if ( lCharCount == size )
						{
							return lCharCount;
						}
					}
				}
			}
		}
	}
	return lCharCount;
}