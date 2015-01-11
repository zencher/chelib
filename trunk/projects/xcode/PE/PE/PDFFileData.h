//
//  PDFFileData.h
//  chelib
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "../../../../include/pdf/che_pdf_file.h"
#import "../../../../include/pdf/che_pdf_document.h"
#import "../../../../include/pdf/che_pdf_contentlistbuilder.h"
#import "../../../../include/pdf/che_pdf_componentmgr.h"
#import "../../../../include/pdf/che_pdf_contentobjs.h"
#import "../../../../include/pdf/che_pdf_renderer_macosx.h"

@interface PDFFileData : NSObject
{
    IHE_Read *                              fileReadInf;
    CHE_Allocator *                         allocator;
    CHE_PDF_File *                          pdfFile;
    CHE_PDF_Document *                      pdfDocument;
    CHE_PDF_PageTree *                      pdfPageTree;
    CHE_PDF_ComponentMgr *                  pdfComponentMgr;
    
    HE_ULONG                                pageCount;
}

-(id)initWithReadInf:(IHE_Read*)read;

-(id)initWithFilePath:(NSString*)path;

-(CHE_PDF_ContentObjectList*)getPageContent:(unsigned int)pageIndex;

-(HE_ULONG)getPageCount;

-(CHE_Rect)getPageRect:(unsigned int)pageIndex;

@end
