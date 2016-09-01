//
//  functions.mm
//  chelib
//
//  Created by zencher on 16/1/19.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "che_pdf_function.h"
#include "che_pdf_file.h"

@interface functions : XCTestCase

@end

@implementation functions

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testFunctionCalculate {
    IHE_Read * pRead = HE_CreateFileRead( "/Users/zencher/testfiles/functionUnitTest000.pdf" );
    CHE_PDF_File file;
    file.Open( pRead );
    
    PDF_RefInfo refInfo;
    refInfo.objNum = 5;
    refInfo.genNum = 0;
    
    CHE_PDF_ObjectPtr objptr = file.GetObject( refInfo );
    
    CHE_PDF_FunctionPtr funcPtr = CHE_PDF_Function::Create( objptr );
    
    XCTAssertTrue( funcPtr->GetFunctionType() == FUNCTION_TYPE_POSTSCRIPT, @"CHE_PDF_Function::GetFunctionType error, shall be FUNCTION_TYPE_POSTSCRIPT" );
    XCTAssertTrue( funcPtr->GetInputCount() == 1, @"CHE_PDF_Function::GetInputCount error, shall be 1" );
    XCTAssertTrue( funcPtr->GetOutputCount() == 4, @"CHE_PDF_Function::GetOutputCount error, shall be 4" );
    XCTAssertTrue( funcPtr->GetDomianMax(0) == 1.0, @"CHE_PDF_Function::GetDomianMax(0) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetDomianMin(0) == 0.0, @"CHE_PDF_Function::GetDomianMin(0) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(0) == 1.0, @"CHE_PDF_Function::GetRangeMax(0) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(0) == 0.0, @"CHE_PDF_Function::GetRangeMin(0) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(1) == 1.0, @"CHE_PDF_Function::GetRangeMax(1) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(1) == 0.0, @"CHE_PDF_Function::GetRangeMin(1) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(2) == 1.0, @"CHE_PDF_Function::GetRangeMax(2) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(2) == 0.0, @"CHE_PDF_Function::GetRangeMin(2) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(3) == 1.0, @"CHE_PDF_Function::GetRangeMax(3) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(3) == 0.0, @"CHE_PDF_Function::GetRangeMin(3) error, shall be 0.0" );
    
    std::vector<HE_FLOAT> input;
    input.push_back( 0.1 );
    std::vector<HE_FLOAT> output;
    funcPtr->Calculate( input, output );
    
    XCTAssertTrue( output.size() == 4, @"CHE_PDF_Function::Calculate() error, output nunber count shall be 4" );
    
    
    
    
    refInfo.objNum = 6;
    refInfo.genNum = 0;
    objptr = file.GetObject( refInfo );
    funcPtr = CHE_PDF_Function::Create( objptr );
    
    XCTAssertTrue( funcPtr->GetFunctionType() == FUNCTION_TYPE_POSTSCRIPT, @"CHE_PDF_Function::GetFunctionType error, shall be FUNCTION_TYPE_POSTSCRIPT" );
    XCTAssertTrue( funcPtr->GetInputCount() == 1, @"CHE_PDF_Function::GetInputCount error, shall be 1" );
    XCTAssertTrue( funcPtr->GetOutputCount() == 3, @"CHE_PDF_Function::GetOutputCount error, shall be 4" );
    XCTAssertTrue( funcPtr->GetDomianMax(0) == 1.0, @"CHE_PDF_Function::GetDomianMax(0) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetDomianMin(0) == 0.0, @"CHE_PDF_Function::GetDomianMin(0) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(0) == 1.0, @"CHE_PDF_Function::GetRangeMax(0) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(0) == 0.0, @"CHE_PDF_Function::GetRangeMin(0) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(1) == 1.0, @"CHE_PDF_Function::GetRangeMax(1) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(1) == 0.0, @"CHE_PDF_Function::GetRangeMin(1) error, shall be 0.0" );
    XCTAssertTrue( funcPtr->GetRangeMax(2) == 1.0, @"CHE_PDF_Function::GetRangeMax(2) error, shall be 1.0" );
    XCTAssertTrue( funcPtr->GetRangeMin(2) == 0.0, @"CHE_PDF_Function::GetRangeMin(2) error, shall be 0.0" );
    
    input.clear();
    input.push_back( 0 );
    output.clear();
    funcPtr->Calculate( input, output );
    XCTAssertTrue( output.size() == 3, @"CHE_PDF_Function::Calculate() error, output nunber count shall be 3" );
    
    XCTAssertTrue( output[0] == 0.0 && output[1] == 0.0 && output[2] == 0.0, @"CHE_PDF_Function::Calculate() output error, output shall be [0.0, 0.0, 0.0]" );
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
