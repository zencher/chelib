#ifndef _CHE_PDF_FONT_MGR_H_
#define _CHE_PDF_FONT_MGR_H_

#include "../che_base.h"
#include "che_pdf_font.h"

class CHE_PDF_FontMgr : public CHE_Object
{
public:

	CHE_PDF_FontMgr(  CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator) {};
	
	~CHE_PDF_FontMgr();

	CHE_PDF_Font * LoadFont( const CHE_PDF_ReferencePtr & pReference );

private:

	CHE_NumToPtrMap mNumToFontMap;
};



//������һ���ӿ��࣬��ƽ̨���
//����ʵ�ֻ�ȡ����·�����������ļ����ݵĽӿ�
//����һЩ��Ҫ����Ϣ����ȡƥ����������Ĭ������Ľӿ�

class IHE_SystemFontMgr : public CHE_Object
{
public:
	static IHE_SystemFontMgr * Create( CHE_Allocator * pAllocator = NULL );
	static HE_VOID Destroy( IHE_SystemFontMgr * pSystemFontMgr );

	IHE_SystemFontMgr( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};

	virtual ~IHE_SystemFontMgr() {};

	virtual CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName ) = 0;
};

#endif
