#include "../pdftext/pdftext.h"
#include <stdio.h>
#include <windows.h>

int main( int argc, char * argv[] )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}

	PDFDocument doc = CHEPDF_OpenDocument( argv[1] ); //������Ի��ɴ򿪵��ļ�·����Ҳ�����ڹ��̵ĵ�������������
	if ( doc )
	{

// 		int iPageCount = CHEPDF_GetPageCount( doc );
// 		char tmpStr[1024];
// 		for ( int i = 0; i < iPageCount; ++i )
// 		{
// 			printf( "Rendering Page : %d\n", i+1 );
// 			PDFPage page = CHEPDF_GetPage( doc, i ); //�򿪵�һҳ
// 			PDFBitmap bitmap = CHEPDF_RenderPage( page, 3 );
// 			if ( bitmap )
// 			{
// 				sprintf_s( tmpStr, "%s_%03d.bmp", argv[1], i );
// 				CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
// 				CHEPDF_CloseBitmap( bitmap );
// 			}
// 			CHEPDF_ClosePage( page );
// 		}
// 		CHEPDF_CloseDocument( doc );

// 		int iPageCount = CHEPDF_GetPageCount( doc );
// 		int index = 0;
// 		char tmpStr[1024];
// 		for ( int i = 1; i < 2; ++i )
// 		{
// 			PDFPage page = CHEPDF_GetPage( doc, i );
// 			PDFPageContent content = CHEPDF_GetPageContent( page ); //��ȡҳ�������
// 			if ( content )
// 			{
// 				int index = 0;
// 				char tmpStr[1024];
// 				PDFPageText text = CHEPDF_GetFirstPageText( content );
// 				while ( text )
// 				{
// 					PDFBitmap bitmap = CHEPDF_RenderText( text ); //��10����ԭʼ��С��Ⱦ�ʵ�λͼ��
// 					if ( bitmap )
// 					{
// 						sprintf_s( tmpStr, "d:\\text%03d.bmp", index );
// 						CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
// 						CHEPDF_CloseBitmap( bitmap );
// 					}
// 					++index;
// 					text = CHEPDF_GetNextPageText( content );
// 				}
// 			}
// 			CHEPDF_ReleasePageContent( content );
// 			CHEPDF_ClosePage( page );
// 		}


		PDFPage page = CHEPDF_GetPage( doc, 34 );
		PDFPageContent content = CHEPDF_GetPageContent( page ); //��ȡҳ�������
		PDFPageWordSet wordset = CHEPDF_GetPageWordSet( content, 2.5 ); //��ҳ�����ݽ����Ű����У���������ʱ�����wordset��
		if ( wordset )
		{
			PDFPageWord tmpWord = CHEPDF_GetFirstPageWord( wordset ); //��ȡ��һ����
			int index = 0;
			char tmpStr[1024];
			WCHAR unicodeText[1024];
			unsigned int uLength = 0;
					
			while ( tmpWord )
			{	
				printf( "text %d\n", index );

				PDFRect rect = CHEPDF_GetWordBox( tmpWord );	//��ȡ�ʵĴ�С��Χ
				printf( "text rect = left : %.2f, bottom : %.2f, width : %.2f, height : %.2f\n", rect.left, rect.bottom, rect.width, rect.height );

				if ( CHEPDF_IsWordSymbolic( tmpWord ) == PDF_WORD_NOTSYMBOLIC )	//�жϸô��ǲ��Ƿ���
				{
					//�ôʲ��Ƿ��ţ���Ҫ����ı�����
					uLength = CHEPDF_GetWordLength( tmpWord );
					if ( uLength > 0 )
					{
						printf( "text length %d\n", uLength );
						CHEPDF_GetWordUnicodes( tmpWord, unicodeText, 1024 ); //��ô��е�unicode������ı�����
						WideCharToMultiByte( CP_ACP, 0, unicodeText, -1, tmpStr, 1024, NULL, NULL );
						printf( "text : %s\n", tmpStr );
					}
				}

				PDFBitmap bitmap = CHEPDF_RenderWord( tmpWord, 3.0 ); //��10����ԭʼ��С��Ⱦ�ʵ�λͼ��
				if ( bitmap )
				{
					sprintf_s( tmpStr, "d:\\text%03d.bmp", index );
					CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
					CHEPDF_CloseBitmap( bitmap );
				}
				++index;
				tmpWord = CHEPDF_GetNextPageWord( wordset ); //��wordset�л����һ����
			}
		}



	}

	return 0;
}

