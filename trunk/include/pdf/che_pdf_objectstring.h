#ifndef _CHE_PDF_OBJECTSTRING_H_
#define _CHE_PDF_OBJECTSTRING_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"

class CHE_PDF_ObjectString
{
public:
	static HE_BOOL PdfObjPtrToBuf( const CHE_PDF_ObjectPtr & objPtr, CHE_DynBuffer & buf );

	static HE_BOOL FloatToBuf( HE_FLOAT val, CHE_DynBuffer & buf );

	static HE_BOOL IntegerToBuf( HE_INT32 val, CHE_DynBuffer & buf );

	static HE_BOOL DWORDToBuf( HE_ULONG val, CHE_DynBuffer & buf );

	static HE_BOOL SpaceToBuf( CHE_DynBuffer & buf );

	static HE_BOOL StringToBuf( HE_LPCSTR pChar, CHE_DynBuffer & buf );

	static HE_BOOL StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf );
}; 

#endif