//
//  CHEPDFImpView.h
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import "PdfDocumentData.h"

@interface CHEPDFMainView : NSView
{
    id                  parentScrollView;
    PdfDocumentData *   pdfDocument;
    NSSize              oldContentSize;
}

-(id)initWithFrameAndParentView:(NSRect)frame
                         parentView:(id)view;

-(void)parentScrollViewFrameChanged;

-(void)newDocument:(PdfDocumentData*)doc;

-(void)drawPages:(NSRect)rect;

-(void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect;

-(void)closeDocument;


-(void)nextPage;

-(void)prevPage;

@end
