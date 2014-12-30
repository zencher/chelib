#ifndef _CHE_PDF_LABELS_H_
#define _CHE_PDF_LABELS_H_

#include "../che_base.h"
#include "che_pdf_numbertree.h"

#include <unordered_map>
using namespace std;

class CHE_PDF_PageLabel : public CHE_Object
{
public:
};

class CHE_PDF_PageLabels : public CHE_Object
{
public:
    CHE_PDF_PageLabels( CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = NULL );
    
private:
    che_pdf_numbertree * mpNumberTree;

};

#endif
