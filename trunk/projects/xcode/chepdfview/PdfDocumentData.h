//
//  PdfDocumentData.h
//  chelib
//
//  Created by Frank Zeng on 13-7-25.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "che_pdf_file.h"
#import "che_pdf_document.h"
#import "che_pdf_contentlistbuilder.h"
#import "che_pdf_componentmgr.h"
#import "che_pdf_contentobject.h"
#import "che_pdf_renderer_macosx.h"
#import "che_pdf_offscreen_renderer_macosx.h"
#import "che_pdf_page_layout.h"
#import "che_pdf_outline.h"

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

struct RENDER_INFO
{
    size_t pageIndex;
    uint32 rotate;
    FLOAT scale;
    FLOAT dpix;
    FLOAT dpiy;
    CPDF_Offscreen_Renderer * render;
};

@interface PdfDocumentData : NSObject
{
    IRead *                              fileReadInf;
    CAllocator *                         allocator;
    CPDF_File *                          pdfFile;
    CPDF_Document *                      pdfDocument;
    CPDF_PageTree *                      pdfPageTree;
    CPDF_PageLayout *                    pdfPageLayout;
    CPDF_ThumbnailPageLayout *           pdfThumbnailLayout;
    size_t                               pageCount;
    CPDF_OutlineItem *                   pdfoutlineRoot;
    std::vector<RENDER_INFO*>            renders;
}

-(id)initWithNSData:(NSData*)data;

-(id)initWithFilePath:(NSString*)path;

-(size_t)getPageCount;

-(int32)getPageRotate:(size_t)pageIndex;

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


//pdf page layout for thumbnail view
-(void)setThumbnailViewFrame:(CGFloat)w height:(CGFloat)h;

-(void)updateThumbnailLayout;

-(CGSize)getThumbnailContentSize;

-(NSRect)getPageRectInThumbnailView:(size_t)pageIndex;

-(CGFloat)getPageScaleInThumbnailView:(size_t)pageInde;

//render page

-(CGLayerRef)renderPage:(size_t)pageIndex
                  cgctx:(CGContextRef)ctx
                   dpix:(FLOAT)dpix
                   dpiy:(FLOAT)dpiy;

-(CPDF_Offscreen_Renderer*)requestRenderForPage:(size_t)pageIndex
                                          cgctx:(CGContextRef)ctx
                                       pageRect:(CRect)pageRect
                                          scale:(FLOAT)scale
                                         rotate:(uint32)rotate
                                           dpix:(FLOAT)dpix
                                           dpiy:(FLOAT)dpiy;




@end
