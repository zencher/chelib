//
//  CHEPDFThumbnailView.h
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"

@interface CPDFThumbnailMainView : NSView
{
    id                  parentScrollView;
    PdfDocumentData *   pdfDocData;
    NSSize              oldContentSize;
}

-(id)initWithFrame:(NSRect)frame
        parentView:(id)parent;

-(void)parentScrollViewFrameChanged;

-(void)setDocumentData:(PdfDocumentData*)doc;

-(void)drawPages:(NSRect)rect;

-(void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect;

@end



@interface CPDFThumbnailScrollView : NSScrollView
{
    CPDFThumbnailMainView * mainView;
    id parentView;
}

-(id)initWithFrame:(NSRect)frame
        parentView:(id)view;

-(void)parentViewFrameChanged;

-(void)setDocumentData:(PdfDocumentData*)doc;

@end



@interface CPDFThumbnailView : NSVisualEffectView
{
    CPDFThumbnailScrollView * scrollView;
}

-(void)setDocumentData:(PdfDocumentData*)doc;

@end
