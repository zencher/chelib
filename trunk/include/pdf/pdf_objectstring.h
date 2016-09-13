#ifndef _CHELIB_PDF_OBJECTSTRING_H_
#define _CHELIB_PDF_OBJECTSTRING_H_

#include "pdf_object.h"

namespace chelib {

class CPDF_ObjectString
{
public:
	static bool ObjectToBuf( const CPDF_ObjectPtr & objPtr, CDynBuffer & buf );

	static bool FloatToBuf( FLOAT val, CDynBuffer & buf );

	static bool IntegerToBuf( uint32 val, CDynBuffer & buf );

	static bool SpaceToBuf( CDynBuffer & buf );

	static bool StringToBuf( char const * pstr, CDynBuffer & buf );

	static bool StringToBuf( const CByteString & str, CDynBuffer & buf );
};

}//namespace

#endif