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
#include "../../../include/pdf/che_pdf_contentlist.h"
#include "../../../include/pdf/che_pdf_contentobject.h"
#include "../../../include/pdf/che_pdf_renderer_macosx.h"

using namespace chelib;

int main(int argc, const char * argv[])
{
    IRead * pFileRead = CreateFileIRead( "/Users/zencher/Desktop/test/test11.pdf" );
    if ( !pFileRead ) {
        cout << "IHE_Read object create failed!" << endl;
        return -1;
    }
    
    CPDF_File pdfFile;
    if ( pdfFile.Open( pFileRead ) == FALSE )
    {
        cout << "Open file failed!" << endl;
        return -2;
    }
    
    cout << "Open file successed." << endl;
    cout << "File version " << pdfFile.GetPDFVersion() << endl;

    CPDF_Document * pDocument = CPDF_Document::CreateDocument( &pdfFile );
    if ( pDocument ) {
        cout << "Create document object successed." << endl;
    }else{
        cout << "Create document object failed!" << endl;
        return -3;
    }
    
    CPDF_PageTree * pPageTree = pDocument->GetPageTree();
    if ( pPageTree ) {
        cout << "Parse page tree successed." << endl;
    }else{
        cout << "Parse page tree failed!" << endl;
        return -4;
    }
    
    /*CPDF_FontMgr fontMgr;
    CGraphicsDrawer drawer( 1, 1 );
    char pathStr[1024];
    
    cout << "Page count : " << pPageTree->GetPageCount() << endl;
    for( size_t i = 0; i < pPageTree->GetPageCount(); ++i )
    {
        CPDF_Page * pPage = pPageTree->GetPage( i );
        if ( pPage )
        {
            CRect rect = pPage->GetMediaBox();
            cout << "page " << i << " size :" << rect.width << " X " << rect.height << endl;
        }
        
        CPDF_DictionaryPtr pageDict = pPage->GetPageDict();
        CPDF_ContentObjectList contentList;
        
        GetPageContent( pageDict, &contentList, &fontMgr );
        
        CRect rect = pPage->GetMediaBox();
        CPDF_Renderer::Render( contentList, drawer, rect );
        
        sprintf( pathStr , "/Users/zencher/page%03d.png", i+1 );
        drawer.SaveToFile( pathStr );
    }*/
    
    return 0;
}


