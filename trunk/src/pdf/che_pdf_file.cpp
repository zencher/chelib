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
		mObjCollector.ReleaseObj();
		mObjCollector.Clear();
		mXRefTable.Clear();
	}
	mpParser = CHE_PDF_Parser::Create( this, pRead, &mXRefTable, GetAllocator() );
	if ( mpParser )
	{
		mVersion = mpParser->GetPDFVersion();
		return TRUE;
	}
	return FALSE;
}

HE_VOID CHE_PDF_File::Close()
{
	mObjCollector.ReleaseObj();
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

	HE_DWORD objCount = 0;
	HE_DWORD offset = 0;
	CHE_PDF_XREF_Entry entry;
	CHE_PDF_XREF_Table xref( GetAllocator() );
	HE_PDF_RefInfo refInfo;
	CHE_PDF_IndirectObject * pInObj = NULL;

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
			pInObj = GetInObject( refInfo );
			if ( pInObj )
			{
				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
				{
					CHE_PDF_Dictionary * pDict = pInObj->GetObj()->ToStream()->GetDict();
					CHE_PDF_Object * pObj = NULL;
					CHE_PDF_Name * pName = NULL;
					CHE_PDF_ObjectCollector objCollecor;
					pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME, objCollecor );
					if ( pObj )
					{
						pName = pObj->ToName();
						if ( pName )
						{
							if ( pName->GetString() == "ObjStm" )
							{
								mXRefTable.MoveNext();
								continue;
							}
						}
					}
				}
				offset = pCreator->OutPutInObject( pInObj );
				entry.Type = XREF_ENTRY_TYPE_COMMON;
				entry.ObjNum = refInfo.objNum;
				entry.Field1 = offset;
				entry.Field2 = 0;
				xref.Add( entry );
				++objCount;
			}
		}
		mXRefTable.MoveNext();
	}

	offset = pCreator->OutPutXRefTable( xref );
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_Dictionary * pDict = mXRefTable.GetTrailer();
	CHE_PDF_Dictionary * pNewDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	pObj = pDict->GetElement( "Root" );
	if ( pObj )
	{
		pNewDict->SetAtObj( "Root", pObj->Clone() );
	}
	pObj = pDict->GetElement( "Info" );
	if ( pObj )
	{
		pNewDict->SetAtObj( "Info", pObj->Clone() );
	}
	pNewDict->SetAtInteger( "Size", objCount );
	pCreator->OutPutTailerDict( pNewDict );
	pCreator->OutPutFileTailer( offset );
	pNewDict->Release();
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
	HE_PDF_RefInfo refInfo;
	CHE_PDF_IndirectObject * pInObj = NULL;
	CHE_Queue< HE_DWORD > objNumQueue;
	CHE_Queue< HE_DWORD > objOffset;

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
			pInObj = GetInObject( refInfo );
			if ( pInObj )
			{
				//跳过不需要输出的对象流对象和交叉索引流对象
				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
				{
					CHE_PDF_Dictionary * pDict = pInObj->GetObj()->ToStream()->GetDict();
					CHE_PDF_Object * pObj = NULL;
					CHE_PDF_Name * pName = NULL;
					CHE_PDF_ObjectCollector objCollecor;
					pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME, objCollecor );
					if ( pObj )
					{
						pName = pObj->ToName();
						if ( pName )
						{
							if ( pName->GetString() == "ObjStm" )
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

	//输出Catalog Dict Object
	CHE_PDF_Reference * pRef = NULL;
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_Dictionary * pTrailerDict = mXRefTable.GetTrailer();
	pObj = pTrailerDict->GetElement( "Root" );
	if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		pRef = pObj->ToReference();
	}
	if ( pRef )
	{
		HE_PDF_RefInfo refInfo = pRef->GetRefInfo();
		CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );

		offset = pCreator->OutPutInObject( pInObj );
		entry.Type = XREF_ENTRY_TYPE_COMMON;
		entry.ObjNum = refInfo.objNum;
		entry.Field1 = offset;
		entry.Field2 = 0;
		xref.Update( refInfo.objNum, entry );
	}

	xref.MoveFirst();
	while( !xref.IsEOF() )
	{
		if ( xref.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			if ( entry.GetType() == XREF_ENTRY_TYPE_FREE )
			{
				refInfo.objNum = entry.GetObjNum();
				refInfo.genNum = 0;
				pInObj = GetInObject( refInfo );
				if ( pInObj == NULL )
				{
					xref.MoveNext();
					continue;
				}
				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
				{
					offset = pCreator->OutPutInObject( pInObj );
					entry.Type = XREF_ENTRY_TYPE_COMMON;
					entry.ObjNum = refInfo.objNum;
					entry.Field1 = offset;
					entry.Field2 = 0;
					xref.Update( refInfo.objNum, entry );
				}else if ( entry.GetGenNum() == 0  )
				{
					offset = pBufWrite->GetCurOffset();
					CHE_PDF_Creator::OutPutObject( pBufWrite, pInObj->GetObj() );
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
						CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );
						CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
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
						pDict->SetAtName( "Type", "ObjStm" );
						pDict->SetAtInteger( "N", objInStmCount );
						pDict->SetAtInteger( "First", offset );
						pStm->SetDict( pDict );

						tmpBuf.Write( dynBuffer );
						pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );

						CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( nextObjNum-1, 0, pStm, this, GetAllocator() );
						offset = pCreator->OutPutInObject( pInObj );
						entry.Type = XREF_ENTRY_TYPE_COMMON;
						entry.ObjNum = nextObjNum-1;
						entry.Field1 = offset;
						entry.Field2 = 0;
						xref.Add( entry );
				
						pInObj->Release();

						dynBuffer.Clear();
						objInStmCount = 0;
						++objCount;
					}
				}else{
					offset = pCreator->OutPutInObject( pInObj );
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
		CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );
		CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
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
		pDict->SetAtName( "Type", "ObjStm" );
		pDict->SetAtInteger( "N", objInStmCount );
		pDict->SetAtInteger( "First", offset );
		pStm->SetDict( pDict );

		tmpBuf.Write( dynBuffer );
		pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );

		CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( nextObjNum-1, 0, pStm, this, GetAllocator() );
		offset = pCreator->OutPutInObject( pInObj );
		entry.Type = XREF_ENTRY_TYPE_COMMON;
		entry.ObjNum = nextObjNum-1;
		entry.Field1 = offset;
		entry.Field2 = 0;
		xref.Add( entry );

		pInObj->Release();

		dynBuffer.Clear();
		objInStmCount = 0;

		++objCount;
	}

	/*CHE_PDF_Object * pObj = NULL;*/
	/*CHE_PDF_Dictionary * pTrailerDict = mXRefTable.GetTrailer();*/
	CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( 0, 0, NULL, GetAllocator() );
	pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( xref.GetMaxObjNum() + 1, 0, pStm, this, GetAllocator() );
	pStm->SetDict( pDict );

	pDict->SetAtName( "Type", "XRef" );
	pObj = pTrailerDict->GetElement( "Root" );
	if ( pObj )
	{
		pDict->SetAtObj( "Root", pObj->Clone() );
	}
	pObj = pTrailerDict->GetElement( "Info" );
	if ( pObj )
	{
		pDict->SetAtObj( "Info", pObj->Clone() );
	}
	pDict->SetAtInteger( "Size", objCount );
	CHE_PDF_Array * pArray = CHE_PDF_Array::Create( GetAllocator() );
	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)3, GetAllocator() ) );
	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
	pDict->SetAtArray( "W", pArray );
	pArray  = CHE_PDF_Array::Create( GetAllocator() );
	pDict->SetAtArray( "Index", pArray );

	CHE_DynBuffer tmpBuf( 1024, 1024, GetAllocator() );

	HE_DWORD lBeginNum = 0;
	HE_DWORD lNextObjNum = 0;
	HE_DWORD lCountNum = 0;
	//CHE_PDF_XREF_Entry entry;
	CHE_Queue< CHE_PDF_XREF_Entry > entryList;

	HE_DWORD field1 = 0, field2 = 0, field3 = 0;

	xref.MoveFirst();
	if ( ! xref.GetCurNode( entry ) )
	{
		return offset;
	}
	lBeginNum = entry.GetObjNum();
	lNextObjNum = lBeginNum+1;
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
				pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
				pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
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
		pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
		pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
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

	pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );

	offset = pCreator->OutPutInObject( pInObj );
	pCreator->OutPutFileTailer( offset );

	pStm->Release();
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

CHE_PDF_Object * CHE_PDF_File::GetObject( const HE_PDF_RefInfo & refInfo )
{
	CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );
	if ( pInObj )
	{
		return pInObj->GetObj()->Clone();
	}
	return NULL;
}

CHE_PDF_IndirectObject * CHE_PDF_File::GetInObject( const HE_PDF_RefInfo & refInfo )
{
	CHE_PDF_IndirectObject * pInObj = mObjCollector.GetInObj( refInfo );
	if ( pInObj )
	{
		return pInObj;
	}
	if ( mpParser == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_XREF_Entry entry;
	if ( mXRefTable.Get( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			pObj = mpParser->GetObject();
			if ( pObj )
			{
				pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, this, GetAllocator() );
				mObjCollector.Add( pInObj );
				return pInObj;
			}
		}else{
			HE_PDF_RefInfo stmRefInfo;
			stmRefInfo.objNum = entry.GetParentObjNum();
			stmRefInfo.genNum = 0;
			pInObj = GetInObject( stmRefInfo );
			if ( pInObj == NULL )
			{
				return NULL;
			}
			CHE_PDF_Object * pTmpObj = pInObj->GetObj();
			if ( pTmpObj == NULL || pTmpObj->GetType() != OBJ_TYPE_STREAM )
			{
				return NULL;
			}
			pObj = mpParser->GetObjectInObjStm( pTmpObj->ToStream(), refInfo, entry.GetIndex() );
			if ( pObj )
			{
				pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, this, GetAllocator() );
				mObjCollector.Add( pInObj );
				return pInObj;
			}
		}
	}
	return NULL;
}

HE_BOOL CHE_PDF_File::ChangeObject( const HE_PDF_RefInfo & refInfo, CHE_PDF_Object * pObj )
{
	if ( pObj == NULL )
	{
		return FALSE;
	}
	CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );
	if ( pInObj )
	{
		pInObj->mpObj = pObj;
		CHE_PDF_XREF_Entry entry;
		entry.ObjNum = refInfo.objNum;
		entry.Type = XREF_ENTRY_TYPE_NEW;
		mXRefTable.Update( refInfo.objNum, entry );
		return TRUE;
	}
	return FALSE;
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

CHE_PDF_Dictionary * CHE_PDF_File::GetTrailerDict() const
{
	return mXRefTable.GetTrailer();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetRootDict()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "Root", OBJ_TYPE_DICTIONARY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToDict();
	}
	return NULL;
}

CHE_PDF_Dictionary * CHE_PDF_File::GetInfoDict()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "Info", OBJ_TYPE_DICTIONARY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToDict();
	}
	return NULL;
}

CHE_PDF_Array * CHE_PDF_File::GetIDArray()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "ID", OBJ_TYPE_ARRAY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToArray();
	}
	return NULL;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Null()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Null * pObj = CHE_PDF_Null::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Boolean( HE_BOOL value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_INT32 value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_FLOAT value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_String( const CHE_ByteString & str )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_String * pObj = CHE_PDF_String::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Name( const CHE_ByteString & str )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Name * pObj = CHE_PDF_Name::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Array()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Array * pObj = CHE_PDF_Array::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Dict()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Dictionary * pObj = CHE_PDF_Dictionary::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject*	CHE_PDF_File::CreateInObj_Stream()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Stream * pObj = CHE_PDF_Stream::Create( entry.ObjNum, 0 );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}