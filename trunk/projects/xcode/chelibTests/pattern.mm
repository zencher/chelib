//
//  pattern.m
//  chelib
//
//  Created by zencher on 16/1/29.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "che_pdf_file.h"
#include "che_pdf_pattern.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_componentmgr.h"
#include "Che_pdf_colorspace.h"
#include "che_pdf_contentlist.h"
#include "che_pdf_gstate.h"

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
    IHE_Read * pRead = HE_CreateFileRead( "/Users/zencher/testfiles/patternUnitTest000.pdf" );
    CHE_PDF_File file;
    file.Open( pRead );

    CHE_PDF_ComponentMgr mgr;
    CHE_PDF_ColorSpacePtr cs;
    
    CHE_PDF_ReferencePtr refPtr = CHE_PDF_Reference::Create( 40, 0, &file);
    
    bool bFound = false;
    CHE_PDF_TilingPtr tiling = CHE_PDF_Tiling::Create( refPtr, &mgr);
    if ( tiling )
    {
        CHE_PDF_ContentObjectList & content = tiling->GetList();
        
        ContentObjectList::iterator it;
        for ( it = content.Begin(); it != content.End(); ++it )
        {
            CHE_PDF_GState * pState = (*it)->GetGState();
            pState->GetFillColorSpace( cs );
            if ( cs->GetColorSpaceType() == COLORSPACE_CIEBASE_ICCBASED )
            {
                CHE_PDF_CS_ICCBasedPtr icc = cs->GetICCBasedPtr();
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
