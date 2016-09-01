//
//  CHEPDFEditView.m
//  chelib
//
//  Created by zencher on 16/2/19.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFEditView.h"

@implementation CHEPDFEditView

- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        pdfDocData = nil;
        
        frameRect.size.width -= 151;
        pageView = [[CHEPDFPageView alloc] initWithFrame:frameRect RootView:self];
        frameRect.size.width = 150;
        outlineView = [[CHEPDFOutlineView alloc] initWithFrame:frameRect];
        
        bookmarkRoot = nil;
        
        [self setVertical:YES];
        [self setDividerStyle:NSSplitViewDividerStyleThin];
        [self addSubview:outlineView];
        [self addSubview:pageView];
        [self setDelegate:self];
    }
    return self;
}

-(void)dealloc
{
}

- (BOOL) loadFile:(NSString*)filePath
{
    [self closeFile];
    
    pdfDocData = [[PdfDocumentData alloc] initWithFilePath:filePath];
    if (pdfDocData) {
        [pageView showDocument:pdfDocData];
        [outlineView setDataSource:self];
        return TRUE;
    }
    return FALSE;
}

- (void) closeFile
{
    bookmarkRoot = nil;
    [outlineView setDataSource:nil];
    [pageView closeDocument];
}

- (void)nextPage
{
    [pageView nextPage];
}

- (void)prevPage
{
    [pageView prevPage];
}

- (void)rotate
{
    [pageView rotate];
}



- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex
{
    if (dividerIndex == 0)
    {
        return 100;
    }else if (dividerIndex == 1)
    {
        return 500;
    }
    return proposedMinimumPosition;
}
 
- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex
{
    if (dividerIndex == 0)
    {
        return 300;
    }else if (dividerIndex == 1)
    {
        return proposedMaximumPosition - 100;
    }
    return proposedMaximumPosition;
}

- (void)splitView:(NSSplitView *)sender resizeSubviewsWithOldSize:(NSSize)oldSize
{
    CGFloat dividerThickness = [sender dividerThickness];
    NSRect leftRect  = [[[sender subviews] objectAtIndex:0] frame];
    NSRect rightRect = [[[sender subviews] objectAtIndex:1] frame];
    NSRect newFrame  = [sender frame];
    
    leftRect.size.height = newFrame.size.height;
    leftRect.origin = NSMakePoint(0, 0);
    rightRect.size.width = newFrame.size.width - leftRect.size.width - dividerThickness;
    rightRect.size.height = newFrame.size.height;
    rightRect.origin.x = leftRect.size.width + dividerThickness;
    
    [[[sender subviews] objectAtIndex:0] setFrame:leftRect];
    [[[sender subviews] objectAtIndex:1] setFrame:rightRect];
}



- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
    if (item == nil)
    {
        return 1;
    }
    return [item numberOfChildren];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    if (item == nil)
    {
        return YES;
    }
    return [item numberOfChildren] > 0 ? YES : NO;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
    if (item == nil)
    {
        if ( bookmarkRoot == nil )
        {
            bookmarkRoot = [[BookmarkItem alloc] initWithItemPointer:[pdfDocData getOutlineRoot]];
        }
        return bookmarkRoot;
    }
    return [item childAtIndex:index];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
    if (item == nil) {
        return @"Outline";
    }
    return [item title];
}

@end



@implementation BookmarkItem

- (id)initWithItemPointer:(CHE_PDF_OutlineItem*)item
{
    self = [super init];
    if (self) {
        outlineItem = item;
        children = [[NSMutableArray alloc] initWithCapacity:[self numberOfChildren]];
    }
    return self;
}

- (NSInteger)numberOfChildren
{
    if (outlineItem)
    {
        NSInteger count = 0;
        CHE_PDF_OutlineItem * item = outlineItem->mpFirst;
        while (item) {
            ++count;
            item = item->mpNext;
        }
        return count;
    }
    return 0;
}

- (BookmarkItem *)childAtIndex:(NSInteger)n
{
    if (outlineItem)
    {
        BookmarkItem * bookmarkItem = nil;
        if ( n+1 > [children count] ) {
            NSInteger count = 0;
            CHE_PDF_OutlineItem * item = outlineItem->mpFirst;
            while (item) {
                if (count++ == n)
                {
                    break;
                }
                item = item->mpNext;
            }
            if (item)
            {
                bookmarkItem = [[BookmarkItem alloc] initWithItemPointer:item];
                [children addObject:bookmarkItem];
            }
        }else{
            bookmarkItem = [children objectAtIndex:n];
        }
        return bookmarkItem;
    }
    return nil;
}

- (NSString *)title
{
    if (outlineItem)
    {
        NSString * str = [[NSString alloc] initWithBytes:outlineItem->mTitle.GetData() length:outlineItem->mTitle.GetLength() encoding:NSASCIIStringEncoding];
        return str;
    }
    return @"Outline";
}

@end

