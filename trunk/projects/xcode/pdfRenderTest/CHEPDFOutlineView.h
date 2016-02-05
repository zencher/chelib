//
//  CHEPDFOutlineView.h
//  chelib
//
//  Created by zencher on 16/2/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CHEPDFOutlineMainView : NSOutlineView
{
    id parentScrollView;
}

-(id)initWithFrameAndParentView:(NSRect)frame
                     parentView:(id)view;

-(void)parentScrollViewFrameChanged;

@end


@interface CHEPDFOutlineView : NSScrollView
{
    CHEPDFOutlineMainView * outlineView;
}

-(id) initWithFrame:(NSRect)frameRect;

-(void) setDataSource:(id)sources;

@end
