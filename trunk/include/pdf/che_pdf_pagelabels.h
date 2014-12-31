#ifndef _CHE_PDF_LABELS_H_
#define _CHE_PDF_LABELS_H_

#include "../che_base.h"
#include "che_pdf_numbertree.h"

#include <unordered_map>
using namespace std;

enum PDF_LABEL_STYLE
{
	LABEL_NONE,
	LABEL_DEC_NUM,
	LABEL_UPPER_ROMAN_NUM,
	LABEL_LOWER_ROMAN_NUM,
	LABEL_UPPER_LETTER,
	LABEL_LOWER_LETTER
};

class CHE_PDF_PageLabel
{
public:
	PDF_LABEL_STYLE	stype;
	HE_INT32		start;
	CHE_ByteString	prefix;
};

class CHE_PDF_PageLabels : public CHE_Object
{
public:
    CHE_PDF_PageLabels(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = NULL);
	~CHE_PDF_PageLabels();
    
private:
	HE_BOOL GetLabel(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_PageLabel & label);

	CHE_PDF_NumberTree * mpNumberTree;
	std::unordered_map<HE_INT32, CHE_PDF_PageLabel> mLabelsInfo;
};

#endif
