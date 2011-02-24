#include "../../include/pdf/che_pdf_objects.h"

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
