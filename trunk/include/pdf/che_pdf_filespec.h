#ifndef _CHE_PDF_FILESPEC_H_
#define _CHE_PDF_FILESPEC_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_objects.h"

class CHE_PDF_FileSpec
{
public:
	CHE_PDF_FileSpec(const CHE_ByteString & path) : mFileString(path) {}
	CHE_PDF_FileSpec(const CHE_PDF_NamePtr & obj) : mFileString(obj->GetString()) {}
	CHE_PDF_FileSpec(const CHE_PDF_DictionaryPtr & obj) : mDict(obj) {}
    
private:
    CHE_ByteString          mFileString;
    CHE_PDF_DictionaryPtr   mDict;
};

#endif
