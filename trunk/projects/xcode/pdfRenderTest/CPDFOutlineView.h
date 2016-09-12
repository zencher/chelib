//
//  CHEPDFOutlineView.h
//  chelib
//
//  Created by zencher on 16/2/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface CPDFOutlineScrollView : NSScrollView
{
    NSOutlineView * outlineView;
    id parentScrollView;
}

-(id)initWithFrameAndParentView:(NSRect)frame
                     parentView:(id)view;

-(void)parentScrollViewFrameChanged;

-(void) setDataSource:(id)sources;

@end



@interface CPDFOutlineView : NSVisualEffectView
{
    CPDFOutlineScrollView * scrollView;
}

-(void)setDataSource:(id)sources;

@end
