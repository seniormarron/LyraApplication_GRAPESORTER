// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothHTMLEditor.pas' rev: 22.00

#ifndef AdvsmoothhtmleditorHPP
#define AdvsmoothhtmleditorHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <ToolWin.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit
#include <ImgList.hpp>	// Pascal unit
#include <AdvSmoothPanel.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothhtmleditor
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TAdvSmoothHTMLEditorForm;
class PASCALIMPLEMENTATION TAdvSmoothHTMLEditorForm : public Forms::TForm
{
	typedef Forms::TForm inherited;
	
__published:
	Comctrls::TToolBar* ToolBar1;
	Stdctrls::TComboBox* Fontname;
	Stdctrls::TComboBox* FontSize;
	Comctrls::TToolButton* BoldButton;
	Comctrls::TToolButton* ItalicButton;
	Comctrls::TToolButton* UnderlineButton;
	Comctrls::TToolButton* LeftAlign;
	Comctrls::TToolButton* CenterAlign;
	Comctrls::TToolButton* RightAlign;
	Comctrls::TToolButton* URLButton;
	Comctrls::TToolButton* ToolButton1;
	Comctrls::TToolButton* Superscript;
	Comctrls::TToolButton* Subscript;
	Comctrls::TToolButton* ToolButton2;
	Comctrls::TToolButton* ToolButton3;
	Controls::TImageList* ImageList1;
	Dialogs::TColorDialog* ColorDialog1;
	Stdctrls::TMemo* Memo1;
	Extctrls::TSplitter* Splitter1;
	Comctrls::TStatusBar* StatusBar1;
	Comctrls::TToolButton* ToolButton4;
	Comctrls::TToolButton* ToolButton5;
	Comctrls::TToolButton* ToolButton6;
	Comctrls::TToolButton* ListButton;
	Comctrls::TToolButton* ToolButton8;
	Comctrls::TToolButton* dbfields;
	Menus::TPopupMenu* PopupMenu1;
	Menus::TMenuItem* Test1;
	Menus::TMenuItem* N11;
	Menus::TMenuItem* N21;
	Menus::TMenuItem* N31;
	Menus::TMenuItem* N441;
	Comctrls::TToolButton* ToolButton7;
	Comctrls::TToolButton* AcceptBtn;
	Comctrls::TToolButton* CancelBtn;
	Comctrls::TToolButton* ToolButton9;
	Advsmoothpanel::TAdvSmoothPanel* AdvSmoothPanel1;
	void __fastcall Memo1Change(System::TObject* Sender);
	void __fastcall BoldButtonClick(System::TObject* Sender);
	void __fastcall ItalicButtonClick(System::TObject* Sender);
	void __fastcall FormCreate(System::TObject* Sender);
	void __fastcall ToolButton1Click(System::TObject* Sender);
	void __fastcall UnderlineButtonClick(System::TObject* Sender);
	void __fastcall CenterAlignClick(System::TObject* Sender);
	void __fastcall RightAlignClick(System::TObject* Sender);
	void __fastcall FontnameChange(System::TObject* Sender);
	void __fastcall URLButtonClick(System::TObject* Sender);
	void __fastcall SuperscriptClick(System::TObject* Sender);
	void __fastcall SubscriptClick(System::TObject* Sender);
	void __fastcall HTMLStaticText1AnchorEnter(System::TObject* Sender, System::UnicodeString Anchor);
	void __fastcall HTMLStaticText1AnchorExit(System::TObject* Sender, System::UnicodeString Anchor);
	void __fastcall Memo1KeyUp(System::TObject* Sender, System::Word &Key, Classes::TShiftState Shift);
	void __fastcall ToolButton2Click(System::TObject* Sender);
	void __fastcall Memo1MouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall LeftAlignClick(System::TObject* Sender);
	void __fastcall FontSizeChange(System::TObject* Sender);
	void __fastcall ToolButton3Click(System::TObject* Sender);
	void __fastcall ToolButton5Click(System::TObject* Sender);
	void __fastcall ToolButton4Click(System::TObject* Sender);
	void __fastcall ToolButton6Click(System::TObject* Sender);
	void __fastcall ListButtonClick(System::TObject* Sender);
	void __fastcall ToolButton8Click(System::TObject* Sender);
	void __fastcall dbfieldsClick(System::TObject* Sender);
	void __fastcall PopupClick(System::TObject* Sender);
	void __fastcall AcceptBtnClick(System::TObject* Sender);
	void __fastcall CancelBtnClick(System::TObject* Sender);
	void __fastcall ToolButton9Click(System::TObject* Sender);
	
private:
	Classes::TStringList* fFieldNames;
	void __fastcall GetFontNames(void);
	void __fastcall SetFieldNames(const Classes::TStringList* Value);
	
public:
	__fastcall virtual TAdvSmoothHTMLEditorForm(Classes::TComponent* aOwner);
	__fastcall virtual ~TAdvSmoothHTMLEditorForm(void);
	void __fastcall InsertTags(System::UnicodeString starttag, System::UnicodeString endtag);
	bool __fastcall HasTags(System::UnicodeString starttag, System::UnicodeString endtag);
	void __fastcall RemoveTags(System::UnicodeString starttag, System::UnicodeString endtag);
	bool __fastcall ScanForward(System::UnicodeString starttag, System::UnicodeString endtag);
	bool __fastcall ScanBackward(System::UnicodeString starttag, System::UnicodeString endtag);
	void __fastcall ChangeTagProp(System::UnicodeString starttag, System::UnicodeString endtag, System::UnicodeString propname, System::UnicodeString propval);
	void __fastcall UpdateButtons(void);
	__property Classes::TStringList* FieldNames = {read=fFieldNames, write=SetFieldNames};
	System::UnicodeString __fastcall GetHTMLString(void);
public:
	/* TCustomForm.CreateNew */ inline __fastcall virtual TAdvSmoothHTMLEditorForm(Classes::TComponent* AOwner, int Dummy) : Forms::TForm(AOwner, Dummy) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothHTMLEditorForm(HWND ParentWindow) : Forms::TForm(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE TAdvSmoothHTMLEditorForm* AdvSmoothHTMLEditorForm;

}	/* namespace Advsmoothhtmleditor */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothhtmleditor;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothhtmleditorHPP
