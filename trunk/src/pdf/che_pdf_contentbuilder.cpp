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
            //����������Ĳ����ı���ʱ��Ҫ����ֹ�ı���
            bTextBlock = FALSE;
            CHE_PDF_ContentString::TextBlockEndToBuf( buf );
        }
        
        if ( itemType != ContentType_Mark )
        {
            //������Ĳ���Mark��ʱ�����Ҫ���ͼ��״̬
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
                
                //������Ҫ���ı���ͼ��״̬�ϲ�����ǰͼ��״̬����������
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
        
        //���µ�ǰͼ��״̬���ݣ���Ҫ�ϲ��ı�ͼ��״̬���ݣ�
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
    
    //�ƺ�������δ�������ı��飬����ǡ����Qָ��
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