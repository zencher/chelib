//
//  pattern.m
//  chelib
//
//  Created by zencher on 16/1/29.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "pdf_file.h"
#include "pdf_pattern.h"
#include "pdf_colorspace.h"
#include "pdf_componentmgr.h"
#include "pdf_colorspace.h"
#include "pdf_contentlist.h"
#include "pdf_gstate.h"

using namespace chelib;

@interface pattern : XCTestCase

@end

@implementation pattern

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)test000 {
    IRead * pRead = CreateFileIRead( "/Users/zencher/testfiles/patternUnitTest000.pdf" );
    CPDF_File file;
    file.Open( pRead );

    CPDF_ComponentMgr mgr;
    CPDF_ColorSpacePtr cs;
    
    CPDF_ReferencePtr refPtr = CPDF_Reference::Create( 40, 0, &file);
    
    bool bFound = false;
    CPDF_TilingPtr tiling = CPDF_Tiling::Create( refPtr, &mgr);
    if ( tiling )
    {
        CPDF_ContentObjectList & content = tiling->GetList();
        
        ContentObjectList::iterator it;
        for ( it = content.Begin(); it != content.End(); ++it )
        {
            CPDF_GState * pState = (*it)->GetGState();
            pState->GetFillColorSpace( cs );
            if ( cs->GetColorSpaceType() == COLORSPACE_CIEBASE_ICCBASED )
            {
                CPDF_CS_ICCBasedPtr icc = cs->GetICCBasedPtr();
                if ( icc->mAlternate->GetColorSpaceType() == COLORSPACE_DEVICE_RGB )
                {
                    bFound = true;
                }
            }
        }

    }
    
    XCTAssertTrue( bFound, @"!!!" );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
