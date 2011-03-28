#include "../../include/pdf/che_pdf_renderer.h"

HE_BOOL CHE_PDF_Renderer::GetPageContent( CHE_PDF_Page & page, CHE_DynBuffer & buffer )
{
	CHE_PDF_Dictionary * pPageDict = page.GetPageDictionary();
	if ( pPageDict == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Object * pPageContent = pPageDict->GetElement( CHE_ByteString("Contents") );
	if ( pPageContent == NULL )
	{
		return FALSE;
	}
	if ( page.GetDocument() == NULL )
	{
		return FALSE;
	}
	if ( page.GetDocument()->GetParser() == NULL )
	{
		return FALSE;
	}
	if ( pPageContent->GetType() == PDFOBJ_REFERENCE )
	{
		HE_DWORD objNum = ((CHE_PDF_Reference*)pPageContent)->GetRefNuml();
		CHE_PDF_IndirectObject * pInObj = page.GetDocument()->GetParser()->GetIndirectObject( objNum );
		if ( pInObj == NULL )
		{
			return FALSE;
		}
		CHE_PDF_Object *  pContentObj = pInObj->GetObject();
		if ( pContentObj->GetType() == PDFOBJ_STREAM )
		{
			CHE_PDF_StreamAcc stmAcc;
			stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
			buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
			stmAcc.Detach();
			return TRUE;
		}else if ( pContentObj->GetType() == PDFOBJ_ARRAY )
		{
			CHE_PDF_Object * pTmpObj = NULL;
			for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pContentObj)->GetCount(); i++ )
			{
				pTmpObj = ((CHE_PDF_Array*)pContentObj)->GetElement( i );
				if ( pTmpObj->GetType() == PDFOBJ_STREAM )
				{
					CHE_PDF_StreamAcc stmAcc;
					stmAcc.Attach( (CHE_PDF_Stream*)pTmpObj );
					buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
					stmAcc.Detach();
					
				}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
				{
					HE_DWORD objNum = ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml();
					CHE_PDF_IndirectObject * pInObj = page.GetDocument()->GetParser()->GetIndirectObject( objNum );
					if ( pInObj == NULL )
					{
						continue;
						//return FALSE;
					}
					CHE_PDF_Object *  pContentObj = pInObj->GetObject();
					if ( pContentObj->GetType() == PDFOBJ_STREAM )
					{
						CHE_PDF_StreamAcc stmAcc;
						stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
						buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
						stmAcc.Detach();
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
