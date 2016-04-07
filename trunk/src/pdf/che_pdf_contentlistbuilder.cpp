#include "../../include/pdf/che_pdf_contentlistbuilder.h"
#include "../../include/pdf/che_pdf_contentobjs.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_extgstate.h"
#include "../../include/pdf/che_pdf_gstate.h"
#include "../../include/pdf/che_pdf_xobject.h"
#include "../../include/pdf/che_pdf_pattern.h"
#include "../../include/pdf/che_pdf_componentmgr.h"



class CHE_PDF_ContentListConstructor : public CHE_Object
{
public:
    CHE_PDF_ContentListConstructor(	CHE_PDF_ContentObjectList * pList,
                                   const CHE_Matrix & matrix,
                                   CHE_Allocator * pAllocator = NULL )
    : mpList(pList), mpGState(NULL), mExtMatrix(matrix),
    mTextLeading(0), mTextXOffset(0), mTextYOffset(0), CHE_Object( pAllocator )
    {
        mpGState = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
    }
    
    ~CHE_PDF_ContentListConstructor()
    {
        if ( mpGState )
        {
            mpGState->GetAllocator()->Delete( mpGState );
        }
        while ( mGStateStack.size() > 0 )
        {
            mpGState = mGStateStack[mGStateStack.size()-1];
            mGStateStack.pop_back();
            if ( mpGState )
            {
                mpGState->GetAllocator()->Delete( mpGState );
            }
        }
    }
    
    HE_VOID State_Matrix( const CHE_Matrix & matrix )
    {
        GetGState()->SetMatrix( matrix );
    }
    
    HE_VOID State_ConcatMatrix( const CHE_Matrix & matrix )
    {
        CHE_Matrix tmpMatirx = matrix;
        CHE_Matrix curMatrix = GetGState()->GetMatrix();
        tmpMatirx.Concat( curMatrix );
        GetGState()->SetMatrix( tmpMatirx );
    }
    
    HE_VOID State_LineWidth( const HE_FLOAT & lineWidth )
    {
        GetGState()->SetLineWidth( lineWidth );
    }
    
    HE_VOID State_LineCap( const GRAPHICS_STATE_LINECAP & lineCap )
    {
        GetGState()->SetLineCap( lineCap );
    }
    
    HE_VOID State_LineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin )
    {
        GetGState()->SetLineJoin( lineJoin );
    }
    
    HE_VOID State_MiterLimit( const HE_FLOAT & miterLimit )
    {
        GetGState()->SetMiterLimit( miterLimit );
    }
    
    HE_VOID State_LineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
    {
        GetGState()->SetLineDash( dashPattern );
    }
    
    HE_VOID State_RenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
    {
        GetGState()->SetRenderIntents( ri );
    }
    
    HE_VOID State_Flatness( const HE_FLOAT & flatness )
    {
        GetGState()->SetFlatness( flatness );
    }
    
    HE_VOID State_ExtGState( const CHE_ByteString & resName, const CHE_PDF_ExtGStatePtr & extgstate )
    {
        GetGState()->PushExtGState( resName, extgstate );
    }
    
    HE_VOID State_FillColor( CHE_PDF_Color & color )
    {
        GetGState()->SetFillColor( color );
    }
    
    HE_VOID State_StrokeColor( CHE_PDF_Color & color )
    {
        GetGState()->SetStrokeColor( color );
    }
    
    HE_VOID State_FillColorSpace( const CHE_PDF_ColorSpacePtr & colorSpace )
    {
        GetGState()->SetFillColorSpace( colorSpace );
    }
    
    CHE_PDF_ColorSpacePtr State_GetFillColorSpace()
    {
        CHE_PDF_ColorSpacePtr csptr;
        GetGState()->GetFillColorSpace( csptr );
        return csptr;
    }
    
    HE_VOID State_StrokeColorSpace( const CHE_PDF_ColorSpacePtr & colorSpace )
    {
        GetGState()->SetStrokeColorSpace( colorSpace );
    }
    
    CHE_PDF_ColorSpacePtr State_GetStrokeColorSpace()
    {
        CHE_PDF_ColorSpacePtr csptr;
        GetGState()->GetStrokeColorSpace( csptr );
        return csptr;
    }
    
    HE_VOID State_TextMatirx( const CHE_Matrix & matrix )
    {
        GetGState()->SetTextMatrix( matrix );
    }
    
    HE_VOID State_TextFont( const CHE_ByteString & resName, CHE_PDF_Font * pFont )
    {
        GetGState()->SetTextFontResName( resName );
        if ( pFont )
        {
            GetGState()->SetTextFont( pFont );
        }
    }
    
    HE_VOID State_TextFontSize( const HE_FLOAT & size )
    {
        GetGState()->SetTextFontSize( size );
    }
    
    HE_VOID State_TextCharSpace( const HE_FLOAT & charSpace )
    {
        GetGState()->SetTextCharSpace( charSpace );
    }
    
    HE_VOID State_TextWordSpace( const HE_FLOAT & wordSpace )
    {
        GetGState()->SetTextWordSpace( wordSpace );
    }
    
    HE_VOID State_TextScaling( const HE_FLOAT & scaling )
    {
        GetGState()->SetTextScaling( scaling );
    }
    
    HE_VOID State_TextLeading( const HE_FLOAT & leading )
    {
        mTextLeading = leading;
    }
    
    HE_VOID State_TextRise( const HE_FLOAT & rise )
    {
        GetGState()->SetTextRise( rise );
    }
    
    HE_VOID State_TextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE rm )
    {
        GetGState()->SetTextRenderMode( rm );
    }
    
    HE_VOID State_ResetTextOffset()
    {
        mTextXOffset = 0;
        mTextYOffset = 0;
    }
    
    HE_VOID State_TextOffset( const HE_FLOAT & xOffset, const HE_FLOAT & yOffset )
    {
        mTextXOffset += xOffset;
        mTextYOffset += yOffset;
    }
    
    HE_VOID State_TextObject( CHE_PDF_ObjectPtr objPtr )
    {
        mTextObj = objPtr;
    }
    
    HE_VOID Operator_Td( const HE_FLOAT & tx, const HE_FLOAT & ty )
    {
        CHE_Matrix matrix, tmpMatrix;
        GetGState()->GetTextMatrix( matrix );
        tmpMatrix.e = tx;
        tmpMatrix.f = ty;
        tmpMatrix.Concat( matrix );
        GetGState()->SetTextMatrix( tmpMatrix );
    }
    
    HE_VOID Operator_TD( const HE_FLOAT & tx, const HE_FLOAT & ty )
    {
        mTextLeading = -ty;
        Operator_Td( tx, ty );
    }
    
    HE_VOID Operator_Tstar()
    {
        Operator_Td( 0, -mTextLeading );
    }
    
    HE_VOID Operator_PushGState()
    {
        if ( mpGState )
        {
            mGStateStack.push_back( mpGState->Clone() );
        }else
        {
            mGStateStack.push_back( NULL );
        }
    }
    
    HE_VOID Operator_PopGState()
    {
        if ( mpGState )
        {
            mpGState->GetAllocator()->Delete( mpGState );
            mpGState = NULL;
        }
        if ( mGStateStack.size() > 0 )
        {
            mpGState = mGStateStack[mGStateStack.size()-1];
            mGStateStack.pop_back();
        }
    }
    
    HE_VOID Operator_Clip( CHE_PDF_ContentObject * pObject )
    {
        if ( pObject && ( pObject->GetType() == ContentType_Path || pObject->GetType() == ContentType_Text ) )
        {
            CHE_PDF_GState * pGState = GetGState()->Clone();
            pObject->SetGState( pGState );
            GetGState()->PushClipElement( pObject );
        }
    }
    
    HE_VOID Operator_Append( CHE_PDF_ContentObject * pObject )
    {
        if ( pObject )
        {
            switch( pObject->GetType() )
            {
                case ContentType_Text:
                {
                    CHE_PDF_GState * pGState = mpGState->Clone();
                    CHE_Matrix textMatrix;
                    pGState->GetTextMatrix( textMatrix );
                    CHE_Matrix tmpMatrix;
                    tmpMatrix.e = mTextXOffset;
                    tmpMatrix.f = mTextYOffset;
                    tmpMatrix.Concat( textMatrix );
                    pGState->SetTextMatrix( tmpMatrix );
                    pObject->SetGState( pGState );
                    
                    CHE_PDF_Text * pText = (CHE_PDF_Text*)( pObject );
                    pText->SetTextObject( mTextObj );
                    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
                    mpGState->GetTextRenderMode( rm );
                    switch ( rm )
                    {
                        case TextRenderMode_FillClip:
                        case TextRenderMode_StrokeClip:
                        case TextRenderMode_FillStrokeClip:
                        case TextRenderMode_Clip:
                        {
                            Operator_Clip( pObject->Clone() );
                            break;
                        }
                        case TextRenderMode_Invisible:
                        case TextRenderMode_Fill:
                        case TextRenderMode_Stroke:
                        case TextRenderMode_FillStroke:
                            break;
                        default: return;
                    }
                    break;
                }
                case ContentType_Path:
                case ContentType_InlineImage:
                case ContentType_Component:
                {
                    pObject->SetGState( mpGState->Clone() );
                    break;
                }
                case ContentType_Mark:
                {
                    break;
                }
                default:
                    return;
            }
            mpList->Append( pObject );
            //CHE_Matrix tmpMatrix = pObject->GetExtMatrix();
            //tmpMatrix.Concat( mExtMatrix );
            //pObject->SetExtMatrix( tmpMatrix );
        }
    }
    
    CHE_Matrix GetExtMatrix()
    {
        return mExtMatrix;
    }
    
    CHE_Matrix GetCurMatrix()
    {
        if ( mpGState )
        {
            return mpGState->GetMatrix();
        }
        return CHE_Matrix();
    }
    
    HE_VOID Operator_D0( const HE_FLOAT wx, const HE_FLOAT wy )
    {
        std::vector<HE_FLOAT> param;
        param.push_back( wx );
        param.push_back( wy );
        mpList->SetType3BBox( 0, param );
    }
    
    HE_VOID Operator_D1(	const HE_FLOAT wx, const HE_FLOAT wy,
                        const HE_FLOAT llx, const HE_FLOAT lly,
                        const HE_FLOAT urx, const HE_FLOAT ury )
    {
        std::vector<HE_FLOAT> param;
        param.push_back( wx );
        param.push_back( wy );
        param.push_back( llx );
        param.push_back( lly );
        param.push_back( urx );
        param.push_back( ury );
        mpList->SetType3BBox( 1, param );
    }
    
private:
    CHE_PDF_GState * GetGState()
    {
        if ( !mpGState )
        {
            mpGState = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
        }
        return mpGState;
    }
    
    CHE_PDF_ContentObjectList * mpList;
    std::vector<CHE_PDF_GState*> mGStateStack;
    CHE_PDF_GState * mpGState;
    CHE_Matrix mExtMatrix;
    HE_FLOAT mTextLeading;
    HE_FLOAT mTextXOffset;
    HE_FLOAT mTextYOffset;
    CHE_PDF_ObjectPtr mTextObj;
};







class CHE_PDF_ContentsParser : public CHE_Object
{
public:
    CHE_PDF_ContentsParser( CHE_PDF_ContentResMgr * pResMgr, CHE_PDF_ComponentMgr * pCmptMgr,
                           CHE_PDF_ContentListConstructor * pConstructor, CHE_Allocator * pAllocator = NULL )
    :	CHE_Object( pAllocator ), mpContentResMgr(pResMgr), mpCmptMgr(pCmptMgr), mpConstructor(pConstructor),
    mpPath(NULL), mpClipPath(NULL), mCurX(0), mCurY(0), mString(pAllocator), mName(pAllocator), mParamFalg(0),
    mbInlineImage(FALSE), mbInterpolate(FALSE), mbMask(FALSE), mWidth(0), mHeight(0), mBpc(0) {}
    
    ~CHE_PDF_ContentsParser()
    {
        mOpdFloatStack.clear();
        if ( mpPath )
        {
            mpPath->GetAllocator()->Delete( mpPath );
            mpPath = NULL;
        }
        if ( mpClipPath )
        {
            mpClipPath->GetAllocator()->Delete( mpClipPath );
            mpClipPath = NULL;
        }
    }
    
    HE_BOOL Parse( const CHE_PDF_StreamPtr & pContentStream );
    
    HE_BOOL Parse( const CHE_PDF_ArrayPtr & pContentArray );
    
private:
    HE_VOID ParseImp( CHE_DynBuffer * pStream );
    
    inline HE_VOID Handle_dquote();
    inline HE_VOID Handle_squote();
    inline HE_VOID Handle_B();
    inline HE_VOID Handle_Bstar();
    inline HE_VOID Handle_BDC();
    inline HE_VOID Handle_BI();
    inline HE_VOID Handle_BMC();
    inline HE_VOID Handle_BT();
    inline HE_VOID Handle_BX();
    inline HE_VOID Handle_CS();
    inline HE_VOID Handle_DP();
    inline HE_VOID Handle_Do();
    inline HE_VOID Handle_EI();
    inline HE_VOID Handle_EMC();
    inline HE_VOID Handle_ET();
    inline HE_VOID Handle_EX();
    inline HE_VOID Handle_F();
    inline HE_VOID Handle_G();
    inline HE_VOID Handle_ID( CHE_PDF_SyntaxParser * pParser );
    inline HE_VOID Handle_J();
    inline HE_VOID Handle_K();
    inline HE_VOID Handle_M();
    inline HE_VOID Handle_MP();
    inline HE_VOID Handle_Q();
    inline HE_VOID Handle_RG();
    inline HE_VOID Handle_S();
    inline HE_VOID Handle_SC();
    inline HE_VOID Handle_SCN();
    inline HE_VOID Handle_Tstar();
    inline HE_VOID Handle_TD();
    inline HE_VOID Handle_TJ();
    inline HE_VOID Handle_TL();
    inline HE_VOID Handle_Tc();
    inline HE_VOID Handle_Td();
    inline HE_VOID Handle_Tf();
    inline HE_VOID Handle_Tj();
    inline HE_VOID Handle_Tm();
    inline HE_VOID Handle_Tr();
    inline HE_VOID Handle_Ts();
    inline HE_VOID Handle_Tw();
    inline HE_VOID Handle_Tz();
    inline HE_VOID Handle_W();
    inline HE_VOID Handle_Wstar();
    inline HE_VOID Handle_b();
    inline HE_VOID Handle_bstar();
    inline HE_VOID Handle_c();
    inline HE_VOID Handle_cm();
    inline HE_VOID Handle_cs();
    inline HE_VOID Handle_d();
    inline HE_VOID Handle_d0();
    inline HE_VOID Handle_d1();
    inline HE_VOID Handle_f();
    inline HE_VOID Handle_fstar();
    inline HE_VOID Handle_g();
    inline HE_VOID Handle_gs();
    inline HE_VOID Handle_h();
    inline HE_VOID Handle_i();
    inline HE_VOID Handle_j();
    inline HE_VOID Handle_k();
    inline HE_VOID Handle_l();
    inline HE_VOID Handle_m();
    inline HE_VOID Handle_n();
    inline HE_VOID Handle_q();
    inline HE_VOID Handle_re();
    inline HE_VOID Handle_rg();
    inline HE_VOID Handle_ri();
    inline HE_VOID Handle_s();
    inline HE_VOID Handle_sc();
    inline HE_VOID Handle_scn();
    inline HE_VOID Handle_sh();
    inline HE_VOID Handle_v();
    inline HE_VOID Handle_w();
    inline HE_VOID Handle_y();
    
    inline HE_BOOL CheckOpdCount( size_t count );
    
    std::vector<HE_FLOAT>	mOpdFloatStack;
    CHE_ByteString			mName;
    CHE_ByteString			mString;
    CHE_PDF_ObjectPtr		mpObj;
    
    CHE_PDF_Path *			mpPath;
    CHE_PDF_Path *			mpClipPath;
    HE_FLOAT				mBeginX;
    HE_FLOAT				mBeginY;
    HE_FLOAT				mCurX;
    HE_FLOAT				mCurY;
    
    //inline image
    HE_BYTE					mParamFalg;
    HE_BOOL					mbInlineImage;
    HE_BOOL					mbInterpolate;
    HE_BOOL					mbMask;
    HE_ULONG				mWidth;
    HE_ULONG				mHeight;
    HE_ULONG				mBpc;
    CHE_PDF_ObjectPtr		mpColorSpace;
    CHE_PDF_ObjectPtr		mpFilter;
    CHE_PDF_ObjectPtr		mpDecode;
    CHE_PDF_ObjectPtr		mpDecodeParam;
    
    CHE_PDF_ComponentMgr *	mpCmptMgr;
    CHE_PDF_ContentResMgr * mpContentResMgr;
    CHE_PDF_ContentListConstructor * mpConstructor;
};






HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_StreamPtr & pContents )
{
    if ( ! pContents || mpConstructor == NULL )
    {
        return FALSE;
    }
    
    CHE_DynBuffer buf( pContents->GetRawSize() * 2, pContents->GetRawSize(), GetAllocator() );
    CHE_PDF_StreamAcc streamAcc( GetAllocator() );
    streamAcc.Attach( pContents );
    buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
    streamAcc.Detach();
    
    ParseImp( &buf );
    
    return TRUE;
}

HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_ArrayPtr & pContentArray )
{
    if ( ! pContentArray || mpConstructor == NULL )
    {
        return FALSE;
    }
    
    CHE_DynBuffer buf( 1024 * 4, 1024 * 8, GetAllocator() );
    
    for ( HE_ULONG i = 0; i < pContentArray->GetCount(); ++i )
    {
        CHE_PDF_StreamPtr pStream = pContentArray->GetElement( i, OBJ_TYPE_STREAM )->GetStreamPtr();
        if ( pStream )
        {
            CHE_PDF_StreamAcc streamAcc( GetAllocator() );
            streamAcc.Attach( pStream );
            buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
            streamAcc.Detach();
        }
    }
    if ( buf.GetSize() > 0 )
    {
        ParseImp( &buf );
    }
    return TRUE;
}

HE_VOID CHE_PDF_ContentsParser::ParseImp( CHE_DynBuffer * pStream )
{
    if ( !pStream )
    {
        return;
    }
    
    IHE_Read * pRead = HE_CreateMemBufRead( pStream->GetData(), pStream->GetSize(), GetAllocator() );
    if ( pRead == NULL )
    {
        return;
    }
    
    CHE_PDF_SyntaxParser sParser( NULL, GetAllocator() );
    if ( sParser.InitParser( pRead ) == FALSE )
    {
        pRead->Release();
        HE_DestoryIHERead( pRead );
        return;
    }
    
    CHE_PDF_ParseWordDes wordDes;
    CHE_PDF_ObjectPtr pTmpNode;
    HE_FLOAT tmpValue = 0;
    HE_BOOL	bOpd = TRUE;
    while( sParser.GetWord( wordDes ) == TRUE )
    {
        bOpd = TRUE;
        switch ( wordDes.type )
        {
            case PARSE_WORD_INTEGER:
            {
                tmpValue = (HE_FLOAT)( sParser.GetInteger()/*wordDes.str.GetInteger()*/ );
                mOpdFloatStack.push_back( tmpValue );
                
                if ( mbInlineImage && mParamFalg > 0 )
                {
                    switch ( mParamFalg )
                    {
                        case 1:
                            mBpc = sParser.GetInteger();//wordDes.str.GetInteger();
                            break;
                        case 6:
                            mHeight = sParser.GetInteger();//wordDes.str.GetInteger();
                            break;
                        case 9:
                            mWidth = sParser.GetInteger();//wordDes.str.GetInteger();
                            break;
                        default:
                            break;
                    }
                    mParamFalg = 0;
                }
                break;
            }
            case PARSE_WORD_FLOAT:
            {
                tmpValue = sParser.GetFloat();//wordDes.str.GetFloat();
                mOpdFloatStack.push_back( tmpValue );
                break;
            }
            case PARSE_WORD_NAME:
            {
                if ( mName.GetLength() > 0 )
                {
                    mString = sParser.GetString()/*wordDes.str*/;
                }
                else
                {
                    mName = sParser.GetString()/*wordDes.str*/;
                }
                if ( mbInlineImage )
                {
                    if ( mParamFalg > 0 )
                    {
                        switch ( mParamFalg )
                        {
                            case 2:
                                mpColorSpace = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
                                break;
                            case 3:
                                mpDecode = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
                                break;
                            case 4:
                                mpDecodeParam = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
                                break;
                            case 5:
                                mpFilter = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
                                break;
                            default:
                                break;
                        }
                        mParamFalg = 0;
                    }else
                    {
                        switch ( StringToDWORD( sParser.GetString()/*wordDes.str*/ ) )
                        {
                            case C('B','P','C'):	mParamFalg = 1; break;
                            case B('C','S'):		mParamFalg = 2; break;
                            case A('D'):			mParamFalg = 3; break;
                            case B('D','P'):		mParamFalg = 4; break;
                            case A('F'):			mParamFalg = 5; break;
                            case A('H'):			mParamFalg = 6; break;
                            case B('I','M'):		mParamFalg = 7; break;
                            case A('I'):			mParamFalg = 8; break;
                            case A('W'):			mParamFalg = 9; break;
                            default:				mParamFalg = 0; break;
                        }
                    }
                }
                break;
            }
            case PARSE_WORD_STRING:
            {
                mString = sParser.GetString();
                mpObj = CHE_PDF_String::Create( mString, GetAllocator() );
                break;
            }
            case PARSE_WORD_ARRAY_B:
            {
                sParser.SetPos( wordDes.offset );
                pTmpNode = sParser.GetArrayPtr();
                mpObj = pTmpNode;
                if ( mbInlineImage )
                {
                    if ( mParamFalg > 0 )
                    {
                        switch ( mParamFalg )
                        {
                            case 3:
                                mpDecode = mpObj;
                                break;
                            case 4:
                                mpDecodeParam = mpObj;
                                break;
                            case 5:
                                mpFilter = mpObj;
                                break;
                            default:
                                break;
                        }
                        mParamFalg = 0;
                    }
                }
                break;
            }
            case PARSE_WORD_DICT_B:
            {
                sParser.SetPos( wordDes.offset );
                pTmpNode = sParser.GetDictionaryPtr();
                mpObj = pTmpNode;
                break;
            }
            default:
            {
                if ( mbInlineImage && mParamFalg == 7 )
                {
                    if ( sParser.IsWord( "true" )/*wordDes.str == "true"*/ )
                    {
                        mbMask = true;
                    }
                    else if ( sParser.IsWord( "false" )/*wordDes.str == "false"*/ )
                    {
                        mbMask = false;
                    }
                    mParamFalg = 0;
                    break;
                }
                bOpd = FALSE;
                break;
            }
        }
        if ( bOpd == TRUE )
        {
            continue;
        }
        
        switch ( StringToDWORD( sParser.GetString()/*wordDes.str*/ ) )
        {
            case A('"'):			Handle_dquote();	break;
            case A('\''):			Handle_squote();	break;
            case A('B'):			Handle_B();			break;
            case B('B','*'):		Handle_Bstar();		break;
            case C('B','D','C'):	Handle_BDC();		break;
            case B('B','I'):		Handle_BI();		break;
            case C('B','M','C'):	Handle_BMC();		break;
            case B('B','T'):		Handle_BT();		break;
            case B('B','X'):		Handle_BX();		break;
            case B('C','S'):		Handle_CS();		break;
            case B('D','P'):		Handle_DP();		break;
            case B('D','o'):		Handle_Do();		break;
            case B('E','I'):		Handle_EI();		break;
            case C('E','M','C'):	Handle_EMC();		break;
            case B('E','T'):		Handle_ET();		break;
            case B('E','X'):		Handle_EX();		break;
            case A('F'):			Handle_F();			break;
            case A('G'):			Handle_G();			break;
            case B('I','D'):		Handle_ID(&sParser);break;
            case A('J'):			Handle_J();			break;
            case A('K'):			Handle_K();			break;
            case A('M'):			Handle_M();			break;
            case B('M','P'):		Handle_MP();		break;
            case A('Q'):			Handle_Q();			break;
            case B('R','G'):		Handle_RG();		break;
            case A('S'):			Handle_S();			break;
            case B('S','C'):		Handle_SC();		break;
            case C('S','C','N'):	Handle_SCN();		break;
            case B('T','*'):		Handle_Tstar();		break;
            case B('T','D'):		Handle_TD();		break;
            case B('T','J'):		Handle_TJ();		break;
            case B('T','L'):		Handle_TL();		break;
            case B('T','c'):		Handle_Tc();		break;
            case B('T','d'):		Handle_Td();		break;
            case B('T','f'):		Handle_Tf();		break;
            case B('T','j'):		Handle_Tj();		break;
            case B('T','m'):		Handle_Tm();		break;
            case B('T','r'):		Handle_Tr();		break;
            case B('T','s'):		Handle_Ts();		break;
            case B('T','w'):		Handle_Tw();		break;
            case B('T','z'):		Handle_Tz();		break;
            case A('W'):			Handle_W();			break;
            case B('W','*'):		Handle_Wstar();		break;
            case A('b'):			Handle_b();			break;
            case B('b','*'):		Handle_bstar();		break;
            case A('c'):			Handle_c();			break;
            case B('c','m'):		Handle_cm();		break;
            case B('c','s'):		Handle_cs();		break;
            case A('d'):			Handle_d();			break;
            case B('d','0'):		Handle_d0();		break;
            case B('d','1'):		Handle_d1();		break;
            case A('f'):			Handle_f();			break;
            case B('f','*'):		Handle_fstar();		break;
            case A('g'):			Handle_g();			break;
            case B('g','s'):		Handle_gs();		break;
            case A('h'):			Handle_h();			break;
            case A('i'):			Handle_i();			break;
            case A('j'):			Handle_j();			break;
            case A('k'):			Handle_k();			break;
            case A('l'):			Handle_l();			break;
            case A('m'):			Handle_m();			break;
            case A('n'):			Handle_n();			break;
            case A('q'):			Handle_q();			break;
            case B('r','e'):		Handle_re();		break;
            case B('r','g'):		Handle_rg();		break;
            case B('r','i'):		Handle_ri();		break;
            case A('s'):			Handle_s();			break;
            case B('s','c'):		Handle_sc();		break;
            case C('s','c','n'):	Handle_scn();		break;
            case B('s','h'):		Handle_sh();		break;
            case A('v'):			Handle_v();			break;
            case A('w'):			Handle_w();			break;
            case A('y'):			Handle_y();			break;
            default:break;
        }
        
        //清除无用的操作数
        mOpdFloatStack.clear();
        mpObj.Reset();
        mName.Clear();
        mString.Clear();
    }
    
    //清除无用的操作数
    mOpdFloatStack.clear();
    mpObj.Reset();
    mName.Clear();
    mString.Clear();
    
    pRead->Release();
    HE_DestoryIHERead( pRead );
}

HE_BOOL CHE_PDF_ContentsParser::CheckOpdCount( size_t count )
{
    if ( mOpdFloatStack.size() >= count )
    {
        return TRUE;
    }
    return FALSE;
}

HE_VOID CHE_PDF_ContentsParser::Handle_dquote()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->State_TextWordSpace( mOpdFloatStack[0] );
        mpConstructor->State_TextCharSpace( mOpdFloatStack[1] );
    }
    mpConstructor->Operator_Tstar();
    mpConstructor->State_ResetTextOffset();
    if ( mpObj )
    {
        CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_squote()
{
    mpConstructor->Operator_Tstar();
    mpConstructor->State_ResetTextOffset();
    if ( mpObj )
    {
        CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_B()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Bstar()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_BDC()
{
    if ( mName.GetLength() > 0 &&  IsPdfDictPtr( mpObj ) )
    {
        CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_BDC );
        pMark->SetTag( mName );
        if ( mpObj )
        {
            pMark->SetProperty( mpObj->GetDictPtr() );
        }
        else if ( mString.GetLength() > 0 )
        {
            CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
            if ( IsPdfRefPtr( objPtr ) )
            {
                objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
            }
            if ( IsPdfDictPtr( objPtr ) )
            {
                pMark->SetProperty( objPtr->GetDictPtr() );
            }
        }
        mpConstructor->Operator_Append( pMark );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_BI()
{
    mbInlineImage = TRUE;
}

HE_VOID CHE_PDF_ContentsParser::Handle_BMC()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_BMC );
        pMark->SetTag( mName );
        mpConstructor->Operator_Append( pMark );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_BT()
{
    mpConstructor->State_TextMatirx( CHE_Matrix() );
}

HE_VOID CHE_PDF_ContentsParser::Handle_BX()
{
    
}

HE_VOID CHE_PDF_ContentsParser::Handle_CS()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ColorSpacePtr colorspacePtr = CHE_PDF_ColorSpace::Create( mName, GetAllocator() );
        if ( ! colorspacePtr )
        {
            CHE_PDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
            if ( cmptPtr )
            {
                mpConstructor->State_StrokeColorSpace( CHE_PDF_ColorSpace::Convert( cmptPtr ) );
            }else{
                CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
                if ( ! objPtr )
                {
                    return;
                }
                if ( IsPdfRefPtr( objPtr ) )
                {
                    CHE_PDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ColorSpace );
                    if ( cmptPtr )
                    {
                        mpConstructor->State_StrokeColorSpace( CHE_PDF_ColorSpace::Convert( cmptPtr ) );
                    }else{
                        colorspacePtr = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
                        mpCmptMgr->PushComponent( objPtr->GetRefPtr(), colorspacePtr );
                        mpConstructor->State_StrokeColorSpace( colorspacePtr );
                    }
                }else{
                    colorspacePtr = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
                    if ( colorspacePtr )
                    {
                        mpContentResMgr->PushComponent( mName, colorspacePtr );
                        mpConstructor->State_StrokeColorSpace( colorspacePtr );
                    }
                }
            }
        }else{
            mpConstructor->State_StrokeColorSpace( colorspacePtr );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_DP()
{
    if ( mName.GetLength() > 0 && IsPdfDictPtr( mpObj ) )
    {
        CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_DP );
        pMark->SetTag( mName );
        if ( mpObj )
        {
            pMark->SetProperty( mpObj->GetDictPtr() );
        }
        else if ( mString.GetLength() > 0 )
        {
            CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
            if ( IsPdfRefPtr( objPtr ) )
            {
                objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
            }
            if ( IsPdfDictPtr( objPtr ) )
            {
                pMark->SetProperty( objPtr->GetDictPtr() );
            }
        }
        mpConstructor->Operator_Append( pMark );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Do()
{
    if ( mName.GetLength() > 0 && mpContentResMgr )
    {
        CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
        if ( !pTmpObj )
        {
            return;
        }
        CHE_PDF_ReferencePtr pRef = pTmpObj->GetRefPtr();
        if ( !pRef )
        {
            return;
        }
        
        CHE_PDF_ComponentPtr ptr = mpCmptMgr->GetComponent( pRef );
        if ( ptr )
        {
            if ( ptr->GetType() == COMPONENT_TYPE_ImageXObject || ptr->GetType() == COMPONENT_TYPE_FormXObject )
            {
                CHE_PDF_ComponentRef * cmtRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, ptr, GetAllocator() );
                mpConstructor->Operator_Append( cmtRef );
                return;
            }
        }
        
        pTmpObj = pRef->GetRefObj( OBJ_TYPE_STREAM );
        if ( !pTmpObj )
        {
            return;
        }
        CHE_PDF_StreamPtr pStm = pTmpObj->GetStreamPtr();
        CHE_PDF_DictionaryPtr pStmDict = pStm->GetDictPtr();
        if ( !pStmDict )
        {
            return;
        }
        pTmpObj = pStmDict->GetElement( "Subtype", OBJ_TYPE_NAME );
        if ( !pTmpObj )
        {
            return;
        }
        CHE_PDF_NamePtr pSubtypeName = pTmpObj->GetNamePtr();
        if ( !pSubtypeName )
        {
            return;
        }
        if ( pSubtypeName->GetString() == "Image" )
        {
            CHE_PDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
            if ( pObj && IsPdfRefPtr( pObj ) )
            {
                CHE_PDF_ImageXObjectPtr refImgPtr = CHE_PDF_ImageXObject::Create( pRef, GetAllocator() );
                CHE_PDF_ComponentRef * cmtRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, refImgPtr, GetAllocator() );
                mpConstructor->Operator_Append( cmtRef );
                
                mpCmptMgr->PushComponent( pRef, refImgPtr );
            }
        }else if ( pSubtypeName->GetString() == "Form" )
        {
            CHE_PDF_FormXObjectPtr formPtr = CHE_PDF_FormXObject::Create( pRef, mpCmptMgr, GetAllocator() );
            
            // 			//CHE_PDF_FormXObject * pForm = GetAllocator()->New<CHE_PDF_FormXObject>( mName, GetAllocator() );
            // 			CHE_PDF_ObjectPtr pTmpObj;
            // 			CHE_PDF_DictionaryPtr pResDict;
            // 			pTmpObj = pStmDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
            // 			if ( pTmpObj )
            // 			{
            // 				pResDict = pTmpObj->GetDictPtr();
            // 			}
            // 			//todo : get the matrix from form dict
            CHE_Matrix extMatrix = mpConstructor->GetExtMatrix();
            CHE_Matrix curMatrix = mpConstructor->GetCurMatrix();
            curMatrix.Concat( extMatrix );
            // 			CHE_PDF_ContentListConstructor * pConstructor = GetAllocator()->New<CHE_PDF_ContentListConstructor>( & formPtr->GetList(), curMatrix, GetAllocator() );
            // 			CHE_PDF_ContentResMgr * pContentResMgr = &( formPtr->GetList().GetResMgr() );
            // 			pContentResMgr->SetDict( pResDict );
            // 			CHE_PDF_ContentsParser contentsParser( pContentResMgr, mpFontMgr, pConstructor );
            // 			contentsParser.Parse( pStm );
            
            CHE_PDF_ComponentRef * cptRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, formPtr, GetAllocator() );
            
            mpConstructor->Operator_Append( cptRef );
            //			pConstructor->GetAllocator()->Delete( pConstructor );
            
            mpCmptMgr->PushComponent( pRef, formPtr );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_EI()
{
    mbInlineImage = FALSE;
    mbInterpolate = FALSE;
    mbMask = FALSE;
    mWidth = 0;
    mHeight = 0;
    mBpc = 0;
    
    mpColorSpace.Reset();
    mpFilter.Reset();
    mpDecode.Reset();
    mpDecodeParam.Reset();
}

HE_VOID CHE_PDF_ContentsParser::Handle_EMC()
{
    CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
    pMark->SetMarkType( Mark_EMC );
    mpConstructor->Operator_Append( pMark );
}

HE_VOID CHE_PDF_ContentsParser::Handle_ET()
{
}

HE_VOID CHE_PDF_ContentsParser::Handle_EX()
{
    
}

HE_VOID CHE_PDF_ContentsParser::Handle_F()
{
    Handle_f();
}

HE_VOID CHE_PDF_ContentsParser::Handle_G()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceGray();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        mpConstructor->State_StrokeColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_ID( CHE_PDF_SyntaxParser * pParser )
{
    if ( pParser == NULL || mbInlineImage == FALSE )
    {
        return;
    }
    HE_BOOL bOver = FALSE;
    HE_ULONG dwRet = 1;
    HE_BYTE byteRet[4];
    std::vector<HE_BYTE> buffer;
    while( !bOver )
    {
        dwRet = pParser->ReadBytes( byteRet, 2 );
        if ( byteRet[0] == '\r' && byteRet[1] == '\n' )
        {
            pParser->Seek( 2 );
        }if ( byteRet[0] == '\n' || byteRet[0] == '\t' ||
             byteRet[0] == '\f' || byteRet[0] == '\0' || byteRet[0] == ' ' )
        {
            pParser->Seek( 1 );
        }
        break;
    }
    while( dwRet )
    {
        dwRet = pParser->ReadBytes( byteRet, 1 );
        if (	byteRet[0] == '\r' || byteRet[0] == '\n' || byteRet[0] == '\t' ||
            byteRet[0] == '\f' || byteRet[0] == '\0' || byteRet[0] == ' ' )
        {
            dwRet = pParser->ReadBytes( byteRet, 4 );
            if ( byteRet[1] == 'E' && byteRet[2] == 'I' )
            {
                pParser->Seek( 1 );
                break;
            }else if ( byteRet[1] == '\n' && byteRet[2] == 'E' && byteRet[3] == 'I' )
            {
                pParser->Seek( 2 );
                break;
            }
            buffer.push_back( byteRet[0] );
            pParser->Seek( 1 );
        }else{
            buffer.push_back( byteRet[0] );
            pParser->Seek( dwRet );
        }
    }
    
    
    CHE_PDF_DictionaryPtr pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
    if ( mpFilter )
    {
        pDict->SetObject( "Filter", mpFilter );
    }
    // 	if ( mpDecode )
    // 	{
    // 		pDict->SetAtObj( "Decode", mpDecode );
    // 	}
    if ( mpDecodeParam )
    {
        pDict->SetObject( "DecodeParam", mpDecodeParam );
    }
    pDict->SetInteger( "Length", (HE_INT32)buffer.size() );
    
    CHE_PDF_ColorSpacePtr colorspace;
    if ( mpColorSpace )
    {
        colorspace = CHE_PDF_ColorSpace::Create( mpColorSpace, GetAllocator() );
        // 		if ( mpColorSpace->GetType() == OBJ_TYPE_NAME )
        // 		{
        // 			pColorspace = CHE_PDF_ColorSpace::Create( mpColorSpace->GetNamePtr(), GetAllocator() );
        // 			if ( pColorspace == NULL )
        // 			{
        // 				CHE_PDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, pObj->GetNamePtr()->GetString() );
        // 				if ( ! pObj )
        // 				{
        // 					/*assert(0);*/
        // 					/*error*/
        // 				}
        // 				if ( pObj->GetType() == OBJ_TYPE_NAME )
        // 				{
        // 					pColorspace = CHE_PDF_ColorSpace::Create( pObj->GetNamePtr()->GetString(), GetAllocator() );
        // 				}else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
        // 				{
        // 					pColorspace = CHE_PDF_ColorSpace::Create( pObj->GetNamePtr()->GetString(), GetAllocator() );
        // 				}else{
        // 					/*assert(0);*/
        // 					/*error*/
        // 				}
        // 			}
        // 		}else if ( mpColorSpace->GetType() == OBJ_TYPE_ARRAY )
        // 		{
        // 			pColorspace = CHE_PDF_ColorSpace::Create( mpColorSpace->GetArrayPtr(), GetAllocator() );
        // 		}
        /*assert( 0 );*/
        /*error*/
    }
    
    CHE_PDF_StreamPtr pStream = CHE_PDF_Stream::Create( &buffer[0], buffer.size(), pDict, 0, 0, NULL, GetAllocator() );
    if ( pStream )
    {
        CHE_PDF_StreamAcc stmAcc( GetAllocator() );
        if ( stmAcc.Attach( pStream ) )
        {
            CHE_PDF_InlineImage * pImage = GetAllocator()->New<CHE_PDF_InlineImage>(	mbMask, mWidth, mHeight, mBpc,
                                                                                    stmAcc.GetData(), stmAcc.GetSize(), mpDecode,
                                                                                    colorspace, RI_AbsoluteColorimetric, GetAllocator() );
            mpConstructor->Operator_Append( pImage );
        }
        stmAcc.Detach();
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_J()
{
    if ( CheckOpdCount( 1 ) )
    {
        GRAPHICS_STATE_LINECAP lineCap = LineCap_Butt;
        unsigned int type = (unsigned int)( mOpdFloatStack[0] );
        switch ( type )
        {
            default:
            case 0:
                lineCap = LineCap_Butt;
                break;
            case 1:
                lineCap = LineCap_Round;
                break;
            case 2:
                lineCap = LineCap_Square;
                break;
        }
        mpConstructor->State_LineCap( lineCap );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_K()
{
    if ( CheckOpdCount( 4 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceCMYK();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        color.Push( mOpdFloatStack[3] );
        mpConstructor->State_StrokeColor( color );
    }
}
HE_VOID CHE_PDF_ContentsParser::Handle_M()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_MiterLimit( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_MP()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_MP );
        pMark->SetTag( mName );
        mpConstructor->Operator_Append( pMark );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Q()
{
    mpConstructor->Operator_PopGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_RG()
{
    if ( CheckOpdCount( 3 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceRGB();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        mpConstructor->State_StrokeColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_S()
{
    if ( mpPath )
    {
        mpPath->SetPaintType( Paint_Stroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}
HE_VOID CHE_PDF_ContentsParser::Handle_SC()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_StrokeColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_SCN()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_StrokeColor( color );
    }
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ObjectPtr objPtr;
        CHE_PDF_ComponentPtr cmptPtr;
        CHE_PDF_ColorSpacePtr csPtr;
        CHE_PDF_TilingPtr tilingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
        csPtr = mpConstructor->State_GetStrokeColorSpace();
        if ( IsPdfRefPtr( objPtr ) && csPtr && csPtr->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
        {
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_Tiling );
            if ( cmptPtr )
            {
                csPtr->GetPatternPtr()->mTiling = CHE_PDF_Tiling::Convert(cmptPtr);
            }else{
                tilingPtr = CHE_PDF_Tiling::Create( objPtr, mpCmptMgr, GetAllocator() );
                mpCmptMgr->PushComponent( objPtr->GetRefPtr(), tilingPtr );
                csPtr->GetPatternPtr()->mTiling = tilingPtr;
            }
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tstar()
{
    mpConstructor->Operator_Tstar();
    mpConstructor->State_ResetTextOffset();
}

HE_VOID CHE_PDF_ContentsParser::Handle_TD()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_TD( mOpdFloatStack[0], mOpdFloatStack[1] );
        mpConstructor->State_ResetTextOffset();
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_TJ()
{
    if ( mpObj )
    {
        CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_TL()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextLeading( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tc()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextCharSpace( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Td()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_Td( mOpdFloatStack[0], mOpdFloatStack[1] );
        mpConstructor->State_ResetTextOffset();
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tf()
{
    if ( CheckOpdCount( 1 ) && mName.GetLength() > 0 )
    {
        //todo load font
        CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_FONT, mName );
        if ( pTmpObj )
        {
            if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
            {
                CHE_PDF_ComponentPtr cmptPtr = mpCmptMgr->LoadFont( pTmpObj->GetRefPtr() );
                if ( cmptPtr->GetType() == COMPONENT_TYPE_Font ) {
                    CHE_PDF_Font * pFont = (CHE_PDF_Font *)cmptPtr.GetPointer();
                    if ( pFont )
                    {
                        mpConstructor->State_TextFont( mName, pFont );
                    }
                }
            }else if ( pTmpObj->GetType() == OBJ_TYPE_DICTIONARY )
            {
                //todo
            }
        }
        mpConstructor->State_TextFontSize( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tj()
{
    if ( mpObj )
    {
        CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tm()
{
    if ( CheckOpdCount( 6 ) )
    {
        CHE_Matrix matrix;
        matrix.a = mOpdFloatStack[0];
        matrix.b = mOpdFloatStack[1];
        matrix.c = mOpdFloatStack[2];
        matrix.d = mOpdFloatStack[3];
        matrix.e = mOpdFloatStack[4];
        matrix.f = mOpdFloatStack[5];
        mpConstructor->State_TextMatirx( matrix );
        mpConstructor->State_ResetTextOffset();
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tr()
{
    if ( CheckOpdCount( 1 ) )
    {
        GRAPHICS_STATE_TEXTRENDERMODE textRm = TextRenderMode_Fill;
        unsigned int type = (unsigned int)( mOpdFloatStack[0] );
        switch ( type )
        {
            default:
            case 0:
                textRm = TextRenderMode_Fill;
                break;
            case 1:
                textRm = TextRenderMode_Stroke;
                break;
            case 2:
                textRm = TextRenderMode_FillStroke;
                break;
            case 3:
                textRm = TextRenderMode_Invisible;
                break;
            case 4:
                textRm = TextRenderMode_FillClip;
                break;
            case 5:
                textRm = TextRenderMode_StrokeClip;
                break;
            case 6:
                textRm = TextRenderMode_FillStrokeClip;
                break;
            case 7:
                textRm = TextRenderMode_Clip;
                break;
        }
        mpConstructor->State_TextRenderMode( textRm );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Ts()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextRise( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tw()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextWordSpace( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tz()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextScaling( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_W()
{
    if ( mpClipPath )
    {
        mpClipPath->GetAllocator()->Delete( mpClipPath );
        mpClipPath = NULL;
    }
    if ( mpPath )
    {
        mpClipPath = (CHE_PDF_Path *)( mpPath->Clone() );
        mpClipPath->SetFillMode( Mode_Nonzero );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_Wstar()
{
    if ( mpClipPath )
    {
        mpClipPath->GetAllocator()->Delete( mpClipPath );
        mpClipPath = NULL;
    }
    if ( mpPath )
    {
        mpClipPath = (CHE_PDF_Path *)( mpPath->Clone() );
        mpClipPath->SetFillMode( Mode_EvenOdd );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_b()
{
    if ( mpPath )
    {
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_bstar()
{
    if ( mpPath )
    {
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_c()
{
    if ( CheckOpdCount( 6 ) )
    {
        if ( mpPath )
        {
            CHE_PDF_PathItem pathItem;
            pathItem.type = PathItem_CurveTo;
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[0];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[1];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[2];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[3];
            mpPath->mItems.push_back( pathItem );
            mCurX = pathItem.value = mOpdFloatStack[4];
            mpPath->mItems.push_back( pathItem );
            mCurY = pathItem.value = mOpdFloatStack[5];
            mpPath->mItems.push_back( pathItem );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_cm()
{
    if ( CheckOpdCount( 6 ) )
    {
        CHE_Matrix matrix;
        matrix.a = mOpdFloatStack[0];
        matrix.b = mOpdFloatStack[1];
        matrix.c = mOpdFloatStack[2];
        matrix.d = mOpdFloatStack[3];
        matrix.e = mOpdFloatStack[4];
        matrix.f = mOpdFloatStack[5];
        mpConstructor->State_ConcatMatrix( matrix );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_cs()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ColorSpacePtr colorspacePtr = CHE_PDF_ColorSpace::Create( mName, GetAllocator() );
        if ( ! colorspacePtr )
        {
            CHE_PDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
            if ( cmptPtr )
            {
                mpConstructor->State_FillColorSpace( CHE_PDF_ColorSpace::Convert( cmptPtr ) );
            }else{
                CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
                if ( ! objPtr )
                {
                    return;
                }
                if ( IsPdfRefPtr( objPtr ) )
                {
                    CHE_PDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ColorSpace );
                    if ( cmptPtr )
                    {
                        mpConstructor->State_FillColorSpace( CHE_PDF_ColorSpace::Convert( cmptPtr ) );
                    }else{
                        colorspacePtr = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
                        mpCmptMgr->PushComponent( objPtr->GetRefPtr(), colorspacePtr );
                        mpConstructor->State_FillColorSpace( colorspacePtr );
                    }
                }else{
                    colorspacePtr = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
                    if ( colorspacePtr )
                    {
                        mpContentResMgr->PushComponent( mName, colorspacePtr );
                        mpConstructor->State_FillColorSpace( colorspacePtr );
                    }
                }
            }
        }else{
            mpConstructor->State_FillColorSpace( colorspacePtr );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_d()
{
    if ( CheckOpdCount( 1 ) )
    {
        GRAPHICS_STATE_DASHPATTERN dashPattern;
        dashPattern.dashPhase = mOpdFloatStack[0];
        
        if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
        {
            CHE_PDF_ObjectPtr objPtr;
            CHE_PDF_NumberPtr numberPtr;
            CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
            for ( HE_ULONG i = 0; i < arrayPtr->GetCount(); ++i )
            {
                objPtr = arrayPtr->GetElement( i );
                if ( objPtr && objPtr->GetType() == OBJ_TYPE_NUMBER )
                {
                    numberPtr = objPtr->GetNumberPtr();
                    dashPattern.dashArray.push_back( numberPtr->GetFloat() );
                }
            }
        }
        mpConstructor->State_LineDash( dashPattern );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_d0()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_D0( mOpdFloatStack[0], mOpdFloatStack[1] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_d1()
{
    if ( CheckOpdCount( 6 ) )
    {
        mpConstructor->Operator_D1( mOpdFloatStack[0], mOpdFloatStack[1],
                                   mOpdFloatStack[2], mOpdFloatStack[3],
                                   mOpdFloatStack[4], mOpdFloatStack[5] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_f()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_Fill );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_fstar()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_Fill );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_g()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceGray();
        mpConstructor->State_FillColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        mpConstructor->State_FillColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_gs()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
        if ( cmptPtr )
        {
            mpConstructor->State_ExtGState( mName, CHE_PDF_ExtGState::Convert( cmptPtr ) );
        }else{
            CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_EXTGSTATE, mName );
            if ( ! objPtr )
            {
                return;
            }
            if ( IsPdfRefPtr( objPtr ) )
            {
                CHE_PDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ExtGState );
                if ( cmptPtr )
                {
                    mpConstructor->State_ExtGState( mName, CHE_PDF_ExtGState::Convert( cmptPtr ) );
                }else{
                    CHE_PDF_ExtGStatePtr extGStatePtr = CHE_PDF_ExtGState::Create( objPtr, GetAllocator() );
                    mpCmptMgr->PushComponent( objPtr->GetRefPtr(), extGStatePtr );
                    mpConstructor->State_ExtGState( mName, extGStatePtr );
                }
            }else{
                CHE_PDF_ExtGStatePtr extGStatePtr = CHE_PDF_ExtGState::Create( objPtr, GetAllocator() );
                if ( extGStatePtr )
                {
                    mpContentResMgr->PushComponent( mName, extGStatePtr );
                    mpConstructor->State_ExtGState( mName, extGStatePtr );
                }
            }
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_h()
{
    if ( mpPath )
    {
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_i()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_Flatness( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_j()
{
    if ( CheckOpdCount( 1 ) )
    {
        GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
        unsigned int type = (unsigned int)( mOpdFloatStack[0] );
        switch ( type )
        {
            default:
            case 0:
                lineJoin = LineJoin_Miter;
                break;
            case 1:
                lineJoin = LineJoin_Round;
                break;
            case 2:
                lineJoin = LineJoin_Bevel;
                break;
        }
        mpConstructor->State_LineJoin( lineJoin );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_k()
{
    if ( CheckOpdCount( 4 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceCMYK();
        mpConstructor->State_FillColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        color.Push( mOpdFloatStack[3] );
        mpConstructor->State_FillColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_l()
{
    if ( CheckOpdCount( 2 ) )
    {
        if ( mpPath )
        {
            CHE_PDF_PathItem pathItem;
            pathItem.type = PathItem_LineTo;
            mpPath->mItems.push_back( pathItem );
            mCurX = pathItem.value = mOpdFloatStack[0];
            mpPath->mItems.push_back( pathItem );
            mCurY = pathItem.value = mOpdFloatStack[1];
            mpPath->mItems.push_back( pathItem );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_m()
{
    if ( CheckOpdCount( 2 ) )
    {
        if ( ! mpPath )
        {
            mpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
        }
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_MoveTo;
        mpPath->mItems.push_back( pathItem );
        mCurX = pathItem.value = mOpdFloatStack[0];
        mpPath->mItems.push_back( pathItem );
        mCurY = pathItem.value = mOpdFloatStack[1];
        mpPath->mItems.push_back( pathItem );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_n()
{
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
    if ( mpPath )
    {
        mpPath->GetAllocator()->Delete( mpPath );
        mpPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_q()
{
    mpConstructor->Operator_PushGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_re()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( ! mpPath )
        {
            mpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
        }
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_Rectangle;
        mpPath->mItems.push_back( pathItem );
        pathItem.value = mOpdFloatStack[0];
        mpPath->mItems.push_back( pathItem );
        pathItem.value = mOpdFloatStack[1];
        mpPath->mItems.push_back( pathItem );
        pathItem.value = mOpdFloatStack[2];
        mpPath->mItems.push_back( pathItem );
        pathItem.value = mOpdFloatStack[3];
        mpPath->mItems.push_back( pathItem );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_rg()
{
    if ( CheckOpdCount( 3 ) )
    {
        CHE_PDF_ColorSpacePtr colorspace = CHE_PDF_ColorSpace::CreateDeviceRGB();
        mpConstructor->State_FillColorSpace( colorspace );
        CHE_PDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        mpConstructor->State_FillColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_ri()
{
    if ( mName.GetLength() > 0 )
    {
        if ( mName == "AbsoluteColorimetric" )
        {
            mpConstructor->State_RenderIntents( RI_AbsoluteColorimetric );
        }else if ( mName == "RelativeColorimetric" )
        {
            mpConstructor->State_RenderIntents( RI_RelativeColorimetric );
        }else if ( mName == "Saturation" )
        {
            mpConstructor->State_RenderIntents( RI_Saturation );
        }else if ( mName == "Perceptual" )
        {
            mpConstructor->State_RenderIntents( RI_Perceptual );
        }	 
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_s()
{
    if ( mpPath )
    {
        CHE_PDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetPaintType( Paint_Stroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = NULL;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = NULL;
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_sc()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_FillColor( color );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_scn()
{
    if ( CheckOpdCount( 1 ) )
    {
        CHE_PDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_FillColor( color );
    }
    
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ObjectPtr objPtr;
        CHE_PDF_ComponentPtr cmptPtr;
        CHE_PDF_ColorSpacePtr csPtr;
        CHE_PDF_TilingPtr tilingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
        csPtr = mpConstructor->State_GetFillColorSpace();
        if ( IsPdfRefPtr( objPtr ) && csPtr && csPtr->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
        {
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_Tiling );
            if ( cmptPtr )
            {
                csPtr->GetPatternPtr()->mTiling = CHE_PDF_Tiling::Convert(cmptPtr);
            }else{
                tilingPtr = CHE_PDF_Tiling::Create( objPtr, mpCmptMgr, GetAllocator() );
                mpCmptMgr->PushComponent( objPtr->GetRefPtr(), tilingPtr );
                csPtr->GetPatternPtr()->mTiling = tilingPtr;
            }
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_sh()
{
    if ( mName.GetLength() > 0 )
    {
        CHE_PDF_ObjectPtr objPtr;
        CHE_PDF_ComponentPtr cmptPtr;
        CHE_PDF_ShadingPtr shadingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_SHADING, mName );
        if ( IsPdfRefPtr( objPtr ) )
        {
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_Shading );
            if ( cmptPtr && cmptPtr->GetType() == COMPONENT_TYPE_Shading )
            {
                CHE_PDF_ComponentRef * cmtRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, cmptPtr, GetAllocator() );
                mpConstructor->Operator_Append(cmtRef);
            }else{
                shadingPtr = CHE_PDF_Shading::Create( objPtr, mpCmptMgr, GetAllocator() );
                mpCmptMgr->PushComponent( objPtr->GetRefPtr(), shadingPtr );
                CHE_PDF_ComponentRef * cmtRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, shadingPtr, GetAllocator() );
                mpConstructor->Operator_Append(cmtRef);
            }
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_v()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( mpPath )
        {
            CHE_PDF_PathItem pathItem;
            pathItem.type = PathItem_CurveTo;
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mCurX;
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mCurY;
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[0];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[1];
            mpPath->mItems.push_back( pathItem );
            mCurX = pathItem.value = mOpdFloatStack[2];
            mpPath->mItems.push_back( pathItem );
            mCurY = pathItem.value = mOpdFloatStack[3];
            mpPath->mItems.push_back( pathItem );
        }
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_w()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_LineWidth( mOpdFloatStack[0] );
    }
}

HE_VOID CHE_PDF_ContentsParser::Handle_y()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( mpPath )
        {
            CHE_PDF_PathItem pathItem;
            pathItem.type = PathItem_CurveTo;
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[0];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[1];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[2];
            mpPath->mItems.push_back( pathItem );
            pathItem.value = mOpdFloatStack[3];
            mpPath->mItems.push_back( pathItem );
            mCurX = pathItem.value = mOpdFloatStack[2];
            mpPath->mItems.push_back( pathItem );
            mCurY = pathItem.value = mOpdFloatStack[3];
            mpPath->mItems.push_back( pathItem );
        }
    }
}

HE_BOOL CHE_PDF_ContentListBuilder::ParsePageContent( const CHE_PDF_DictionaryPtr & pageDict, CHE_PDF_ContentObjectList & contentList, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator /*= NULL*/ )
{
    if ( ! pageDict )
    {
        return FALSE;
    }
    if ( pComponentMgr == nullptr )
    {
        return FALSE;
    }
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    CHE_PDF_ObjectPtr		objPtr;
    CHE_PDF_DictionaryPtr	resDictPtr;
    CHE_PDF_StreamPtr		contentStreamPtr;
    CHE_PDF_ArrayPtr		contentArrayPtr;
    
    objPtr = pageDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
    if ( objPtr )
    {
        resDictPtr = objPtr->GetDictPtr();
    }
    
    objPtr = pageDict->GetElement( "Contents" , OBJ_TYPE_ARRAY );
    if ( objPtr )
    {
        contentArrayPtr = objPtr->GetArrayPtr();
    }
    if ( ! contentArrayPtr )
    {
        objPtr = pageDict->GetElement( "Contents", OBJ_TYPE_STREAM );
        if ( objPtr )
        {
            contentStreamPtr = objPtr->GetStreamPtr();
        }
    }
    
    contentList.GetResMgr().SetDict( resDictPtr ); 
    
    CHE_PDF_ContentListConstructor * pConstructor =  pAllocator->New<CHE_PDF_ContentListConstructor>( &contentList, CHE_Matrix(), pAllocator );
    CHE_PDF_ContentsParser contentsParser( &( contentList.GetResMgr() ), pComponentMgr, pConstructor );
    
    if ( contentStreamPtr )
    {
        contentsParser.Parse( contentStreamPtr );
    }
    else if ( contentArrayPtr )
    {
        contentsParser.Parse( contentArrayPtr );
    }
    
    pConstructor->GetAllocator()->Delete( pConstructor );
    pConstructor = NULL;
    
    return TRUE;
}

HE_BOOL CHE_PDF_ContentListBuilder::ParseContentStream( const CHE_PDF_StreamPtr & stmPtr, CHE_PDF_ContentObjectList & contentList, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator /*= NULL*/ )
{
    if ( ! stmPtr )
    {
        return FALSE;
    }
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    CHE_PDF_ObjectPtr objPtr;
    CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
    if ( ! dictPtr )
    {
        return FALSE;
    }
    objPtr = dictPtr->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
    if ( objPtr )
    {
        contentList.GetResMgr().SetDict( objPtr->GetDictPtr() );
    }
    
    
    CHE_Matrix matrix;
    objPtr = dictPtr->GetElement( "Matrix", OBJ_TYPE_ARRAY );
    if ( objPtr )
    {
        objPtr->GetArrayPtr()->GetMatrix( matrix );
    }
    
    CHE_PDF_ContentListConstructor * pConstructor =  pAllocator->New<CHE_PDF_ContentListConstructor>( &contentList, matrix, pAllocator );
    CHE_PDF_ContentsParser contentsParser( &( contentList.GetResMgr() ), pComponentMgr, pConstructor );
    contentsParser.Parse( stmPtr );
    pConstructor->GetAllocator()->Delete( pConstructor );
    return TRUE;
}

HE_BOOL	CHE_PDF_ContentListBuilder::ParseContentStream( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_ContentObjectList & contentList, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator /*= NULL*/ )
{
    CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
    CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
    return ParseContentStream( stmPtr, contentList, pComponentMgr, pAllocator );
}

