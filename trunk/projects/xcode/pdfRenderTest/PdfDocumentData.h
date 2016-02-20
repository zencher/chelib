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
#import "../../../include/pdf/che_pdf_page_layout.h"
#import "../../../include/pdf/che_pdf_outline.h"


enum PDFVIEW_PAGE_MODE
{
    PAGE_MODE_SINGLE,
    PAGE_MODE_DOUBLE,
    PAGE_MODE_SINGLE_SCROLL,
    PAGE_MODE_DOUBLE_SCROLL
};

enum PDFVIEW_ZOOM_MODE
{
    ZOOM_MODE_FIX,
    ZOOM_MODE_FIT
};

enum PDFVIEW_ROTATE_MODE
{
    ROTATE_MODE_0,
    ROTATE_MODE_90,
    ROTATE_MODE_180,
    ROTATE_MODE_270
};

@interface PdfDocumentData : NSObject
{
    IHE_Read *                              fileReadInf;
    CHE_Allocator *                         allocator;
    CHE_PDF_File *                          pdfFile;
    CHE_PDF_Document *                      pdfDocument;
    CHE_PDF_PageTree *                      pdfPageTree;
    CHE_PDF_PageLayout *                    pdfPageLayout;
    HE_ULONG                                pageCount;
    CHE_PDF_OutlineItem *                   pdfoutlineRoot;
}

-(id)initWithFilePath:(NSString*)path;

-(HE_ULONG)getPageCount;

-(CHE_Rect)getPageRect:(HE_ULONG)pageIndex;

-(CHE_PDF_ContentObjectList*)getPageContent:(HE_ULONG)pageIndex;




-(void)setPageMode:(PDFVIEW_PAGE_MODE)mode;
-(PDFVIEW_PAGE_MODE)getPageMode;

-(void)setZoomMode:(PDFVIEW_ZOOM_MODE)mode;
-(PDFVIEW_ZOOM_MODE)getZoomMode;


-(void)setRotateMode:(PDFVIEW_ROTATE_MODE)mode;
-(PDFVIEW_ROTATE_MODE)getRotateMode;

-(void)setScale:(CGFloat)scale;
-(CGFloat)getScale;

-(void)setViewFrame:(CGFloat)w height:(CGFloat)h;

-(void)updateLayout;

-(void)nextPage;

-(void)prePage;

-(void)rotate;

-(CGSize)getContentSize;

-(NSRect)getPageRectInView:(HE_ULONG)pageIndex;

-(CGFloat)getPageScaleInViwe:(HE_ULONG)pageInde;

-(HE_PDF_PAGE_RANGE)getCurPageRange;

-(CHE_PDF_Outline*)getOutline;

-(CHE_PDF_OutlineItem*)getOutlineRoot;

@end
