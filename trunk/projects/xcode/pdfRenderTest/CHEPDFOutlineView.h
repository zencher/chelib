//
//  CHEPDFOutlineView.h
//  chelib
//
//  Created by zencher on 16/2/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface CHEPDFOutlineScrollView : NSScrollView
{
    NSOutlineView * outlineView;
    id parentScrollView;
}

-(id)initWithFrameAndParentView:(NSRect)frame
                     parentView:(id)view;

-(void)parentScrollViewFrameChanged;

-(void) setDataSource:(id)sources;

@end



@interface CHEPDFOutlineView : NSVisualEffectView
{
    CHEPDFOutlineScrollView * scrollView;
}

-(void)setDataSource:(id)sources;

@end
