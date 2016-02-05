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

-(void)rotate;

-(CHE_PDF_Outline*)outline;


/* for outline data sources */
- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item;

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item;

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item;

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;

@end


@interface BookmarkItem : NSObject {
    CHE_PDF_OutlineItem * outlineItem;
}

- (id)initWithItemPointer:(CHE_PDF_OutlineItem*)item;
- (NSInteger)numberOfChildren;
- (BookmarkItem *)childAtIndex:(NSInteger)n;
- (NSString *)title;

@end
