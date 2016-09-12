//
//  PdfDocumentData.h
//  chelib
//
//  Created by Frank Zeng on 13-7-25.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "../../../include/pdf/pdf_file.h"
#import "../../../include/pdf/pdf_document.h"
#import "../../../include/pdf/pdf_contentlistbuilder.h"
#import "../../../include/pdf/pdf_componentmgr.h"
#import "../../../include/pdf/pdf_contentobjs.h"
#import "../../../include/pdf/pdf_renderer_macosx.h"
#import "../../../include/pdf/pdf_page_layout.h"
#import "../../../include/pdf/pdf_outline.h"

using namespace chelib;

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
    IRead *                              fileReadInf;
    CAllocator *                         allocator;
    CPDF_File *                          pdfFile;
    CPDF_Document *                      pdfDocument;
    CPDF_PageTree *                      pdfPageTree;
    CPDF_PageLayout *                    pdfPageLayout;
    size_t                               pageCount;
    CPDF_OutlineItem *                   pdfoutlineRoot;
}

-(id)initWithFilePath:(NSString*)path;

-(size_t)getPageCount;

-(CRect)getPageRect:(size_t)pageIndex;

-(CPDF_ContentObjectList*)getPageContent:(size_t)pageIndex;




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

-(NSRect)getPageRectInView:(size_t)pageIndex;

-(CGFloat)getPageScaleInViwe:(size_t)pageInde;

-(PDF_PAGE_RANGE)getCurPageRange;

-(CPDF_Outline*)getOutline;

-(CPDF_OutlineItem*)getOutlineRoot;

@end
