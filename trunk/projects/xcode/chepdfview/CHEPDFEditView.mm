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
        bookmarkRoot = nil;
        
        [self setVertical:YES];
        [self setDividerStyle:NSSplitViewDividerStyleThin];
        [self setDelegate:self];
        
        NSRect rect = frameRect;
        rect.size.width = 150;
        if ( frameRect.size.width >= 452 )
        {
            frameRect.size.width -= (150 * 2 + 2);
        }else{
            frameRect.size.width = 150;
        }
        leftBarView = [[NSSplitView alloc] initWithFrame:rect];
        rightBarView = [[NSSplitView alloc] initWithFrame:rect];
        middleView = [[NSSplitView alloc] initWithFrame:frameRect];
        [self addArrangedSubview:leftBarView];
        [self addArrangedSubview:middleView];
        [self addArrangedSubview:rightBarView];
        
        thumbnailView = [[CHEPDFThumbnailView alloc] initWithFrame:[leftBarView frame]];
        outlineView = [[CHEPDFOutlineView alloc] initWithFrame:[rightBarView frame]];
        pageView = [[CHEPDFPageView alloc] initWithFrame:[middleView frame]];
        
        [leftBarView addSubview:thumbnailView];
        [middleView addSubview:pageView];
        //[rightBarView addSubview:outlineView];
    }
    return self;
}

-(void)dealloc
{
}

-(BOOL)load:(PdfDocumentData*)data
{
    [self closeFile];
    pdfDocData = data;
    [pageView showDocument:pdfDocData];
    [outlineView setDataSource:self];
    [thumbnailView setDocumentData:pdfDocData];
    return TRUE;
}

- (BOOL)loadFile:(NSString*)filePath
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

- (void)closeFile
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


// view controller
- (void)showBookmarkView
{
    NSView * view = [[leftBarView subviews] objectAtIndex:0];
    if ( view != outlineView)
    {
        [leftBarView replaceSubview:view with:outlineView];
    }
}

- (void)showThumbnailView
{
    NSView * view = [[leftBarView subviews] objectAtIndex:0];
    if ( view != thumbnailView)
    {
        [leftBarView replaceSubview:view with:thumbnailView];
    }
}

- (void)showLeftBar:(bool)bShow
{
    if (bShow)
    {
        [self insertArrangedSubview:leftBarView atIndex:0];
        [self adjustSubviews];
    }else{
        [leftBarView removeFromSuperview];
        [self adjustSubviews];
    }
}

- (void)showRightBar:(bool)bShow
{
    if (bShow)
    {
        [self insertArrangedSubview:rightBarView atIndex:2];
        [self adjustSubviews];
    }else{
        [rightBarView removeFromSuperview];
        [self adjustSubviews];
    }
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex
{
    NSRect newFrame  = [splitView frame];
    NSInteger viewsCount = [[splitView subviews] count];
    if (viewsCount == 2)
    {
        if ([[splitView subviews] objectAtIndex:0] == middleView)
        {
            return newFrame.size.width/2;
        }
        return 150;
    }else if (viewsCount == 3)
    {
        if (dividerIndex == 0)
        {
            return 150;
        }
        return newFrame.size.width*2/3;
    }
    return 150;
}
 
- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex
{
    NSRect newFrame  = [splitView frame];
    NSUInteger viewsCount = [[splitView subviews] count];
    if (viewsCount == 2)
    {
        if ([[splitView subviews] objectAtIndex:0] == middleView)
        {
            return newFrame.size.width-150;
            
        }
        return newFrame.size.width/2;
    }else if (viewsCount == 3)
    {
        if (dividerIndex == 0)
        {
            return newFrame.size.width/3;
        }
        return newFrame.size.width-150;
    }
    return 150;
}

- (BOOL)splitView:(NSSplitView *)splitView canCollapseSubview:(NSView *)subview
{
    NSUInteger viewsCount = [[splitView subviews] count];
    if (viewsCount == 2)
    {
        if (middleView == subview)
        {
            return NO;
        }
        return YES;
    }else if (viewsCount == 3)
    {
        if ([[splitView subviews] objectAtIndex:0] == subview)
        {
            return YES;
        }else if ([[splitView subviews] objectAtIndex:2] == subview)
        {
            return YES;
        }
    }
    return NO;
}

- (void)splitView:(NSSplitView *)splitView resizeSubviewsWithOldSize:(NSSize)oldSize
{
    CGFloat dividerThickness = [splitView dividerThickness];
    NSUInteger viewsCount = [[splitView subviews] count];
    
    if (viewsCount == 3)
    {
        NSRect leftRect  = [[[splitView subviews] objectAtIndex:0] frame];
        NSRect middleRect = [[[splitView subviews] objectAtIndex:1] frame];
        NSRect rightRect = [[[splitView subviews] objectAtIndex:2] frame];
        NSRect newFrame  = [splitView frame];
        if (newFrame.size.width <= 150 * 3 + dividerThickness * 2)
        {
            leftRect.size.width = 0;
            leftRect.size.height = newFrame.size.height;
            leftRect.origin.x = 0;
            middleRect.size.width = newFrame.size.width;
            middleRect.size.height = newFrame.size.height;
            middleRect.origin.x = 0;
            rightRect.size.width = 0;
            rightRect.size.height = newFrame.size.height;
            rightRect.origin.x = middleRect.size.width;
            [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
            [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
            [[[splitView subviews] objectAtIndex:2] setFrame:rightRect];
            return;
        }
        
        if (leftRect.size.width > newFrame.size.width/3 || rightRect.size.width > newFrame.size.width/3)
        {
            if (leftRect.size.width > newFrame.size.width/3) {
                leftRect.size.width = newFrame.size.width/3;
            }
            if (rightRect.size.width > newFrame.size.width/3) {
                rightRect.size.width = newFrame.size.width/3;
            }
            middleRect.size.width = newFrame.size.width - leftRect.size.width - rightRect.size.width - 2 * dividerThickness;
            leftRect.origin.x = 0;
            middleRect.origin.x = leftRect.size.width + dividerThickness;
            rightRect.origin.x = newFrame.size.width - rightRect.size.width;
            [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
            [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
            [[[splitView subviews] objectAtIndex:2] setFrame:rightRect];
            return;
        }
        
        leftRect.size.height = newFrame.size.height;
        leftRect.origin = NSMakePoint(0, 0);
        middleRect.size.height = newFrame.size.height;
        middleRect.origin = NSMakePoint(0, 0);
        rightRect.size.height = newFrame.size.height;
        rightRect.origin = NSMakePoint(0, 0);
        middleRect.size.width = newFrame.size.width - 2 * dividerThickness;
        bool bVal = [splitView isSubviewCollapsed:[[splitView subviews] objectAtIndex:0]];
        if (bVal)
        {
            middleRect.origin.x = dividerThickness;
            leftRect.size.width = 0;
        }else{
            middleRect.origin.x = leftRect.origin.x + leftRect.size.width + dividerThickness;
            middleRect.size.width -= leftRect.size.width;
        }
        bVal = [splitView isSubviewCollapsed:[[splitView subviews] objectAtIndex:2]];
        if (bVal)
        {
            rightRect.size.width = 0;
            rightRect.origin.x = middleRect.origin.x + middleRect.size.width + dividerThickness;
        }else{
            middleRect.size.width -= rightRect.size.width;
            rightRect.origin.x = middleRect.origin.x + middleRect.size.width + dividerThickness;
        }
        [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
        [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
        [[[splitView subviews] objectAtIndex:2] setFrame:rightRect];
    }else if (viewsCount == 2)
    {
        if ([[splitView subviews] objectAtIndex:0] == leftBarView)
        {
            NSRect leftRect = [[[splitView subviews] objectAtIndex:0] frame];
            NSRect middleRect = [[[splitView subviews] objectAtIndex:1] frame];
            NSRect newFrame  = [splitView frame];
            if (newFrame.size.width <= 150 * 3 + dividerThickness * 2)
            {
                leftRect.size.width = 0;
                leftRect.size.height = newFrame.size.height;
                leftRect.origin = NSMakePoint(0, 0);
                middleRect.size.width = newFrame.size.width - dividerThickness;
                middleRect.size.height = newFrame.size.height;
                middleRect.origin.x = dividerThickness;
                [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
                [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
                return;
            }
            
            if (leftRect.size.width >= newFrame.size.width/2)
            {
                leftRect.size.width = newFrame.size.width/2;
                leftRect.size.height = newFrame.size.height;
                leftRect.origin.x = 0;
                middleRect.size.width = newFrame.size.width - leftRect.size.width - dividerThickness;
                middleRect.size.height = newFrame.size.height;
                middleRect.origin.x =  leftRect.origin.x + leftRect.size.width + dividerThickness;
                [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
                [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
                return;
            }
            
            bool bVal = [splitView isSubviewCollapsed:[[splitView subviews] objectAtIndex:0]];
            middleRect.size.width = newFrame.size.width  - dividerThickness;
            middleRect.size.height = newFrame.size.height;
            leftRect.size.height = newFrame.size.height;
            if (bVal)
            {
                middleRect.origin.x = dividerThickness;
            }else{
                middleRect.size.width -= leftRect.size.width;
                middleRect.origin.x = leftRect.origin.x + leftRect.size.width + dividerThickness;
            }
            [[[splitView subviews] objectAtIndex:0] setFrame:leftRect];
            [[[splitView subviews] objectAtIndex:1] setFrame:middleRect];
        }else{
            NSRect middleRect = [[[splitView subviews] objectAtIndex:0] frame];
            NSRect rightRect = [[[splitView subviews] objectAtIndex:1] frame];
            NSRect newFrame  = [splitView frame];
            if (newFrame.size.width <= 150 * 3 + dividerThickness * 2)
            {
                middleRect.size.width = newFrame.size.width;
                middleRect.size.height = newFrame.size.height;
                middleRect.origin.x = 0;
                rightRect.size.width = 0;
                rightRect.size.height = newFrame.size.height;
                rightRect.origin.x = middleRect.size.width;
                [[[splitView subviews] objectAtIndex:0] setFrame:middleRect];
                [[[splitView subviews] objectAtIndex:1] setFrame:rightRect];
                return;
            }
            
            if (rightRect.size.width >= newFrame.size.width/2)
            {
                rightRect.size.width = newFrame.size.width/2;
                middleRect.size.width = newFrame.size.width - rightRect.size.width - dividerThickness;
                middleRect.origin.x =  0;
                rightRect.origin.x = middleRect.size.width + dividerThickness;
                [[[splitView subviews] objectAtIndex:0] setFrame:middleRect];
                [[[splitView subviews] objectAtIndex:1] setFrame:rightRect];
                return;
            }
            
            bool bVal = [splitView isSubviewCollapsed:[[splitView subviews] objectAtIndex:1]];
            
            middleRect.size.width = newFrame.size.width  - dividerThickness;
            if (!bVal) {
                middleRect.size.width -= rightRect.size.width;
            }
            middleRect.size.height = newFrame.size.height;
            middleRect.origin.x = 0;
            rightRect.origin.x = middleRect.size.width + dividerThickness;
            rightRect.size.height = newFrame.size.height;
            rightRect.size.width = rightRect.size.width;
            [[[splitView subviews] objectAtIndex:0] setFrame:middleRect];
            [[[splitView subviews] objectAtIndex:1] setFrame:rightRect];
        }
    }
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
    unsigned char * pStr = NULL;
    if (outlineItem)
    {
        pStr = (unsigned char * )(outlineItem->mTitle.GetData());
        if ( outlineItem->mTitle.GetLength() > 2 )
        {
            if ( pStr[0] == 0xFE && pStr[1] == 0xFF )
            {
                NSString * str = [[NSString alloc] initWithBytes:outlineItem->mTitle.GetData() length:outlineItem->mTitle.GetLength() encoding:NSUTF16BigEndianStringEncoding];
                return str;
            }else if ( pStr[0] == 0xFF && pStr[1] == 0xFE )
            {
                NSString * str = [[NSString alloc] initWithBytes:outlineItem->mTitle.GetData() length:outlineItem->mTitle.GetLength() encoding:NSUTF16LittleEndianStringEncoding];
                return str;
            }
            
        }
        NSString * str = [[NSString alloc] initWithBytes:outlineItem->mTitle.GetData() length:outlineItem->mTitle.GetLength() encoding:NSASCIIStringEncoding];
        return str;
    }
    return @"Outline";
}

@end

