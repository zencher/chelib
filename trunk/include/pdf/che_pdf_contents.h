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
	virtual HE_VOID Init() = 0;
	virtual HE_VOID Over() = 0;
	virtual HE_VOID State_Matrix( const CHE_PDF_Matrix & matrix ) = 0;
	virtual HE_VOID State_ConcatMatrix( const CHE_PDF_Matrix & matrix ) = 0;
	virtual HE_VOID State_LineWidth( const HE_FLOAT & lineWidth ) = 0;
	virtual HE_VOID State_LineCap( const PDF_GSTATE_LINECAP & lineCap ) = 0;
	virtual HE_VOID State_LineJoin( const PDF_GSTATE_LINEJOIN & lineJoin ) = 0;
	virtual HE_VOID State_MiterLimit( const HE_FLOAT & miterLimit ) = 0;
	virtual HE_VOID State_LineDash( const PDF_GSTATE_DASHPATTERN & dashPattern ) = 0;
	virtual HE_VOID State_RenderIntents( const PDF_GSTATE_RENDERINTENTS & ri ) = 0;
	virtual HE_VOID State_Flatness( const HE_FLOAT & flatness ) = 0;
	virtual HE_VOID State_ExtGState( const CHE_ByteString & resName, CHE_PDF_Dictionary * pDict ) = 0;
	virtual HE_VOID State_FillColor( CHE_PDF_Color * pColor ) = 0;
	virtual HE_VOID State_StrokeColor( CHE_PDF_Color * pColor ) = 0;
	virtual HE_VOID State_FillColorSpace( CHE_PDF_ColorSpace * pColorSpace ) = 0;
	virtual HE_VOID State_StrokeColorSpace( CHE_PDF_ColorSpace * pColorSpace ) = 0;
	virtual HE_VOID State_TextMatirx( const CHE_PDF_Matrix & matrix ) = 0;
	virtual HE_VOID State_TextFont( const CHE_ByteString & resName /*todo*/ ) = 0;
	virtual HE_VOID State_TextFontSize( const HE_FLOAT & size ) = 0;
	virtual HE_VOID State_TextCharSpace( const HE_FLOAT & charSpace ) = 0;
	virtual HE_VOID State_TextWordSpace( const HE_FLOAT & wordSpace ) = 0;
	virtual HE_VOID State_TextScaling( const HE_FLOAT & scaling ) = 0;
	virtual HE_VOID State_TextLeading( const HE_FLOAT & leading ) = 0;
	virtual HE_VOID State_TextRise( const HE_FLOAT & rise ) = 0;
	virtual HE_VOID State_TextRenderMode( const PDF_GSTATE_TEXTRENDERMODE rm ) = 0;
	virtual HE_VOID Operator_Td( const HE_FLOAT & tx, const HE_FLOAT & ty ) = 0;
	virtual HE_VOID Operator_TD( const HE_FLOAT & tx, const HE_FLOAT & ty ) = 0;
	virtual HE_VOID Operator_Tstar() = 0;
	virtual HE_VOID Operator_PushGState() = 0;
	virtual HE_VOID Operator_PopGState() = 0;
	virtual HE_VOID Operator_Clip( CHE_PDF_ContentObject * pObject ) = 0;
	virtual HE_VOID Operator_Append( CHE_PDF_ContentObject * pObject ) = 0;
};

IHE_PDF_ContentListConstructor * CreateConstructor( std::vector<CHE_PDF_ContentObject*> * pVector, CHE_Allocator * pAllocator = NULL );

enum PDF_CONTENTRES_TYPE
{
	CONTENTRES_EXTGSTATE = 0,
	CONTENTRES_COLORSPACE = 1,
	CONTENTRES_PATTERN = 2,
	CONTENTRES_SHADING = 3,
	CONTENTRES_XOBJECT = 4,
	CONTENTRES_FONT = 5
};

class CHE_PDF_ContentResMgr : public CHE_Object
{
public:
	CHE_PDF_ContentResMgr( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( CHE_PDF_Dictionary::Create(pAllocator) ) {}
	CHE_PDF_ContentResMgr( CHE_PDF_Dictionary * pResDict, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( pResDict ? pResDict->Clone() : CHE_PDF_Dictionary::Create(pAllocator) ) {}

	~CHE_PDF_ContentResMgr() {}

	HE_VOID SetDict( CHE_PDF_Dictionary * pDict ) { pDict ? ( mpResDict = pDict->Clone() ) : ( mpResDict = CHE_PDF_Dictionary::Create(GetAllocator()) ); }
	CHE_PDF_Dictionary * GetDict() { return mpResDict; }

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, CHE_PDF_Object * pObj );

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name, CHE_Object * pObj );

	HE_BOOL	DeleteName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_Object * GetResObj( PDF_CONTENTOBJ_TYPE type, const CHE_ByteString & name );

	CHE_Object * GetResObj( const CHE_ByteString & name );

private:
	CHE_PDF_Dictionary * GetSubDict( PDF_CONTENTRES_TYPE type );

	CHE_PDF_Dictionary * CreateSubDict( PDF_CONTENTRES_TYPE type );

	CHE_ByteString RequestName( CHE_PDF_Dictionary * pSubDict, const CHE_ByteString & name );

	CHE_PDF_Dictionary * mpResDict;
};

class CHE_PDF_ContentsParser : public CHE_Object
{
public:
	CHE_PDF_ContentsParser( CHE_PDF_ContentResMgr * pResMgr, IHE_PDF_ContentListConstructor * pConstructor, CHE_Allocator * pAllocator = NULL )
		:	mpContentResMgr(pResMgr), mpConstructor(pConstructor), mpPath(NULL), mCurX(0), mCurY(0),
			mString(pAllocator), mName(pAllocator), mpObj(NULL), CHE_Object( pAllocator ) {}

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

	HE_BOOL Parse( const CHE_PDF_Stream * pContentStream );

	HE_BOOL Parse( const CHE_PDF_Array * pContentArray );

private:
	HE_VOID ParseImp( CHE_DynBuffer * pStream );

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

	HE_BOOL CheckOpdCount( size_t count );

	//CHE_PDF_ContentResMgr * mpResMgr;
	std::vector<HE_FLOAT>	mOpdFloatStack;
	CHE_ByteString			mName;
	CHE_ByteString			mString;
	CHE_PDF_Object *		mpObj;

	CHE_PDF_Path *			mpPath;
	HE_FLOAT				mCurX;
	HE_FLOAT				mCurY;

	CHE_PDF_ContentResMgr * mpContentResMgr;
	IHE_PDF_ContentListConstructor * mpConstructor;
};


#endif