//
//  CHEPDFThumbnailView.h
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"

@interface CHEPDFThumbnailMainView : NSView
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

@interface CHEPDFThumbnailView : NSScrollView
{
    CHEPDFThumbnailMainView * mainView;
}

-(void)setDocumentData:(PdfDocumentData*)doc;

@end
