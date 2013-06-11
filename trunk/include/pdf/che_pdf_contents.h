#ifndef _CHE_PDF_CONTENTS_H_
#define _CHE_PDF_CONTENTS_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_gstate.h"
#include "che_pdf_contentobjs.h"
#include "che_pdf_fontmgr.h"

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

	HE_VOID State_ExtGState( const CHE_ByteString & resName, CHE_PDF_DictionaryPtr dictPtr )
	{
		GetGState()->PushExtGState( resName, dictPtr );
	}

	HE_VOID State_FillColor( CHE_PDF_Color * pColor )
	{
		GetGState()->SetFillColor( pColor );
	}

	HE_VOID State_StrokeColor( CHE_PDF_Color * pColor )
	{
		GetGState()->SetStrokeColor( pColor );
	}

	HE_VOID State_FillColorSpace( CHE_PDF_ColorSpace * pColorSpace )
	{
		GetGState()->SetFillColorSpace( pColorSpace );
	}

	HE_VOID State_StrokeColorSpace( CHE_PDF_ColorSpace * pColorSpace )
	{
		GetGState()->SetStrokeColorSpace( pColorSpace );
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
			CHE_PDF_GState * pGState = mpGState->Clone();
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
					GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
					mpGState->GetTextRenderMode( rm );
					switch ( rm )
					{
					case TextRenderMode_FillClip:
					case TextRenderMode_StrokeClip:
					case TextRenderMode_FillStrokeClip:
					case TextRenderMode_Clip:
						{
							Operator_Clip( pObject );
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
			case ContentType_RefImage:
			case ContentType_InlineImage:
			case ContentType_Form:
			case ContentType_Shading:
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

			CHE_Matrix tmpMatrix = pObject->GetExtMatrix();
			tmpMatrix.Concat( mExtMatrix );
			pObject->SetExtMatrix( tmpMatrix );
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
};

HE_BOOL GetPageContent( CHE_PDF_DictionaryPtr & pageDict, CHE_PDF_ContentObjectList * pList, CHE_PDF_FontMgr * pFontMgr, CHE_Allocator * pAllocator = NULL );

class CHE_PDF_ContentsParser : public CHE_Object
{
public:
	CHE_PDF_ContentsParser( CHE_PDF_ContentResMgr * pResMgr, CHE_PDF_FontMgr * pFontMgr, 
							CHE_PDF_ContentListConstructor * pConstructor, CHE_Allocator * pAllocator = NULL )
		:	CHE_Object( pAllocator ), mpContentResMgr(pResMgr), mpFontMgr(pFontMgr), mpConstructor(pConstructor), 
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

	CHE_PDF_FontMgr *		mpFontMgr;
	CHE_PDF_ContentResMgr * mpContentResMgr;
	CHE_PDF_ContentListConstructor * mpConstructor;
};


#endif