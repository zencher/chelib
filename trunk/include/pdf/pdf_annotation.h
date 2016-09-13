#ifndef _CHELIB_PDF_ANNOTATION_H_
#define _CHELIB_PDF_ANNOTATION_H_

#include "pdf_object.h"

namespace chelib {

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


class CPDF_Annotation : public CObject
{
public:
    CPDF_Annotation(CPDF_DictionaryPtr & dictPtr, CAllocator * pAllocator = nullptr);
    
    bool IsInvisable() const;
    bool IsHidden() const;
    bool IsPrint() const;
    bool IsNoZoom() const;
    bool IsNoRotate() const;
    bool IsNoView() const;
    bool IsReadOnly() const;
    bool Locked() const;
    bool ToggleNoView() const;
    bool LockedContents() const;
    
protected:
    uint32                  mFlag;
    PDF_ANNOT_TYPE          mType;
    CPDF_DictionaryPtr      mDictPtr;
};

}//namespace

#endif