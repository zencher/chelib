#ifndef _CHE_PDF_ACTION_H_
#define _CHE_PDF_ACTION_H_

#include "che_pdf_objects.h"
#include "che_pdf_destination.h"
#include "che_pdf_filespec.h"
#include "che_pdf_namedict.h"

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

class CHE_PDF_Action;
class CHE_PDF_GoToAction;
class CHE_PDF_GoToRAction;
class CHE_PDF_GotoEAction;
class CHE_PDF_LaunchAction;
class CHE_PDF_ThreadAction;
class CHE_PDF_URIAction;
class CHE_PDF_SoundAction;
class CHE_PDF_MovieAction;
class CHE_PDF_HideAction;
class CHE_PDF_NamedAction;
class CHE_PDF_SubmitAction;
class CHE_PDF_ResetAction;
class CHE_PDF_ImportAction;
class CHE_PDF_JavaScriptAction;
class CHE_PDF_OCGAction;
class CHE_PDF_RenditionAction;
class CHE_PDF_TransAction;
class CHE_PDF_GoTo3DAction;

class CHE_PDF_ActionPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_Action * operator->() const { return (CHE_PDF_Action*)mpCom; }
};

class CHE_PDF_Action : public CHE_PDF_Component
{
public:
	static CHE_PDF_ActionPtr Create(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_NameDict * pNameDict = nullptr, CHE_Allocator * pAllocator = nullptr);

	PDF_ACTION_TYPE GetActionType() const { return mActionType; }

	CHE_PDF_ActionPtr GetNext() const { return mNext; }

protected:
	CHE_PDF_Action( PDF_ACTION_TYPE type, const CHE_PDF_ObjectPtr & obj, CHE_Allocator * pAllocator = nullptr );

	PDF_ACTION_TYPE		mActionType;
	CHE_PDF_ActionPtr	mNext;

	friend class CHE_Allocator;
};

class CHE_PDF_GoToActionPtr : public CHE_PDF_ActionPtr
{
public:
	CHE_PDF_GoToAction* operator->() { return (CHE_PDF_GoToAction*)mpCom; }
};

class CHE_PDF_GoToAction : public CHE_PDF_Action
{
public:
	CHE_PDF_DestinationPtr GetDest() const { return mDestPtr; }

private:
	CHE_PDF_GoToAction(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_NameDict * pNameDict = nullptr, CHE_Allocator * pAllocator = nullptr);

	CHE_PDF_DestinationPtr	mDestPtr;

	friend class CHE_Allocator;
};


class CHE_PDF_GoToRActionPtr : public CHE_PDF_ActionPtr
{
public:
	CHE_PDF_GoToRAction * operator->() const { return (CHE_PDF_GoToRAction *)mpCom; }
};

class CHE_PDF_GoToRAction : public  CHE_PDF_Action
{
public:

private:
	CHE_PDF_GoToRAction(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_NameDict * pNameDict = nullptr, CHE_Allocator * pAllocator = nullptr);

	bool					mbNewWindow;
	CHE_PDF_FileSpec		mFileSpec;
	CHE_PDF_DestinationPtr	mDestPtr;

};


// class CHE_PDF_Action_GoToE : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Launch : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Thread : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_URI : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Sound : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Movie : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Hide : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Named : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Submit : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Reset : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Import : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_JavaScript : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_SetOCGState : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Rendition : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Trans : public CHE_PDF_Action
// {
// 
// };
// 
// class CHE_PDF_Action_Goto3D : public CHE_PDF_Action
// {
// 
// };

#endif