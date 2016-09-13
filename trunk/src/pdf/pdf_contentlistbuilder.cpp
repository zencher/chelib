#include "../../include/pdf/pdf_contentlistbuilder.h"
#include "../../include/pdf/pdf_contentobject.h"
#include "../../include/pdf/pdf_parser.h"
#include "../../include/pdf/pdf_extgstate.h"
#include "../../include/pdf/pdf_gstate.h"
#include "../../include/pdf/pdf_xobject.h"
#include "../../include/pdf/pdf_pattern.h"
#include "../../include/pdf/pdf_componentmgr.h"

namespace chelib {

class CPDF_ContentListConstructor : public CObject
{
public:
    CPDF_ContentListConstructor(	CPDF_ContentObjectList * pList,
                                   const CMatrix & matrix,
                                   CAllocator * pAllocator = nullptr )
    : mpList(pList), mpGState(nullptr), mExtMatrix(matrix),
    mTextLeading(0), mTextXOffset(0), mTextYOffset(0), CObject( pAllocator )
    {
        mpGState = GetAllocator()->New<CPDF_GState>( GetAllocator() );
    }
    
    ~CPDF_ContentListConstructor()
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
    
    void State_Matrix( const CMatrix & matrix )
    {
        GetGState()->SetMatrix( matrix );
    }
    
    void State_ConcatMatrix( const CMatrix & matrix )
    {
        CMatrix tmpMatirx = matrix;
        CMatrix curMatrix = GetGState()->GetMatrix();
        tmpMatirx.Concat( curMatrix );
        GetGState()->SetMatrix( tmpMatirx );
    }
    
    void State_LineWidth( const FLOAT & lineWidth )
    {
        GetGState()->SetLineWidth( lineWidth );
    }
    
    void State_LineCap( const GRAPHICS_STATE_LINECAP & lineCap )
    {
        GetGState()->SetLineCap( lineCap );
    }
    
    void State_LineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin )
    {
        GetGState()->SetLineJoin( lineJoin );
    }
    
    void State_MiterLimit( const FLOAT & miterLimit )
    {
        GetGState()->SetMiterLimit( miterLimit );
    }
    
    void State_LineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
    {
        GetGState()->SetLineDash( dashPattern );
    }
    
    void State_RenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
    {
        GetGState()->SetRenderIntents( ri );
    }
    
    void State_Flatness( const FLOAT & flatness )
    {
        GetGState()->SetFlatness( flatness );
    }
    
    void State_ExtGState( const CByteString & resName, const CPDF_ExtGStatePtr & extgstate )
    {
        GetGState()->PushExtGState( resName, extgstate );
    }
    
    void State_FillColor( CPDF_Color & color )
    {
        GetGState()->SetFillColor( color );
    }
    
    void State_StrokeColor( CPDF_Color & color )
    {
        GetGState()->SetStrokeColor( color );
    }
    
    void State_FillColorSpace( const CPDF_ColorSpacePtr & colorSpace )
    {
        GetGState()->SetFillColorSpace( colorSpace );
    }
    
    CPDF_ColorSpacePtr State_GetFillColorSpace()
    {
        CPDF_ColorSpacePtr csptr;
        GetGState()->GetFillColorSpace( csptr );
        return csptr;
    }
    
    void State_StrokeColorSpace( const CPDF_ColorSpacePtr & colorSpace )
    {
        GetGState()->SetStrokeColorSpace( colorSpace );
    }
    
    CPDF_ColorSpacePtr State_GetStrokeColorSpace()
    {
        CPDF_ColorSpacePtr csptr;
        GetGState()->GetStrokeColorSpace( csptr );
        return csptr;
    }
    
    void State_TextMatirx( const CMatrix & matrix )
    {
        GetGState()->SetTextMatrix( matrix );
    }
    
    void State_TextFont( const CByteString & resName, CPDF_Font * pFont )
    {
        GetGState()->SetTextFontResName( resName );
        if ( pFont )
        {
            GetGState()->SetTextFont( pFont );
        }
    }
    
    void State_TextFontSize( const FLOAT & size )
    {
        GetGState()->SetTextFontSize( size );
    }
    
    void State_TextCharSpace( const FLOAT & charSpace )
    {
        GetGState()->SetTextCharSpace( charSpace );
    }
    
    void State_TextWordSpace( const FLOAT & wordSpace )
    {
        GetGState()->SetTextWordSpace( wordSpace );
    }
    
    void State_TextScaling( const FLOAT & scaling )
    {
        GetGState()->SetTextScaling( scaling );
    }
    
    void State_TextLeading( const FLOAT & leading )
    {
        mTextLeading = leading;
    }
    
    void State_TextRise( const FLOAT & rise )
    {
        GetGState()->SetTextRise( rise );
    }
    
    void State_TextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE rm )
    {
        GetGState()->SetTextRenderMode( rm );
    }
    
    void State_ResetTextOffset()
    {
        mTextXOffset = 0;
        mTextYOffset = 0;
    }
    
    void State_TextOffset( const FLOAT & xOffset, const FLOAT & yOffset )
    {
        mTextXOffset += xOffset;
        mTextYOffset += yOffset;
    }
    
    void State_TextObject( CPDF_ObjectPtr objPtr )
    {
        mTextObj = objPtr;
    }
    
    void Operator_Td( const FLOAT & tx, const FLOAT & ty )
    {
        CMatrix matrix, tmpMatrix;
        GetGState()->GetTextMatrix( matrix );
        tmpMatrix.e = tx;
        tmpMatrix.f = ty;
        tmpMatrix.Concat( matrix );
        GetGState()->SetTextMatrix( tmpMatrix );
    }
    
    void Operator_TD( const FLOAT & tx, const FLOAT & ty )
    {
        mTextLeading = -ty;
        Operator_Td( tx, ty );
    }
    
    void Operator_Tstar()
    {
        Operator_Td( 0, -mTextLeading );
    }
    
    void Operator_PushGState()
    {
        if ( mpGState )
        {
            mGStateStack.push_back( mpGState->Clone() );
        }else
        {
            mGStateStack.push_back( nullptr );
        }
    }
    
    void Operator_PopGState()
    {
        if ( mpGState )
        {
            mpGState->GetAllocator()->Delete( mpGState );
            mpGState = nullptr;
        }
        if ( mGStateStack.size() > 0 )
        {
            mpGState = mGStateStack[mGStateStack.size()-1];
            mGStateStack.pop_back();
        }
    }
    
    void Operator_Clip( CPDF_ContentObject * pObject )
    {
        if ( pObject && ( pObject->GetType() == ContentType_Path || pObject->GetType() == ContentType_Text ) )
        {
            CPDF_GState * pGState = GetGState()->Clone();
            pObject->SetGState( pGState );
            GetGState()->PushClipElement( pObject );
        }
    }
    
    void Operator_Append( CPDF_ContentObject * pObject )
    {
        if ( pObject )
        {
            switch( pObject->GetType() )
            {
                case ContentType_Text:
                {
                    CPDF_GState * pGState = mpGState->Clone();
                    CMatrix textMatrix;
                    pGState->GetTextMatrix( textMatrix );
                    CMatrix tmpMatrix;
                    tmpMatrix.e = mTextXOffset;
                    tmpMatrix.f = mTextYOffset;
                    tmpMatrix.Concat( textMatrix );
                    pGState->SetTextMatrix( tmpMatrix );
                    pObject->SetGState( pGState );
                    
                    CPDF_Text * pText = (CPDF_Text*)( pObject );
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
            //CMatrix tmpMatrix = pObject->GetExtMatrix();
            //tmpMatrix.Concat( mExtMatrix );
            //pObject->SetExtMatrix( tmpMatrix );
        }
    }
    
    CMatrix GetExtMatrix()
    {
        return mExtMatrix;
    }
    
    CMatrix GetCurMatrix()
    {
        if ( mpGState )
        {
            return mpGState->GetMatrix();
        }
        return CMatrix();
    }
    
    void Operator_D0( const FLOAT wx, const FLOAT wy )
    {
        std::vector<FLOAT> param;
        param.push_back( wx );
        param.push_back( wy );
        mpList->SetType3BBox( 0, param );
    }
    
    void Operator_D1(	const FLOAT wx, const FLOAT wy,
                        const FLOAT llx, const FLOAT lly,
                        const FLOAT urx, const FLOAT ury )
    {
        std::vector<FLOAT> param;
        param.push_back( wx );
        param.push_back( wy );
        param.push_back( llx );
        param.push_back( lly );
        param.push_back( urx );
        param.push_back( ury );
        mpList->SetType3BBox( 1, param );
    }
    
private:
    CPDF_GState * GetGState()
    {
        if ( !mpGState )
        {
            mpGState = GetAllocator()->New<CPDF_GState>( GetAllocator() );
        }
        return mpGState;
    }
    
    CPDF_ContentObjectList * mpList;
    std::vector<CPDF_GState*> mGStateStack;
    CPDF_GState * mpGState;
    CMatrix mExtMatrix;
    FLOAT mTextLeading;
    FLOAT mTextXOffset;
    FLOAT mTextYOffset;
    CPDF_ObjectPtr mTextObj;
};


class CPDF_ContentsParser : public CObject
{
public:
    CPDF_ContentsParser( CPDF_ContentResMgr * pResMgr, CPDF_ComponentMgr * pCmptMgr,
                           CPDF_ContentListConstructor * pConstructor, CAllocator * pAllocator = nullptr )
    :	CObject( pAllocator ), mpContentResMgr(pResMgr), mpCmptMgr(pCmptMgr), mpConstructor(pConstructor),
    mpPath(nullptr), mpClipPath(nullptr), mCurX(0), mCurY(0), mString(pAllocator), mName(pAllocator), mParamFalg(0),
    mbInlineImage(false), mbInterpolate(false), mbMask(false), mWidth(0), mHeight(0), mBpc(0) {}
    
    ~CPDF_ContentsParser()
    {
        mOpdFloatStack.clear();
        if ( mpPath )
        {
            mpPath->GetAllocator()->Delete( mpPath );
            mpPath = nullptr;
        }
        if ( mpClipPath )
        {
            mpClipPath->GetAllocator()->Delete( mpClipPath );
            mpClipPath = nullptr;
        }
    }
    
    bool Parse( const CPDF_StreamPtr & pContentStream );
    
    bool Parse( const CPDF_ArrayPtr & pContentArray );
    
private:
    void ParseImp( CDynBuffer * pStream );
    
    inline void Handle_dquote();
    inline void Handle_squote();
    inline void Handle_B();
    inline void Handle_Bstar();
    inline void Handle_BDC();
    inline void Handle_BI();
    inline void Handle_BMC();
    inline void Handle_BT();
    inline void Handle_BX();
    inline void Handle_CS();
    inline void Handle_DP();
    inline void Handle_Do();
    inline void Handle_EI();
    inline void Handle_EMC();
    inline void Handle_ET();
    inline void Handle_EX();
    inline void Handle_F();
    inline void Handle_G();
    inline void Handle_ID( CPDF_SyntaxParser * pParser );
    inline void Handle_J();
    inline void Handle_K();
    inline void Handle_M();
    inline void Handle_MP();
    inline void Handle_Q();
    inline void Handle_RG();
    inline void Handle_S();
    inline void Handle_SC();
    inline void Handle_SCN();
    inline void Handle_Tstar();
    inline void Handle_TD();
    inline void Handle_TJ();
    inline void Handle_TL();
    inline void Handle_Tc();
    inline void Handle_Td();
    inline void Handle_Tf();
    inline void Handle_Tj();
    inline void Handle_Tm();
    inline void Handle_Tr();
    inline void Handle_Ts();
    inline void Handle_Tw();
    inline void Handle_Tz();
    inline void Handle_W();
    inline void Handle_Wstar();
    inline void Handle_b();
    inline void Handle_bstar();
    inline void Handle_c();
    inline void Handle_cm();
    inline void Handle_cs();
    inline void Handle_d();
    inline void Handle_d0();
    inline void Handle_d1();
    inline void Handle_f();
    inline void Handle_fstar();
    inline void Handle_g();
    inline void Handle_gs();
    inline void Handle_h();
    inline void Handle_i();
    inline void Handle_j();
    inline void Handle_k();
    inline void Handle_l();
    inline void Handle_m();
    inline void Handle_n();
    inline void Handle_q();
    inline void Handle_re();
    inline void Handle_rg();
    inline void Handle_ri();
    inline void Handle_s();
    inline void Handle_sc();
    inline void Handle_scn();
    inline void Handle_sh();
    inline void Handle_v();
    inline void Handle_w();
    inline void Handle_y();
    
    inline bool CheckOpdCount( size_t count );
    
    std::vector<FLOAT>	mOpdFloatStack;
    CByteString			mName;
    CByteString			mString;
    CPDF_ObjectPtr		mpObj;
    
    CPDF_Path *			mpPath;
    CPDF_Path *			mpClipPath;
    FLOAT                   mBeginX;
    FLOAT                   mBeginY;
    FLOAT                   mCurX;
    FLOAT                   mCurY;
    
    //inline image
    BYTE					mParamFalg;
    bool					mbInlineImage;
    bool					mbInterpolate;
    bool					mbMask;
    uint32                  mWidth;
    uint32                  mHeight;
    uint32                  mBpc;
    CPDF_ObjectPtr		mpColorSpace;
    CPDF_ObjectPtr		mpFilter;
    CPDF_ObjectPtr		mpDecode;
    CPDF_ObjectPtr		mpDecodeParam;
    
    CPDF_ComponentMgr *	mpCmptMgr;
    CPDF_ContentResMgr * mpContentResMgr;
    CPDF_ContentListConstructor * mpConstructor;
};


bool CPDF_ContentsParser::Parse( const CPDF_StreamPtr & pContents )
{
    if ( ! pContents || mpConstructor == nullptr )
    {
        return false;
    }
    
    CDynBuffer buf( pContents->GetRawSize() * 2, pContents->GetRawSize(), GetAllocator() );
    CPDF_StreamAcc streamAcc( GetAllocator() );
    streamAcc.Attach( pContents );
    buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
    streamAcc.Detach();
    
    ParseImp( &buf );
    
    return true;
}

bool CPDF_ContentsParser::Parse( const CPDF_ArrayPtr & pContentArray )
{
    if ( ! pContentArray || mpConstructor == nullptr )
    {
        return false;
    }
    
    CDynBuffer buf( 1024 * 4, 1024 * 8, GetAllocator() );
    
    for ( size_t i = 0; i < pContentArray->GetCount(); ++i )
    {
        CPDF_StreamPtr pStream = pContentArray->GetElement( i, OBJ_TYPE_STREAM )->GetStreamPtr();
        if ( pStream )
        {
            CPDF_StreamAcc streamAcc( GetAllocator() );
            streamAcc.Attach( pStream );
            buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
            streamAcc.Detach();
        }
    }
    if ( buf.GetSize() > 0 )
    {
        ParseImp( &buf );
    }
    return true;
}

void CPDF_ContentsParser::ParseImp( CDynBuffer * pStream )
{
    if ( !pStream )
    {
        return;
    }
    
    IRead * pRead = CreateMemBufIRead( pStream->GetData(), pStream->GetSize(), GetAllocator() );
    if ( pRead == nullptr )
    {
        return;
    }
    
    CPDF_SyntaxParser sParser( nullptr, GetAllocator() );
    if ( sParser.InitParser( pRead ) == false )
    {
        pRead->Release();
        DestoryIRead( pRead );
        return;
    }
    
    CPDF_ParseWordDes wordDes;
    CPDF_ObjectPtr pTmpNode;
    FLOAT tmpValue = 0;
    bool	bOpd = true;
    while( sParser.GetWord( wordDes ) == true )
    {
        bOpd = true;
        switch ( wordDes.type )
        {
            case PARSE_WORD_INTEGER:
            {
                tmpValue = (FLOAT)( sParser.GetInteger() );
                mOpdFloatStack.push_back( tmpValue );
                
                if ( mbInlineImage && mParamFalg > 0 )
                {
                    switch ( mParamFalg )
                    {
                        case 1:
                            mBpc = sParser.GetInteger();
                            break;
                        case 6:
                            mHeight = sParser.GetInteger();
                            break;
                        case 9:
                            mWidth = sParser.GetInteger();
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
                tmpValue = sParser.GetFloat();
                mOpdFloatStack.push_back( tmpValue );
                break;
            }
            case PARSE_WORD_NAME:
            {
                if ( mName.GetLength() > 0 )
                {
                    mString = sParser.GetString();
                }
                else
                {
                    mName = sParser.GetString();
                }
                if ( mbInlineImage )
                {
                    if ( mParamFalg > 0 )
                    {
                        switch ( mParamFalg )
                        {
                            case 2:
                                mpColorSpace = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
                                break;
                            case 3:
                                mpDecode = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
                                break;
                            case 4:
                                mpDecodeParam = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
                                break;
                            case 5:
                                mpFilter = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
                                break;
                            default:
                                break;
                        }
                        mParamFalg = 0;
                    }else
                    {
                        switch ( StringToUINT32( sParser.GetString() ) )
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
                mpObj = CPDF_String::Create( mString, GetAllocator() );
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
                    if ( sParser.IsWord( "true" ) )
                    {
                        mbMask = true;
                    }
                    else if ( sParser.IsWord( "false" ) )
                    {
                        mbMask = false;
                    }
                    mParamFalg = 0;
                    break;
                }
                bOpd = false;
                break;
            }
        }
        if ( bOpd == true )
        {
            continue;
        }
        
        switch ( StringToUINT32( sParser.GetString() ) )
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
    DestoryIRead( pRead );
}

bool CPDF_ContentsParser::CheckOpdCount( size_t count )
{
    if ( mOpdFloatStack.size() >= count )
    {
        return true;
    }
    return false;
}

void CPDF_ContentsParser::Handle_dquote()
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
        CPDF_Text * pText = GetAllocator()->New<CPDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

void CPDF_ContentsParser::Handle_squote()
{
    mpConstructor->Operator_Tstar();
    mpConstructor->State_ResetTextOffset();
    if ( mpObj )
    {
        CPDF_Text * pText = GetAllocator()->New<CPDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

void CPDF_ContentsParser::Handle_B()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_Bstar()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_BDC()
{
    if ( mName.GetLength() > 0 &&  IsPdfDictPtr( mpObj ) )
    {
        CPDF_Mark * pMark = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_BDC );
        pMark->SetTag( mName );
        if ( mpObj )
        {
            pMark->SetProperty( mpObj->GetDictPtr() );
        }
        else if ( mString.GetLength() > 0 )
        {
            CPDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
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

void CPDF_ContentsParser::Handle_BI()
{
    mbInlineImage = true;
}

void CPDF_ContentsParser::Handle_BMC()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_Mark * pMark = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_BMC );
        pMark->SetTag( mName );
        mpConstructor->Operator_Append( pMark );
    }
}

void CPDF_ContentsParser::Handle_BT()
{
    mpConstructor->State_TextMatirx( CMatrix() );
}

void CPDF_ContentsParser::Handle_BX()
{
    
}

void CPDF_ContentsParser::Handle_CS()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_ColorSpacePtr colorspacePtr = CPDF_ColorSpace::Create( mName, GetAllocator() );
        if ( ! colorspacePtr )
        {
            CPDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
            if ( cmptPtr )
            {
                mpConstructor->State_StrokeColorSpace( CPDF_ColorSpace::Convert( cmptPtr ) );
            }else{
                CPDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
                if ( ! objPtr )
                {
                    return;
                }
                if ( IsPdfRefPtr( objPtr ) )
                {
                    CPDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ColorSpace );
                    if ( cmptPtr )
                    {
                        mpConstructor->State_StrokeColorSpace( CPDF_ColorSpace::Convert( cmptPtr ) );
                    }else{
                        colorspacePtr = CPDF_ColorSpace::Create( objPtr, GetAllocator() );
                        mpCmptMgr->PushComponent( objPtr->GetRefPtr(), colorspacePtr );
                        mpConstructor->State_StrokeColorSpace( colorspacePtr );
                    }
                }else{
                    colorspacePtr = CPDF_ColorSpace::Create( objPtr, GetAllocator() );
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

void CPDF_ContentsParser::Handle_DP()
{
    if ( mName.GetLength() > 0 && IsPdfDictPtr( mpObj ) )
    {
        CPDF_Mark * pMark = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_DP );
        pMark->SetTag( mName );
        if ( mpObj )
        {
            pMark->SetProperty( mpObj->GetDictPtr() );
        }
        else if ( mString.GetLength() > 0 )
        {
            CPDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
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

void CPDF_ContentsParser::Handle_Do()
{
    if ( mName.GetLength() > 0 && mpContentResMgr )
    {
        CPDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
        if ( !pTmpObj )
        {
            return;
        }
        CPDF_ReferencePtr pRef = pTmpObj->GetRefPtr();
        if ( !pRef )
        {
            return;
        }
        
        CPDF_ComponentPtr ptr = mpCmptMgr->GetComponent( pRef );
        if ( ptr )
        {
            if ( ptr->GetType() == COMPONENT_TYPE_ImageXObject || ptr->GetType() == COMPONENT_TYPE_FormXObject )
            {
                CPDF_ComponentRef * cmtRef = GetAllocator()->New<CPDF_ComponentRef>( mName, ptr, GetAllocator() );
                mpConstructor->Operator_Append( cmtRef );
                return;
            }
        }
        
        pTmpObj = pRef->GetRefObj( OBJ_TYPE_STREAM );
        if ( !pTmpObj )
        {
            return;
        }
        CPDF_StreamPtr pStm = pTmpObj->GetStreamPtr();
        CPDF_DictionaryPtr pStmDict = pStm->GetDictPtr();
        if ( !pStmDict )
        {
            return;
        }
        pTmpObj = pStmDict->GetElement( "Subtype", OBJ_TYPE_NAME );
        if ( !pTmpObj )
        {
            return;
        }
        CPDF_NamePtr pSubtypeName = pTmpObj->GetNamePtr();
        if ( !pSubtypeName )
        {
            return;
        }
        if ( pSubtypeName->GetString() == "Image" )
        {
            CPDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
            if ( pObj && IsPdfRefPtr( pObj ) )
            {
                CPDF_ImageXObjectPtr refImgPtr = CPDF_ImageXObject::Create( pRef, GetAllocator() );
                CPDF_ComponentRef * cmtRef = GetAllocator()->New<CPDF_ComponentRef>( mName, refImgPtr, GetAllocator() );
                mpConstructor->Operator_Append( cmtRef );
                
                mpCmptMgr->PushComponent( pRef, refImgPtr );
            }
        }else if ( pSubtypeName->GetString() == "Form" )
        {
            CPDF_FormXObjectPtr formPtr = CPDF_FormXObject::Create( pRef, mpCmptMgr, GetAllocator() );
            
            // 			//CPDF_FormXObject * pForm = GetAllocator()->New<CPDF_FormXObject>( mName, GetAllocator() );
            // 			CPDF_ObjectPtr pTmpObj;
            // 			CPDF_DictionaryPtr pResDict;
            // 			pTmpObj = pStmDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
            // 			if ( pTmpObj )
            // 			{
            // 				pResDict = pTmpObj->GetDictPtr();
            // 			}
            // 			//todo : get the matrix from form dict
            CMatrix extMatrix = mpConstructor->GetExtMatrix();
            CMatrix curMatrix = mpConstructor->GetCurMatrix();
            curMatrix.Concat( extMatrix );
            // 			CPDF_ContentListConstructor * pConstructor = GetAllocator()->New<CPDF_ContentListConstructor>( & formPtr->GetList(), curMatrix, GetAllocator() );
            // 			CPDF_ContentResMgr * pContentResMgr = &( formPtr->GetList().GetResMgr() );
            // 			pContentResMgr->SetDict( pResDict );
            // 			CPDF_ContentsParser contentsParser( pContentResMgr, mpFontMgr, pConstructor );
            // 			contentsParser.Parse( pStm );
            
            CPDF_ComponentRef * cptRef = GetAllocator()->New<CPDF_ComponentRef>( mName, formPtr, GetAllocator() );
            
            mpConstructor->Operator_Append( cptRef );
            //			pConstructor->GetAllocator()->Delete( pConstructor );
            
            mpCmptMgr->PushComponent( pRef, formPtr );
        }
    }
}

void CPDF_ContentsParser::Handle_EI()
{
    mbInlineImage = false;
    mbInterpolate = false;
    mbMask = false;
    mWidth = 0;
    mHeight = 0;
    mBpc = 0;
    
    mpColorSpace.Reset();
    mpFilter.Reset();
    mpDecode.Reset();
    mpDecodeParam.Reset();
}

void CPDF_ContentsParser::Handle_EMC()
{
    CPDF_Mark * pMark = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
    pMark->SetMarkType( Mark_EMC );
    mpConstructor->Operator_Append( pMark );
}

void CPDF_ContentsParser::Handle_ET()
{
}

void CPDF_ContentsParser::Handle_EX()
{
    
}

void CPDF_ContentsParser::Handle_F()
{
    Handle_f();
}

void CPDF_ContentsParser::Handle_G()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceGray();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        mpConstructor->State_StrokeColor( color );
    }
}

void CPDF_ContentsParser::Handle_ID( CPDF_SyntaxParser * pParser )
{
    if ( pParser == nullptr || mbInlineImage == false )
    {
        return;
    }
    bool bOver = false;
    size_t dwRet = 1;
    BYTE byteRet[4];
    std::vector<BYTE> buffer;
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
    
    
    CPDF_DictionaryPtr pDict = CPDF_Dictionary::Create( GetAllocator() );
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
    pDict->SetInteger( "Length", (int32)buffer.size() );
    
    CPDF_ColorSpacePtr colorspace;
    if ( mpColorSpace )
    {
        colorspace = CPDF_ColorSpace::Create( mpColorSpace, GetAllocator() );
        // 		if ( mpColorSpace->GetType() == OBJ_TYPE_NAME )
        // 		{
        // 			pColorspace = CPDF_ColorSpace::Create( mpColorSpace->GetNamePtr(), GetAllocator() );
        // 			if ( pColorspace == nullptr )
        // 			{
        // 				CPDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, pObj->GetNamePtr()->GetString() );
        // 				if ( ! pObj )
        // 				{
        // 					/*assert(0);*/
        // 					/*error*/
        // 				}
        // 				if ( pObj->GetType() == OBJ_TYPE_NAME )
        // 				{
        // 					pColorspace = CPDF_ColorSpace::Create( pObj->GetNamePtr()->GetString(), GetAllocator() );
        // 				}else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
        // 				{
        // 					pColorspace = CPDF_ColorSpace::Create( pObj->GetNamePtr()->GetString(), GetAllocator() );
        // 				}else{
        // 					/*assert(0);*/
        // 					/*error*/
        // 				}
        // 			}
        // 		}else if ( mpColorSpace->GetType() == OBJ_TYPE_ARRAY )
        // 		{
        // 			pColorspace = CPDF_ColorSpace::Create( mpColorSpace->GetArrayPtr(), GetAllocator() );
        // 		}
        /*assert( 0 );*/
        /*error*/
    }
    
    CPDF_StreamPtr pStream = CPDF_Stream::Create( &buffer[0], buffer.size(), pDict, 0, 0, nullptr, GetAllocator() );
    if ( pStream )
    {
        CPDF_StreamAcc stmAcc( GetAllocator() );
        if ( stmAcc.Attach( pStream ) )
        {
            CPDF_InlineImage * pImage = GetAllocator()->New<CPDF_InlineImage>(	mbMask, mWidth, mHeight, mBpc,
                                                                                    stmAcc.GetData(), stmAcc.GetSize(), mpDecode,
                                                                                    colorspace, RI_AbsoluteColorimetric, GetAllocator() );
            mpConstructor->Operator_Append( pImage );
        }
        stmAcc.Detach();
    }
}

void CPDF_ContentsParser::Handle_J()
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

void CPDF_ContentsParser::Handle_K()
{
    if ( CheckOpdCount( 4 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceCMYK();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        color.Push( mOpdFloatStack[3] );
        mpConstructor->State_StrokeColor( color );
    }
}
void CPDF_ContentsParser::Handle_M()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_MiterLimit( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_MP()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_Mark * pMark = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
        pMark->SetMarkType( Mark_MP );
        pMark->SetTag( mName );
        mpConstructor->Operator_Append( pMark );
    }
}

void CPDF_ContentsParser::Handle_Q()
{
    mpConstructor->Operator_PopGState();
}

void CPDF_ContentsParser::Handle_RG()
{
    if ( CheckOpdCount( 3 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceRGB();
        mpConstructor->State_StrokeColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        mpConstructor->State_StrokeColor( color );
    }
}

void CPDF_ContentsParser::Handle_S()
{
    if ( mpPath )
    {
        mpPath->SetPaintType( Paint_Stroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}
void CPDF_ContentsParser::Handle_SC()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_StrokeColor( color );
    }
}

void CPDF_ContentsParser::Handle_SCN()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_StrokeColor( color );
    }
    if ( mName.GetLength() > 0 )
    {
        CPDF_ObjectPtr objPtr;
        CPDF_ComponentPtr cmptPtr;
        CPDF_ColorSpacePtr csPtr;
        CPDF_TilingPtr tilingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
        csPtr = mpConstructor->State_GetStrokeColorSpace();
        if ( IsPdfRefPtr( objPtr ) && csPtr && csPtr->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
        {
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_Tiling );
            if ( cmptPtr )
            {
                csPtr->GetPatternPtr()->mPattern = cmptPtr;//CPDF_Tiling::Convert(cmptPtr);
            }else{
                tilingPtr = CPDF_Tiling::Create( objPtr, mpCmptMgr, GetAllocator() );
                if (tilingPtr)
                {
                    mpCmptMgr->PushComponent( objPtr->GetRefPtr(), tilingPtr );
                    csPtr->GetPatternPtr()->mPattern = tilingPtr;
                }
            }
        }
    }
}

void CPDF_ContentsParser::Handle_Tstar()
{
    mpConstructor->Operator_Tstar();
    mpConstructor->State_ResetTextOffset();
}

void CPDF_ContentsParser::Handle_TD()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_TD( mOpdFloatStack[0], mOpdFloatStack[1] );
        mpConstructor->State_ResetTextOffset();
    }
}

void CPDF_ContentsParser::Handle_TJ()
{
    if ( mpObj )
    {
        CPDF_Text * pText = GetAllocator()->New<CPDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

void CPDF_ContentsParser::Handle_TL()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextLeading( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_Tc()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextCharSpace( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_Td()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_Td( mOpdFloatStack[0], mOpdFloatStack[1] );
        mpConstructor->State_ResetTextOffset();
    }
}

void CPDF_ContentsParser::Handle_Tf()
{
    if ( CheckOpdCount( 1 ) && mName.GetLength() > 0 )
    {
        //todo load font
        CPDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_FONT, mName );
        if ( pTmpObj )
        {
            if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
            {
                CPDF_ComponentPtr cmptPtr = mpCmptMgr->LoadFont( pTmpObj->GetRefPtr() );
                if ( cmptPtr->GetType() == COMPONENT_TYPE_Font ) {
                    CPDF_Font * pFont = (CPDF_Font *)cmptPtr.GetPointer();
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

void CPDF_ContentsParser::Handle_Tj()
{
    if ( mpObj )
    {
        CPDF_Text * pText = GetAllocator()->New<CPDF_Text>( GetAllocator() );
        mpConstructor->State_TextObject( mpObj );
        mpConstructor->Operator_Append( pText );
        mpConstructor->State_TextOffset( pText->GetOffSet(), 0 );
    }
}

void CPDF_ContentsParser::Handle_Tm()
{
    if ( CheckOpdCount( 6 ) )
    {
        CMatrix matrix;
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

void CPDF_ContentsParser::Handle_Tr()
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

void CPDF_ContentsParser::Handle_Ts()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextRise( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_Tw()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextWordSpace( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_Tz()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_TextScaling( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_W()
{
    if ( mpClipPath )
    {
        mpClipPath->GetAllocator()->Delete( mpClipPath );
        mpClipPath = nullptr;
    }
    if ( mpPath )
    {
        mpClipPath = (CPDF_Path *)( mpPath->Clone() );
        mpClipPath->SetFillMode( Mode_Nonzero );
    }
}

void CPDF_ContentsParser::Handle_Wstar()
{
    if ( mpClipPath )
    {
        mpClipPath->GetAllocator()->Delete( mpClipPath );
        mpClipPath = nullptr;
    }
    if ( mpPath )
    {
        mpClipPath = (CPDF_Path *)( mpPath->Clone() );
        mpClipPath->SetFillMode( Mode_EvenOdd );
    }
}

void CPDF_ContentsParser::Handle_b()
{
    if ( mpPath )
    {
        CPDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_bstar()
{
    if ( mpPath )
    {
        CPDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_FillStroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_c()
{
    if ( CheckOpdCount( 6 ) )
    {
        if ( mpPath )
        {
            CPDF_PathItem pathItem;
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

void CPDF_ContentsParser::Handle_cm()
{
    if ( CheckOpdCount( 6 ) )
    {
        CMatrix matrix;
        matrix.a = mOpdFloatStack[0];
        matrix.b = mOpdFloatStack[1];
        matrix.c = mOpdFloatStack[2];
        matrix.d = mOpdFloatStack[3];
        matrix.e = mOpdFloatStack[4];
        matrix.f = mOpdFloatStack[5];
        mpConstructor->State_ConcatMatrix( matrix );
    }
}

void CPDF_ContentsParser::Handle_cs()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_ColorSpacePtr colorspacePtr = CPDF_ColorSpace::Create( mName, GetAllocator() );
        if ( ! colorspacePtr )
        {
            CPDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
            if ( cmptPtr )
            {
                mpConstructor->State_FillColorSpace( CPDF_ColorSpace::Convert( cmptPtr ) );
            }else{
                CPDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
                if ( ! objPtr )
                {
                    return;
                }
                if ( IsPdfRefPtr( objPtr ) )
                {
                    CPDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ColorSpace );
                    if ( cmptPtr )
                    {
                        mpConstructor->State_FillColorSpace( CPDF_ColorSpace::Convert( cmptPtr ) );
                    }else{
                        colorspacePtr = CPDF_ColorSpace::Create( objPtr, GetAllocator() );
                        mpCmptMgr->PushComponent( objPtr->GetRefPtr(), colorspacePtr );
                        mpConstructor->State_FillColorSpace( colorspacePtr );
                    }
                }else{
                    colorspacePtr = CPDF_ColorSpace::Create( objPtr, GetAllocator() );
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

void CPDF_ContentsParser::Handle_d()
{
    if ( CheckOpdCount( 1 ) )
    {
        GRAPHICS_STATE_DASHPATTERN dashPattern;
        dashPattern.dashPhase = mOpdFloatStack[0];
        
        if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
        {
            CPDF_ObjectPtr objPtr;
            CPDF_NumberPtr numberPtr;
            CPDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
            for ( size_t i = 0; i < arrayPtr->GetCount(); ++i )
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

void CPDF_ContentsParser::Handle_d0()
{
    if ( CheckOpdCount( 2 ) )
    {
        mpConstructor->Operator_D0( mOpdFloatStack[0], mOpdFloatStack[1] );
    }
}

void CPDF_ContentsParser::Handle_d1()
{
    if ( CheckOpdCount( 6 ) )
    {
        mpConstructor->Operator_D1( mOpdFloatStack[0], mOpdFloatStack[1],
                                   mOpdFloatStack[2], mOpdFloatStack[3],
                                   mOpdFloatStack[4], mOpdFloatStack[5] );
    }
}

void CPDF_ContentsParser::Handle_f()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_Nonzero );
        mpPath->SetPaintType( Paint_Fill );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_fstar()
{
    if ( mpPath )
    {
        mpPath->SetFillMode( Mode_EvenOdd );
        mpPath->SetPaintType( Paint_Fill );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_g()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceGray();
        mpConstructor->State_FillColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        mpConstructor->State_FillColor( color );
    }
}

void CPDF_ContentsParser::Handle_gs()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_ComponentPtr cmptPtr = mpContentResMgr->GetComponent( mName, COMPONENT_TYPE_ColorSpace );
        if ( cmptPtr )
        {
            mpConstructor->State_ExtGState( mName, CPDF_ExtGState::Convert( cmptPtr ) );
        }else{
            CPDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_EXTGSTATE, mName );
            if ( ! objPtr )
            {
                return;
            }
            if ( IsPdfRefPtr( objPtr ) )
            {
                CPDF_ComponentPtr cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_ExtGState );
                if ( cmptPtr )
                {
                    mpConstructor->State_ExtGState( mName, CPDF_ExtGState::Convert( cmptPtr ) );
                }else{
                    CPDF_ExtGStatePtr extGStatePtr = CPDF_ExtGState::Create( objPtr, GetAllocator() );
                    mpCmptMgr->PushComponent( objPtr->GetRefPtr(), extGStatePtr );
                    mpConstructor->State_ExtGState( mName, extGStatePtr );
                }
            }else{
                CPDF_ExtGStatePtr extGStatePtr = CPDF_ExtGState::Create( objPtr, GetAllocator() );
                if ( extGStatePtr )
                {
                    mpContentResMgr->PushComponent( mName, extGStatePtr );
                    mpConstructor->State_ExtGState( mName, extGStatePtr );
                }
            }
        }
    }
}

void CPDF_ContentsParser::Handle_h()
{
    if ( mpPath )
    {
        CPDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
    }
}

void CPDF_ContentsParser::Handle_i()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_Flatness( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_j()
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

void CPDF_ContentsParser::Handle_k()
{
    if ( CheckOpdCount( 4 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceCMYK();
        mpConstructor->State_FillColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        color.Push( mOpdFloatStack[3] );
        mpConstructor->State_FillColor( color );
    }
}

void CPDF_ContentsParser::Handle_l()
{
    if ( CheckOpdCount( 2 ) )
    {
        if ( mpPath )
        {
            CPDF_PathItem pathItem;
            pathItem.type = PathItem_LineTo;
            mpPath->mItems.push_back( pathItem );
            mCurX = pathItem.value = mOpdFloatStack[0];
            mpPath->mItems.push_back( pathItem );
            mCurY = pathItem.value = mOpdFloatStack[1];
            mpPath->mItems.push_back( pathItem );
        }
    }
}

void CPDF_ContentsParser::Handle_m()
{
    if ( CheckOpdCount( 2 ) )
    {
        if ( ! mpPath )
        {
            mpPath = GetAllocator()->New<CPDF_Path>( GetAllocator() );
        }
        CPDF_PathItem pathItem;
        pathItem.type = PathItem_MoveTo;
        mpPath->mItems.push_back( pathItem );
        mCurX = pathItem.value = mOpdFloatStack[0];
        mpPath->mItems.push_back( pathItem );
        mCurY = pathItem.value = mOpdFloatStack[1];
        mpPath->mItems.push_back( pathItem );
    }
}

void CPDF_ContentsParser::Handle_n()
{
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
    if ( mpPath )
    {
        mpPath->GetAllocator()->Delete( mpPath );
        mpPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_q()
{
    mpConstructor->Operator_PushGState();
}

void CPDF_ContentsParser::Handle_re()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( ! mpPath )
        {
            mpPath = GetAllocator()->New<CPDF_Path>( GetAllocator() );
        }
        CPDF_PathItem pathItem;
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

void CPDF_ContentsParser::Handle_rg()
{
    if ( CheckOpdCount( 3 ) )
    {
        CPDF_ColorSpacePtr colorspace = CPDF_ColorSpace::CreateDeviceRGB();
        mpConstructor->State_FillColorSpace( colorspace );
        CPDF_Color color;
        color.Push( mOpdFloatStack[0] );
        color.Push( mOpdFloatStack[1] );
        color.Push( mOpdFloatStack[2] );
        mpConstructor->State_FillColor( color );
    }
}

void CPDF_ContentsParser::Handle_ri()
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

void CPDF_ContentsParser::Handle_s()
{
    if ( mpPath )
    {
        CPDF_PathItem pathItem;
        pathItem.type = PathItem_Close;
        mpPath->mItems.push_back( pathItem );
        mpPath->SetPaintType( Paint_Stroke );
        mpConstructor->Operator_Append( mpPath );
        mpPath = nullptr;
    }
    if ( mpClipPath )
    {
        mpConstructor->Operator_Clip( mpClipPath );
        mpClipPath = nullptr;
    }
}

void CPDF_ContentsParser::Handle_sc()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_FillColor( color );
    }
}

void CPDF_ContentsParser::Handle_scn()
{
    if ( CheckOpdCount( 1 ) )
    {
        CPDF_Color color;
        for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
        {
            color.Push( mOpdFloatStack[i] );
        }
        mpConstructor->State_FillColor( color );
    }
    
    if ( mName.GetLength() > 0 )
    {
        CPDF_ObjectPtr rootObjPtr;
        CPDF_ObjectPtr objPtr;
        CPDF_ComponentPtr cmptPtr;
        CPDF_ColorSpacePtr csPtr;
        CPDF_TilingPtr tilingPtr;
        CPDF_ShadingPtr shadingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
        csPtr = mpConstructor->State_GetFillColorSpace();
        if ( IsPdfRefPtr( objPtr ) && csPtr && csPtr->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
        {
            rootObjPtr = objPtr;
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr() );
            if ( cmptPtr )
            {
                if (cmptPtr->GetType() == COMPONENT_TYPE_Tiling)
                {
                    csPtr->GetPatternPtr()->mPattern = cmptPtr;
                }else if (cmptPtr->GetType() == COMPONENT_TYPE_Shading)
                {
                    csPtr->GetPatternPtr()->mPattern = cmptPtr;
                }
            }else{
                if (objPtr->GetType() == OBJ_TYPE_REFERENCE)
                {
                    objPtr = objPtr->GetRefPtr()->GetRefObj();
                }
                if (objPtr->GetType() == OBJ_TYPE_STREAM)
                {
                    tilingPtr = CPDF_Tiling::Create(rootObjPtr, mpCmptMgr, GetAllocator() );
                    mpCmptMgr->PushComponent( rootObjPtr->GetRefPtr(), tilingPtr );
                    csPtr->GetPatternPtr()->mPattern = tilingPtr;
                }else if (objPtr->GetType() == OBJ_TYPE_DICTIONARY)
                {
                    CPDF_DictionaryPtr dict = objPtr->GetDictPtr();
                    objPtr = dict->GetElement("Shading");
                    shadingPtr = CPDF_Shading::Create(objPtr, mpCmptMgr, GetAllocator() );
                    mpCmptMgr->PushComponent( rootObjPtr->GetRefPtr(), shadingPtr );
                    csPtr->GetPatternPtr()->mPattern = shadingPtr;
                    
                    objPtr = dict->GetElement("Matrix", OBJ_TYPE_ARRAY);
                    if (objPtr)
                    {
                        CPDF_ArrayPtr arr = objPtr->GetArrayPtr();
                        CMatrix matrix;
                        arr->GetMatrix(matrix);
                        csPtr->GetPatternPtr()->mPatternMatrix = matrix;
                    }
                }
            }
        }
    }
}

void CPDF_ContentsParser::Handle_sh()
{
    if ( mName.GetLength() > 0 )
    {
        CPDF_ObjectPtr objPtr;
        CPDF_ComponentPtr cmptPtr;
        CPDF_ShadingPtr shadingPtr;
        objPtr = mpContentResMgr->GetResObj( CONTENTRES_SHADING, mName );
        if ( IsPdfRefPtr( objPtr ) )
        {
            cmptPtr = mpCmptMgr->GetComponent( objPtr->GetRefPtr(), COMPONENT_TYPE_Shading );
            if ( cmptPtr && cmptPtr->GetType() == COMPONENT_TYPE_Shading )
            {
                CPDF_ComponentRef * cmtRef = GetAllocator()->New<CPDF_ComponentRef>( mName, cmptPtr, GetAllocator() );
                mpConstructor->Operator_Append(cmtRef);
            }else{
                shadingPtr = CPDF_Shading::Create( objPtr, mpCmptMgr, GetAllocator() );
                mpCmptMgr->PushComponent( objPtr->GetRefPtr(), shadingPtr );
                CPDF_ComponentRef * cmtRef = GetAllocator()->New<CPDF_ComponentRef>( mName, shadingPtr, GetAllocator() );
                mpConstructor->Operator_Append(cmtRef);
            }
        }
    }
}

void CPDF_ContentsParser::Handle_v()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( mpPath )
        {
            CPDF_PathItem pathItem;
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

void CPDF_ContentsParser::Handle_w()
{
    if ( CheckOpdCount( 1 ) )
    {
        mpConstructor->State_LineWidth( mOpdFloatStack[0] );
    }
}

void CPDF_ContentsParser::Handle_y()
{
    if ( CheckOpdCount( 4 ) )
    {
        if ( mpPath )
        {
            CPDF_PathItem pathItem;
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

bool CPDF_ContentListBuilder::ParsePageContent( const CPDF_DictionaryPtr & pageDict, CPDF_ContentObjectList & contentList, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator /*= nullptr*/ )
{
    if ( ! pageDict )
    {
        return false;
    }
    if ( pComponentMgr == nullptr )
    {
        return false;
    }
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    CPDF_ObjectPtr		objPtr;
    CPDF_DictionaryPtr	resDictPtr;
    CPDF_StreamPtr		contentStreamPtr;
    CPDF_ArrayPtr		contentArrayPtr;
    
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
    
    CPDF_ContentListConstructor * pConstructor =  pAllocator->New<CPDF_ContentListConstructor>( &contentList, CMatrix(), pAllocator );
    CPDF_ContentsParser contentsParser( &( contentList.GetResMgr() ), pComponentMgr, pConstructor );
    
    if ( contentStreamPtr )
    {
        contentsParser.Parse( contentStreamPtr );
    }
    else if ( contentArrayPtr )
    {
        contentsParser.Parse( contentArrayPtr );
    }
    
    pConstructor->GetAllocator()->Delete( pConstructor );
    pConstructor = nullptr;
    
    return true;
}

bool CPDF_ContentListBuilder::ParseContentStream( const CPDF_StreamPtr & stmPtr, CPDF_ContentObjectList & contentList, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator /*= nullptr*/ )
{
    if ( ! stmPtr )
    {
        return false;
    }
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    CPDF_ObjectPtr objPtr;
    CPDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
    if ( ! dictPtr )
    {
        return false;
    }
    objPtr = dictPtr->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
    if ( objPtr )
    {
        contentList.GetResMgr().SetDict( objPtr->GetDictPtr() );
    }
    
    
    CMatrix matrix;
    objPtr = dictPtr->GetElement( "Matrix", OBJ_TYPE_ARRAY );
    if ( objPtr )
    {
        objPtr->GetArrayPtr()->GetMatrix( matrix );
    }
    
    CPDF_ContentListConstructor * pConstructor =  pAllocator->New<CPDF_ContentListConstructor>( &contentList, matrix, pAllocator );
    CPDF_ContentsParser contentsParser( &( contentList.GetResMgr() ), pComponentMgr, pConstructor );
    contentsParser.Parse( stmPtr );
    pConstructor->GetAllocator()->Delete( pConstructor );
    return true;
}

bool	CPDF_ContentListBuilder::ParseContentStream( const CPDF_ReferencePtr & refPtr, CPDF_ContentObjectList & contentList, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator /*= nullptr*/ )
{
    CPDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
    CPDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
    return ParseContentStream( stmPtr, contentList, pComponentMgr, pAllocator );
}

}//namespace
