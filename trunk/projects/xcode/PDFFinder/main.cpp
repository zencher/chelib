//
//  main.cpp
//  PDFFinder
//
//  Created by zencher on 16/4/11.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#include <iostream>
#include "pdf_document.h"

using namespace chelib;

int main(int argc, const char * argv[])
{
    if ( argv[1] )
    {
        std::cout << argv[1] << std::endl;
    }
    
    IRead * pRead = CreateFileIRead( argv[1] );
    if ( pRead )
    {
        CPDF_File file( GetDefaultAllocator() );
        if ( file.Open( pRead ) )
        {
            CPDF_XRefTable * xtable = file.GetXRefTable();
            if ( xtable )
            {
                size_t maxObjNum = xtable->GetMaxObjNum();
                PDF_RefInfo refInfo;
                CPDF_ObjectPtr objptr;
                CPDF_DictionaryPtr dictPtr;
                for ( size_t i = 0; i < maxObjNum; ++i )
                {
                    refInfo.objNum = i;
                    refInfo.genNum = 0;
                    objptr = file.GetObject( refInfo );
                    if ( objptr )
                    {
                        if ( objptr->GetType() == OBJ_TYPE_ARRAY )
                        {
                            objptr = objptr->GetArrayPtr()->GetElement(0, OBJ_TYPE_NAME);
                            if ( objptr )
                            {
                                if ( objptr->GetNamePtr()->GetString() == "DeviceN" )
                                {
                                    std::cout << "DeviceN found at " << refInfo.objNum << " " << refInfo.genNum << " obj" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
        file.Close();
        DestoryIRead( pRead );
        pRead = NULL;
    }
    return 0;
}