#ifndef _CHE_PDF_ANNOTATION_H_
#define _CHE_PDF_ANNOTATION_H_

#include "che_pdf_objects.h"

enum PDF_ANNOT_TYPE
{
    ANNOT_TYPE_Unknown,
    ANNOT_TYPE_Text,
    ANNOT_TYPE_Link,
    ANNOT_TYPE_FreeText,
    ANNOT_TYPE_Line,
    ANNOT_TYPE_Square,
    ANNOT_TYPE_Circle,
    ANNOT_TYPE_Polygon,
    ANNOT_TYPE_PolyLine,
    ANNOT_TYPE_Highlight,
    ANNOT_TYPE_Underline,
    ANNOT_TYPE_Squlggly,
    ANNOT_TYPE_StrikeOut,
    ANNOT_TYPE_Stamp,
    ANNOT_TYPE_Caret,
    ANNOT_TYPE_Ink,
    ANNOT_TYPE_Popup,
    ANNOT_TYPE_FileAttachment,
    ANNOT_TYPE_Sound,
    ANNOT_TYPE_Moive,
    ANNOT_TYPE_Widget,
    ANNOT_TYPE_Screen,
    ANNOT_TYPE_PrintMark,
    ANNOT_TYPE_TrapNet,
    ANNOT_TYPE_Watermark,
    ANNOT_TYPE_3D
};




class CHE_PDF_Annotation : public CHE_Object
{
public:
    CHE_PDF_Annotation(CHE_PDF_DictionaryPtr & dictPtr, CHE_Allocator * pAllocator = NULL);
    
    HE_BOOL IsInvisable() const;
    HE_BOOL IsHidden() const;
    HE_BOOL IsPrint() const;
    HE_BOOL IsNoZoom() const;
    HE_BOOL IsNoRotate() const;
    HE_BOOL IsNoView() const;
    HE_BOOL IsReadOnly() const;
    HE_BOOL Locked() const;
    HE_BOOL ToggleNoView() const;
    HE_BOOL LockedContents() const;
    
protected:
    HE_UINT32                   mFlag;
    PDF_ANNOT_TYPE              mType;
    CHE_PDF_DictionaryPtr       mDictPtr;
};

#endif