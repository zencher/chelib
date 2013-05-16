#include "../../include/pdf/che_pdf_file.h"
#include "../../include/pdf/che_pdf_creator.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mpParser(NULL), mVersion(PDF_VERSION_1_7), mObjCollector(pAllocator), mXRefTable(pAllocator) {}

CHE_PDF_File::~CHE_PDF_File()
{
	Close();
}

HE_BOOL CHE_PDF_File::Open( IHE_Read * pRead )
{
	if ( pRead == NULL )
	{
		return FALSE;
	}

	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mObjCollector.Clear();
		mXRefTable.Clear();
	}



	mpParser = CHE_PDF_Parser::Create( this, pRead, &mXRefTable, GetAllocator() );
	
	if ( mpParser )
	{
		mVersion = mpParser->GetPDFVersion();

		if ( mVersion == PDF_VERSION_UNKNOWN )
		{
			return FALSE;
		}

		mpParser->GetStartxref( 1024 );
		mpParser->ParseXRef();		
		
		return TRUE;
	}

	return FALSE;
}

HE_VOID CHE_PDF_File::Close()
{
 	mObjCollector.Clear();
 	mXRefTable.Clear();
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mpParser = NULL;
	}
}

HE_BOOL CHE_PDF_File::Save( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return FALSE;
	}

	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
	if ( pCreator == NULL )
	{
		return FALSE;
	}

	if ( mpParser && mpParser->m_pStrEncrypt )
	{
		pCreator->SetEncrypt( mpParser->m_pStrEncrypt );
	}

	HE_DWORD				objCount = 0;
	HE_DWORD				offset = 0;
	PDF_RefInfo				refInfo;
	HE_BOOL					bEncrypt = TRUE;

	CHE_PDF_ObjectPtr		ObjPtr;
	CHE_PDF_NamePtr			NamePtr;
	CHE_PDF_DictionaryPtr	DictPtr;

	CHE_PDF_XREF_Entry		entry;
	CHE_PDF_XREF_Table		xref( GetAllocator() );

	//PDF File Head
	pCreator->OutPutFileHead( GetPDFVersion() );

	//XRef Beginning
	entry.ObjNum = 0;
	entry.Field1 = 0;
	entry.Field2 = 65535;
	entry.Type = XREF_ENTRY_TYPE_FREE;
	++objCount;
	xref.Add( entry );

	//Out put all objects
	mXRefTable.MoveFirst();
	while( !mXRefTable.IsEOF() )
	{
		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			refInfo.objNum = entry.GetObjNum();
			refInfo.genNum = 0;

			if (	mpParser && mpParser->mEncryptRef &&
					mpParser->mEncryptRef->GetRefInfo().objNum == refInfo.objNum &&
					mpParser->mEncryptRef->GetRefInfo().genNum == refInfo.genNum )
			{
				bEncrypt = FALSE;
			}else{
				bEncrypt = TRUE;
			}

			ObjPtr = GetObject( refInfo );
			if ( ObjPtr )
			{
				if ( ObjPtr->GetType() == OBJ_TYPE_STREAM )
				{
					DictPtr = ObjPtr->GetStreamPtr()->GetDictPtr();
					CHE_PDF_ObjectPtr tmpObjPtr = DictPtr->GetElement( "Type", OBJ_TYPE_NAME );
					if ( tmpObjPtr )
					{
						NamePtr = tmpObjPtr->GetNamePtr();
					}
					if ( NamePtr )
					{
						if ( ( NamePtr->GetString() == "ObjStm" ) || ( NamePtr->GetString() == "XRef" ) )
						{
							mXRefTable.MoveNext();
							NamePtr.reset();
							continue;
						}
					}
				}

				offset = pCreator->OutPutInObject( refInfo, ObjPtr, bEncrypt );

				entry.Type = XREF_ENTRY_TYPE_COMMON;
				entry.ObjNum = refInfo.objNum;
				entry.Field1 = offset;
				entry.Field2 = 0;
				++objCount;

				xref.Add( entry );
			}
		}
		mXRefTable.MoveNext();
	}

	offset = pCreator->OutPutXRefTable( xref );

	DictPtr = GetTrailerDict();

	CHE_PDF_DictionaryPtr newTriailerDict = CHE_PDF_Dictionary::Create( GetAllocator() );

	ObjPtr = DictPtr->GetElement( "Root", OBJ_TYPE_REFERENCE );
	if ( ObjPtr )
	{
		newTriailerDict->SetAtObj( "Root", ObjPtr );
	}
	ObjPtr = DictPtr->GetElement( "Info", OBJ_TYPE_REFERENCE );
	if ( ObjPtr )
	{
		newTriailerDict->SetAtObj( "Info", ObjPtr->GetRefPtr() );
	}

	if ( mpParser && mpParser->mEncryptRef )
	{
		newTriailerDict->SetAtReference( "Encrypt", mpParser->mEncryptRef->GetRefInfo().objNum, mpParser->mEncryptRef->GetRefInfo().genNum, this );
	}

	if ( mpParser && mpParser->mIDArrayPtr )
	{
		newTriailerDict->SetAtArray( "ID", mpParser->mIDArrayPtr->Clone() );
	}

	newTriailerDict->SetAtInteger( "Size", objCount );

	pCreator->OutPutTailerDict( newTriailerDict );

	pCreator->OutPutFileTailer( offset );

	pCreator->GetAllocator()->Delete( pCreator );

	return TRUE;
}

HE_BOOL CHE_PDF_File::SaveCompact( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return FALSE;
	}

	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
	if ( pCreator == NULL )
	{
		return FALSE;
	}

	CHE_DynBuffer dynBuffer( 1024, 1024 * 4, GetAllocator() );
	IHE_Write * pBufWrite = HE_CreateDynBufferWrite( &dynBuffer, GetAllocator() );
	if ( pWrite == NULL )
	{
		pCreator->GetAllocator()->Delete( pCreator );
		return FALSE;
	}

	HE_DWORD objCount = 0;
	HE_DWORD objInStmCount = 0;
	HE_DWORD offset = 0;
	HE_DWORD nextObjNum = mXRefTable.GetMaxObjNum()+1;
	CHE_PDF_XREF_Entry entry;
	CHE_PDF_XREF_Table xref( GetAllocator() );
	PDF_RefInfo refInfo;
	CHE_Queue< HE_DWORD > objNumQueue;
	CHE_Queue< HE_DWORD > objOffset;

	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_NamePtr NamePtr;
	CHE_PDF_DictionaryPtr DictPtr;

	pCreator->OutPutFileHead( GetPDFVersion() );

	entry.ObjNum = 0;
	entry.Field1 = 0;
	entry.Field2 = 65535;
	entry.Type = XREF_ENTRY_TYPE_FREE;

	xref.Add( entry );
	++objCount;

	mXRefTable.MoveFirst();
	while( !mXRefTable.IsEOF() )
	{
		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			refInfo.objNum = entry.GetObjNum();
			refInfo.genNum = 0;
			ObjPtr = GetObject( refInfo );
			if ( ObjPtr )
			{
				//跳过不需要输出的对象流对象和交叉索引流对象
				if ( IsPdfStreamPtr( ObjPtr ) )
				{
					DictPtr = ObjPtr->GetStreamPtr()->GetDictPtr();
					
					CHE_PDF_ObjectPtr tmpObjPtr = DictPtr->GetElement( "Type", OBJ_TYPE_NAME );
					if ( tmpObjPtr )
					{
						NamePtr = tmpObjPtr->GetNamePtr();
						if ( NamePtr )
						{
							if ( NamePtr->GetString() == "ObjStm" || NamePtr->GetString() == "XRef" )
							{
								mXRefTable.MoveNext();
								continue;
							}
						}
					}
				}
				entry.Type = XREF_ENTRY_TYPE_FREE;
				xref.Add( entry );
				++objCount;
			}
		}
		mXRefTable.MoveNext();
	}

	CHE_PDF_ReferencePtr RefPtr;
	CHE_PDF_DictionaryPtr trailerDictPtr = GetTrailerDict();

// 	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_REFERENCE );
// 
// 	if ( ObjPtr )
// 	{
// 		RefPtr = ObjPtr->GetRefPtr();
// 		refInfo = RefPtr->GetRefInfo();
// 
// 		offset = pCreator->OutPutInObject( refInfo, RefPtr->GetRefObj() );
// 
// 		entry.Type = XREF_ENTRY_TYPE_COMMON;
// 		entry.ObjNum = refInfo.objNum;
// 		entry.Field1 = offset;
// 		entry.Field2 = 0;
// 		xref.Update( refInfo.objNum, entry );
// 	}

	xref.MoveFirst();
	while( !xref.IsEOF() )
	{
		if ( xref.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			if ( entry.GetType() == XREF_ENTRY_TYPE_FREE )
			{
				refInfo.objNum = entry.GetObjNum();
				refInfo.genNum = 0;
				ObjPtr = GetObject( refInfo );
				if ( ! ObjPtr )
				{
					xref.MoveNext();
					continue;
				}
				if ( ObjPtr->GetType() == OBJ_TYPE_STREAM )
				{
					offset = pCreator->OutPutInObject( refInfo, ObjPtr );
					entry.Type = XREF_ENTRY_TYPE_COMMON;
					entry.ObjNum = refInfo.objNum;
					entry.Field1 = offset;
					entry.Field2 = 0;
					xref.Update( refInfo.objNum, entry );
				}else if ( entry.GetGenNum() == 0  )
				{
					offset = pBufWrite->GetCurOffset();
					CHE_PDF_Creator::OutPutObject( pBufWrite, refInfo, ObjPtr );
					pBufWrite->WriteByte( '\n' );
					objNumQueue.Push( refInfo.objNum );
					objOffset.Push( offset );
					++objInStmCount;
					
					if ( objInStmCount == 30 )
					{
						HE_DWORD objNum = 0;
						HE_DWORD offset = 0;
						HE_DWORD index = 0;
						CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
						HE_CHAR tmpStr[1024];

						CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );

						CHE_PDF_DictionaryPtr DictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );

						while( objNumQueue.Pop( objNum ) )
						{
							objOffset.Pop( offset );

							entry.Type = XREF_ENTRY_TYPE_COMPRESSED;
							entry.ObjNum = objNum;
							entry.Field1 = nextObjNum-1;
							entry.Field2 = index;
							xref.Update( objNum, entry );

							sprintf( tmpStr, "%d %d ", objNum, offset );
							tmpBuf.Write( (HE_LPCBYTE)(&(tmpStr[0])), strlen(tmpStr) );

							++index;
						}

						offset = tmpBuf.GetSize();
						DictPtr->SetAtName( "Type", "ObjStm" );
						DictPtr->SetAtInteger( "N", objInStmCount );
						DictPtr->SetAtInteger( "First", offset );
						StmPtr->SetDict( DictPtr );

						tmpBuf.Write( dynBuffer );
						StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );

						refInfo.objNum = nextObjNum - 1;
						refInfo.genNum = 0;

						offset = pCreator->OutPutInObject( refInfo, StmPtr );
						entry.Type = XREF_ENTRY_TYPE_COMMON;
						entry.ObjNum = nextObjNum-1;
						entry.Field1 = offset;
						entry.Field2 = 0;
						xref.Add( entry );

						dynBuffer.Clear();
						objInStmCount = 0;
						++objCount;
					}
				}else{
					offset = pCreator->OutPutInObject( refInfo, ObjPtr );
					entry.Type = XREF_ENTRY_TYPE_COMMON;
					entry.ObjNum = refInfo.objNum;
					entry.Field1 = offset;
					entry.Field2 = 0;
					xref.Update( refInfo.objNum, entry );
				}
			}
		}
		xref.MoveNext();
	}

	if ( ! objNumQueue.IsEmpty() )
	{
		HE_DWORD objNum = 0;
		HE_DWORD offset = 0;
		HE_DWORD index = 0;
		CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
		HE_CHAR tmpStr[1024];
		CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );
		CHE_PDF_DictionaryPtr DictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
		while( objNumQueue.Pop( objNum ) )
		{
			objOffset.Pop( offset );

			entry.Type = XREF_ENTRY_TYPE_COMPRESSED;
			entry.ObjNum = objNum;
			entry.Field1 = nextObjNum-1;
			entry.Field2 = index;
			xref.Update( objNum, entry );

			sprintf( tmpStr, "%d %d ", objNum, offset );
			tmpBuf.Write( (HE_LPCBYTE)( &(tmpStr[0]) ), strlen(tmpStr) );

			++index;
		}

		offset = tmpBuf.GetSize();
		DictPtr->SetAtName( "Type", "ObjStm" );
		DictPtr->SetAtInteger( "N", objInStmCount );
		DictPtr->SetAtInteger( "First", offset );
		StmPtr->SetDict( DictPtr );

		tmpBuf.Write( dynBuffer );
		StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );

		refInfo.objNum = nextObjNum - 1;
		refInfo.genNum = 0;
		offset = pCreator->OutPutInObject( refInfo, StmPtr );
		entry.Type = XREF_ENTRY_TYPE_COMMON;
		entry.ObjNum = nextObjNum - 1;
		entry.Field1 = offset;
		entry.Field2 = 0;
		xref.Add( entry );

		dynBuffer.Clear();
		objInStmCount = 0;

		++objCount;
	}

	CHE_PDF_DictionaryPtr StmDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
	CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( 0, 0, NULL, GetAllocator() );

	refInfo.objNum = xref.GetMaxObjNum() + 1;
	refInfo.genNum = 0;
	
	StmPtr->SetDict( StmDictPtr );

	StmDictPtr->SetAtName( "Type", "XRef" );

	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_REFERENCE );
	if ( ObjPtr )
	{
		StmDictPtr->SetAtObj( "Root", ObjPtr->Clone() );
	}
	ObjPtr = trailerDictPtr->GetElement( "Info", OBJ_TYPE_REFERENCE );
	if ( ObjPtr )
	{
		StmDictPtr->SetAtObj( "Info", ObjPtr->Clone() );
	}
	StmDictPtr->SetAtInteger( "Size", objCount );
	CHE_PDF_ArrayPtr ArrayPtr = CHE_PDF_Array::Create( GetAllocator() );
	ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
	ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)3, GetAllocator() ) );
	ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
	StmDictPtr->SetAtArray( "W", ArrayPtr );
	ArrayPtr  = CHE_PDF_Array::Create( GetAllocator() );
	StmDictPtr->SetAtArray( "Index", ArrayPtr );

	CHE_DynBuffer tmpBuf( 1024, 1024, GetAllocator() );

	HE_DWORD lBeginNum = 0;
	HE_DWORD lNextObjNum = 0;
	HE_DWORD lCountNum = 0;

	CHE_Queue< CHE_PDF_XREF_Entry > entryList;

	HE_DWORD field1 = 0, field2 = 0, field3 = 0;

	xref.MoveFirst();
	if ( ! xref.GetCurNode( entry ) )
	{
		return FALSE/*offset*/;
	}
	lBeginNum = entry.GetObjNum();
	lNextObjNum = lBeginNum + 1;
	lCountNum = 1;
	entryList.Push( entry );

	xref.MoveNext();
	while( !xref.IsEOF() )
	{
		if ( xref.GetCurNode( entry ) )
		{
			if ( entry.GetObjNum() == lNextObjNum )
			{
				entryList.Push( entry );
				++lCountNum;
			}else{
				CHE_PDF_XREF_Entry tmpEntry;
				ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
				ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
				while( entryList.Pop( tmpEntry ) )
				{
					switch ( tmpEntry.GetType() )
					{
					case XREF_ENTRY_TYPE_FREE:
						field1 = 0; field2 = tmpEntry.GetObjNum(); field3 = tmpEntry.GetGenNum();
						break;
					case XREF_ENTRY_TYPE_COMMON:
						field1 = 1; field2 = tmpEntry.GetOffset(); field3 = tmpEntry.GetGenNum();
						break;
					case XREF_ENTRY_TYPE_COMPRESSED:
						field1 = 2; field2 = tmpEntry.GetParentObjNum(); field3 = tmpEntry.GetIndex();
						break;
					}
					HE_BYTE byte = 0;
					tmpBuf.Write( HE_LPCBYTE( &field1 ), 1 );
					byte = (HE_BYTE)( field2 >> 16 );
					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
					byte = (HE_BYTE)( field2 >> 8 );
					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
					byte = (HE_BYTE)( field2 );
					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
					tmpBuf.Write( HE_LPCBYTE( &field3 ), 1 );
				}
				entryList.Push( entry );
				lBeginNum = entry.GetObjNum();
				lCountNum = 1;
				lNextObjNum = lBeginNum;
			}
			++lNextObjNum;
		}else{
			break;
		}
		xref.MoveNext();
	}

	if ( lCountNum > 0 )
	{
		CHE_PDF_XREF_Entry tmpEntry;
		ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
		ArrayPtr->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
		while( entryList.Pop( tmpEntry ) )
		{
			switch ( tmpEntry.GetType() )
			{
			case XREF_ENTRY_TYPE_FREE:
				field1 = 0; field2 = tmpEntry.GetObjNum(); field3 = tmpEntry.GetGenNum();
				break;
			case XREF_ENTRY_TYPE_COMMON:
				field1 = 1; field2 = tmpEntry.GetOffset(); field3 = tmpEntry.GetGenNum();
				break;
			case XREF_ENTRY_TYPE_COMPRESSED:
				field1 = 2; field2 = tmpEntry.GetParentObjNum(); field3 = tmpEntry.GetIndex();
				break;
			}
			HE_BYTE byte = 0;
			tmpBuf.Write( HE_LPCBYTE( &field1 ), 1 );
			byte = (HE_BYTE)( field2 >> 16 );
			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
			byte = (HE_BYTE)( field2 >> 8 );
			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
			byte = (HE_BYTE)( field2 );
			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
			tmpBuf.Write( HE_LPCBYTE( &field3 ), 1 );
		}
	}

	StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );

	refInfo.objNum = nextObjNum + 1;
	refInfo.genNum= 0;

	offset = pCreator->OutPutInObject( refInfo, StmPtr );

	pCreator->OutPutFileTailer( offset );

	pCreator->GetAllocator()->Delete( pCreator );
 
 	return TRUE;
}

HE_BOOL CHE_PDF_File::Authenticate( const CHE_ByteString & password ) const
{
	if ( mpParser )
	{
		return mpParser->Authenticate( password );
	}
	return FALSE;
}

CHE_PDF_ObjectPtr CHE_PDF_File::GetObject( const PDF_RefInfo & refInfo )
{
	CHE_PDF_ObjectPtr ObjPtr = mObjCollector.GetObj( refInfo );
	if ( ObjPtr )
	{
		return ObjPtr;
	}
	if ( mpParser == NULL )
	{
		return ObjPtr;
	}
	CHE_PDF_XREF_Entry entry;
	if ( mXRefTable.Get( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			ObjPtr = mpParser->GetObject();
			if ( ObjPtr )
			{
				mObjCollector.Add( refInfo, ObjPtr );
				return ObjPtr;
			}
		}else if ( entry.GetType() == XREF_ENTRY_TYPE_COMPRESSED )
		{
			PDF_RefInfo stmRefInfo;
			stmRefInfo.objNum = entry.GetParentObjNum();
			stmRefInfo.genNum = 0;
			ObjPtr = GetObject( stmRefInfo );
			if ( ! ObjPtr )
			{
				return ObjPtr;
			}
			CHE_PDF_StreamPtr pStm = ObjPtr->GetStreamPtr();
			if ( ! pStm )
			{
				return pStm;
			}
			ObjPtr = mpParser->GetObjectInObjStm( pStm, refInfo, entry.GetIndex() );
			if ( ObjPtr )
			{
				mObjCollector.Add( refInfo, ObjPtr );
				return ObjPtr;
			}
		}
	}
	return ObjPtr;
}

HE_DWORD CHE_PDF_File::GetFileSize() const
{
	if ( mpParser )
	{
		return mpParser->GetFileSize();
	}
	return 0;
}

PDF_VERSION CHE_PDF_File::GetPDFVersion() const
{
	return mVersion;
}

HE_VOID CHE_PDF_File::SetPDFVersion( PDF_VERSION version )
{
	mVersion = version;
}

HE_BOOL CHE_PDF_File::SetInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return FALSE;
	}
	if ( infoType > 8 || infoType < 0 )
	{
		return FALSE;
	}
	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_DictionaryPtr InfoDictPtr;

	ObjPtr = GetTrailerDict()->GetElement( "Info", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		InfoDictPtr = ObjPtr->GetDictPtr();
	}else{
		PDF_RefInfo refInfo = CreateDictObject( InfoDictPtr );
		if ( InfoDictPtr )
		{
			GetTrailerDict()->SetAtReference( "Info", refInfo.objNum, refInfo.genNum, this );
		}
	}

	if ( ! InfoDictPtr )
	{
		return FALSE;
	}

	switch( infoType )
	{
	case DOCUMENT_INFO_TITLE:
		InfoDictPtr->SetAtString( "Title", str );
		break;
	case DOCUMENT_INFO_AUTHOR:
		InfoDictPtr->SetAtString( "Author", str );
		break;
	case DOCUMENT_INFO_SUBJECT:
		InfoDictPtr->SetAtString( "Subject", str );
		break;
	case DOCUMENT_INFO_KEYWORDS:
		InfoDictPtr->SetAtString( "Keywords", str );
		break;
	case DOCUMENT_INFO_CREATOR:
		InfoDictPtr->SetAtString( "Creator", str );
		break;
	case DOCUMENT_INFO_PRODUCER:
		InfoDictPtr->SetAtString( "Producer", str );
		break;
	case DOCUMENT_INFO_CREATIONDATE:
		InfoDictPtr->SetAtString( "CreationDate", str );
		break;
	case DOCUMENT_INFO_MODDATE:
		InfoDictPtr->SetAtString( "ModDate", str );
		break;
	case DOCUMENT_INFO_TRAPPED:
		InfoDictPtr->SetAtString( "Trapped", str );
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetTrailerDict() const
{
	return mXRefTable.GetTrailer();
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetRootDict()
{
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY )->GetDictPtr();
	}
	return CHE_PDF_DictionaryPtr();
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetInfoDict()
{
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "Info", OBJ_TYPE_DICTIONARY )->GetDictPtr();
	}
	return CHE_PDF_DictionaryPtr();
}

CHE_PDF_ArrayPtr CHE_PDF_File::GetIDArray()
{
	CHE_PDF_ArrayPtr arrayPtr;
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "ID", OBJ_TYPE_ARRAY )->GetArrayPtr();
	}
	return arrayPtr;
}

HE_VOID CHE_PDF_File::CreateTrailerDict()
{
	CHE_PDF_DictionaryPtr pTmpDict =  CHE_PDF_Dictionary::Create( GetAllocator() );
	mXRefTable.AddTrailerDict( pTmpDict );
}

HE_VOID CHE_PDF_File::CreateCatalogDict()
{
	CHE_PDF_DictionaryPtr trailerDict = mXRefTable.GetTrailer();
	if ( ! trailerDict )
	{
		CreateTrailerDict();
		trailerDict = mXRefTable.GetTrailer();
	}
	if ( trailerDict )
	{
		CHE_PDF_DictionaryPtr pagesDictPtr;
		PDF_RefInfo pagesDictRef = CreateDictObject( pagesDictPtr );
		if ( pagesDictPtr )
		{
			pagesDictPtr->SetAtName( "Type", "Pages" );
			CHE_PDF_ArrayPtr pArray = CHE_PDF_Array::Create( GetAllocator() );
			pagesDictPtr->SetAtArray( "Kids", pArray );
			pagesDictPtr->SetAtInteger( "Count", 0 );

			CHE_PDF_DictionaryPtr catalogDictPtr;
			PDF_RefInfo catalogDictRef = CreateDictObject( catalogDictPtr );
			if ( catalogDictPtr )
			{
				catalogDictPtr->SetAtName( "Type", "Catalog" );
				catalogDictPtr->SetAtReference( "Pages", pagesDictRef.objNum, pagesDictRef.genNum, this );
				trailerDict->SetAtReference( "Root", catalogDictRef.objNum, catalogDictRef.genNum, this );
			}
		}
	}	 
}

PDF_RefInfo CHE_PDF_File::CreateNullObject( CHE_PDF_NullPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Null::Create( GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateBooleanObject( CHE_PDF_BooleanPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Boolean::Create( false, GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateNumberObject( CHE_PDF_NumberPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Number::Create( 0, GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateStringObject( CHE_PDF_StringPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_String::Create( "", GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateNameObject( CHE_PDF_NamePtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Name::Create( "", GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateArrayObject( CHE_PDF_ArrayPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Array::Create( GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateDictObject( CHE_PDF_DictionaryPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Dictionary::Create( GetAllocator() );
	if ( ptrRet )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateStreamObject( CHE_PDF_StreamPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	mXRefTable.AddNewEntry( entry );

	ptrRet = CHE_PDF_Stream::Create( entry.ObjNum, 0 );
	if ( ptrRet )
	{
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, ptrRet );
	}
	return refInfo;
}