#ifndef _CHE_PDF_CONTENTS_H_
#define _CHE_PDF_CONTENTS_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_matrix.h"
#include "che_pdf_graphicsState.h"
#include "che_pdf_contentobjs.h"

class IHE_PDF_ContentListConstructor : public CHE_Object
{
public:
	IHE_PDF_ContentListConstructor( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}
	virtual ~IHE_PDF_ContentListConstructor() {};

	virtual void SetMatrix( const CHE_PDF_Matrix & matrix ) = 0;
	virtual void SetLineWidth( float lineWidth ) = 0;
	virtual void SetLineCap( PDF_GSTATE_LINECAP lineCap ) = 0;
	virtual void SetLineJion( PDF_GSTATE_LINEJOIN lineJion ) = 0;
	virtual void SetMiterLimit( float miterLimit ) = 0;
	virtual void SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern ) = 0;
	virtual void SetRenderingIntents( PDF_GSTATE_RENDERINTENTS ri ) = 0;
	virtual void SetFlatness( unsigned char flatness ) = 0;
	virtual void SetExtGState( std::string name, const CHE_PDF_Dictionary * pDict ) = 0;
	virtual void SetStrokeColor() = 0;
	virtual void SetFillColor() = 0;
	virtual void SetStrokeColorSpace() = 0;
	virtual void SetFillColorSpace() = 0;
	virtual void SetTextFont() = 0;
	virtual void SetTextFontSize( float size ) = 0;
	virtual void SetTextCharSpace( float charspace ) = 0;
	virtual void SetTextWordSpace( float wordspace ) = 0;
	virtual void SetTextHScaling( float scaling ) = 0;
	virtual void SetTextLeading( float leading ) = 0;
	virtual void SetTextRise( float rise ) = 0;
	virtual void SetTextMatirx( const CHE_PDF_Matrix & matrix ) = 0;
	virtual void SetTextRenderMode( PDF_GSTATE_TEXTRENDERMODE rm ) = 0;

	virtual void PushGState() = 0;
	virtual void PopGState() = 0;

	virtual void AddClip() = 0;

	virtual void Append( CHE_PDF_ContentObject * pObject ) = 0;

	virtual void Over() = 0;
};

IHE_PDF_ContentListConstructor * CreateConstructor( std::vector<CHE_PDF_ContentObject*> * pVector, CHE_Allocator * pAllocator );

class CHE_PDF_ContentResMgr : public CHE_Object
{

};

class CHE_PDF_ContentsParser : public CHE_Object
{
public:
	CHE_PDF_ContentsParser( CHE_Allocator * pAllocator = NULL )
		:	mpConstructor(NULL), mpPath(NULL), mCurX(0), mCurY(0),
			mString(pAllocator), mName(pAllocator), mpObj(NULL),
			CHE_Object( pAllocator ) {}

	~CHE_PDF_ContentsParser()
	{
		if ( mpObj )
		{
			mpObj->Release();
		}
		mOpdFloatStack.clear();
		if ( mpPath )
		{ 
			GetAllocator()->Delete<CHE_PDF_Path>( mpPath );
		}
	}

	HE_VOID Parse( const CHE_PDF_Stream * pContents, const CHE_PDF_Dictionary * pResources, IHE_PDF_ContentListConstructor * pConstructor );

private:
	HE_VOID Handle_dquote();
	HE_VOID Handle_squote();
	HE_VOID Handle_B();
	HE_VOID Handle_Bstar();
	HE_VOID Handle_BDC();
	HE_VOID Handle_BI();
	HE_VOID Handle_BMC();
	HE_VOID Handle_BT();
	HE_VOID Handle_BX();
	HE_VOID Handle_CS();
	HE_VOID Handle_DP();
	HE_VOID Handle_Do();
	HE_VOID Handle_EMC();
	HE_VOID Handle_ET();
	HE_VOID Handle_EX();
	HE_VOID Handle_F();
	HE_VOID Handle_G();
	HE_VOID Handle_J();
	HE_VOID Handle_K();
	HE_VOID Handle_M();
	HE_VOID Handle_MP();
	HE_VOID Handle_Q();
	HE_VOID Handle_RG();
	HE_VOID Handle_S();
	HE_VOID Handle_SC();
	HE_VOID Handle_SCN();
	HE_VOID Handle_Tstar();
	HE_VOID Handle_TD();
	HE_VOID Handle_TJ();
	HE_VOID Handle_TL();
	HE_VOID Handle_Tc();
	HE_VOID Handle_Td();
	HE_VOID Handle_Tf();
	HE_VOID Handle_Tj();
	HE_VOID Handle_Tm();
	HE_VOID Handle_Tr();
	HE_VOID Handle_Ts();
	HE_VOID Handle_Tw();
	HE_VOID Handle_Tz();
	HE_VOID Handle_W();
	HE_VOID Handle_Wstar();
	HE_VOID Handle_b();
	HE_VOID Handle_bstar();
	HE_VOID Handle_c();
	HE_VOID Handle_cm();
	HE_VOID Handle_cs();
	HE_VOID Handle_d();
	HE_VOID Handle_d0();
	HE_VOID Handle_d1();
	HE_VOID Handle_f();
	HE_VOID Handle_fstar();
	HE_VOID Handle_g();
	HE_VOID Handle_gs();
	HE_VOID Handle_h();
	HE_VOID Handle_i();
	HE_VOID Handle_j();
	HE_VOID Handle_k();
	HE_VOID Handle_l();
	HE_VOID Handle_m();
	HE_VOID Handle_n();
	HE_VOID Handle_q();
	HE_VOID Handle_re();
	HE_VOID Handle_rg();
	HE_VOID Handle_ri();
	HE_VOID Handle_s();
	HE_VOID Handle_sc();
	HE_VOID Handle_scn();
	HE_VOID Handle_sh();
	HE_VOID Handle_v();
	HE_VOID Handle_w();
	HE_VOID Handle_y();

	//CHE_PDF_ContentResMgr * mpResMgr;
	std::vector<float>	mOpdFloatStack;
	CHE_ByteString		mName;
	CHE_ByteString		mString;
	CHE_PDF_Object *	mpObj;

	CHE_PDF_Path *		mpPath;
	float				mCurX;
	float				mCurY;

	IHE_PDF_ContentListConstructor * mpConstructor;
};


#endif