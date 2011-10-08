#ifndef _CHE_PDF_CONTENTS_H_
#define _CHE_PDF_CONTENTS_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"
#include "che_pdf_pageobjs.h"
#include "che_pdf_parser.h"

class CHE_PDF_ContentsParser : CHE_Object
{
public:
	CHE_PDF_ContentsParser( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	HE_VOID Parse( const CHE_PDF_Stream * pContents, const CHE_PDF_Dictionary * pResources, std::vector<CHE_PDF_ContentObject*> & vectorRet );

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

	CHE_PDF_Dictionary*	m_pFontDict;
	CHE_PDF_Dictionary* m_pExtGStateDict;
	CHE_Stack<CHE_PDF_Object*> m_OpdStack;

	//Path
	HE_FLOAT			m_fPathBeginX;
	HE_FLOAT			m_fPathBeginY;
	HE_FLOAT			m_fPathCurX;
	HE_FLOAT			m_fPathCurY;
	HE_BOOL				m_bPathConnect;
	HE_BOOL				m_bSubPathClosed;
	HE_BOOL				m_bClipPath;
	PDF_FILL_MODE		m_ClipFillMode;
	CHE_Queue<CHE_GraphicsObject*> m_SupPathQueue;

	//Text
	HE_FLOAT			m_fPosiX;
	HE_FLOAT			m_fPosiY;
	HE_FLOAT			m_fCharSpace;
	HE_FLOAT			m_fWordSpace;
	HE_DWORD			m_dwScale;
	HE_FLOAT			m_fLeading;
	HE_DWORD			m_dwSize;
	HE_BYTE				m_byteRender;
	HE_DWORD			m_dwRise;
	HE_BOOL				m_bKnockout;
	HE_FLOAT			m_fMatrixA;
	HE_FLOAT			m_fMatrixB;
	HE_FLOAT			m_fMatrixC;
	HE_FLOAT			m_fMatrixD;
	HE_FLOAT			m_fMatrixE;
	HE_FLOAT			m_fMatrixF;
	HE_DWORD			m_dwFontObjNum;
};


#endif