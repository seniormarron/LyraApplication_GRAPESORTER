// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothSelectors.pas' rev: 22.00

#ifndef AdvsmoothselectorsHPP
#define AdvsmoothselectorsHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <ImgList.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Math.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <AdvSmoothXPVS.hpp>	// Pascal unit
#include <Types.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothselectors
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum TAdvSelectorStyle { ssButton, ssCombo };
#pragma option pop

#pragma option push -b-
enum TAdvAppearanceStyle { esOffice2003Blue, esOffice2003Silver, esOffice2003Olive, esOffice2003Classic, esWhidbey, esCustom, esXP };
#pragma option pop

#pragma option push -b-
enum TAdvButtonState { absUp, absDown, absDropDown };
#pragma option pop

#pragma option push -b-
enum TGradientDirection { gdVertical, gdHorizontal };
#pragma option pop

#pragma option push -b-
enum TSelectorItemType { itAutoSizeButton, itFullWidthButton };
#pragma option pop

#pragma option push -b-
enum TColorSelectionStyle { csDiscrete, csColorCube, csSpectrum };
#pragma option pop

typedef System::ShortInt TNoOfButtons;

class DELPHICLASS TAdvSmoothSelectorItem;
class PASCALIMPLEMENTATION TAdvSmoothSelectorItem : public Classes::TCollectionItem
{
	typedef Classes::TCollectionItem inherited;
	
private:
	Types::TRect FItemRect;
	System::UnicodeString FCaption;
	bool FEnable;
	int FImageIndex;
	System::UnicodeString FValue;
	System::UnicodeString FHint;
	Classes::TAlignment FCaptionAlignment;
	TSelectorItemType FItemType;
	Graphics::TColor FBackGroundColor;
	int FTag;
	void __fastcall SetCaption(const System::UnicodeString Value);
	void __fastcall SetEnable(const bool Value);
	void __fastcall SetHint(const System::UnicodeString Value);
	void __fastcall SetImageIndex(const int Value);
	void __fastcall SetValue(const System::UnicodeString Value);
	void __fastcall SetvCaptionAlignment(const Classes::TAlignment Value);
	void __fastcall SetItemType(const TSelectorItemType Value);
	void __fastcall SetBackGroundColor(const Graphics::TColor Value);
	void __fastcall SetTag(const int Value);
	
protected:
	__property Types::TRect ItemRect = {read=FItemRect, write=FItemRect};
	__property bool Enable = {read=FEnable, write=SetEnable, nodefault};
	
public:
	__fastcall virtual TAdvSmoothSelectorItem(Classes::TCollection* Collection);
	__fastcall virtual ~TAdvSmoothSelectorItem(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	
__published:
	__property Graphics::TColor BackGroundColor = {read=FBackGroundColor, write=SetBackGroundColor, default=536870911};
	__property System::UnicodeString Caption = {read=FCaption, write=SetCaption};
	__property Classes::TAlignment CaptionAlignment = {read=FCaptionAlignment, write=SetvCaptionAlignment, nodefault};
	__property int ImageIndex = {read=FImageIndex, write=SetImageIndex, nodefault};
	__property System::UnicodeString Hint = {read=FHint, write=SetHint};
	__property System::UnicodeString Value = {read=FValue, write=SetValue};
	__property TSelectorItemType ItemType = {read=FItemType, write=SetItemType, default=0};
	__property int Tag = {read=FTag, write=SetTag, default=0};
};


class DELPHICLASS TAdvSmoothSelectorItems;
class PASCALIMPLEMENTATION TAdvSmoothSelectorItems : public Classes::TCollection
{
	typedef Classes::TCollection inherited;
	
public:
	TAdvSmoothSelectorItem* operator[](int Index) { return Items[Index]; }
	
private:
	Classes::TPersistent* FOwner;
	HIDESBASE TAdvSmoothSelectorItem* __fastcall GetItem(int Index);
	HIDESBASE void __fastcall SetItem(int Index, const TAdvSmoothSelectorItem* Value);
	
protected:
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	
public:
	__fastcall TAdvSmoothSelectorItems(Classes::TPersistent* AOwner);
	HIDESBASE TAdvSmoothSelectorItem* __fastcall Add(void);
	HIDESBASE TAdvSmoothSelectorItem* __fastcall Insert(int Index);
	__property TAdvSmoothSelectorItem* Items[int Index] = {read=GetItem, write=SetItem/*, default*/};
public:
	/* TCollection.Destroy */ inline __fastcall virtual ~TAdvSmoothSelectorItems(void) { }
	
};


typedef void __fastcall (__closure *TChangeSelectionEvent)(int OldItemIndex, int NewItemIndex);

typedef void __fastcall (__closure *THotToolEvent)(System::TObject* Sender, int HotItemIndex);

class DELPHICLASS TAdvSmoothCustomSelectorPanel;
class PASCALIMPLEMENTATION TAdvSmoothCustomSelectorPanel : public Extctrls::TCustomPanel
{
	typedef Extctrls::TCustomPanel inherited;
	
private:
	Classes::TComponent* FOwner;
	Graphics::TColor FColorTo;
	TGradientDirection FGradientDirection;
	Graphics::TColor FWindowBorderColor;
	Classes::TNotifyEvent FOnShouldHide;
	void __fastcall SetColorTo(const Graphics::TColor Value);
	void __fastcall SetTGradientDirection(const TGradientDirection Value);
	void __fastcall SetWindowBorderColor(const Graphics::TColor Value);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	virtual int __fastcall GetVisibleHeight(void);
	__property Classes::TNotifyEvent OnShouldHide = {read=FOnShouldHide, write=FOnShouldHide};
	
public:
	__fastcall virtual TAdvSmoothCustomSelectorPanel(Classes::TComponent* AOwner);
	
__published:
	__property Graphics::TColor ColorTo = {read=FColorTo, write=SetColorTo, default=536870911};
	__property Graphics::TColor WindowBorderColor = {read=FWindowBorderColor, write=SetWindowBorderColor, default=8421504};
	__property TGradientDirection GradientDirection = {read=FGradientDirection, write=SetTGradientDirection, nodefault};
public:
	/* TCustomControl.Destroy */ inline __fastcall virtual ~TAdvSmoothCustomSelectorPanel(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothCustomSelectorPanel(HWND ParentWindow) : Extctrls::TCustomPanel(ParentWindow) { }
	
};


typedef void __fastcall (__closure *TItemDrawEvent)(System::TObject* Sender, int Index, const Types::TRect &R);

typedef void __fastcall (__closure *TSelectEvent)(System::TObject* Sender, int Index, TAdvSmoothSelectorItem* Item);

class DELPHICLASS TAdvSmoothSelectorPanel;
class PASCALIMPLEMENTATION TAdvSmoothSelectorPanel : public TAdvSmoothCustomSelectorPanel
{
	typedef TAdvSmoothCustomSelectorPanel inherited;
	
private:
	int FItemIndex;
	TAdvSmoothSelectorItems* FItems;
	Graphics::TColor FColorHotTo;
	Graphics::TColor FBorderColor;
	Graphics::TColor FBorderDownColor;
	Graphics::TColor FColorHot;
	Graphics::TColor FBorderHotColor;
	Graphics::TColor FColorDownTo;
	Graphics::TColor FColorDown;
	Imglist::TCustomImageList* FImages;
	int FHotItemIndex;
	int FDownItemIndex;
	int FButtonHeight;
	int FTopOffSet;
	int FLeftOffSet;
	TNoOfButtons FButtonsPerRow;
	TChangeSelectionEvent FOnChangeSelection;
	int FButtonMargin;
	bool FMouseDown;
	Classes::TNotifyEvent FOnSelect;
	Graphics::TColor FBorderSelectedColor;
	Graphics::TColor FColorSelected;
	Graphics::TColor FColorSelectedTo;
	THotToolEvent FOnHotTool;
	int FMaxCaptionLength;
	TItemDrawEvent FOnDrawItem;
	int FMinButtonWidth;
	int FMinButtonHeight;
	bool FNoPrefix;
	bool FTwoColorImages;
	HIDESBASE MESSAGE void __fastcall WMChar(Messages::TWMKey &Msg);
	MESSAGE void __fastcall CMWantSpecialKey(Messages::TWMKey &Msg);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall SetBorderColor(const Graphics::TColor Value);
	void __fastcall SetItemIndex(const int Value);
	void __fastcall SetItems(const TAdvSmoothSelectorItems* Value);
	void __fastcall SetImages(const Imglist::TCustomImageList* Value);
	void __fastcall SetButtonsPerRow(const TNoOfButtons Value);
	void __fastcall SetButtonMargin(const int Value);
	void __fastcall SetMinButtonHeight(const int Value);
	void __fastcall SetMinButtonWidth(const int Value);
	void __fastcall SetTwoColorImages(const bool Value);
	
protected:
	void __fastcall DrawItem(int Index, bool RefreshItem = false);
	virtual void __fastcall Paint(void);
	void __fastcall SetItemsPosition(void);
	int __fastcall GetMaxWidth(void);
	int __fastcall TotalAutoSizeButtons(void);
	int __fastcall ItemAtPos(int X, int Y);
	void __fastcall SetPanelHeight(void);
	void __fastcall AutoSizeBtnSize(int &W, int &H);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	__property int MinButtonWidth = {read=FMinButtonWidth, write=SetMinButtonWidth, default=16};
	__property int MinButtonHeight = {read=FMinButtonHeight, write=SetMinButtonHeight, default=16};
	__property bool NoPrefix = {read=FNoPrefix, write=FNoPrefix, nodefault};
	__property bool TwoColorImages = {read=FTwoColorImages, write=SetTwoColorImages, nodefault};
	
public:
	__fastcall virtual TAdvSmoothSelectorPanel(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothSelectorPanel(void);
	__property TAdvSmoothSelectorItems* Items = {read=FItems, write=SetItems};
	__property int ItemIndex = {read=FItemIndex, write=SetItemIndex, nodefault};
	__property int HotItemIndex = {read=FHotItemIndex, nodefault};
	
__published:
	__property Graphics::TColor BorderColor = {read=FBorderColor, write=SetBorderColor, default=536870911};
	__property Graphics::TColor BorderDownColor = {read=FBorderDownColor, write=FBorderDownColor, default=536870911};
	__property Graphics::TColor BorderHotColor = {read=FBorderHotColor, write=FBorderHotColor, default=536870911};
	__property Graphics::TColor BorderSelectedColor = {read=FBorderSelectedColor, write=FBorderSelectedColor, default=0};
	__property Graphics::TColor ColorDown = {read=FColorDown, write=FColorDown, nodefault};
	__property Graphics::TColor ColorDownTo = {read=FColorDownTo, write=FColorDownTo, default=536870911};
	__property Graphics::TColor ColorHot = {read=FColorHot, write=FColorHot, nodefault};
	__property Graphics::TColor ColorHotTo = {read=FColorHotTo, write=FColorHotTo, default=536870911};
	__property Graphics::TColor ColorSelected = {read=FColorSelected, write=FColorSelected, nodefault};
	__property Graphics::TColor ColorSelectedTo = {read=FColorSelectedTo, write=FColorSelectedTo, nodefault};
	__property TNoOfButtons ButtonsPerRow = {read=FButtonsPerRow, write=SetButtonsPerRow, default=1};
	__property Imglist::TCustomImageList* Images = {read=FImages, write=SetImages};
	__property int ButtonMargin = {read=FButtonMargin, write=SetButtonMargin, default=3};
	__property TChangeSelectionEvent OnChangeSelection = {read=FOnChangeSelection, write=FOnChangeSelection};
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
	__property THotToolEvent OnHotTool = {read=FOnHotTool, write=FOnHotTool};
	__property TItemDrawEvent OnDrawItem = {read=FOnDrawItem, write=FOnDrawItem};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothSelectorPanel(HWND ParentWindow) : TAdvSmoothCustomSelectorPanel(ParentWindow) { }
	
};


struct DECLSPEC_DRECORD TColorCubeCell
{
	
public:
	Types::TPoint CenterPos;
	Graphics::TColor Color;
};


class DELPHICLASS TAdvSmoothColorCubePanel;
class PASCALIMPLEMENTATION TAdvSmoothColorCubePanel : public TAdvSmoothCustomSelectorPanel
{
	typedef TAdvSmoothCustomSelectorPanel inherited;
	
private:
	System::StaticArray<TColorCubeCell, 127> FCubeCells;
	Types::TPoint FCubeSize;
	Graphics::TColor FSelectedColor;
	int FSelectedIndex;
	int FHotIndex;
	Classes::TNotifyEvent FOnSelect;
	bool FShowRGBHint;
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall DrawColorCube(int Index);
	void __fastcall DrawAllColorCube(void);
	void __fastcall DrawSelectedBorder(void);
	void __fastcall DrawHotBorder(void);
	void __fastcall SetSelectedColor(const Graphics::TColor Value);
	void __fastcall SetSelectedIndexAndColor(Graphics::TColor clr, int index = 0xffffffff);
	void __fastcall SetShowRGBHint(const bool Value);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	bool __fastcall PtInCell(int Index, const Types::TPoint &P);
	int __fastcall IndexOfCellAt(int X, int Y);
	void __fastcall DrawHexagon(Graphics::TCanvas* aCanvas, const Types::TPoint &P, int X, int Y);
	void __fastcall Initialize(void);
	void __fastcall SetItemsPosition(void);
	void __fastcall SetPanelSize(void);
	
public:
	__fastcall virtual TAdvSmoothColorCubePanel(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothColorCubePanel(void);
	
__published:
	__property Graphics::TColor SelectedColor = {read=FSelectedColor, write=SetSelectedColor, nodefault};
	__property bool ShowRGBHint = {read=FShowRGBHint, write=SetShowRGBHint, nodefault};
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothColorCubePanel(HWND ParentWindow) : TAdvSmoothCustomSelectorPanel(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothColorSpectrumPanel;
class PASCALIMPLEMENTATION TAdvSmoothColorSpectrumPanel : public TAdvSmoothCustomSelectorPanel
{
	typedef TAdvSmoothCustomSelectorPanel inherited;
	
private:
	Extctrls::TImage* FSpectrumImage;
	Graphics::TColor FHotColor;
	Graphics::TColor FSelectedColor;
	Types::TRect FHotRect;
	Types::TRect FSelectedRect;
	Classes::TNotifyEvent FOnSelect;
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall SpectrumImageMouseMove(System::TObject* Sender, Classes::TShiftState Shift, int X, int Y);
	void __fastcall SpectrumImageMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall SpectrumImageMouseUp(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall DrawHotRect(void);
	void __fastcall DrawSelectedRect(void);
	void __fastcall DrawFocusPoint(void);
	void __fastcall SetSelectedColor(const Graphics::TColor Value);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall SetItemsPosition(void);
	void __fastcall SetPanelSize(void);
	
public:
	__fastcall virtual TAdvSmoothColorSpectrumPanel(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothColorSpectrumPanel(void);
	
__published:
	__property Graphics::TColor SelectedColor = {read=FSelectedColor, write=SetSelectedColor, nodefault};
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothColorSpectrumPanel(HWND ParentWindow) : TAdvSmoothCustomSelectorPanel(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothTableSelectorPanel;
class PASCALIMPLEMENTATION TAdvSmoothTableSelectorPanel : public TAdvSmoothCustomSelectorPanel
{
	typedef TAdvSmoothCustomSelectorPanel inherited;
	
private:
	Classes::TNotifyEvent FOnSelect;
	int FSelectedCols;
	int FSelectedRows;
	int FColCount;
	int FRowCount;
	int FCellSize;
	int FLeftMargin;
	int FTopMargin;
	int FCellSpace;
	int FLabelHeight;
	Graphics::TColor FSelectionColor;
	System::UnicodeString FTextTable;
	System::UnicodeString FTextCancel;
	void __fastcall SetColCount(const int Value);
	void __fastcall SetRowCount(const int Value);
	void __fastcall SetSelectedCols(const int Value);
	void __fastcall SetSelectedRows(const int Value);
	void __fastcall SetCellSize(const int Value);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall DrawAllCells(void);
	void __fastcall DrawLabel(void);
	void __fastcall InvalidateSelection(int OldSelectedCols, int OldSelectedRows);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall SetItemsPosition(void);
	void __fastcall SetPanelSize(void);
	
public:
	__fastcall virtual TAdvSmoothTableSelectorPanel(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothTableSelectorPanel(void);
	
__published:
	__property int ColCount = {read=FColCount, write=SetColCount, default=5};
	__property int RowCount = {read=FRowCount, write=SetRowCount, default=4};
	__property int SelectedCols = {read=FSelectedCols, write=SetSelectedCols, default=0};
	__property int SelectedRows = {read=FSelectedRows, write=SetSelectedRows, default=0};
	__property int CellSize = {read=FCellSize, write=SetCellSize, default=16};
	__property Graphics::TColor SelectionColor = {read=FSelectionColor, write=FSelectionColor, default=8388608};
	__property System::UnicodeString TextTable = {read=FTextTable, write=FTextTable};
	__property System::UnicodeString TextCancel = {read=FTextCancel, write=FTextCancel};
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothTableSelectorPanel(HWND ParentWindow) : TAdvSmoothCustomSelectorPanel(ParentWindow) { }
	
};


class DELPHICLASS TSelectorDropDownWindow;
class PASCALIMPLEMENTATION TSelectorDropDownWindow : public Forms::TCustomForm
{
	typedef Forms::TCustomForm inherited;
	
private:
	TAdvSmoothCustomSelectorPanel* FSelectorPanel;
	bool FHideOnDeActivate;
	bool FShowAbove;
	Classes::TComponent* FOwner;
	bool FShowFullBorder;
	Extctrls::TTimer* FHideTimer;
	bool FShowLeft;
	HIDESBASE MESSAGE void __fastcall WMActivate(Messages::TWMActivate &Message);
	HIDESBASE MESSAGE void __fastcall WMNCHitTest(Messages::TWMNCHitTest &Message);
	void __fastcall HideTimerOnTime(System::TObject* Sender);
	
protected:
	DYNAMIC void __fastcall Paint(void);
	HWND __fastcall GetParentWnd(void);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	__property bool HideOnDeActivate = {read=FHideOnDeActivate, write=FHideOnDeActivate, nodefault};
	__property bool ShowAbove = {read=FShowAbove, write=FShowAbove, nodefault};
	__property bool ShowLeft = {read=FShowLeft, write=FShowLeft, default=0};
	__property bool ShowFullBorder = {read=FShowFullBorder, write=FShowFullBorder, nodefault};
	
public:
	__fastcall virtual TSelectorDropDownWindow(Classes::TComponent* AOwner);
	__fastcall virtual TSelectorDropDownWindow(Classes::TComponent* AOwner, int Dummy);
	__fastcall virtual ~TSelectorDropDownWindow(void);
	void __fastcall SetWindowSize(void);
	
__published:
	__property TAdvSmoothCustomSelectorPanel* SelectorPanel = {read=FSelectorPanel, write=FSelectorPanel};
public:
	/* TWinControl.CreateParented */ inline __fastcall TSelectorDropDownWindow(HWND ParentWindow) : Forms::TCustomForm(ParentWindow) { }
	
};


typedef void __fastcall (__closure *TDrawToolEvent)(System::TObject* Sender, Graphics::TCanvas* Canvas, int ItemIndex, const Types::TRect &R);

class DELPHICLASS TAdvSmoothCustomSelector;
class PASCALIMPLEMENTATION TAdvSmoothCustomSelector : public Controls::TCustomControl
{
	typedef Controls::TCustomControl inherited;
	
private:
	TSelectorDropDownWindow* FDropDownWindow;
	TAdvSmoothSelectorPanel* FSelectorPanel;
	bool FMouseInControl;
	bool FMouseDown;
	int FDropDownBtnWidth;
	bool FDown;
	bool FShaded;
	bool FFlat;
	bool FAllowAllUp;
	bool FDropDownButton;
	bool FAutoThemeAdapt;
	TAdvSelectorStyle FStyle;
	Graphics::TBitmap* FGlyphDown;
	Graphics::TBitmap* FGlyphDisabled;
	Graphics::TBitmap* FGlyphHot;
	Graphics::TBitmap* FGlyph;
	Graphics::TBitmap* FGlyphShade;
	Graphics::TColor FBorderColor;
	Graphics::TColor FColorDownTo;
	Graphics::TColor FColorTo;
	Graphics::TColor FColorHotTo;
	Graphics::TColor FColorDown;
	Graphics::TColor FBorderDownColor;
	Graphics::TColor FColorHot;
	Graphics::TColor FColorCheckedTo;
	Graphics::TColor FColorChecked;
	Graphics::TColor FBorderHotColor;
	Imglist::TCustomImageList* FImages;
	Classes::TNotifyEvent FOnMouseEnter;
	Classes::TNotifyEvent FOnDropDown;
	Classes::TNotifyEvent FOnMouseLeave;
	int FGroupIndex;
	TAdvAppearanceStyle FAppearanceStyle;
	TAdvButtonState FState;
	Graphics::TColor FColorDropDownTo;
	Graphics::TColor FColorDropDown;
	Graphics::TColor FBorderDropDownColor;
	int FDropDownCount;
	TGradientDirection FGradientDirection;
	int FSelectedIndex;
	TAdvSmoothSelectorItems* FTools;
	TNoOfButtons FButtonsPerRow;
	Graphics::TColor FColorSelectedTo;
	Graphics::TColor FColorSelected;
	Graphics::TColor FBorderSelectedColor;
	THotToolEvent FOnHotTool;
	Classes::TNotifyEvent FOnClick;
	TSelectEvent FOnSelect;
	Graphics::TColor FColorSelectionHotTo;
	Graphics::TColor FColorSelectionHot;
	Graphics::TColor FColorSelectionDownTo;
	Graphics::TColor FColorSelectionDown;
	TDrawToolEvent FOnDrawTool;
	int FDupSelectedIndex;
	bool FTwoColorImages;
	Graphics::TColor FBackGroundImageColor;
	Graphics::TColor FForeGroundImageColor;
	Graphics::TColor FOldForeGroundImgColor;
	Graphics::TColor FOldBkGroundImgColor;
	bool FStretchImageDraw;
	bool FIsThemed;
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMTextChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall GlyphChanged(System::TObject* Sender);
	void __fastcall OnDropDownWindowHide(System::TObject* Sender);
	void __fastcall OnDropDownPanelShouldHide(System::TObject* Sender);
	void __fastcall OnDropDownPanelSelect(System::TObject* Sender);
	void __fastcall OnDropDownPanelHotTool(System::TObject* Sender, int HotItemIndex);
	void __fastcall OnDropDownPanelDrawTool(System::TObject* Sender, int ItemIndex, const Types::TRect &R);
	void __fastcall PopupBtnDown(void);
	void __fastcall ButtonDown(void);
	void __fastcall DoDropDown(void);
	void __fastcall SetAllowAllUp(const bool Value);
	void __fastcall SetBorderColor(const Graphics::TColor Value);
	void __fastcall SetColorChecked(const Graphics::TColor Value);
	void __fastcall SetColorCheckedTo(const Graphics::TColor Value);
	void __fastcall SetColorTo(const Graphics::TColor Value);
	void __fastcall SetDown(const bool Value);
	void __fastcall SetDropDownButton(const bool Value);
	void __fastcall SetFlat(const bool Value);
	void __fastcall SetGlyph(const Graphics::TBitmap* Value);
	void __fastcall SetGlyphDisabled(const Graphics::TBitmap* Value);
	void __fastcall SetGlyphDown(const Graphics::TBitmap* Value);
	void __fastcall SetGlyphHot(const Graphics::TBitmap* Value);
	void __fastcall SetImages(const Imglist::TCustomImageList* Value);
	void __fastcall SetShaded(const bool Value);
	void __fastcall SetStyle(const TAdvSelectorStyle Value);
	void __fastcall SetGroupIndex(const int Value);
	void __fastcall SetAppearanceStyle(const TAdvAppearanceStyle Value);
	void __fastcall SetColorDropDown(const Graphics::TColor Value);
	void __fastcall SetColorDropDownTo(const Graphics::TColor Value);
	void __fastcall SetDropDownCount(const int Value);
	void __fastcall SetTGradientDirection(const TGradientDirection Value);
	System::UnicodeString __fastcall GetCaption(void);
	void __fastcall SetCaption(const System::UnicodeString Value);
	void __fastcall SetSelectedIndex(const int Value);
	void __fastcall SetTools(const TAdvSmoothSelectorItems* Value);
	int __fastcall GetSelectedIndex(void);
	void __fastcall SetButtonsPerRow(const TNoOfButtons Value);
	System::UnicodeString __fastcall GetVersion(void);
	void __fastcall SetVersion(const System::UnicodeString Value);
	void __fastcall SetTwoColorImages(const bool Value);
	void __fastcall SetForeGroundImageColor(const Graphics::TColor Value);
	void __fastcall SetStretchImageDraw(const bool Value);
	
protected:
	virtual void __fastcall OnToolSelect(void);
	int __fastcall DrawGlyph(Graphics::TBitmap* aGlyph, const Types::TRect &aRect);
	virtual void __fastcall DrawGlyphAndCaption(Graphics::TBitmap* aGlyph, const Types::TRect &R);
	void __fastcall DrawButton(void);
	void __fastcall DrawComboButton(void);
	void __fastcall SetSelectorPanelItems(void);
	virtual void __fastcall SetSelectorPanel(void);
	virtual void __fastcall Loaded(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	virtual void __fastcall Paint(void);
	__property bool MouseInControl = {read=FMouseInControl, nodefault};
	virtual void __fastcall WndProc(Messages::TMessage &Message);
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation AOperation);
	void __fastcall ThemeAdapt(void);
	void __fastcall SetAutoThemeAdapt(const bool Value);
	virtual int __fastcall GetVersionNr(void);
	bool __fastcall DoVisualStyles(void);
	void __fastcall ChangeImagesColor(Graphics::TColor ForeGColor, Graphics::TColor BkGColor);
	__property bool TwoColorImages = {read=FTwoColorImages, write=SetTwoColorImages, nodefault};
	__property Graphics::TColor OldForeGroundImgColor = {read=FOldForeGroundImgColor, nodefault};
	__property Graphics::TColor OldBkGroundImgColor = {read=FOldBkGroundImgColor, nodefault};
	__property Graphics::TColor ForeGroundImageColor = {read=FForeGroundImageColor, write=SetForeGroundImageColor, nodefault};
	__property Graphics::TColor BackGroundImageColor = {read=FBackGroundImageColor, write=FBackGroundImageColor, nodefault};
	__property bool StretchImageDraw = {read=FStretchImageDraw, write=SetStretchImageDraw, default=1};
	__property bool AllowAllUp = {read=FAllowAllUp, write=SetAllowAllUp, default=0};
	__property bool AutoThemeAdapt = {read=FAutoThemeAdapt, write=SetAutoThemeAdapt, nodefault};
	__property TNoOfButtons ButtonsPerRow = {read=FButtonsPerRow, write=SetButtonsPerRow, default=1};
	__property Graphics::TColor BorderColor = {read=FBorderColor, write=SetBorderColor, default=536870911};
	__property Graphics::TColor BorderDownColor = {read=FBorderDownColor, write=FBorderDownColor, default=536870911};
	__property Graphics::TColor BorderHotColor = {read=FBorderHotColor, write=FBorderHotColor, default=536870911};
	__property Graphics::TColor BorderDropDownColor = {read=FBorderDropDownColor, write=FBorderDropDownColor, default=8421504};
	__property Graphics::TColor BorderSelectedColor = {read=FBorderSelectedColor, write=FBorderSelectedColor, default=0};
	__property System::UnicodeString Caption = {read=GetCaption, write=SetCaption};
	__property Color = {default=-16777211};
	__property Graphics::TColor ColorTo = {read=FColorTo, write=SetColorTo, default=536870911};
	__property Graphics::TColor ColorDown = {read=FColorDown, write=FColorDown, nodefault};
	__property Graphics::TColor ColorDownTo = {read=FColorDownTo, write=FColorDownTo, default=536870911};
	__property Graphics::TColor ColorHot = {read=FColorHot, write=FColorHot, nodefault};
	__property Graphics::TColor ColorHotTo = {read=FColorHotTo, write=FColorHotTo, default=536870911};
	__property Graphics::TColor ColorChecked = {read=FColorChecked, write=SetColorChecked, default=8421504};
	__property Graphics::TColor ColorCheckedTo = {read=FColorCheckedTo, write=SetColorCheckedTo, default=536870911};
	__property Graphics::TColor ColorDropDown = {read=FColorDropDown, write=SetColorDropDown, nodefault};
	__property Graphics::TColor ColorDropDownTo = {read=FColorDropDownTo, write=SetColorDropDownTo, default=536870911};
	__property Graphics::TColor ColorSelected = {read=FColorSelected, write=FColorSelected, nodefault};
	__property Graphics::TColor ColorSelectedTo = {read=FColorSelectedTo, write=FColorSelectedTo, nodefault};
	__property Graphics::TColor ColorSelectionHot = {read=FColorSelectionHot, write=FColorSelectionHot, nodefault};
	__property Graphics::TColor ColorSelectionHotTo = {read=FColorSelectionHotTo, write=FColorSelectionHotTo, nodefault};
	__property Graphics::TColor ColorSelectionDown = {read=FColorSelectionDown, write=FColorSelectionDown, nodefault};
	__property Graphics::TColor ColorSelectionDownTo = {read=FColorSelectionDownTo, write=FColorSelectionDownTo, nodefault};
	__property bool Down = {read=FDown, write=SetDown, default=0};
	__property bool DropDownButton = {read=FDropDownButton, write=SetDropDownButton, default=0};
	__property int DropDownCount = {read=FDropDownCount, write=SetDropDownCount, nodefault};
	__property bool Flat = {read=FFlat, write=SetFlat, default=1};
	__property int GroupIndex = {read=FGroupIndex, write=SetGroupIndex, default=0};
	__property Graphics::TBitmap* Glyph = {read=FGlyph, write=SetGlyph};
	__property Graphics::TBitmap* GlyphHot = {read=FGlyphHot, write=SetGlyphHot};
	__property Graphics::TBitmap* GlyphDown = {read=FGlyphDown, write=SetGlyphDown};
	__property Graphics::TBitmap* GlyphDisabled = {read=FGlyphDisabled, write=SetGlyphDisabled};
	__property TGradientDirection GradientDirection = {read=FGradientDirection, write=SetTGradientDirection, nodefault};
	__property Imglist::TCustomImageList* Images = {read=FImages, write=SetImages};
	__property bool Shaded = {read=FShaded, write=SetShaded, default=1};
	__property TAdvSelectorStyle Style = {read=FStyle, write=SetStyle, nodefault};
	__property TAdvButtonState State = {read=FState, write=FState, nodefault};
	__property TAdvSmoothSelectorItems* Tools = {read=FTools, write=SetTools};
	__property int SelectedIndex = {read=GetSelectedIndex, write=SetSelectedIndex, nodefault};
	__property Classes::TNotifyEvent OnMouseEnter = {read=FOnMouseEnter, write=FOnMouseEnter};
	__property Classes::TNotifyEvent OnMouseLeave = {read=FOnMouseLeave, write=FOnMouseLeave};
	__property Classes::TNotifyEvent OnDropDown = {read=FOnDropDown, write=FOnDropDown};
	__property TSelectEvent OnSelect = {read=FOnSelect, write=FOnSelect};
	__property THotToolEvent OnHotTool = {read=FOnHotTool, write=FOnHotTool};
	__property TDrawToolEvent OnDrawTool = {read=FOnDrawTool, write=FOnDrawTool};
	__property Classes::TNotifyEvent OnClick = {read=FOnClick, write=FOnClick};
	
public:
	__fastcall virtual TAdvSmoothCustomSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothCustomSelector(void);
	DYNAMIC void __fastcall Click(void);
	void __fastcall ShowDropDown(void);
	void __fastcall HideDropDown(void);
	
__published:
	__property Action;
	__property TabOrder = {default=-1};
	__property TabStop = {default=0};
	__property TAdvAppearanceStyle AppearanceStyle = {read=FAppearanceStyle, write=SetAppearanceStyle, default=0};
	__property System::UnicodeString Version = {read=GetVersion, write=SetVersion};
	__property Visible = {default=1};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothCustomSelector(HWND ParentWindow) : Controls::TCustomControl(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothToolSelector;
class PASCALIMPLEMENTATION TAdvSmoothToolSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
public:
	__property AppearanceStyle = {default=0};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property ButtonsPerRow = {default=1};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property Images;
	__property Style;
	__property ShowHint;
	__property SelectedIndex;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TAdvSmoothCustomSelector.Create */ inline __fastcall virtual TAdvSmoothToolSelector(Classes::TComponent* AOwner) : TAdvSmoothCustomSelector(AOwner) { }
	/* TAdvSmoothCustomSelector.Destroy */ inline __fastcall virtual ~TAdvSmoothToolSelector(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothToolSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


#pragma option push -b-
enum TSelectionType { stOffice, stBorland };
#pragma option pop

class DELPHICLASS TAdvSmoothPenStyleSelector;
class PASCALIMPLEMENTATION TAdvSmoothPenStyleSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	TSelectionType FSelectionType;
	void __fastcall Initialize(void);
	Graphics::TPenStyle __fastcall GetSelectedPenStyle(void);
	void __fastcall SetSelectedPenStyle(const Graphics::TPenStyle Value);
	void __fastcall SetSelectionType(const TSelectionType Value);
	Graphics::TColor __fastcall GetPenColor(void);
	void __fastcall SetPenColor(const Graphics::TColor Value);
	
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall SetSelectorPanel(void);
	Graphics::TPenStyle __fastcall GetPenStyleAtIndex(int Index);
	int __fastcall GetIndexOfStyle(Graphics::TPenStyle APenStyle);
	
public:
	__fastcall virtual TAdvSmoothPenStyleSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothPenStyleSelector(void);
	__property AppearanceStyle = {default=0};
	__property Graphics::TPenStyle SelectedPenStyle = {read=GetSelectedPenStyle, write=SetSelectedPenStyle, nodefault};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property Graphics::TColor PenColor = {read=GetPenColor, write=SetPenColor, nodefault};
	__property SelectedIndex;
	__property TSelectionType SelectionType = {read=FSelectionType, write=SetSelectionType, default=0};
	__property ShowHint;
	__property Style;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothPenStyleSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothPenWidthSelector;
class PASCALIMPLEMENTATION TAdvSmoothPenWidthSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	void __fastcall Initialize(void);
	
protected:
	virtual void __fastcall SetSelectorPanel(void);
	
public:
	__fastcall virtual TAdvSmoothPenWidthSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothPenWidthSelector(void);
	__property AppearanceStyle = {default=0};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property Style;
	__property SelectedIndex;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothPenWidthSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothBrushStyleSelector;
class PASCALIMPLEMENTATION TAdvSmoothBrushStyleSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	TSelectionType FSelectionType;
	void __fastcall Initialize(void);
	Graphics::TBrushStyle __fastcall GetSelectedBrushStyle(void);
	void __fastcall SetSelectedBrushStyle(const Graphics::TBrushStyle Value);
	Graphics::TColor __fastcall GetBrushColor(void);
	void __fastcall SetBrushColor(const Graphics::TColor Value);
	void __fastcall SetSelectionType(const TSelectionType Value);
	
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall SetSelectorPanel(void);
	Graphics::TBrushStyle __fastcall GetBrushStyleAtIndex(int Index);
	int __fastcall GetIndexOfBrushStyle(Graphics::TBrushStyle ABrushStyle);
	
public:
	__fastcall virtual TAdvSmoothBrushStyleSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothBrushStyleSelector(void);
	__property AppearanceStyle = {default=0};
	__property Graphics::TBrushStyle SelectedBrushStyle = {read=GetSelectedBrushStyle, write=SetSelectedBrushStyle, nodefault};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Graphics::TColor BrushColor = {read=GetBrushColor, write=SetBrushColor, nodefault};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property TSelectionType SelectionType = {read=FSelectionType, write=SetSelectionType, default=0};
	__property SelectedIndex;
	__property ShowHint;
	__property Style;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothBrushStyleSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothShadowSelector;
class PASCALIMPLEMENTATION TAdvSmoothShadowSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	void __fastcall Initialize(void);
	
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall SetSelectorPanel(void);
	
public:
	__fastcall virtual TAdvSmoothShadowSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothShadowSelector(void);
	__property AppearanceStyle = {default=0};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property Style;
	__property SelectedIndex;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothShadowSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothTableBorderSelector;
class PASCALIMPLEMENTATION TAdvSmoothTableBorderSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	void __fastcall Initialize(void);
	
protected:
	virtual void __fastcall SetSelectorPanel(void);
	
public:
	__fastcall virtual TAdvSmoothTableBorderSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothTableBorderSelector(void);
	__property AppearanceStyle = {default=0};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property Style;
	__property ShowHint;
	__property SelectedIndex;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
	__property OnDrawTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothTableBorderSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothGradientDirectionSelector;
class PASCALIMPLEMENTATION TAdvSmoothGradientDirectionSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	Graphics::TColor FStartColor;
	Graphics::TColor FEndColor;
	bool FShowSelectedGradient;
	void __fastcall SetEndColor(const Graphics::TColor Value);
	void __fastcall SetStartColor(const Graphics::TColor Value);
	void __fastcall SelectorPanelOnDrawItem(System::TObject* Sender, int Index, const Types::TRect &R);
	void __fastcall Initialize(void);
	void __fastcall SetShowSelectedGradient(const bool Value);
	
protected:
	virtual void __fastcall SetSelectorPanel(void);
	virtual void __fastcall DrawGlyphAndCaption(Graphics::TBitmap* aGlyph, const Types::TRect &R);
	
public:
	__fastcall virtual TAdvSmoothGradientDirectionSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothGradientDirectionSelector(void);
	__property AppearanceStyle = {default=0};
	
__published:
	__property Graphics::TColor StartColor = {read=FStartColor, write=SetStartColor, nodefault};
	__property Graphics::TColor EndColor = {read=FEndColor, write=SetEndColor, nodefault};
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property Style;
	__property SelectedIndex;
	__property bool ShowSelectedGradient = {read=FShowSelectedGradient, write=SetShowSelectedGradient, default=0};
	__property Tools;
	__property ShowHint;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
	__property OnHotTool;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothGradientDirectionSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothCustomColorSelector;
class PASCALIMPLEMENTATION TAdvSmoothCustomColorSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	TAdvSmoothColorCubePanel* FColorCubePanel;
	TAdvSmoothColorSpectrumPanel* FSpectrumPanel;
	TColorSelectionStyle FColorSelectionStyle;
	bool FShowSelectedColor;
	Graphics::TColor FSelectedColor;
	bool FShowRGBHint;
	void __fastcall SetColorSelectionStyle(const TColorSelectionStyle Value);
	void __fastcall SelectorPanelOnDrawItem(System::TObject* Sender, int Index, const Types::TRect &R);
	void __fastcall SetShowSelectedColor(const bool Value);
	void __fastcall CubePanelOnSelect(System::TObject* Sender);
	void __fastcall SpectrumPanelOnSelect(System::TObject* Sender);
	void __fastcall SetSelectedColor(const Graphics::TColor Value);
	Graphics::TColor __fastcall GetSelectedColor(void);
	
protected:
	virtual void __fastcall Initialize(void);
	virtual void __fastcall Loaded(void);
	virtual void __fastcall OnToolSelect(void);
	virtual void __fastcall SetSelectorPanel(void);
	virtual void __fastcall DrawGlyphAndCaption(Graphics::TBitmap* aGlyph, const Types::TRect &R);
	
public:
	__fastcall virtual TAdvSmoothCustomColorSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothCustomColorSelector(void);
	__property AppearanceStyle = {default=0};
	__property ButtonsPerRow = {default=1};
	__property TColorSelectionStyle SelectionStyle = {read=FColorSelectionStyle, write=SetColorSelectionStyle, default=0};
	__property bool ShowSelectedColor = {read=FShowSelectedColor, write=SetShowSelectedColor, default=1};
	__property Graphics::TColor SelectedColor = {read=GetSelectedColor, write=SetSelectedColor, nodefault};
	__property bool ShowRGBHint = {read=FShowRGBHint, write=FShowRGBHint, nodefault};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothCustomColorSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothColorSelector;
class PASCALIMPLEMENTATION TAdvSmoothColorSelector : public TAdvSmoothCustomColorSelector
{
	typedef TAdvSmoothCustomColorSelector inherited;
	
protected:
	virtual void __fastcall Initialize(void);
	
public:
	__property SelectedIndex;
	
__published:
	__property SelectionStyle = {default=0};
	__property ShowSelectedColor = {default=1};
	__property SelectedColor;
	__property ShowRGBHint;
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property Style;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
public:
	/* TAdvSmoothCustomColorSelector.Create */ inline __fastcall virtual TAdvSmoothColorSelector(Classes::TComponent* AOwner) : TAdvSmoothCustomColorSelector(AOwner) { }
	/* TAdvSmoothCustomColorSelector.Destroy */ inline __fastcall virtual ~TAdvSmoothColorSelector(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothColorSelector(HWND ParentWindow) : TAdvSmoothCustomColorSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothTextColorSelector;
class PASCALIMPLEMENTATION TAdvSmoothTextColorSelector : public TAdvSmoothCustomColorSelector
{
	typedef TAdvSmoothCustomColorSelector inherited;
	
protected:
	virtual void __fastcall Initialize(void);
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property ColorSelected;
	__property ColorSelectedTo;
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property ShowSelectedColor = {default=1};
	__property SelectedColor;
	__property Style;
	__property Tools;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
public:
	/* TAdvSmoothCustomColorSelector.Create */ inline __fastcall virtual TAdvSmoothTextColorSelector(Classes::TComponent* AOwner) : TAdvSmoothCustomColorSelector(AOwner) { }
	/* TAdvSmoothCustomColorSelector.Destroy */ inline __fastcall virtual ~TAdvSmoothTextColorSelector(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothTextColorSelector(HWND ParentWindow) : TAdvSmoothCustomColorSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothCustomTableSelector;
class PASCALIMPLEMENTATION TAdvSmoothCustomTableSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	TAdvSmoothTableSelectorPanel* FTableSelectorPanel;
	int FSelectedColumns;
	int FSelectedRows;
	int FDefaultRowCount;
	int FDefaultColCount;
	Classes::TNotifyEvent FOnSelect;
	System::UnicodeString FTextTable;
	System::UnicodeString FTextCancel;
	void __fastcall SetDefaultColCount(const int Value);
	void __fastcall SetDefaultRowCount(const int Value);
	void __fastcall TableSelectorOnSelect(System::TObject* Sender);
	
protected:
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	virtual void __fastcall Loaded(void);
	virtual void __fastcall SetSelectorPanel(void);
	__property int DefaultColCount = {read=FDefaultColCount, write=SetDefaultColCount, default=5};
	__property int DefaultRowCount = {read=FDefaultRowCount, write=SetDefaultRowCount, default=4};
	__property int SelectedColumns = {read=FSelectedColumns, default=0};
	__property int SelectedRows = {read=FSelectedRows, default=0};
	
public:
	__fastcall virtual TAdvSmoothCustomTableSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothCustomTableSelector(void);
	__property System::UnicodeString TextTable = {read=FTextTable, write=FTextTable};
	__property System::UnicodeString TextCancel = {read=FTextCancel, write=FTextCancel};
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothCustomTableSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothTableSelector;
class PASCALIMPLEMENTATION TAdvSmoothTableSelector : public TAdvSmoothCustomTableSelector
{
	typedef TAdvSmoothCustomTableSelector inherited;
	
public:
	__property SelectedColumns = {default=0};
	__property SelectedRows = {default=0};
	__property AppearanceStyle = {default=0};
	
__published:
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property DefaultColCount = {default=5};
	__property DefaultRowCount = {default=4};
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property Style;
	__property TextTable;
	__property TextCancel;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property OnSelect;
public:
	/* TAdvSmoothCustomTableSelector.Create */ inline __fastcall virtual TAdvSmoothTableSelector(Classes::TComponent* AOwner) : TAdvSmoothCustomTableSelector(AOwner) { }
	/* TAdvSmoothCustomTableSelector.Destroy */ inline __fastcall virtual ~TAdvSmoothTableSelector(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothTableSelector(HWND ParentWindow) : TAdvSmoothCustomTableSelector(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothCharacterSelector;
class PASCALIMPLEMENTATION TAdvSmoothCharacterSelector : public TAdvSmoothCustomSelector
{
	typedef TAdvSmoothCustomSelector inherited;
	
private:
	Classes::TNotifyEvent FOnSelect;
	System::UnicodeString FCharacters;
	System::WideChar FSelectedChar;
	bool FAutoLoad;
	Graphics::TFont* FCharFont;
	void __fastcall AddItemsFromChars(void);
	void __fastcall LoadCharFromFont(void);
	void __fastcall SetCharacters(const System::UnicodeString Value);
	void __fastcall SetSelectedChar(const System::WideChar Value);
	void __fastcall SetAutoLoad(const bool Value);
	void __fastcall SetCharFont(const Graphics::TFont* Value);
	
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall OnToolSelect(void);
	virtual void __fastcall SetSelectorPanel(void);
	virtual void __fastcall DrawGlyphAndCaption(Graphics::TBitmap* AGlyph, const Types::TRect &R);
	
public:
	__fastcall virtual TAdvSmoothCharacterSelector(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothCharacterSelector(void);
	__property AppearanceStyle = {default=0};
	__property System::WideChar SelectedChar = {read=FSelectedChar, write=SetSelectedChar, nodefault};
	__property Tools;
	
__published:
	__property bool AutoLoad = {read=FAutoLoad, write=SetAutoLoad, default=1};
	__property AutoThemeAdapt;
	__property BorderColor = {default=536870911};
	__property BorderDownColor = {default=536870911};
	__property BorderHotColor = {default=536870911};
	__property BorderDropDownColor = {default=8421504};
	__property ButtonsPerRow = {default=1};
	__property Caption;
	__property Color = {default=-16777211};
	__property ColorTo = {default=536870911};
	__property ColorDown;
	__property ColorDownTo = {default=536870911};
	__property ColorHot;
	__property ColorHotTo = {default=536870911};
	__property ColorDropDown;
	__property ColorDropDownTo = {default=536870911};
	__property System::UnicodeString Characters = {read=FCharacters, write=SetCharacters};
	__property Graphics::TFont* CharFont = {read=FCharFont, write=SetCharFont};
	__property DropDownButton = {default=0};
	__property Glyph;
	__property GlyphHot;
	__property GlyphDown;
	__property ShowHint;
	__property Style;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnDropDown;
	__property OnClick;
	__property Classes::TNotifyEvent OnSelect = {read=FOnSelect, write=FOnSelect};
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothCharacterSelector(HWND ParentWindow) : TAdvSmoothCustomSelector(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
static const System::ShortInt MINBUTTONSIZE = 0x10;
static const System::ShortInt TABLECELLSIZE = 0x18;
static const System::ShortInt MAJ_VER = 0x1;
static const System::ShortInt MIN_VER = 0x3;
static const System::ShortInt REL_VER = 0x5;
static const System::ShortInt BLD_VER = 0x0;

}	/* namespace Advsmoothselectors */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothselectors;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothselectorsHPP
