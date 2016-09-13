//
//  annotation.m
//  chelib
//
//  Created by zencher on 16/5/29.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "che_pdf_file.h"
#include "che_pdf_document.h"

using namespace chelib;

@interface annotation : XCTestCase

@end

@implementation annotation

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)createAnnot {
    
    char const * pFilePath = "pdffilepath";
    char tmp[1024];
    IRead * pfile = CreateFileIRead( pFilePath );
    if ( pfile )
    {
        CPDF_File file;
        if ( file.Open( pfile ) )
        {
            CPDF_Document * pdoc = CPDF_Document::CreateDocument(&file);
            CPDF_Page * pPage = pdoc->GetPage(0);
            CPDF_DictionaryPtr pageDict = pPage->GetPageDict();
            CPDF_DictionaryPtr highligthDict;
            CPDF_DictionaryPtr popupDict;
            PDF_RefInfo refinfo = file.CreateDictObject(highligthDict);
            PDF_RefInfo popupRef = file.CreateDictObject(popupDict);
            
            CPDF_ArrayPtr arrayPtr = pageDict->SetArray("Annots");
            CPDF_ReferencePtr refPtr = arrayPtr->AppendReference(&file);
            refPtr->SetRefInfo(refinfo);
            
            highligthDict->SetName("Type", "Annot");
            highligthDict->SetName("Subtype", "Highlight");
            arrayPtr = highligthDict->SetArray("Rect");
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(110);
            arrayPtr->AppendNumber(110);
            
            arrayPtr = highligthDict->SetArray("QuadPoints");
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(110);
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(10);
            arrayPtr->AppendNumber(110);
            arrayPtr->AppendNumber(110);
            arrayPtr->AppendNumber(110);
            
            arrayPtr = highligthDict->SetArray("C");
            arrayPtr->AppendNumber(1);
            arrayPtr->AppendNumber(1);
            arrayPtr->AppendNumber(0);
            
            highligthDict->SetInteger("CA", 1);
            
            highligthDict->SetReference("P", 30, 0, &file);
            
            highligthDict->SetString("NM", "dsfasdfasdfasdfasdfasdfadasd");
            
            highligthDict->SetInteger("F", 4);
            
            highligthDict->SetReference("Popup", popupRef.objNum, popupRef.genNum, &file);
            
            popupDict->SetName("Type", "Annot");
            popupDict->SetName("Subtype", "Popup");
            arrayPtr = popupDict->SetArray("Rect");
            arrayPtr->AppendNumber(200);
            arrayPtr->AppendNumber(200);
            arrayPtr->AppendNumber(400);
            arrayPtr->AppendNumber(400);
            
            popupDict->SetBoolean("Open", FALSE);
            
            popupDict->SetReference("Parent", refinfo.objNum, refinfo.genNum, &file);
            
            sprintf( tmp, "%s.uncompressd.pdf", pFilePath);
            IWrite * pWrite = CreateFileIWrite(tmp);
            if ( pWrite )
            {
                file.Save( pWrite, FALSE );
            }
        }
    }
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
