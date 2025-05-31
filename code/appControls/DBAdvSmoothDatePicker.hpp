// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'DBAdvSmoothDatePicker.pas' rev: 22.00

#ifndef DbadvsmoothdatepickerHPP
#define DbadvsmoothdatepickerHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <AdvSmoothDatePicker.hpp>	// Pascal unit
#include <AdvSmoothCalendar.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <DB.hpp>	// Pascal unit
#include <DBCtrls.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <AdvSmoothEditButton.hpp>	// Pascal unit
#include <AdvSmoothEdit.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Dbadvsmoothdatepicker
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TDBAdvSmoothDatePicker;
class PASCALIMPLEMENTATION TDBAdvSmoothDatePicker : public Advsmoothdatepicker::TAdvSmoothDatePicker
{
	typedef Advsmoothdatepicker::TAdvSmoothDatePicker inherited;
	
private:
	Dbctrls::TFieldDataLink* FDataLink;
	Db::TDataSetState FOldState;
	bool FInternalCall;
	void __fastcall DataUpdate(System::TObject* Sender);
	void __fastcall DataChange(System::TObject* Sender);
	void __fastcall ActiveChange(System::TObject* Sender);
	System::UnicodeString __fastcall GetDataField(void);
	Db::TDataSource* __fastcall GetDataSource(void);
	void __fastcall SetDataField(const System::UnicodeString Value);
	void __fastcall SetDataSource(const Db::TDataSource* Value);
	MESSAGE void __fastcall CMGetDataLink(Messages::TMessage &Message);
	
protected:
	bool __fastcall EditCanModify(void);
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation Operation);
	virtual void __fastcall CalendarDaySelect(System::TObject* Sender, Advsmoothcalendar::TAdvSmoothCalendarDateMode Mode, System::TDateTime Date);
	
public:
	__fastcall virtual TDBAdvSmoothDatePicker(Classes::TComponent* AOwner);
	__fastcall virtual ~TDBAdvSmoothDatePicker(void);
	DYNAMIC bool __fastcall ExecuteAction(Classes::TBasicAction* Action);
	virtual bool __fastcall UpdateAction(Classes::TBasicAction* Action);
	
__published:
	__property System::UnicodeString DataField = {read=GetDataField, write=SetDataField};
	__property Db::TDataSource* DataSource = {read=GetDataSource, write=SetDataSource};
public:
	/* TWinControl.CreateParented */ inline __fastcall TDBAdvSmoothDatePicker(HWND ParentWindow) : Advsmoothdatepicker::TAdvSmoothDatePicker(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------

}	/* namespace Dbadvsmoothdatepicker */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Dbadvsmoothdatepicker;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// DbadvsmoothdatepickerHPP
