#include "../../include/che_pdf_annotation.h"

namespace chelib {

#define ANNOT_FLAG_INVISIABLE       0x00000001
#define ANNOT_FLAG_HIDDEN           0x00000002
#define ANNOT_FLAG_PRINT            0x00000004
#define ANNOT_FLAG_NOZOOM           0x00000008
#define ANNOT_FLAG_NOROTATE         0x00000010
#define ANNOT_FLAG_NOVIEW           0x00000020
#define ANNOT_FLAG_READONLY         0x00000040
#define ANNOT_FLAG_LOCKED           0x00000080
#define ANNOT_FLAG_TOGGLENOVIEW     0x00000100
#define ANNOT_FLAG_LOCKEDCONTENTS   0x00000200

CPDF_Annotation::CPDF_Annotation(CPDF_DictionaryPtr & dictPtr, CAllocator * pAllocator/*= nullptr*/)
 :CObject(pAllocator), mDictPtr(dictPtr), mType(ANNOT_TYPE_Unknown), mFlag(0)
{
    if (!dictPtr)
    {
        return;
    }

    CPDF_ObjectPtr objPtr;
    objPtr = dictPtr->GetElement("Type", OBJ_TYPE_NAME);
    if (objPtr)
    {
        if (objPtr->GetNamePtr()->GetString() != "Annot")
        {
            return;
        }
    }
    
    objPtr = dictPtr->GetElement("Subtype", OBJ_TYPE_NAME);
    if (objPtr)
    {
        CByteString str(GetAllocator());
        str = objPtr->GetNamePtr()->GetString();
        if (str == "Text")
        {
            mType = ANNOT_TYPE_Text;
        }else if (str == "Link")
        {
            mType = ANNOT_TYPE_Text;
        }else if (str == "FreeText")
        {
            mType = ANNOT_TYPE_FreeText;
        }else if (str == "Square")
        {
            mType = ANNOT_TYPE_Square;
        }else if (str == "Circle")
        {
            mType = ANNOT_TYPE_Circle;
        }else if (str == "Polygon")
        {
            mType = ANNOT_TYPE_Polygon;
        }else if (str == "PolyLine")
        {
            mType = ANNOT_TYPE_PolyLine;
        }else if (str == "Highlight")
        {
            mType = ANNOT_TYPE_Highlight;
        }else if (str == "Underline")
        {
            mType = ANNOT_TYPE_Underline;
        }else if (str == "Squlggly")
        {
            mType = ANNOT_TYPE_Squlggly;
        }else if (str == "StrikeOut")
        {
            mType = ANNOT_TYPE_StrikeOut;
        }else if (str == "Square")
        {
            mType = ANNOT_TYPE_Square;
        }else if (str == "Stamp")
        {
            mType = ANNOT_TYPE_Stamp;
        }else if (str == "Caret")
        {
            mType = ANNOT_TYPE_Caret;
        }else if (str == "Ink")
        {
            mType = ANNOT_TYPE_Ink;
        }else if (str == "Popup")
        {
            mType = ANNOT_TYPE_Popup;
        }else if (str == "FileAttachment")
        {
            mType = ANNOT_TYPE_FileAttachment;
        }else if (str == "Sound")
        {
            mType = ANNOT_TYPE_Sound;
        }else if (str == "Movie")
        {
            mType = ANNOT_TYPE_Moive;
        }else if (str == "Widget")
        {
            mType = ANNOT_TYPE_Widget;
        }else if (str == "Screen")
        {
            mType = ANNOT_TYPE_Screen;
        }else if (str == "PrintMark")
        {
            mType = ANNOT_TYPE_PrintMark;
        }else if (str == "TrapNet")
        {
            mType = ANNOT_TYPE_TrapNet;
        }else if (str == "Watermark")
        {
            mType = ANNOT_TYPE_Watermark;
        }else if (str == "3D")
        {
            mType = ANNOT_TYPE_3D;
        }
    }
    
    objPtr = dictPtr->GetElement("Flag", OBJ_TYPE_NUMBER);
    if (objPtr)
    {
        mFlag = objPtr->GetNumberPtr()->GetInteger();
    }
}

bool CPDF_Annotation::IsInvisable() const
{
    return (mFlag & ANNOT_FLAG_INVISIABLE) != 0;
}

bool CPDF_Annotation::IsHidden() const
{
    return (mFlag & ANNOT_FLAG_HIDDEN) != 0;
}

bool CPDF_Annotation::IsPrint() const
{
    return (mFlag & ANNOT_FLAG_PRINT) != 0;
}

bool CPDF_Annotation::IsNoZoom() const
{
    return (mFlag & ANNOT_FLAG_NOZOOM) != 0;
}

bool CPDF_Annotation::IsNoRotate() const
{
    return (mFlag & ANNOT_FLAG_NOROTATE) != 0;
}

bool CPDF_Annotation::IsNoView() const
{
    return (mFlag & ANNOT_FLAG_NOVIEW) != 0;
}

bool CPDF_Annotation::IsReadOnly() const
{
    return (mFlag & ANNOT_FLAG_READONLY) != 0;
}

bool CPDF_Annotation::Locked() const
{
    return (mFlag & ANNOT_FLAG_LOCKED) != 0;
}

bool CPDF_Annotation::ToggleNoView() const
{
    return (mFlag & ANNOT_FLAG_TOGGLENOVIEW) != 0;
}

bool CPDF_Annotation::LockedContents() const
{
    return (mFlag & ANNOT_FLAG_LOCKEDCONTENTS) != 0;
}
    
}//namespace