//
//  main.cpp
//  PDFFinder
//
//  Created by zencher on 16/4/11.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#include <iostream>
#include "che_pdf_document.h"

int main(int argc, const char * argv[])
{
    if ( argv[1] )
    {
        std::cout << argv[1] << endl;
    }
    
    IHE_Read * pRead = HE_CreateFileRead( argv[1] );
    if ( pRead )
    {
        CHE_PDF_File file( GetDefaultAllocator() );
        if ( file.Open( pRead ) )
        {
            CHE_PDF_XRefTable * xtable = file.GetXRefTable();
            if ( xtable )
            {
                HE_ULONG maxObjNum = xtable->GetMaxObjNum();
                PDF_RefInfo refInfo;
                CHE_PDF_ObjectPtr objptr;
                CHE_PDF_DictionaryPtr dictPtr;
                for ( HE_ULONG i = 0; i < maxObjNum; ++i )
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
                                    std::cout << "DeviceN found at " << refInfo.objNum << " " << refInfo.genNum << " obj" <<endl;
                                }
                            }
                        }
                    }
                }
            }
        }
        file.Close();
        HE_DestoryIHERead( pRead );
        pRead = NULL;
    }
    return 0;
}