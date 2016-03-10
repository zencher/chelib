//
//  CHEPDFThumbnailView.h
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CHEPDFThumbnailMainView : NSView

@end

@interface CHEPDFThumbnailScrollView : NSScrollView
{
    CHEPDFThumbnailMainView * mainView;
}

@end

@interface CHEPDFThumbnailView : NSVisualEffectView
{
    CHEPDFThumbnailScrollView * scrollView;
}

@end
