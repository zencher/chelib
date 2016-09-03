#include <cassert>
#include <stack>
using namespace std;

#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_objectstring.h"
#include "../../include/pdf/che_pdf_contentstring.h"
#include "../../include/pdf/che_pdf_contentbuilder.h"

bool CHE_PDF_ContentBuilder::ContentToBuf( CHE_PDF_ContentObjectList * pList, CHE_DynBuffer & buf )
{
    if ( pList == NULL )
    {
        return FALSE;
    }
    
    bool bTextBlock = FALSE;
    PDF_CONTENTOBJ_TYPE itemType;
    ContentObjectList::iterator it;
    CHE_PDF_GState * pCurGState = NULL;
    CHE_PDF_GState * pItemGState = NULL;
    stack<CHE_PDF_GState*> gstateStack;
    
    for ( it = pList->Begin(); it != pList->End(); ++it )
    {
        itemType = (*it)->GetType();
        
        if ( itemType == ContentType_Mark )
        {
            continue;
        }
        
        if ( itemType != ContentType_Text && bTextBlock )
        {
            //当发现输出的不是文本的时候，要先终止文本块
            bTextBlock = FALSE;
            CHE_PDF_ContentString::TextBlockEndToBuf( buf );
        }
        
        if ( itemType != ContentType_Mark )
        {
            //当输出的不是Mark的时候才需要输出图形状态
            pItemGState = (*it)->GetGState();
            CHE_PDF_ContentString::GStateToBuf( pCurGState, pItemGState, buf, gstateStack, bTextBlock );
        }
        
        if ( itemType == ContentType_Text && ! bTextBlock )
        {
            bTextBlock = TRUE;
            CHE_PDF_ContentString::TextBlockBeginToBuf( buf );
            CHE_PDF_ContentString::TextStateToBuf( pCurGState, pItemGState, buf, TRUE );
        }
        
        switch ( itemType )
        {
            case ContentType_Text:
            {
                CHE_PDF_ContentString::TextToBuf( (CHE_PDF_Text*)( *it ), buf );
                
                //这里需要将文本的图形状态合并到当前图形状态的数据中来
                if ( pCurGState == NULL )
                {
                    pCurGState = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
                }
                pCurGState->CopyTextState( pItemGState );
                break;
            }
            case ContentType_Path:
                CHE_PDF_ContentString::PathToBuf( (CHE_PDF_Path*)( *it ), buf );
                break;
            case ContentType_Mark:
                CHE_PDF_ContentString::MarkToBuf( (CHE_PDF_Mark *)( *it ), buf );
                break;
                /*case ContentType_RefImage:
                 CHE_PDF_ContentString::RefImageToBuf( (CHE_PDF_RefImage*)( *it ), buf );
                 break;
                 case ContentType_InlineImage:
                 CHE_PDF_ContentString::InlineImageToBuf( (CHE_PDF_InlineImage*)( *it ), buf );
                 break;
                 case ContentType_Form:
                 CHE_PDF_ContentString::FormToBuf( (CHE_PDF_Form*)( *it ), buf );
                 break;
                 case ContentType_Shading:
                 CHE_PDF_ContentString::ShadingToBuf( (CHE_PDF_Shading*)( *it ), buf );*/
            default:
                break;
        }
        
        //更新当前图形状态数据（需要合并文本图形状态数据）
        CHE_PDF_GState * pTmpGState = NULL;
        if ( pItemGState )
        {
            pTmpGState = pItemGState->Clone();
            pTmpGState->CopyTextState( pCurGState );
        }
        
        if ( pCurGState != NULL )
        {
            pCurGState->GetAllocator()->Delete( pCurGState );
            pCurGState = NULL;
        }
        pCurGState = pTmpGState;
        
    }
    
    //善后处理，结束未结束的文本块，补上恰当的Q指令
    if ( bTextBlock )
    {
        //bTextBlock = FALSE;
        CHE_PDF_ContentString::TextBlockEndToBuf( buf );
    }
    if ( pCurGState )
    {
        pCurGState->GetAllocator()->Delete( pCurGState );
        pCurGState = NULL;
    }
    /*while ( gstateStack.pop( pCurGState ) )
     {
     if ( pCurGState )
     {
     pCurGState->GetAllocator()->Delete( pCurGState );
     pCurGState = NULL;
     }
     CHE_PDF_ObjectString::StringToBuf( "Q\n", buf );
     }*/
    
    return TRUE;
}