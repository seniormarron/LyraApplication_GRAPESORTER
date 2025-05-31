// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothSpin.pas' rev: 22.00

#ifndef AdvsmoothspinHPP
#define AdvsmoothspinHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Buttons.hpp>	// Pascal unit
#include <Mask.hpp>	// Pascal unit
#include <AdvSmoothXPVS.hpp>	// Pascal unit
#include <Character.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit
#include <MaskUtils.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothspin
{
//-- type declarations -------------------------------------------------------
typedef Buttons::TNumGlyphs TNumGlyphs;

class DELPHICLASS TWinCtrl;
class PASCALIMPLEMENTATION TWinCtrl : public Controls::TWinControl
{
	typedef Controls::TWinControl inherited;
	
public:
	/* TWinControl.Create */ inline __fastcall virtual TWinCtrl(Classes::TComponent* AOwner) : Controls::TWinControl(AOwner) { }
	/* TWinControl.CreateParented */ inline __fastcall TWinCtrl(HWND ParentWindow) : Controls::TWinControl(ParentWindow) { }
	/* TWinControl.Destroy */ inline __fastcall virtual ~TWinCtrl(void) { }
	
};


#pragma option push -b-
enum TSpinDirection { spVertical, spHorizontal };
#pragma option pop

#pragma option push -b-
enum TLabelPosition { lpLeftTop, lpLeftCenter, lpLeftBottom, lpTopLeft, lpBottomLeft, lpLeftTopLeft, lpLeftCenterLeft, lpLeftBottomLeft, lpTopCenter, lpBottomCenter };
#pragma option pop

#pragma option push -b-
enum TEditAlign { eaLeft, eaRight, eaCenter };
#pragma option pop

class DELPHICLASS TAdvSmoothSpinButton;
class DELPHICLASS TAdvSmoothTimerSpeedButton;
class PASCALIMPLEMENTATION TAdvSmoothSpinButton : public Controls::TWinControl
{
	typedef Controls::TWinControl inherited;
	
private:
	TAdvSmoothTimerSpeedButton* FUpButton;
	TAdvSmoothTimerSpeedButton* FDownButton;
	TAdvSmoothTimerSpeedButton* FFocusedButton;
	Controls::TWinControl* FFocusControl;
	Classes::TNotifyEvent FOnUpClick;
	Classes::TNotifyEvent FOnDownClick;
	TSpinDirection FDirection;
	TAdvSmoothTimerSpeedButton* __fastcall CreateButton(void);
	Graphics::TBitmap* __fastcall GetUpGlyph(void);
	Graphics::TBitmap* __fastcall GetDownGlyph(void);
	void __fastcall SetUpGlyph(Graphics::TBitmap* Value);
	void __fastcall SetDownGlyph(Graphics::TBitmap* Value);
	Buttons::TNumGlyphs __fastcall GetUpNumGlyphs(void);
	Buttons::TNumGlyphs __fastcall GetDownNumGlyphs(void);
	void __fastcall SetUpNumGlyphs(Buttons::TNumGlyphs Value);
	void __fastcall SetDownNumGlyphs(Buttons::TNumGlyphs Value);
	void __fastcall BtnClick(System::TObject* Sender);
	void __fastcall BtnMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall SetFocusBtn(TAdvSmoothTimerSpeedButton* Btn);
	void __fastcall SetDirection(const TSpinDirection Value);
	HIDESBASE void __fastcall AdjustSize(int &W, int &H);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Messages::TWMSize &Message);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Messages::TWMSetFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Messages::TWMKillFocus &Message);
	MESSAGE void __fastcall WMGetDlgCode(Messages::TWMNoParams &Message);
	
protected:
	virtual void __fastcall Loaded(void);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation Operation);
	
public:
	__fastcall virtual TAdvSmoothSpinButton(Classes::TComponent* AOwner);
	virtual void __fastcall SetBounds(int ALeft, int ATop, int AWidth, int AHeight);
	
__published:
	__property Align = {default=0};
	__property Anchors = {default=3};
	__property DragKind = {default=0};
	__property Constraints;
	__property OnStartDock;
	__property OnEndDock;
	__property Ctl3D;
	__property TSpinDirection Direction = {read=FDirection, write=SetDirection, nodefault};
	__property Graphics::TBitmap* DownGlyph = {read=GetDownGlyph, write=SetDownGlyph};
	__property Buttons::TNumGlyphs DownNumGlyphs = {read=GetDownNumGlyphs, write=SetDownNumGlyphs, default=1};
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Enabled = {default=1};
	__property Controls::TWinControl* FocusControl = {read=FFocusControl, write=FFocusControl};
	__property ParentCtl3D = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ShowHint;
	__property TabOrder = {default=-1};
	__property TabStop = {default=0};
	__property Graphics::TBitmap* UpGlyph = {read=GetUpGlyph, write=SetUpGlyph};
	__property Buttons::TNumGlyphs UpNumGlyphs = {read=GetUpNumGlyphs, write=SetUpNumGlyphs, default=1};
	__property Visible = {default=1};
	__property Classes::TNotifyEvent OnDownClick = {read=FOnDownClick, write=FOnDownClick};
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnStartDrag;
	__property Classes::TNotifyEvent OnUpClick = {read=FOnUpClick, write=FOnUpClick};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothSpinButton(HWND ParentWindow) : Controls::TWinControl(ParentWindow) { }
	/* TWinControl.Destroy */ inline __fastcall virtual ~TAdvSmoothSpinButton(void) { }
	
};


#pragma option push -b-
enum TAdvSmoothSpinType { sptNormal, sptFloat, sptDate, sptTime, sptHex };
#pragma option pop

class DELPHICLASS TAdvSmoothSpinEdit;
class PASCALIMPLEMENTATION TAdvSmoothSpinEdit : public Mask::TCustomMaskEdit
{
	typedef Mask::TCustomMaskEdit inherited;
	
private:
	bool FFlat;
	Graphics::TColor FFlatLineColor;
	Stdctrls::TLabel* FLabel;
	Graphics::TFont* FLabelFont;
	TLabelPosition FLabelPosition;
	int FLabelMargin;
	bool FLabelTransparent;
	int FMinValue;
	int FMaxValue;
	double FMinFloatValue;
	double FMaxFloatValue;
	System::TDateTime FMinDateValue;
	System::TDateTime FMaxDateValue;
	System::TDateTime FDateValue;
	System::TDateTime FTimeValue;
	int FHexValue;
	int FIncrement;
	double FIncrementFloat;
	int FIncrementPage;
	double FIncrementFloatPage;
	bool FIncrementSmart;
	TAdvSmoothSpinButton* FButton;
	bool FEditorEnabled;
	TSpinDirection FDirection;
	TAdvSmoothSpinType FSpinType;
	int FPrecision;
	bool FReturnIsTab;
	Graphics::TColor FNormalColor;
	bool FUSDates;
	bool FSpinFlat;
	bool FSpinTransparent;
	bool FTransparent;
	bool FAutoFocus;
	int FIncrementHours;
	int FIncrementMinutes;
	bool FShowSeconds;
	bool FSigned;
	bool FLabelAlwaysEnabled;
	TEditAlign FEditAlign;
	Graphics::TColor FFocusBorderColor;
	Graphics::TColor FFocusColor;
	Classes::TNotifyEvent FOnUpClick;
	Classes::TNotifyEvent FOnDownClick;
	bool FAllowNullValue;
	int __fastcall GetMinHeight(void);
	int __fastcall GetValue(void);
	int __fastcall CheckValue(int NewValue);
	void __fastcall SetValue(int NewValue);
	double __fastcall GetFloatValue(void);
	double __fastcall CheckFloatValue(double NewValue);
	System::TDateTime __fastcall CheckDateValue(System::TDateTime NewValue);
	void __fastcall SetFloatValue(double NewValue);
	void __fastcall SetEditRect(void);
	void __fastcall SetEditorEnabled(bool NewValue);
	void __fastcall SetDirection(const TSpinDirection Value);
	void __fastcall SetPrecision(const int Value);
	void __fastcall SetSpinType(const TAdvSmoothSpinType Value);
	System::TDateTime __fastcall GetTimeValue(void);
	void __fastcall SetTimeValue(const System::TDateTime Value);
	System::TDateTime __fastcall GetDateValue(void);
	void __fastcall SetDateValue(const System::TDateTime Value);
	int __fastcall GetHexValue(void);
	void __fastcall SetHexValue(const int Value);
	void __fastcall SetSpinFlat(const bool Value);
	void __fastcall SetSpinTransparent(const bool value);
	HIDESBASE MESSAGE void __fastcall CNCommand(Messages::TWMCommand &Message);
	MESSAGE void __fastcall CNCtlColorEdit(Messages::TWMCtlColor &Message);
	MESSAGE void __fastcall CNCtlColorStatic(Messages::TWMCtlColor &Message);
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Messages::TWMSize &Message);
	HIDESBASE MESSAGE void __fastcall CMEnter(Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall CMExit(Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall CMShowingChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMWantSpecialKey(Messages::TWMKey &Msg);
	HIDESBASE MESSAGE void __fastcall WMPaste(Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall WMCut(Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall WMChar(Messages::TWMKey &Msg);
	HIDESBASE MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Msg);
	HIDESBASE MESSAGE void __fastcall WMNCPaint(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMKeyDown(Messages::TWMKey &Msg);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Messages::TWMKillFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Messages::TWMSetFocus &Message);
	System::UnicodeString __fastcall GetLabelCaption(void);
	void __fastcall SetLabelCaption(const System::UnicodeString Value);
	void __fastcall SetLabelMargin(const int Value);
	void __fastcall SetLabelPosition(const TLabelPosition Value);
	void __fastcall SetLabelTransparent(const bool Value);
	bool __fastcall GetVisible(void);
	HIDESBASE void __fastcall SetVisible(const bool Value);
	void __fastcall LabelFontChange(System::TObject* Sender);
	void __fastcall SetLabelFont(const Graphics::TFont* Value);
	void __fastcall SetTransparent(const bool Value);
	void __fastcall SetFlat(const bool Value);
	void __fastcall SetFlatRect(const bool Value);
	void __fastcall SetFlatLineColor(const Graphics::TColor Value);
	void __fastcall SetShowSeconds(const bool Value);
	void __fastcall SetLabelAlwaysEnabled(const bool Value);
	bool __fastcall GetEnabledEx(void);
	void __fastcall SetEnabledEx(const bool Value);
	void __fastcall SetEditAign(const TEditAlign Value);
	System::UnicodeString __fastcall GetVersion(void);
	void __fastcall SetVersion(const System::UnicodeString Value);
	void __fastcall DrawBorder(void);
	void __fastcall DrawControlBorder(HDC DC);
	
protected:
	virtual int __fastcall GetVersionNr(void);
	virtual bool __fastcall IsValidChar(System::WideChar &Key);
	virtual void __fastcall UpClick(System::TObject* Sender);
	virtual void __fastcall DownClick(System::TObject* Sender);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	DYNAMIC void __fastcall KeyPress(System::WideChar &Key);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	virtual void __fastcall SetParent(Controls::TWinControl* AParent);
	virtual void __fastcall CreateWnd(void);
	virtual void __fastcall Loaded(void);
	void __fastcall UpdateLabel(void);
	Stdctrls::TLabel* __fastcall CreateLabel(void);
	void __fastcall PaintEdit(void);
	
public:
	DYNAMIC void __fastcall GetChildren(Classes::TGetChildProc Proc, Classes::TComponent* Root);
	__fastcall virtual TAdvSmoothSpinEdit(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothSpinEdit(void);
	__property TAdvSmoothSpinButton* Button = {read=FButton};
	void __fastcall IncSmart(void);
	void __fastcall DecSmart(void);
	virtual void __fastcall SetBounds(int ALeft, int ATop, int AWidth, int AHeight);
	void __fastcall Init(void);
	__property Stdctrls::TLabel* SpinLabel = {read=FLabel};
	
__published:
	__property bool AllowNullValue = {read=FAllowNullValue, write=FAllowNullValue, default=1};
	__property bool AutoFocus = {read=FAutoFocus, write=FAutoFocus, default=0};
	__property TSpinDirection Direction = {read=FDirection, write=SetDirection, default=0};
	__property bool ReturnIsTab = {read=FReturnIsTab, write=FReturnIsTab, default=0};
	__property int Precision = {read=FPrecision, write=SetPrecision, default=0};
	__property TAdvSmoothSpinType SpinType = {read=FSpinType, write=SetSpinType, default=0};
	__property int Value = {read=GetValue, write=SetValue, nodefault};
	__property double FloatValue = {read=GetFloatValue, write=SetFloatValue};
	__property System::TDateTime TimeValue = {read=GetTimeValue, write=SetTimeValue};
	__property System::TDateTime DateValue = {read=GetDateValue, write=SetDateValue};
	__property int HexValue = {read=GetHexValue, write=SetHexValue, nodefault};
	__property bool Flat = {read=FFlat, write=SetFlat, default=0};
	__property Graphics::TColor FlatLineColor = {read=FFlatLineColor, write=SetFlatLineColor, default=0};
	__property bool SpinFlat = {read=FSpinFlat, write=SetSpinFlat, default=0};
	__property bool SpinTransparent = {read=FSpinTransparent, write=SetSpinTransparent, default=0};
	__property Anchors = {default=3};
	__property Constraints;
	__property AutoSelect = {default=1};
	__property AutoSize = {default=1};
	__property BorderStyle = {default=1};
	__property Color = {default=-16777211};
	__property Ctl3D;
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property TEditAlign EditAlign = {read=FEditAlign, write=SetEditAign, default=0};
	__property bool EditorEnabled = {read=FEditorEnabled, write=SetEditorEnabled, default=1};
	__property bool Enabled = {read=GetEnabledEx, write=SetEnabledEx, nodefault};
	__property Graphics::TColor FocusBorderColor = {read=FFocusBorderColor, write=FFocusBorderColor, default=536870911};
	__property Graphics::TColor FocusColor = {read=FFocusColor, write=FFocusColor, default=-16777211};
	__property Font;
	__property int Increment = {read=FIncrement, write=FIncrement, default=1};
	__property double IncrementFloat = {read=FIncrementFloat, write=FIncrementFloat};
	__property int IncrementPage = {read=FIncrementPage, write=FIncrementPage, default=10};
	__property double IncrementFloatPage = {read=FIncrementFloatPage, write=FIncrementFloatPage};
	__property bool IncrementSmart = {read=FIncrementSmart, write=FIncrementSmart, default=0};
	__property int IncrementMinutes = {read=FIncrementMinutes, write=FIncrementMinutes, default=1};
	__property int IncrementHours = {read=FIncrementHours, write=FIncrementHours, default=1};
	__property bool LabelAlwaysEnabled = {read=FLabelAlwaysEnabled, write=SetLabelAlwaysEnabled, default=0};
	__property System::UnicodeString LabelCaption = {read=GetLabelCaption, write=SetLabelCaption};
	__property TLabelPosition LabelPosition = {read=FLabelPosition, write=SetLabelPosition, default=0};
	__property int LabelMargin = {read=FLabelMargin, write=SetLabelMargin, default=4};
	__property bool LabelTransparent = {read=FLabelTransparent, write=SetLabelTransparent, default=0};
	__property Graphics::TFont* LabelFont = {read=FLabelFont, write=SetLabelFont};
	__property MaxLength = {default=0};
	__property int MaxValue = {read=FMaxValue, write=FMaxValue, default=0};
	__property int MinValue = {read=FMinValue, write=FMinValue, default=0};
	__property double MinFloatValue = {read=FMinFloatValue, write=FMinFloatValue};
	__property double MaxFloatValue = {read=FMaxFloatValue, write=FMaxFloatValue};
	__property System::TDateTime MinDateValue = {read=FMinDateValue, write=FMinDateValue};
	__property System::TDateTime MaxDateValue = {read=FMaxDateValue, write=FMaxDateValue};
	__property ParentColor = {default=0};
	__property ParentCtl3D = {default=1};
	__property ParentFont = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ReadOnly = {default=0};
	__property bool Signed = {read=FSigned, write=FSigned, default=0};
	__property ShowHint;
	__property bool ShowSeconds = {read=FShowSeconds, write=SetShowSeconds, default=1};
	__property TabOrder = {default=-1};
	__property TabStop = {default=1};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, default=0};
	__property bool Visible = {read=GetVisible, write=SetVisible, nodefault};
	__property System::UnicodeString Version = {read=GetVersion, write=SetVersion};
	__property OnChange;
	__property OnClick;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnStartDrag;
	__property Classes::TNotifyEvent OnUpClick = {read=FOnUpClick, write=FOnUpClick};
	__property Classes::TNotifyEvent OnDownClick = {read=FOnDownClick, write=FOnDownClick};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothSpinEdit(HWND ParentWindow) : Mask::TCustomMaskEdit(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothMaskSpinEdit;
class PASCALIMPLEMENTATION TAdvSmoothMaskSpinEdit : public TAdvSmoothSpinEdit
{
	typedef TAdvSmoothSpinEdit inherited;
	
__published:
	__property EditMask;
public:
	/* TAdvSmoothSpinEdit.Create */ inline __fastcall virtual TAdvSmoothMaskSpinEdit(Classes::TComponent* AOwner) : TAdvSmoothSpinEdit(AOwner) { }
	/* TAdvSmoothSpinEdit.Destroy */ inline __fastcall virtual ~TAdvSmoothMaskSpinEdit(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothMaskSpinEdit(HWND ParentWindow) : TAdvSmoothSpinEdit(ParentWindow) { }
	
};


#pragma option push -b-
enum Advsmoothspin__5 { tbFocusRect, tbAllowTimer };
#pragma option pop

typedef System::Set<Advsmoothspin__5, Advsmoothspin__5::tbFocusRect, Advsmoothspin__5::tbAllowTimer>  TTimeBtnState;

#pragma option push -b-
enum TButtonDirection { bdLeft, bdRight, bdUp, bdDown };
#pragma option pop

class PASCALIMPLEMENTATION TAdvSmoothTimerSpeedButton : public Buttons::TSpeedButton
{
	typedef Buttons::TSpeedButton inherited;
	
private:
	Extctrls::TTimer* FRepeatTimer;
	TTimeBtnState FTimeBtnState;
	TButtonDirection FButtonDirection;
	bool FIsWinXP;
	bool FHasMouse;
	void __fastcall TimerExpired(System::TObject* Sender);
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	
protected:
	bool __fastcall DoVisualStyles(void);
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	
public:
	__property bool IsWinXP = {read=FIsWinXP, nodefault};
	__fastcall virtual TAdvSmoothTimerSpeedButton(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothTimerSpeedButton(void);
	__property TTimeBtnState TimeBtnState = {read=FTimeBtnState, write=FTimeBtnState, nodefault};
	__property TButtonDirection ButtonDirection = {read=FButtonDirection, write=FButtonDirection, nodefault};
};


//-- var, const, procedure ---------------------------------------------------
static const System::ShortInt MAJ_VER = 0x1;
static const System::ShortInt MIN_VER = 0x0;
static const System::ShortInt REL_VER = 0x0;
static const System::ShortInt BLD_VER = 0x0;

}	/* namespace Advsmoothspin */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothspin;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothspinHPP
