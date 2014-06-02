//
//  CHEPDFImpView.h
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"

enum PDFVIEW_PAGE_MODE
{
    PAGE_MODE_SINGLE,
    PAGE_MODE_DOUBLE,
    PAGE_MODE_SINGLE_SCROLL,
    PAGE_MODE_DOUBLE_SCROLL
};

enum PDFVIEW_ZOOM_MODE
{
    ZOOM_MODE_ACTUAL,
    ZOOM_MODE_FIX_SCALE,
    ZOOM_MODE_FIT_WIDTH,
    ZOOM_MODE_FIT_HEIGHT
};

enum PDFVIEW_ROTATE_MODE
{
    ROTATE_MODE_0,
    ROTATE_MODE_90,
    ROTATE_MODE_180,
    ROTATE_MODE_270
};

@interface CHEPDFMainView : NSView
{
    id                  parentScrollView;
    PdfDocumentData *   pdfDocument;
    
    CGFloat             allPageWidth;
    CGFloat             allPageHeight;
    CGFloat             biggestPageWidth;
    CGFloat             smallestPageWidth;
    CGFloat             biggestPageHeight;
    CGFloat             smallestPageHeight;
}

@property CGFloat               pageSpaceX;
@property CGFloat               pageSpaceY;
@property CGFloat               scale;
@property PDFVIEW_PAGE_MODE     pageMode;
@property PDFVIEW_ZOOM_MODE     zoomMode;
@property PDFVIEW_ROTATE_MODE   rotateMode;

-(void)setParentScrollView:(id)scrollView;

-(void)parentScrollViewFrameChanged;

-(void)newDocument:(PdfDocumentData*)doc;

-(void)drawPages:(NSRect)rect;

-(void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect;

-(void)closeDocument;

-(void)parseAllPageSizeInfo;

//获取视图内所有页面内容的大小（根据页面数量，页面旋转和缩放等），决定ScrollView的滚动范围
-(CGSize)getViewContentSize;

-(CGRect)getAllPagesRectInView;

@end
