//
//  main.cpp
//  pdfDocumentTest
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#include <iostream>
using namespace std;

#include "../../../include/pdf/che_pdf_file.h"
#include "../../../include/pdf/che_pdf_document.h"
#include "../../../include/pdf/che_pdf_contents.h"
#include "../../../include/pdf/che_pdf_contentobjs.h"
#include "../../../include/pdf/che_pdf_renderer_macosx.h"

int main(int argc, const char * argv[])
{
    IHE_Read * pFileRead = HE_CreateFileRead( "/Users/zencher/Desktop/test1.pdf" );
    if ( !pFileRead ) {
        cout << "IHE_Read object create failed!" << endl;
        return -1;
    }
    
    CHE_PDF_File pdfFile;
    if ( pdfFile.Open( pFileRead ) == FALSE )
    {
        cout << "Open file failed!" << endl;
        return -2;
    }
    
    cout << "Open file successed." << endl;
    cout << "File version " << pdfFile.GetPDFVersion() << endl;

    CHE_PDF_Document * pDocument = CHE_PDF_Document::CreateDocument( &pdfFile );
    if ( pDocument ) {
        cout << "Create document object successed." << endl;
    }else{
        cout << "Create document object failed!" << endl;
        return -3;
    }
    
    CHE_PDF_PageTree * pPageTree = pDocument->GetPageTree();
    if ( pPageTree ) {
        cout << "Parse page tree successed." << endl;
    }else{
        cout << "Parse page tree failed!" << endl;
        return -4;
    }
    
    cout << "Page count : " << pPageTree->GetPageCount() << endl;
    for( size_t i = 0; i < pPageTree->GetPageCount(); ++i )
    {
        CHE_PDF_Page * pPage = pPageTree->GetPage( i );
        if ( pPage )
        {
            CHE_Rect rect = pPage->GetMediaBox();
            cout << "page " << i << " size :" << rect.width << " X " << rect.height << endl;
        }
        
        CHE_PDF_DictionaryPtr pageDict = pPage->GetPageDict();
        CHE_PDF_ContentObjectList contentList;
        CHE_PDF_FontMgr fontMgr;
        
        GetPageContent( pageDict, &contentList, &fontMgr );
    }

    /*CHE_PDF_Page * pPage = pPageTree->GetPage( 0 );
    if ( !pPage )
    {
        cout << "Get page object failed!" << endl;
        return -5;
    }
     
    CHE_PDF_DictionaryPtr pageDict = pPage->GetPageDict();
    CHE_PDF_ContentObjectList contentList;
    CHE_PDF_FontMgr fontMgr;
    
    GetPageContent( pageDict, &contentList, &fontMgr );
    CHE_Rect rect;
    rect.left = 0;
    rect.bottom = 0;
    rect.width = 600;
    rect.height = 600;
    
    CHE_GraphicsDrawer drawer(600*96/72.0, 600*96/72.0);
    
    CHE_PDF_Renderer::Render( contentList, drawer, rect );*/
    
    return 0;
}


