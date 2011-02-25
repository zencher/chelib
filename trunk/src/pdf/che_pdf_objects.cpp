#include "../../include/pdf/che_pdf_objects.h"
#include <memory.h>

HE_VOID CHE_PDF_Array::Append( CHE_PDF_Object * pObj )
{
	if ( pObj )
	{
		m_array.Append( (HE_LPVOID)pObj );
	}
}

CHE_PDF_Object * CHE_PDF_Array::GetElement( HE_DWORD index ) const
{
	return (CHE_PDF_Object*)m_array.GetItem( index );
}

CHE_PDF_Object*	CHE_PDF_Dictionary::GetElement( CHE_ByteString & key )const
{
	return (CHE_PDF_Object*)m_Map.GetItem( key );
}

HE_VOID	CHE_PDF_Dictionary::SetAtBoolean( CHE_ByteString & key, bool value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtInteger( CHE_ByteString & key, HE_INT32 value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtFloatNumber( CHE_ByteString & key, HE_FLOAT value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtString( CHE_ByteString & key, const CHE_ByteString& string )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_String * pObj = CHE_PDF_String::Create( string );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtName( CHE_ByteString & key, const CHE_ByteString& name )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Name * pObj = CHE_PDF_Name::Create( name );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtArray( CHE_ByteString & key, CHE_PDF_Array * pArray )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pArray );
	}
}

HE_VOID CHE_PDF_Dictionary::SetAtDictionary( CHE_ByteString & key, CHE_PDF_Dictionary * pDict )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pDict );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtReference( CHE_ByteString & key, HE_DWORD objnum )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Reference * pObj = CHE_PDF_Reference::Create( objnum );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

CHE_PDF_Stream::CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict )
{
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = NULL;
	if ( pData != NULL && size != 0 )
	{
		m_pDataBuf = new HE_BYTE[size];
		memcpy( m_pDataBuf, pData, size );
	}
	if ( pDict )
	{
		m_pDict = pDict;
	}
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_FileRead* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict )
{
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = FALSE;
	m_FileOffset = 0;
	m_pFile = NULL;
	if ( pFile != NULL )
	{
		m_pFile = pFile;
		m_FileOffset = offset;
		m_dwSize = size;
	}
	if ( pDict )
	{
		m_pDict = pDict;
	}
}

CHE_PDF_IndirectObject::CHE_PDF_IndirectObject( HE_DWORD objNum, CHE_PDF_Object * pObj )
{
	m_ObjNum = objNum;
	m_pObj = pObj;
}

CHE_PDF_Dictionary* CHE_PDF_IndirectObject::GetDict() const
{
	if ( m_pObj )
	{
		if ( m_pObj->GetType() == PDFOBJ_DICTIONARY  )
		{
			return (CHE_PDF_Dictionary*)m_pObj;
		}else if ( m_pObj->GetType() == PDFOBJ_STREAM )
		{
			return ((CHE_PDF_Stream*)m_pObj)->GetDict();
		}	 
	}
	return NULL;
}

CHE_PDF_Stream*	CHE_PDF_IndirectObject::GetStream() const
{
	if ( m_pObj )
	{
		 if ( m_pObj->GetType() == PDFOBJ_STREAM )
		{
			return (CHE_PDF_Stream*)m_pObj;
		}	 
	}
	return NULL;
}

CHE_PDF_Object* CHE_PDF_IndirectObject::GetObject() const
{
	return m_pObj;
}
