#include "../../include/pdf/che_pdf_text.h"
#include "../../include/pdf/che_pdf_resource.h"
#include "../../include/che_datastructure.h"
#include <string.h>
#include <windows.h>

HE_DWORD CHE_PDF_TextExtractor::Extract( CHE_PDF_Page * page, CHE_DynWideByteBuffer & buf )
{
	if ( page == NULL )
	{
		return 0;
	}
	IHE_PDF_GetInObj *			pIHE_GetInObj = page->GetDocument()->GetParser()->GetIHE_GetPDFInObj();
	IHE_PDF_GetFont *			pIHE_GetFontCodeMgr = page->GetDocument()->GetIHE_GetPDFFont();
	CHE_PDF_Dictionary *		pPageDict = page->GetPageDictionary();
	CHE_PDF_Dictionary *		pPageResourcDict = page->GetPageResources();
	if ( pPageDict == NULL || pPageResourcDict == NULL || pIHE_GetFontCodeMgr == NULL || pIHE_GetInObj == NULL )
	{
		return 0;
	}
	CHE_DynBuffer tmpbuf( 4096, 4096 );
	if ( !page->GetPageContent( tmpbuf ) )
	{
		return 0;
	}
	return Extract( tmpbuf, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
}

HE_DWORD CHE_PDF_TextExtractor::Extract(	CHE_PDF_Stream * pContent, CHE_PDF_Dictionary * pResourceDict,
											IHE_PDF_GetFont * pIHE_GetFont, IHE_PDF_GetInObj * pIHE_InObj,
											CHE_DynWideByteBuffer & buf )
{
	if ( pContent == NULL || pResourceDict == NULL || pIHE_GetFont == NULL || pIHE_InObj == NULL )
	{
		return 0;
	}
	CHE_PDF_StreamAcc stmAcc;
	if ( !stmAcc.Attach( pContent ) )
	{
		return 0;
	}
	CHE_DynBuffer tmpbuf( stmAcc.GetSize() );
	tmpbuf.Write( stmAcc.GetData(), stmAcc.GetSize() );
	return Extract( tmpbuf, pResourceDict, pIHE_GetFont, pIHE_InObj, buf );
}

HE_DWORD CHE_PDF_TextExtractor::Extract(	CHE_DynBuffer & content, CHE_PDF_Dictionary * pResourceDict,
											IHE_PDF_GetFont * pIHE_GetFont, IHE_PDF_GetInObj * pIHE_InObj,
											CHE_DynWideByteBuffer & buf )
{
	if ( pResourceDict == NULL || pIHE_GetFont == NULL || pIHE_InObj == NULL )
	{
		return 0;
	}

	CHE_PDF_Font * pCurFont = NULL;
	CHE_PDF_Dictionary * pFontDict = (CHE_PDF_Dictionary *)pResourceDict->GetElement( CHE_ByteString("Font") );
	if ( pFontDict == NULL )
	{
		return 0;
	}else if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInObj = pIHE_InObj->GetInObj( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
		pFontDict = pInObj->GetDict();
		if ( pFontDict == NULL )
		{
			return 0;
		}
	}
	IHE_Read* pContentRead = HE_CreateMemBufRead( content.GetData(), content.GetByteCount() );	//need to delete
	if (  pContentRead == NULL )
	{
		return 0;
	}
	CHE_PDF_SyntaxParser sParser;
	if ( sParser.InitParser( pContentRead ) == FALSE )
	{
		pContentRead->Release();
		delete pContentRead;
		return 0;
	}

	HE_DWORD lCharCount = 0;
	CHE_PtrStack OpdStack;
	PDFPARSER_WORD_DES wordDes;
	CHE_PDF_Object * pTmpNode = NULL;
	HE_BOOL	bOpd = TRUE;
	while( sParser.GetWord( wordDes ) == TRUE )
	{
		if ( wordDes.str == "BI" )
		{
			sParser.SeekToMark( CHE_ByteString("EI") );
			continue;
		}
		if ( wordDes.str != "BT" )
			 continue;
		
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
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToInteger(wordDes.str), 0, 0 );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_FLOAT:
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToFloat(wordDes.str), 0, 0 );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_NAME:
				pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0 );
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
				pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0 );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_UNKNOWN:
				{
					if ( wordDes.str == "null" )
					{
						pTmpNode = CHE_PDF_Null::Create( 0, 0 );
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "false" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( FALSE, 0, 0 );
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "true" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( TRUE, 0, 0 );
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
				CHE_PDF_Reference * pFontRef =  (CHE_PDF_Reference *)pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() );
				if ( pFontRef == NULL || pFontRef->GetType() != PDFOBJ_REFERENCE )
				{
					pCurFont = NULL;
				}else{
					pCurFont = pIHE_GetFont->GetFont( pFontRef->GetRefNuml() );
				}
				if ( pTmpNode )
				{
					pTmpNode->Release();
				}
				continue;
			}else if ( wordDes.str == "Tj" || wordDes.str == "'" || wordDes.str == "\"" ) 
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
				CHE_WideString wstr;
				if ( pCurFont == NULL )
				{
					continue;
				}
				pCurFont->GetUnicodeString( str, wstr );
				if ( wstr.GetLength() > 0 )
				{
					buf.Write( wstr.GetData(), wstr.GetLength() );
					lCharCount += wstr.GetLength();
				}
				if ( pTmpNode )
				{
					pTmpNode->Release();
				}
			}else if ( wordDes.str == "TJ" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				if ( pCurFont == NULL )
				{
					continue;
				}
				CHE_PDF_Array * pArray = (CHE_PDF_Array*)pTmpNode;
				for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
				{
					pTmpNode = pArray->GetElement( i );
					if ( pTmpNode->GetType() != PDFOBJ_STRING )
					{
						continue;
					}
					CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
					CHE_WideString wstr;
					pCurFont->GetUnicodeString( str, wstr );
					if ( wstr.GetLength() > 0 )
					{
						buf.Write( wstr.GetData(), wstr.GetLength() );
						lCharCount += wstr.GetLength();
					}
				}
				if ( pArray )
				{
					pArray->Release();
				}
			}
			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
		}
	}
	pContentRead->Release();
	delete pContentRead; 

	while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
	{
		if ( pTmpNode != NULL )
		{
			pTmpNode->Release();
		}
	}
	return lCharCount;
}

// HE_DWORD CHE_PDF_TextExtractor::TestExtract(	CHE_DynBuffer & content, CHE_PDF_Dictionary * pResourceDict,
// 												IHE_PDF_GetFont * pIHE_GetFont, IHE_PDF_GetInObj * pIHE_InObj,
// 												CHE_DynWideByteBuffer & buf )
// {
// 	if ( pResourceDict == NULL || pIHE_FontCodeMgr == NULL || pIHE_InObj == NULL )
// 	{
// 		return 0;
// 	}
// 
// 	CHE_PDF_FontCharCodeMgr * pCurFontCharCodeMgr = NULL;
// 	CHE_PDF_Dictionary * pFontDict = (CHE_PDF_Dictionary *)pResourceDict->GetElement( CHE_ByteString("Font") );
// 	if ( pFontDict == NULL )
// 	{
// 		return 0;
// 	}else if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
// 	{
// 		CHE_PDF_IndirectObject * pInObj = pIHE_InObj->GetInObj( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
// 		pFontDict = pInObj->GetDict();
// 		if ( pFontDict == NULL )
// 		{
// 			return 0;
// 		}
// 	}
// 	IHE_Read* pContentRead = HE_CreateMemBufRead( content.GetData(), content.GetByteCount() );	//need to delete
// 	if (  pContentRead == NULL )
// 	{
// 		return 0;
// 	}
// 	CHE_PDF_SyntaxParser sParser;
// 	if ( sParser.InitParser( pContentRead ) == FALSE )
// 	{
// 		pContentRead->Release();
// 		delete pContentRead;
// 		return 0;
// 	}
// 
// 	HE_DWORD lCharCount = 0;
// 	CHE_PtrStack OpdStack;
// 	PDFPARSER_WORD_DES wordDes;
// 	CHE_PDF_Object * pTmpNode = NULL;
// 	HE_BOOL	bOpd = TRUE;
// 	HE_WCHAR wch, wch1, wch2;
// 	while( sParser.GetWord( wordDes ) == TRUE )
// 	{
// 		if ( wordDes.str == "BI" )
// 		{
// 			sParser.SeekToMark( CHE_ByteString("EI") );
// 			continue;
// 		}
// 		if ( wordDes.str != "BT" )
// 			 continue;
// 		
// 		while ( sParser.GetWord( wordDes ) == TRUE )
// 		{
// 			bOpd = TRUE;
// 			if ( wordDes.str == "ET" )
// 			{
// 				break;
// 			}
// 			switch ( wordDes.type )
// 			{
// 			case PDFPARSER_WORD_INTEGER:
// 				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToInteger(wordDes.str), 0, 0 );
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_FLOAT:
// 				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToFloat(wordDes.str), 0, 0 );
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_NAME:
// 				pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0 );
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_ARRAY_B:
// 				sParser.SetPos( wordDes.offset );
// 				pTmpNode = sParser.GetArray();
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_DICT_B:
// 				sParser.SetPos( wordDes.offset );
// 				pTmpNode = sParser.GetDictionary();
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_STRING:
// 				pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0 );
// 				OpdStack.Push( pTmpNode );
// 				break;
// 			case PDFPARSER_WORD_UNKNOWN:
// 				{
// 					if ( wordDes.str == "null" )
// 					{
// 						pTmpNode = CHE_PDF_Null::Create( 0, 0 );
// 						OpdStack.Push( pTmpNode );
// 						break;
// 					}else if ( wordDes.str == "false" )
// 					{
// 						pTmpNode = CHE_PDF_Boolean::Create( FALSE, 0, 0 );
// 						OpdStack.Push( pTmpNode );
// 						break;
// 					}else if ( wordDes.str == "true" )
// 					{
// 						pTmpNode = CHE_PDF_Boolean::Create( TRUE, 0, 0 );
// 						OpdStack.Push( pTmpNode );
// 						break;
// 					}
// 				}
// 			default:
// 				bOpd = FALSE;
// 				break;
// 			}
// 			if ( bOpd == TRUE )
// 			{
// 				continue;
// 			}
// 			//处理指令
// 			if ( wordDes.str == "Tf" )
// 			{
// 				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
// 				pTmpNode->Release();
// 				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
// 				CHE_PDF_Reference * pFontRef =  (CHE_PDF_Reference *)pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() );
// 				if ( pFontRef == NULL || pFontRef->GetType() != PDFOBJ_REFERENCE )
// 				{
// 					pCurFontCharCodeMgr = NULL;
// 				}else{
// 					pCurFontCharCodeMgr = pIHE_FontCodeMgr->GetFontCodeMgr( pFontRef->GetRefNuml() );
// 				}
// 				HE_WCHAR wStr[128];
// 				wsprintfW( wStr, L"\nTf : %d obj\n", pFontRef->GetRefNuml() );
// 				buf.Write( wStr, wcslen(wStr) );
// 				if ( pTmpNode )
// 				{
// 					pTmpNode->Release();
// 				}
// 				continue;
// 			}else if ( wordDes.str == "Tj" || wordDes.str == "'" || wordDes.str == "\"" ) 
// 			{
// 				HE_WCHAR wStr[128];
// 				wsprintfW( wStr, L" Tj " );
// 				buf.Write( wStr, wcslen(wStr) );
// 				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
// 				CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
// 				if ( pCurFontCharCodeMgr == NULL )
// 				{
// 					continue;
// 				}
// 				if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
// 				{
// 					if ( pCurFontCharCodeMgr->GetEncodingType() == PDFENCODING_GBK_UCS2 )
// 					{
// 						for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
// 						{
// 							wch1 = str[i] & 0x00FF;
// 							wch2 = str[i+1] & 0x00FF;
// 							wch1 = wch1 << 8;
// 							wch = wch1 + wch2;
// 							wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 							buf.Write( &wch, 1 );
// 							HE_WCHAR wStr[128];
// 							wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 							buf.Write( wStr, wcslen(wStr) );
// 							lCharCount++;
// 						}
// 					}else if ( pCurFontCharCodeMgr->IsDefaultEncoding() == TRUE )
// 					{
// 						for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
// 						{
// 							if ( (HE_BYTE)(str[i]) < 0xA1 )
// 							{
// 								wch1 = pCurFontCharCodeMgr->GetUnicode( str[i] );
// 								wch2 = pCurFontCharCodeMgr->GetUnicode( str[i+1] );
// 								buf.Write( &wch1, 1 );
// 								HE_WCHAR wStr[128];
// 								wsprintfW( wStr, L"(%d)", wch1 );
// 								buf.Write( wStr, wcslen(wStr) );
// 								buf.Write( &wch2, 1 );
// 								wsprintfW( wStr, L"(%d)", wch2 );
// 								buf.Write( wStr, wcslen(wStr) );
// 								lCharCount+=2;
// 							}else{
// 								wch1 = str[i] & 0x00FF;
// 								wch2 = str[i+1] & 0x00FF;
// 								wch1 = wch1 << 8;
// 								wch = wch1 + wch2;
// 								wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 								buf.Write( &wch, 1 );
// 								HE_WCHAR wStr[128];
// 								wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 								buf.Write( wStr, wcslen(wStr) );
// 								lCharCount++;
// 							}
// 						}
// 					}else{
// 						for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
// 						{
// 							wch1 = str[i] & 0x00FF;
// 							wch2 = str[i+1] & 0x00FF;
// 							wch1 = wch1 << 8;
// 							wch = wch1 + wch2;
// 							wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 							buf.Write( &wch, 1 );
// 							HE_WCHAR wStr[128];
// 							wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 							buf.Write( wStr, wcslen(wStr) );
// 							lCharCount++;
// 						}
// 					}
// 				}else{
// 					for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
// 					{
// 						wch = str[i] & 0xFF;
// 						wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 						buf.Write( &wch, 1 );
// 						HE_WCHAR wStr[128];
// 						wsprintfW( wStr, L"(%d)", str[i] & 0xFF );
// 						buf.Write( wStr, wcslen(wStr) );
// 						lCharCount++;
// 					}
// 				}
// 				if ( pTmpNode )
// 				{
// 					pTmpNode->Release();
// 				}
// 			}else if ( wordDes.str == "TJ" )
// 			{
// 				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
// 				if ( pCurFontCharCodeMgr == NULL )
// 				{
// 					continue;
// 				}
// 				CHE_PDF_Array * pArray = (CHE_PDF_Array*)pTmpNode;
// 				for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
// 				{
// 					pTmpNode = pArray->GetElement( i );
// 					if ( pTmpNode->GetType() != PDFOBJ_STRING )
// 					{
// 						continue;
// 					}
// 					CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
// 					if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
// 					{
// 						if ( pCurFontCharCodeMgr->GetEncodingType() == PDFENCODING_GBK_UCS2 )
// 						{
// 							for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
// 							{
// 								wch1 = str[i] & 0x00FF;
// 								wch2 = str[i+1] & 0x00FF;
// 								wch1 = wch1 << 8;
// 								wch = wch1 + wch2;
// 								wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 								buf.Write( &wch, 1 );
// 								HE_WCHAR wStr[128];
// 								wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 								buf.Write( wStr, wcslen(wStr) );
// 								lCharCount++;
// 							}
// 						}else if ( pCurFontCharCodeMgr->IsDefaultEncoding() == TRUE )
// 						{
// 							for ( HE_DWORD i = 0; i < str.GetLength(); i+= 2 )
// 							{
// 								if ( (HE_BYTE)(str[i]) < 0xA1 )
// 								{
// 									wch1 = pCurFontCharCodeMgr->GetUnicode( str[i] );
// 									wch2 = pCurFontCharCodeMgr->GetUnicode( str[i+1] );
// 									buf.Write( &wch1, 1 );
// 									HE_WCHAR wStr[128];
// 									wsprintfW( wStr, L"(%d)", wch1 );
// 									buf.Write( wStr, wcslen(wStr) );
// 									buf.Write( &wch2, 1 );
// 									wsprintfW( wStr, L"(%d)", wch2 );
// 									buf.Write( wStr, wcslen(wStr) );
// 									lCharCount+=2;
// 								}else{
// 									wch1 = str[i] & 0x00FF;
// 									wch2 = str[i+1] & 0x00FF;
// 									wch1 = wch1 << 8;
// 									wch = wch1 + wch2;
// 									wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 									buf.Write( &wch, 1 );
// 									HE_WCHAR wStr[128];
// 									wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 									buf.Write( wStr, wcslen(wStr) );
// 									lCharCount++;
// 								}
// 							}
// 						}else{
// 							for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
// 							{
// 								wch1 = str[i] & 0x00FF;
// 								wch2 = str[i+1] & 0x00FF;
// 								wch1 = wch1 << 8;
// 								wch = wch1 + wch2;
// 								wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 								buf.Write( &wch, 1 );
// 								HE_WCHAR wStr[128];
// 								wsprintfW( wStr, L"(%d)", wch1 + wch2 );
// 								buf.Write( wStr, wcslen(wStr) );
// 								lCharCount++;
// 							}
// 						}
// 					}else{
// 						for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
// 						{
// 							wch = str[i] & 0xFF;
// 							wch = pCurFontCharCodeMgr->GetUnicode( wch );
// 							buf.Write( &wch, 1 );
// 							HE_WCHAR wStr[128];
// 							wsprintfW( wStr, L"(%d)", str[i] & 0xFF );
// 							buf.Write( wStr, wcslen(wStr) );
// 							lCharCount++;
// 						}
// 					}
// 				}
// 				if ( pArray )
// 				{
// 					pArray->Release();
// 				}
// 			}
// 			//清除无用的操作数
// 			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
// 			{
// 				if ( pTmpNode != NULL )
// 				{
// 					pTmpNode->Release();
// 				}
// 			}
// 		}
// 	}
// 	pContentRead->Release();
// 	delete pContentRead; 
// 
// 	while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
// 	{
// 		if ( pTmpNode != NULL )
// 		{
// 			pTmpNode->Release();
// 		}
// 	}
// 	return lCharCount;
// }
// 
// HE_DWORD CHE_PDF_TextExtractor::TestExtract( CHE_PDF_Page * page, CHE_DynWideByteBuffer & buf )
// {
// 	if ( page == NULL )
// 	{
// 		return 0;
// 	}
// 	IHE_PDF_GetInObj *			pIHE_GetInObj = page->GetDocument()->GetParser()->GetIHE_GetPDFInObj();
// 	IHE_PDF_GetFontCodeMgr *		pIHE_GetFontCodeMgr = page->GetDocument()->GetIHE_GetPDFFontCodeMgr();
// 	CHE_PDF_Dictionary *		pPageDict = page->GetPageDictionary();
// 	CHE_PDF_Dictionary *		pPageResourcDict = page->GetPageResources();
// 	if ( pPageDict == NULL || pPageResourcDict == NULL || pIHE_GetFontCodeMgr == NULL || pIHE_GetInObj == NULL )
// 	{
// 		return 0;
// 	}
// 	CHE_DynBuffer tmpbuf( 4096, 4096 );
// 	if ( !page->GetPageContent( tmpbuf ) )
// 	{
// 		return 0;
// 	}
// 	return TestExtract( tmpbuf, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// }