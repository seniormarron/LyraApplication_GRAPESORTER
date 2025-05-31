// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothStepControl.pas' rev: 22.00

#ifndef AdvsmoothstepcontrolHPP
#define AdvsmoothstepcontrolHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <AdvStyleIF.hpp>	// Pascal unit
#include <GDIPFill.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <GDIPPictureContainer.hpp>	// Pascal unit
#include <ImgList.hpp>	// Pascal unit
#include <Math.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <AdvGDIP.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothstepcontrol
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TStepActionAppearance;
class DELPHICLASS TAdvSmoothStepControl;
class PASCALIMPLEMENTATION TStepActionAppearance : public Classes::TPersistent
{
	typedef Classes::TPersistent inherited;
	
private:
	TAdvSmoothStepControl* FOwner;
	Graphics::TColor FShapeColor;
	Graphics::TColor FCaptionColor;
	Graphics::TColor FDescriptionColor;
	Gdipfill::TGDIPFill* FBackGroundFill;
	void __fastcall SetCaptionColor(const Graphics::TColor Value);
	void __fastcall SetDescriptionColor(const Graphics::TColor Value);
	void __fastcall SetBackGroundFill(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetShapeColor(const Graphics::TColor Value);
	
protected:
	void __fastcall Changed(void);
	void __fastcall FillChanged(System::TObject* Sender);
	
public:
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	__fastcall TStepActionAppearance(TAdvSmoothStepControl* AOwner);
	__fastcall virtual ~TStepActionAppearance(void);
	
__published:
	__property Graphics::TColor ShapeColor = {read=FShapeColor, write=SetShapeColor, default=32768};
	__property Graphics::TColor CaptionColor = {read=FCaptionColor, write=SetCaptionColor, default=0};
	__property Graphics::TColor DescriptionColor = {read=FDescriptionColor, write=SetDescriptionColor, default=8421504};
	__property Gdipfill::TGDIPFill* BackGroundFill = {read=FBackGroundFill, write=SetBackGroundFill};
};


#pragma option push -b-
enum TStepActionsLayout { salPointer, salSquare, salProgress, salNone };
#pragma option pop

#pragma option push -b-
enum TStepActionsShape { sasCircle, sasSquare, sasTriangle, sasCustom, sasNone };
#pragma option pop

#pragma option push -b-
enum TStepActionsDescriptionLayout { dlNormal, dlBottom };
#pragma option pop

class DELPHICLASS TStepActionsAppearance;
class PASCALIMPLEMENTATION TStepActionsAppearance : public Classes::TPersistent
{
	typedef Classes::TPersistent inherited;
	
private:
	TAdvSmoothStepControl* FOwner;
	Graphics::TFont* FProcessedCaptionFont;
	Graphics::TFont* FActiveCaptionFont;
	Graphics::TFont* FInActiveCaptionFont;
	TStepActionAppearance* FProcessedAppearance;
	TStepActionAppearance* FActiveAppearance;
	TStepActionAppearance* FInActiveAppearance;
	TStepActionsLayout FLayout;
	Graphics::TFont* FInActiveDescriptionFont;
	Graphics::TFont* FProcessedDescriptionFont;
	Graphics::TFont* FActiveDescriptionFont;
	int FLayoutRounding;
	int FLayoutSpacingVertical;
	int FLayoutSpacingHorizontal;
	int FProgressSize;
	Gdipfill::TGDIPFill* FProgressBackGround;
	Gdipfill::TGDIPFill* FProgress;
	int FSeparatorWidth;
	Gdipfill::TGDIPFill* FSeparatorFill;
	int FShapeSize;
	TStepActionsShape FShapeStyle;
	int FShapeBorderWidth;
	TStepActionsDescriptionLayout FDescriptionLayout;
	int FDescriptionSize;
	Gdipfill::TGDIPFill* FDescriptionFill;
	void __fastcall SetActiveCaptionFont(const Graphics::TFont* Value);
	void __fastcall SetInActiveCaptionFont(const Graphics::TFont* Value);
	void __fastcall SetProcessedCaptionFont(const Graphics::TFont* Value);
	void __fastcall SetActiveAppearance(const TStepActionAppearance* Value);
	void __fastcall SetInActiveAppearance(const TStepActionAppearance* Value);
	void __fastcall SetProcessedAppearance(const TStepActionAppearance* Value);
	void __fastcall SetLayout(const TStepActionsLayout Value);
	void __fastcall SetActiveDescriptionFont(const Graphics::TFont* Value);
	void __fastcall SetInActiveDescriptionFont(const Graphics::TFont* Value);
	void __fastcall SetProcessedDescriptionFont(const Graphics::TFont* Value);
	void __fastcall SetLayoutRounding(const int Value);
	void __fastcall SetLayoutSpacingHorizontal(const int Value);
	void __fastcall SetLayoutSpacingVertical(const int Value);
	void __fastcall SetProgressSize(const int Value);
	void __fastcall SetProgress(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetProgressBackGround(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetSeparatorFill(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetSeparatorWidth(const int Value);
	void __fastcall SetShapeBorderWidth(const int Value);
	void __fastcall SetShapeSize(const int Value);
	void __fastcall SetShapeStyle(const TStepActionsShape Value);
	void __fastcall SetDescriptionLayout(const TStepActionsDescriptionLayout Value);
	void __fastcall SetDescriptionFill(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetDescriptionSize(const int Value);
	
protected:
	void __fastcall Changed(void);
	void __fastcall FontChanged(System::TObject* Sender);
	void __fastcall FillChanged(System::TObject* Sender);
	
public:
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	__fastcall TStepActionsAppearance(TAdvSmoothStepControl* AOwner);
	__fastcall virtual ~TStepActionsAppearance(void);
	
__published:
	__property Graphics::TFont* InActiveCaptionFont = {read=FInActiveCaptionFont, write=SetInActiveCaptionFont};
	__property Graphics::TFont* ActiveCaptionFont = {read=FActiveCaptionFont, write=SetActiveCaptionFont};
	__property Graphics::TFont* ProcessedCaptionFont = {read=FProcessedCaptionFont, write=SetProcessedCaptionFont};
	__property Graphics::TFont* InActiveDescriptionFont = {read=FInActiveDescriptionFont, write=SetInActiveDescriptionFont};
	__property Graphics::TFont* ActiveDescriptionFont = {read=FActiveDescriptionFont, write=SetActiveDescriptionFont};
	__property Graphics::TFont* ProcessedDescriptionFont = {read=FProcessedDescriptionFont, write=SetProcessedDescriptionFont};
	__property TStepActionAppearance* ActiveAppearance = {read=FActiveAppearance, write=SetActiveAppearance};
	__property TStepActionAppearance* InActiveAppearance = {read=FInActiveAppearance, write=SetInActiveAppearance};
	__property TStepActionAppearance* ProcessedAppearance = {read=FProcessedAppearance, write=SetProcessedAppearance};
	__property TStepActionsLayout Layout = {read=FLayout, write=SetLayout, default=0};
	__property int ShapeSize = {read=FShapeSize, write=SetShapeSize, default=40};
	__property TStepActionsShape ShapeStyle = {read=FShapeStyle, write=SetShapeStyle, default=0};
	__property int ShapeBorderWidth = {read=FShapeBorderWidth, write=SetShapeBorderWidth, default=5};
	__property int LayoutSpacingVertical = {read=FLayoutSpacingVertical, write=SetLayoutSpacingVertical, default=0};
	__property int LayoutSpacingHorizontal = {read=FLayoutSpacingHorizontal, write=SetLayoutSpacingHorizontal, default=0};
	__property int LayoutRounding = {read=FLayoutRounding, write=SetLayoutRounding, default=0};
	__property int ProgressSize = {read=FProgressSize, write=SetProgressSize, default=25};
	__property Gdipfill::TGDIPFill* ProgressBackGround = {read=FProgressBackGround, write=SetProgressBackGround};
	__property Gdipfill::TGDIPFill* Progress = {read=FProgress, write=SetProgress};
	__property Gdipfill::TGDIPFill* SeparatorFill = {read=FSeparatorFill, write=SetSeparatorFill};
	__property int SeparatorWidth = {read=FSeparatorWidth, write=SetSeparatorWidth, default=5};
	__property TStepActionsDescriptionLayout DescriptionLayout = {read=FDescriptionLayout, write=SetDescriptionLayout, default=0};
	__property Gdipfill::TGDIPFill* DescriptionFill = {read=FDescriptionFill, write=SetDescriptionFill};
	__property int DescriptionSize = {read=FDescriptionSize, write=SetDescriptionSize, default=50};
};


#pragma option push -b-
enum TStepActionTextLocation { tlTopLeft, tlTopCenter, tlTopRight, tlCenterLeft, tlCenterCenter, tlCenterRight, tlBottomLeft, tlBottomCenter, tlBottomRight, tlCustom };
#pragma option pop

class DELPHICLASS TStepActionContentHTMLText;
class DELPHICLASS TStepActionContent;
class PASCALIMPLEMENTATION TStepActionContentHTMLText : public Classes::TPersistent
{
	typedef Classes::TPersistent inherited;
	
private:
	TStepActionContent* FOwner;
	Graphics::TColor FURLColor;
	int FShadowOffset;
	Graphics::TFont* FFont;
	System::UnicodeString FText;
	Graphics::TColor FShadowColor;
	Classes::TNotifyEvent FOnChange;
	TStepActionTextLocation FLocation;
	int FTop;
	int FLeft;
	void __fastcall SetFont(const Graphics::TFont* Value);
	void __fastcall SetLeft(const int Value);
	void __fastcall SetLocation(const TStepActionTextLocation Value);
	void __fastcall SetShadowColor(const Graphics::TColor Value);
	void __fastcall SetShadowOffset(const int Value);
	void __fastcall SetText(const System::UnicodeString Value);
	void __fastcall SetTop(const int Value);
	void __fastcall SetURLColor(const Graphics::TColor Value);
	
protected:
	void __fastcall Changed(void);
	
public:
	__fastcall TStepActionContentHTMLText(TStepActionContent* AOwner);
	__fastcall virtual ~TStepActionContentHTMLText(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	
__published:
	__property System::UnicodeString Text = {read=FText, write=SetText};
	__property TStepActionTextLocation Location = {read=FLocation, write=SetLocation, default=0};
	__property int Top = {read=FTop, write=SetTop, default=0};
	__property int Left = {read=FLeft, write=SetLeft, default=0};
	__property Graphics::TColor URLColor = {read=FURLColor, write=SetURLColor, default=16711680};
	__property Graphics::TColor ShadowColor = {read=FShadowColor, write=SetShadowColor, default=8421504};
	__property int ShadowOffset = {read=FShadowOffset, write=SetShadowOffset, default=5};
	__property Graphics::TFont* Font = {read=FFont, write=SetFont};
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
};


class DELPHICLASS TStepAction;
class PASCALIMPLEMENTATION TStepActionContent : public Classes::TPersistent
{
	typedef Classes::TPersistent inherited;
	
private:
	TStepAction* FOwner;
	System::UnicodeString FImageName;
	System::UnicodeString FCaption;
	TStepActionContentHTMLText* FDescription;
	int FImageIndex;
	System::UnicodeString FHint;
	TStepActionTextLocation FCaptionLocation;
	int FCaptionTop;
	int FCaptionLeft;
	void __fastcall SetCaption(const System::UnicodeString Value);
	void __fastcall SetDescription(const TStepActionContentHTMLText* Value);
	void __fastcall SetImageIndex(const int Value);
	void __fastcall SetImageName(const System::UnicodeString Value);
	void __fastcall SetHint(const System::UnicodeString Value);
	void __fastcall SetCaptionLocation(const TStepActionTextLocation Value);
	void __fastcall SetCaptionLeft(const int Value);
	void __fastcall SetCaptionTop(const int Value);
	
protected:
	void __fastcall Changed(void);
	
public:
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	__fastcall TStepActionContent(TStepAction* AOwner);
	__fastcall virtual ~TStepActionContent(void);
	System::UnicodeString __fastcall GetAnchorAt(Advgdip::TGPGraphics* g, int X, int Y, const Advgdip::TGPRectF &R, Graphics::TFont* ft);
	System::UnicodeString __fastcall DrawHTMLText(Advgdip::TGPGraphics* g, TStepActionContentHTMLText* HTML, const Advgdip::TGPRectF &r, Graphics::TFont* ft, System::UnicodeString str, bool DoAnchor = false, int fX = 0xffffffff, int fY = 0xffffffff);
	
__published:
	__property System::UnicodeString Caption = {read=FCaption, write=SetCaption};
	__property int CaptionLeft = {read=FCaptionLeft, write=SetCaptionLeft, default=0};
	__property int CaptionTop = {read=FCaptionTop, write=SetCaptionTop, default=0};
	__property TStepActionTextLocation CaptionLocation = {read=FCaptionLocation, write=SetCaptionLocation, default=6};
	__property TStepActionContentHTMLText* Description = {read=FDescription, write=SetDescription};
	__property int ImageIndex = {read=FImageIndex, write=SetImageIndex, default=-1};
	__property System::UnicodeString ImageName = {read=FImageName, write=SetImageName};
	__property System::UnicodeString Hint = {read=FHint, write=SetHint};
};


class PASCALIMPLEMENTATION TStepAction : public Classes::TCollectionItem
{
	typedef Classes::TCollectionItem inherited;
	
private:
	Advgdip::TGPRectF FItemRect;
	Advgdip::TGPRectF FItemRectOrig;
	TAdvSmoothStepControl* FOwner;
	System::TObject* FItemObject;
	int FTag;
	TStepActionContent* FProcessedContent;
	TStepActionContent* FActiveContent;
	TStepActionContent* FInActiveContent;
	void __fastcall SetActiveContent(const TStepActionContent* Value);
	void __fastcall SetInActiveContent(const TStepActionContent* Value);
	void __fastcall SetProcessedContent(const TStepActionContent* Value);
	
protected:
	HIDESBASE void __fastcall Changed(void);
	void __fastcall Draw(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &r);
	
public:
	__fastcall virtual TStepAction(Classes::TCollection* Collection);
	__fastcall virtual ~TStepAction(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	Advgdip::TGPRectF __fastcall GetHTMLRect(const Advgdip::TGPRectF &R);
	System::UnicodeString __fastcall GetAnchorAt(int X, int Y);
	
__published:
	__property TStepActionContent* ActiveContent = {read=FActiveContent, write=SetActiveContent};
	__property TStepActionContent* InActiveContent = {read=FInActiveContent, write=SetInActiveContent};
	__property TStepActionContent* ProcessedContent = {read=FProcessedContent, write=SetProcessedContent};
	__property System::TObject* ItemObject = {read=FItemObject, write=FItemObject};
	__property int Tag = {read=FTag, write=FTag, nodefault};
};


class DELPHICLASS TStepActions;
class PASCALIMPLEMENTATION TStepActions : public Classes::TCollection
{
	typedef Classes::TCollection inherited;
	
public:
	TStepAction* operator[](int Index) { return Items[Index]; }
	
private:
	TAdvSmoothStepControl* FOwner;
	Classes::TNotifyEvent FOnChange;
	HIDESBASE TStepAction* __fastcall GetItem(int Index);
	HIDESBASE void __fastcall SetItem(int Index, const TStepAction* Value);
	
protected:
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	
public:
	__fastcall TStepActions(TAdvSmoothStepControl* AOwner);
	HIDESBASE TStepAction* __fastcall Add(void);
	HIDESBASE TStepAction* __fastcall Insert(int Index);
	__property TStepAction* Items[int Index] = {read=GetItem, write=SetItem/*, default*/};
	HIDESBASE void __fastcall Delete(int Index);
	
__published:
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
public:
	/* TCollection.Destroy */ inline __fastcall virtual ~TStepActions(void) { }
	
};


#pragma option push -b-
enum TStepMode { mActive, mProcessed, mInactive };
#pragma option pop

typedef void __fastcall (__closure *TStepClickEvent)(System::TObject* Sender, int StepIndex, TStepMode StepMode);

typedef void __fastcall (__closure *TStepMouseDownEvent)(System::TObject* Sender, int StepIndex, TStepMode StepMode, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);

typedef void __fastcall (__closure *TStepMouseMoveEvent)(System::TObject* Sender, int StepIndex, TStepMode StepMode, Classes::TShiftState Shift, int X, int Y);

typedef void __fastcall (__closure *TStepMouseUpEvent)(System::TObject* Sender, int StepIndex, TStepMode StepMode, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);

typedef void __fastcall (__closure *TStepHintEvent)(System::TObject* Sender, int StepIndex, TStepMode StepMode, System::UnicodeString &Hint);

typedef void __fastcall (__closure *TStepShapeDrawEvent)(System::TObject* Sender, int StepIndex, Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &InnerRect, const Advgdip::TGPRectF &Rect);

typedef void __fastcall (__closure *TStepDrawEvent)(System::TObject* Sender, int StepIndex, Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &Rect);

typedef void __fastcall (__closure *TStepAnchorEvent)(System::TObject* Sender, int StepIndex, System::UnicodeString Anchor);

class PASCALIMPLEMENTATION TAdvSmoothStepControl : public Controls::TCustomControl
{
	typedef Controls::TCustomControl inherited;
	
private:
	int FUpdateCount;
	float FCurPos;
	Extctrls::TTimer* FAnimate;
	bool FConstructed;
	bool FDesignTime;
	bool FLastDesignChoice;
	int FMouseMoveIdx;
	bool FFocused;
	Advgdip::TAntiAlias FAntiAlias;
	bool FTransparent;
	Gdipfill::TGDIPFill* FFill;
	TStepActions* FStepActions;
	TStepActionsAppearance* FStepActionsAppearance;
	Gdippicturecontainer::TGDIPPictureContainer* FPictureContainer;
	Imglist::TCustomImageList* FImageList;
	int FActiveStep;
	bool FReadOnly;
	int FVisibleSteps;
	bool FShowFocus;
	TStepMouseMoveEvent FOnStepMouseMove;
	TStepHintEvent FOnStepHint;
	TStepMouseDownEvent FOnStepMouseDown;
	TStepMouseUpEvent FOnStepMouseUp;
	TStepClickEvent FOnStepClick;
	TStepShapeDrawEvent FOnStepShapeDraw;
	TStepDrawEvent FOnStepDraw;
	TStepAnchorEvent FOnStepAnchor;
	bool FAnimation;
	void __fastcall SetAntiAlias(const Advgdip::TAntiAlias Value);
	void __fastcall SetFill(const Gdipfill::TGDIPFill* Value);
	void __fastcall SetTransparent(const bool Value);
	void __fastcall SetStepActions(const TStepActions* Value);
	void __fastcall SetStepActionsAppearance(const TStepActionsAppearance* Value);
	void __fastcall SetImageList(const Imglist::TCustomImageList* Value);
	void __fastcall SetPictureContainer(const Gdippicturecontainer::TGDIPPictureContainer* Value);
	void __fastcall SetActiveStep(const int Value);
	void __fastcall SetReadOnly(const bool Value);
	void __fastcall SetVisibleSteps(const int Value);
	System::UnicodeString __fastcall GetVersion(void);
	MESSAGE void __fastcall WMGetDlgCode(Messages::TWMNoParams &Message);
	HIDESBASE MESSAGE void __fastcall CMHintShow(Messages::TMessage &Message);
	void __fastcall SetShowFocus(const bool Value);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	void __fastcall SetAnimation(const bool Value);
	HIDESBASE MESSAGE void __fastcall CMDesignHitTest(Messages::TWMMouse &Msg);
	
protected:
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation AOperation);
	void __fastcall Draw(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &r);
	void __fastcall DrawBackGround(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &r);
	void __fastcall DrawSteps(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &r);
	void __fastcall CalculateSteps(const Advgdip::TGPRectF &r);
	void __fastcall DrawProgress(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &r);
	HIDESBASE void __fastcall Changed(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	int __fastcall GetStepCount(void);
	int __fastcall GetStartStep(void);
	int __fastcall GetStopStep(void);
	virtual int __fastcall GetVersionNr(void);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	DYNAMIC void __fastcall KeyUp(System::Word &Key, Classes::TShiftState Shift);
	bool __fastcall MouseOverDesignChoice(int X, int Y);
	void __fastcall HandleDesignChoice(int X, int Y);
	void __fastcall PaintDesigner(void);
	
public:
	void __fastcall BeginUpdate(void);
	void __fastcall EndUpdate(void);
	__fastcall virtual TAdvSmoothStepControl(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothStepControl(void);
	virtual void __fastcall CreateWnd(void);
	DYNAMIC void __fastcall DoEnter(void);
	DYNAMIC void __fastcall DoExit(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	void __fastcall FillChanged(System::TObject* Sender);
	void __fastcall StepActionsChanged(System::TObject* Sender);
	void __fastcall Animate(System::TObject* Sender);
	virtual void __fastcall Paint(void);
	void __fastcall SetComponentStyle(Advstyleif::TTMSStyle AStyle);
	void __fastcall NextStep(void);
	void __fastcall PreviousStep(void);
	void __fastcall GoToStep(int Index);
	void __fastcall Finish(void);
	int __fastcall XYToStep(int X, int Y);
	void __fastcall UseDefaultStyle(void);
	void __fastcall InitializeSteps(void);
	TStepMode __fastcall GetStepMode(int Item);
	void __fastcall SaveToImage(System::UnicodeString Filename, Advgdip::TImageType ImageType = (Advgdip::TImageType)(0x1), int ImageQualityPercentage = 0x64);
	DYNAMIC void __fastcall Resize(void);
	
__published:
	__property bool Animation = {read=FAnimation, write=SetAnimation, default=1};
	__property Advgdip::TAntiAlias AntiAlias = {read=FAntiAlias, write=SetAntiAlias, default=1};
	__property Gdipfill::TGDIPFill* Fill = {read=FFill, write=SetFill};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, default=1};
	__property TStepActions* StepActions = {read=FStepActions, write=SetStepActions};
	__property TStepActionsAppearance* Appearance = {read=FStepActionsAppearance, write=SetStepActionsAppearance};
	__property Imglist::TCustomImageList* ImageList = {read=FImageList, write=SetImageList};
	__property Gdippicturecontainer::TGDIPPictureContainer* PictureContainer = {read=FPictureContainer, write=SetPictureContainer};
	__property int ActiveStep = {read=FActiveStep, write=SetActiveStep, default=0};
	__property int VisibleSteps = {read=FVisibleSteps, write=SetVisibleSteps, default=0};
	__property bool ReadOnly = {read=FReadOnly, write=SetReadOnly, default=0};
	__property System::UnicodeString Version = {read=GetVersion};
	__property bool ShowFocus = {read=FShowFocus, write=SetShowFocus, default=1};
	__property TStepClickEvent OnStepClick = {read=FOnStepClick, write=FOnStepClick};
	__property TStepMouseMoveEvent OnStepMouseMove = {read=FOnStepMouseMove, write=FOnStepMouseMove};
	__property TStepMouseUpEvent OnStepMouseUp = {read=FOnStepMouseUp, write=FOnStepMouseUp};
	__property TStepMouseDownEvent OnStepMouseDown = {read=FOnStepMouseDown, write=FOnStepMouseDown};
	__property TStepHintEvent OnStepHint = {read=FOnStepHint, write=FOnStepHint};
	__property TStepShapeDrawEvent OnStepShapeDraw = {read=FOnStepShapeDraw, write=FOnStepShapeDraw};
	__property TStepDrawEvent OnStepDraw = {read=FOnStepDraw, write=FOnStepDraw};
	__property TStepAnchorEvent OnStepAnchor = {read=FOnStepAnchor, write=FOnStepAnchor};
	__property Align = {default=0};
	__property Anchors = {default=3};
	__property Constraints;
	__property PopupMenu;
	__property TabOrder = {default=-1};
	__property ParentShowHint = {default=1};
	__property ShowHint;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMouseUp;
	__property OnMouseMove;
	__property OnMouseDown;
	__property OnMouseActivate;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property DragKind = {default=0};
	__property DragMode = {default=0};
	__property OnResize;
	__property OnDblClick;
	__property OnClick;
	__property OnEnter;
	__property OnExit;
	__property OnStartDrag;
	__property OnEndDrag;
	__property Visible = {default=1};
	__property TabStop = {default=1};
	__property OnGesture;
	__property Touch;
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothStepControl(HWND ParentWindow) : Controls::TCustomControl(ParentWindow) { }
	
private:
	void *__ITMSStyle;	/* Advstyleif::ITMSStyle */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator Advstyleif::_di_ITMSStyle()
	{
		Advstyleif::_di_ITMSStyle intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator Advstyleif::ITMSStyle*(void) { return (Advstyleif::ITMSStyle*)&__ITMSStyle; }
	#endif
	
};


//-- var, const, procedure ---------------------------------------------------
static const System::ShortInt MAJ_VER = 0x1;
static const System::ShortInt MIN_VER = 0x0;
static const System::ShortInt REL_VER = 0x5;
static const System::ShortInt BLD_VER = 0x0;
#define s_Edit L"Layout"

}	/* namespace Advsmoothstepcontrol */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothstepcontrol;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothstepcontrolHPP
