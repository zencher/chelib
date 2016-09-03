#ifndef _CHE_PDF_OBJECTSTRING_H_
#define _CHE_PDF_OBJECTSTRING_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

class CHE_PDF_ObjectString
{
public:
	static bool ObjectToBuf( const CHE_PDF_ObjectPtr & objPtr, CHE_DynBuffer & buf );

	static bool FloatToBuf( FLOAT val, CHE_DynBuffer & buf );

	static bool IntegerToBuf( uint32 val, CHE_DynBuffer & buf );

	static bool SpaceToBuf( CHE_DynBuffer & buf );

	static bool StringToBuf( char const * pstr, CHE_DynBuffer & buf );

	static bool StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf );
}; 

#endif