//
//  PdfDocumentData.h
//  chelib
//
//  Created by Frank Zeng on 13-7-25.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "../../../include/pdf/che_pdf_file.h"
#import "../../../include/pdf/che_pdf_document.h"
#import "../../../include/pdf/che_pdf_contentlistbuilder.h"
#import "../../../include/pdf/che_pdf_componentmgr.h"
#import "../../../include/pdf/che_pdf_contentobjs.h"
#import "../../../include/pdf/che_pdf_renderer_macosx.h"

@interface PdfDocumentData : NSObject
{
    IHE_Read *                              fileReadInf;
    CHE_Allocator *                         allocator;
    CHE_PDF_File *                          pdfFile;
    CHE_PDF_Document *                      pdfDocument;
    CHE_PDF_PageTree *                      pdfPageTree;
    CHE_PDF_ComponentMgr *                  pdfComponentMgr;
    
    HE_ULONG                                pageCount;
    std::vector<CHE_PDF_ContentObjectList*> pageContents;
}

-(id)initWithFilePath:(NSString*)path;


-(CHE_PDF_ContentObjectList*)getPageContent:(unsigned int)pageIndex;

@end
