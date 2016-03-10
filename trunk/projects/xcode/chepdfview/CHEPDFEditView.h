//
//  CHEPDFEditView.h
//  chelib
//
//  Created by zencher on 16/2/19.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CHEPDFPageView.h"
#import "CHEPDFOutlineView.h"
#import "CHEPDFThumbnailView.h"
#import "PdfDocumentData.h"


@interface BookmarkItem : NSObject
{
    CHE_PDF_OutlineItem * outlineItem;
    NSMutableArray * children;
}

- (id)initWithItemPointer:(CHE_PDF_OutlineItem*)item;
- (NSInteger)numberOfChildren;
- (BookmarkItem *)childAtIndex:(NSInteger)n;
- (NSString *)title;

@end


@interface CHEPDFEditView : NSSplitView <NSSplitViewDelegate>
{
    PdfDocumentData * pdfDocData;
    CHEPDFOutlineView * outlineView;
    CHEPDFThumbnailView * thumbnailView;
    CHEPDFPageView * pageView;
    
    
    NSSplitView * middleView;
    NSSplitView * leftBarView;
    NSSplitView * rightBarView;
    
    
    BookmarkItem * bookmarkRoot;
}

-(BOOL)load:(PdfDocumentData*)data;

-(BOOL)loadFile:(NSString*)filePath;

-(void)closeFile;

-(void)nextPage;

-(void)prevPage;

-(void)rotate;




// view controller
-(void)showBookmarkView;

-(void)showThumbnailView;



-(void)showLeftBar:(bool)bShow;

-(void)showRightBar:(bool)bShow;



/* for outline data sources */
- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item;

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item;

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item;

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;

@end








