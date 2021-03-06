//
//  CHEPDFPageView.h
//  chelib
//
//  Created by zencher on 16/2/19.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"

@interface CPDFPageMainView : NSView
{
    id                  parentScrollView;
    PdfDocumentData *   pdfDocData;
    NSSize              oldContentSize;
}

-(id)initWithFrame:(NSRect)frame
        ParentView:(id)parent;

-(void)parentScrollViewFrameChanged;

-(void)showDocument:(PdfDocumentData*)docData;

-(void)drawPages:(NSRect)rect;

-(void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect;

-(void)closeDocument;

-(void)nextPage;

-(void)prevPage;

-(void)rotate;

-(void)renderThread;

@end



@interface CPDFPageView : NSScrollView
{
    CPDFPageMainView * mainView;
}

-(id)initWithFrame:(NSRect)frameRect;

-(void)showDocument:(PdfDocumentData*)docData;

-(void)closeDocument;

-(void)nextPage;

-(void)prevPage;

-(void)rotate;

@end
