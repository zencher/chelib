#ifndef _CHELIB_PDF_ACTION_H_
#define _CHELIB_PDF_ACTION_H_

#include "che_pdf_object.h"
#include "che_pdf_destination.h"
#include "che_pdf_filespec.h"
#include "che_pdf_namedict.h"

namespace chelib {

enum PDF_ACTION_TYPE
{
	ACTION_GOTO,
	ACTION_GOTOR,
	ACTION_GOTOE,
	ACTION_LAUNCH,
	ACTION_THREAD,
	ACTION_URI,
	ACTION_SOUND,
	ACTION_MOVIE,
	ACTION_HIDE,
	ACTION_NAMED,
	ACTION_SUBMIT,
	ACTION_RESET,
	ACTION_IMPORT,
	ACTION_JS,
	ACTION_OCG,
	ACTION_RENDITION,
	ACTION_TRANS,
	ACTION_GOTO3D
};

class CPDF_Action;
class CPDF_GoToAction;
class CPDF_GoToRAction;
class CPDF_GotoEAction;
class CPDF_LaunchAction;
class CPDF_ThreadAction;
class CPDF_URIAction;
class CPDF_SoundAction;
class CPDF_MovieAction;
class CPDF_HideAction;
class CPDF_NamedAction;
class CPDF_SubmitAction;
class CPDF_ResetAction;
class CPDF_ImportAction;
class CPDF_JavaScriptAction;
class CPDF_OCGAction;
class CPDF_RenditionAction;
class CPDF_TransAction;
class CPDF_GoTo3DAction;

class CPDF_ActionPtr : public CPDF_ComponentPtr
{
public:
	CPDF_Action * operator->() const { return (CPDF_Action*)mpCom; }
};

class CPDF_Action : public CPDF_Component
{
public:
	static CPDF_ActionPtr Create( const CPDF_DictionaryPtr & dict,
                                  CPDF_NameDict * pNameDict = nullptr,
                                  CAllocator * pAllocator = nullptr );

	PDF_ACTION_TYPE GetActionType() const { return mActionType; }

	CPDF_ActionPtr GetNext() const { return mNext; }

protected:
	CPDF_Action( PDF_ACTION_TYPE type, const CPDF_ObjectPtr & obj,
                 CAllocator * pAllocator = nullptr );

	PDF_ACTION_TYPE mActionType;
	CPDF_ActionPtr mNext;

	friend class CAllocator;
};

class CPDF_GoToActionPtr : public CPDF_ActionPtr
{
public:
	CPDF_GoToAction* operator->() { return (CPDF_GoToAction*)mpCom; }
};

class CPDF_GoToAction : public CPDF_Action
{
public:
	CPDF_DestinationPtr GetDest() const { return mDestPtr; }

private:
	CPDF_GoToAction( const CPDF_DictionaryPtr & dict,
                     CPDF_NameDict * pNameDict = nullptr,
                     CAllocator * pAllocator = nullptr);

	CPDF_DestinationPtr	mDestPtr;

	friend class CAllocator;
};


class CPDF_GoToRActionPtr : public CPDF_ActionPtr
{
public:
	CPDF_GoToRAction * operator->() const { return (CPDF_GoToRAction *)mpCom; }
};

class CPDF_GoToRAction : public  CPDF_Action
{
public:

private:
	CPDF_GoToRAction( const CPDF_DictionaryPtr & dict,
                      CPDF_NameDict * pNameDict = nullptr,
                      CAllocator * pAllocator = nullptr);

	bool					mbNewWindow;
	CPDF_FileSpec		mFileSpec;
	CPDF_DestinationPtr	mDestPtr;

};


// class CPDF_Action_GoToE : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Launch : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Thread : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_URI : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Sound : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Movie : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Hide : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Named : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Submit : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Reset : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Import : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_JavaScript : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_SetOCGState : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Rendition : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Trans : public CPDF_Action
// {
// 
// };
// 
// class CPDF_Action_Goto3D : public CPDF_Action
// {
// 
// };
    
}//namespace

#endif