#ifndef _CHELIB_PDF_FILESPEC_H_
#define _CHELIB_PDF_FILESPEC_H_

#include "che_pdf_object.h"

namespace chelib {

class CPDF_FileSpec
{
public:
	CPDF_FileSpec(const CByteString & path) : mFileString(path) {}
	CPDF_FileSpec(const CPDF_NamePtr & obj) : mFileString(obj->GetString()) {}
	CPDF_FileSpec(const CPDF_DictionaryPtr & obj) : mDict(obj) {}
    
private:
    CByteString          mFileString;
    CPDF_DictionaryPtr   mDict;
};
    
}//namespace

#endif
