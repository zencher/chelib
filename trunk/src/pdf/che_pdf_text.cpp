#include "../../include/pdf/che_pdf_text.h"
#include "../../include/pdf/che_pdf_resource.h"
#include "../../include/che_datastructure.h"

HE_DWORD CHE_PDF_TextExtractor::Extract( CHE_PDF_Page * page, CHE_DynWideByteBuffer & buf )
{
	if ( page == NULL )
	{
		return 0;
	}
	IHE_PDF_GetInObj *			pIHE_GetInObj = page->GetDocument()->GetParser()->GetIHE_GetPDFInObj();
	IHE_PDF_GetFontCodeMgr *		pIHE_GetFontCodeMgr = page->GetDocument()->GetIHE_GetPDFFontCodeMgr();
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
// 
// 	HE_DWORD lCharCount = 0;
// 	IHE_GetPDFInObj *			pIHE_GetInObj = page->GetDocument()->GetParser()->GetIHE_GetPDFInObj();
// 	IHE_GetPDFFontCodeMgr *		pIHE_GetFontCodeMgr = page->GetDocument()->GetIHE_GetPDFFontCodeMgr();
// 	CHE_PDF_Dictionary *		pPageDict = page->GetPageDictionary();
// 	CHE_PDF_Dictionary *		pPageResourcDict = page->GetPageResources();
// 	if ( pPageDict == NULL || pPageResourcDict == NULL || pIHE_GetFontCodeMgr == NULL || pIHE_GetInObj == NULL )
// 	{
// 		return 0;
// 	}
// 	CHE_PDF_Object * pPageContent = pPageDict->GetElement( CHE_ByteString("Contents") );
// 	if ( pPageContent == NULL )
// 	{
// 		return 0;
// 	}
// 	if ( pPageContent->GetType() == PDFOBJ_REFERENCE )
// 	{
// 		CHE_PDF_IndirectObject * pInObj = pIHE_GetInObj->GetInObj( ((CHE_PDF_Reference*)pPageContent)->GetRefNuml() );
// 		if ( pInObj == NULL )
// 		{
// 			return 0;
// 		}
// 		CHE_PDF_Object *  pContentObj = pInObj->GetObject();
// 		if ( pContentObj->GetType() == PDFOBJ_STREAM )
// 		{
// 			lCharCount = Extract( (CHE_PDF_Stream*)pContentObj, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// 		}else if ( pContentObj->GetType() == PDFOBJ_ARRAY )
// 		{
// 			CHE_PDF_Object * pTmpObj = NULL;
// 			for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pContentObj)->GetCount(); i++ )
// 			{
// 				pTmpObj = ((CHE_PDF_Array*)pContentObj)->GetElement( i );
// 				if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
// 				{
// 					CHE_PDF_IndirectObject * pInObj = pIHE_GetInObj->GetInObj( ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml() );
// 					if ( pInObj == NULL )
// 					{
// 						continue;
// 					}
// 					CHE_PDF_Object *  pContentObj = pInObj->GetObject();
// 					if ( pContentObj->GetType() == PDFOBJ_STREAM )
// 					{
// 						lCharCount += Extract( (CHE_PDF_Stream*)pContentObj, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// 					}
// 				}else{
// 					continue;
// 				}
// 			}
// 		}
// 	}else if ( pPageContent->GetType() == PDFOBJ_ARRAY )
// 	{
// 		HE_DWORD objCount = ((CHE_PDF_Array*)pPageContent)->GetCount();
// 		for ( HE_DWORD i = 0; i < objCount; i++ )
// 		{
// 			CHE_PDF_Object * pObj = ((CHE_PDF_Array*)pPageContent)->GetElement( i );
// 			if ( pObj == NULL )
// 			{
// 				continue;
// 			}
// 			if ( pObj->GetType() != PDFOBJ_REFERENCE )
// 			{
// 				continue;
// 			}
// 			CHE_PDF_IndirectObject * pInObj = pIHE_GetInObj->GetInObj( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
// 			if ( pInObj == NULL )
// 			{
// 				continue;
// 			}
// 			CHE_PDF_Object *  pContentObj = pInObj->GetObject();
// 			if ( pContentObj->GetType() == PDFOBJ_STREAM )
// 			{
// 				lCharCount += Extract( (CHE_PDF_Stream*)pContentObj, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// 			}else if ( pContentObj->GetType() == PDFOBJ_ARRAY )
// 			{
// 				CHE_PDF_Object * pTmpObj = NULL;
// 				for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pContentObj)->GetCount(); i++ )
// 				{
// 					pTmpObj = ((CHE_PDF_Array*)pContentObj)->GetElement( i );
// 					if ( pTmpObj->GetType() == PDFOBJ_STREAM )
// 					{
// 						lCharCount += Extract( (CHE_PDF_Stream*)pTmpObj, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// 					}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
// 					{
// 						HE_DWORD objNum = ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml();
// 						CHE_PDF_IndirectObject * pInObj = pIHE_GetInObj->GetInObj( ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml() );
// 						if ( pInObj == NULL )
// 						{
// 							continue;
// 						}
// 						CHE_PDF_Object *  pContentObj = pInObj->GetObject();
// 						if ( pContentObj->GetType() == PDFOBJ_STREAM )
// 						{
// 							lCharCount += Extract( (CHE_PDF_Stream*)pTmpObj, pPageResourcDict, pIHE_GetFontCodeMgr, pIHE_GetInObj, buf );
// 						}
// 					}
// 				}
// 			}
// 		}
// 		return TRUE;
// 	}
// 	return lCharCount;
}

HE_DWORD CHE_PDF_TextExtractor::Extract(	CHE_PDF_Stream * pContent, CHE_PDF_Dictionary * pResourceDict,
											IHE_PDF_GetFontCodeMgr * pIHE_FontCodeMgr, IHE_PDF_GetInObj * pIHE_InObj,
											CHE_DynWideByteBuffer & buf )
{
	if ( pContent == NULL || pResourceDict == NULL || pIHE_FontCodeMgr == NULL || pIHE_InObj == NULL )
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
	return Extract( tmpbuf, pResourceDict, pIHE_FontCodeMgr, pIHE_InObj, buf );
}

HE_DWORD CHE_PDF_TextExtractor::Extract(	CHE_DynBuffer & content, CHE_PDF_Dictionary * pResourceDict,
											IHE_PDF_GetFontCodeMgr * pIHE_FontCodeMgr, IHE_PDF_GetInObj * pIHE_InObj,
											CHE_DynWideByteBuffer & buf )
{
	if ( pResourceDict == NULL || pIHE_FontCodeMgr == NULL || pIHE_InObj == NULL )
	{
		return 0;
	}

	CHE_PDF_FontCharCodeMgr * pCurFontCharCodeMgr = NULL;
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
	HE_WCHAR wch, wch1, wch2;
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
					pCurFontCharCodeMgr = NULL;
				}else{
					pCurFontCharCodeMgr = pIHE_FontCodeMgr->GetFontCodeMgr( pFontRef->GetRefNuml() );
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
				if ( pCurFontCharCodeMgr == NULL )
				{
					continue;
				}
				if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
				{
					if ( pCurFontCharCodeMgr->IsDefaultEncoding() == TRUE )
					{
						for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
						{
							if ( (HE_BYTE)(str[i]) < 0xA1 )
							{
								wch1 = pCurFontCharCodeMgr->GetUnicode( str[i] );
								wch2 = pCurFontCharCodeMgr->GetUnicode( str[i+1] );
								buf.Write( &wch1, 1 );
								buf.Write( &wch2, 1 );
								lCharCount+=2;
							}else{
								wch1 = str[i] & 0x00FF;
								wch2 = str[i+1] & 0x00FF;
								wch1 = wch1 << 8;
								wch = wch1 + wch2;
								wch = pCurFontCharCodeMgr->GetUnicode( wch );
								buf.Write( &wch, 1 );
								lCharCount++;
							}
						}
					}else{
						for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
						{
							wch1 = str[i] & 0x00FF;
							wch2 = str[i+1] & 0x00FF;
							wch1 = wch1 << 8;
							wch = wch1 + wch2;
							wch = pCurFontCharCodeMgr->GetUnicode( wch );
							buf.Write( &wch, 1 );
							lCharCount++;
						}
					}
				}else{
					for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
					{
						wch = str[i] & 0xFF;
						wch = pCurFontCharCodeMgr->GetUnicode( wch );
						buf.Write( &wch, 1 );
						lCharCount++;
					}
				}
				if ( pTmpNode )
				{
					pTmpNode->Release();
				}
			}else if ( wordDes.str == "TJ" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				if ( pCurFontCharCodeMgr == NULL )
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
					if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
					{
						if ( pCurFontCharCodeMgr->IsDefaultEncoding() == TRUE )
						{
							for ( HE_DWORD i = 0; i < str.GetLength(); i+= 2 )
							{
								if ( (HE_BYTE)(str[i]) < 0xA1 )
								{
									wch1 = pCurFontCharCodeMgr->GetUnicode( str[i] );
									wch2 = pCurFontCharCodeMgr->GetUnicode( str[i+1] );
									buf.Write( &wch1, 1 );
									buf.Write( &wch2, 1 );
									lCharCount+=2;
								}else{
									wch1 = str[i] & 0x00FF;
									wch2 = str[i+1] & 0x00FF;
									wch1 = wch1 << 8;
									wch = wch1 + wch2;
									wch = pCurFontCharCodeMgr->GetUnicode( wch );
									buf.Write( &wch, 1 );
									lCharCount++;
								}
							}
						}else{
							for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
							{
								wch1 = str[i] & 0x00FF;
								wch2 = str[i+1] & 0x00FF;
								wch1 = wch1 << 8;
								wch = wch1 + wch2;
								wch = pCurFontCharCodeMgr->GetUnicode( wch );
								buf.Write( &wch, 1 );
								lCharCount++;
							}
						}
					}else{
						for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
						{
							wch = pCurFontCharCodeMgr->GetUnicode( str[i] );
							buf.Write( &wch, 1 );
							lCharCount++;
						}
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