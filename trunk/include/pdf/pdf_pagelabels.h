#ifndef _CHELIB_PDF_LABELS_H_
#define _CHELIB_PDF_LABELS_H_

#include "../base.h"
#include "pdf_numbertree.h"

#include <unordered_map>

namespace chelib {

enum PDF_LABEL_STYLE
{
	LABEL_NONE,
	LABEL_DEC_NUM,
	LABEL_UPPER_ROMAN_NUM,
	LABEL_LOWER_ROMAN_NUM,
	LABEL_UPPER_LETTER,
	LABEL_LOWER_LETTER
};

class CPDF_PageLabel
{
public:
	PDF_LABEL_STYLE stype;
	int32           start;
	CByteString     prefix;
};

class CPDF_PageLabels : public CObject
{
public:
    CPDF_PageLabels(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator = nullptr);
	~CPDF_PageLabels();
    
private:
	bool GetLabel(const CPDF_DictionaryPtr & dict, CPDF_PageLabel & label);

	CPDF_NumberTree * mpNumberTree;
	std::unordered_map<int32, CPDF_PageLabel> mLabelsInfo;
};
    
}//namespace

#endif
