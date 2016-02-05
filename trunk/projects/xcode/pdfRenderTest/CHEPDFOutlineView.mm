//
//  CHEPDFOutlineView.m
//  chelib
//
//  Created by zencher on 16/2/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFOutlineView.h"

@implementation CHEPDFOutlineMainView

-(id)initWithFrameAndParentView:(NSRect)frame
                     parentView:(id)view
{
    self = [super initWithFrame:frame];
    if (self)
    {
        //parentScrollView = view;
        //[[NSNotificationCenter defaultCenter] addObserver:self
        //                                         selector:@selector(parentScrollViewFrameChanged)
        //                                             name:NSViewFrameDidChangeNotification
        //                                           object:parentScrollView];
    }
    return self;
}

-(void)parentScrollViewFrameChanged
{
    //[self sizeLastColumnToFit];
}

@end


@implementation CHEPDFOutlineView

-(id) initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        outlineView = [[CHEPDFOutlineMainView alloc] initWithFrameAndParentView:frameRect parentView:self];
        [outlineView setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleRegular];
        [outlineView setColumnAutoresizingStyle:NSTableViewUniformColumnAutoresizingStyle];
        [outlineView setHeaderView:nil];
        NSTableColumn *c = [[NSTableColumn alloc] initWithIdentifier: @"Column"];
        [c setEditable: NO];
        //[c setMinWidth:frameRect.size.width];
        //[c setMaxWidth:frameRect.size.width];
        [c setResizingMask:NSTableColumnAutoresizingMask];
        [outlineView addTableColumn: c];
        [outlineView setOutlineTableColumn:c];
        [outlineView sizeLastColumnToFit];
        
        
        [self setDocumentView:outlineView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:NO];
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
}

-(void) setDataSource:(id)source
{
    if (outlineView) {
        [outlineView setDataSource:source];
        [outlineView sizeLastColumnToFit];
    }
}

@end
