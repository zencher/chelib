//
//  CustomView.h
//  chelib
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"

@interface CustomView : NSView
{
    unsigned int        pageIndex;
    PdfDocumentData *   pdfDocument;
}

-(void)newDocument:(PdfDocumentData*)doc;

-(void)closeDocument;

-(void)onPrePage;

-(void)onNextPage;

@end
