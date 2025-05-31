// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothPopup.pas' rev: 22.00

#ifndef AdvsmoothpopupHPP
#define AdvsmoothpopupHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <AdvGDIP.hpp>	// Pascal unit
#include <AdvStyleIF.hpp>	// Pascal unit
#include <Math.hpp>	// Pascal unit
#include <GDIPFill.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <GDIPPictureContainer.hpp>	// Pascal unit
#include <ImgList.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothpopup
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TAdvSmoothPopupFormPlaceHolder;
class DELPHICLASS TAdvSmoothPopup;
class PASCALIMPLEMENTATION TAdvSmoothPopupFormPlaceHolder : public Forms::TForm
{
	typedef Forms::TForm inherited;
	
private:
	TAdvSmoothPopup* FOwner;
	
public:
	void __fastcall Init(void);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	DYNAMIC void __fastcall KeyUp(System::Word &Key, Classes::TShiftState Shift);
public:
	/* TCustomForm.Create */ inline __fastcall virtual TAdvSmoothPopupFormPlaceHolder(Classes::TComponent* AOwner) : Forms::TForm(AOwner) { }
	/* TCustomForm.CreateNew */ inline __fastcall virtual TAdvSmoothPopupFormPlaceHolder(Classes::TComponent* AOwner, int Dummy) : Forms::TForm(AOwner, Dummy) { }
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TAdvSmoothPopupFormPlaceHolder(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothPopupFormPlaceHolder(HWND ParentWindow) : Forms::TForm(ParentWindow) { }
	
};


class DELPHICLASS TAdvSmoothPopupForm;
class PASCALIMPLEMENTATION TAdvSmoothPopupForm : public Forms::TForm
{
	typedef Forms::TForm inherited;
	
private:
	Advgdip::TGPBitmap* FMainBuffer;
	TAdvSmoothPopup* FPopup;
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	HIDESBASE MESSAGE void __fastcall WMMouseActivate(Messages::TWMMouseActivate &Message);
	HIDESBASE MESSAGE void __fastcall WMActivate(Messages::TMessage &Message);
	
protected:
	virtual void __fastcall CreateWnd(void);
	virtual void __fastcall DoCreate(void);
	virtual void __fastcall DoDestroy(void);
	DYNAMIC void __fastcall Paint(void);
	void __fastcall Draw(Advgdip::TGPGraphics* Graphics);
	void __fastcall CreateMainBuffer(void);
	void __fastcall DestroyMainBuffer(void);
	void __fastcall ClearBuffer(Advgdip::TGPGraphics* Graphics);
	Advgdip::TGPGraphics* __fastcall CreateGraphics(void);
	void __fastcall SetLayeredWindow(void);
	void __fastcall UpdateLayered(void);
	void __fastcall UpdateMainWindow(void);
	void __fastcall UpdateWindow(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	
public:
	void __fastcall Init(void);
	__fastcall virtual TAdvSmoothPopupForm(Classes::TComponent* AOwner, int Dummy);
	__property TAdvSmoothPopup* Popup = {read=FPopup, write=FPopup};
public:
	/* TCustomForm.Create */ inline __fastcall virtual TAdvSmoothPopupForm(Classes::TComponent* AOwner) : Forms::TForm(AOwner) { }
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TAdvSmoothPopupForm(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothPopupForm(HWND ParentWindow) : Forms::TForm(ParentWindow) { }
	
};


#pragma option push -b-
enum TAdvSmoothPopupArrowPosition { paLeftTop, paLeftCenter, paLeftBottom, paTopLeft, paTopCenter, paTopRight, paRightTop, paRightCenter, paRightBottom, paBottomLeft, paBottomCenter, paBottomRight };
#pragma option pop

#pragma option push -b-
enum TAdvSmoothPopupDirection { pdLeftTop, pdLeftCenter, pdLeftBottom, pdTopLeft, pdTopCenter, pdTopRight, pdRightTop, pdRightCenter, pdRightBottom, pdBottomLeft, pdBottomCenter, pdBottomRight };
#pragma option pop

#pragma option push -b-
enum TPopupButtonPosition { bpTopLeft, bpTopRight, bpBottomLeft, bpBottomRight };
#pragma option pop

class DELPHICLASS TPopupButton;
class PASCALIMPLEMENTATION TPopupButton : public Classes::TCollectionItem
{
	typedef Classes::TCollectionItem inherited;
	
private:
	Advgdip::TGPRectF FRect;
	Advgdip::TGPRectF FTextR;
	TAdvSmoothPopup* FOwner;
	System::UnicodeString FCaption;
	bool FEnabled;
	Advgdip::TAdvGDIPPicture* FImage;
	System::UnicodeString FImageName;
	int FImageIndex;
	bool FVisible;
	TPopupButtonPosition FPosition;
	int FTag;
	void __fastcall SetCaption(const System::UnicodeString Value);
	void __fastcall SetEnabled(const bool Value);
	void __fastcall SetImage(const Advgdip::TAdvGDIPPicture* Value);
	void __fastcall SetImageIndex(const int Value);
	void __fastcall SetImageName(const System::UnicodeString Value);
	void __fastcall SetPosition(const TPopupButtonPosition Value);
	void __fastcall SetVisible(const bool Value);
	
protected:
	HIDESBASE void __fastcall Changed(void);
	void __fastcall ImageChanged(System::TObject* Sender);
	
public:
	__fastcall virtual TPopupButton(Classes::TCollection* Collection);
	__fastcall virtual ~TPopupButton(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	
__published:
	__property System::UnicodeString Caption = {read=FCaption, write=SetCaption};
	__property Advgdip::TAdvGDIPPicture* Image = {read=FImage, write=SetImage};
	__property System::UnicodeString ImageName = {read=FImageName, write=SetImageName};
	__property int ImageIndex = {read=FImageIndex, write=SetImageIndex, default=-1};
	__property bool Enabled = {read=FEnabled, write=SetEnabled, default=1};
	__property bool Visible = {read=FVisible, write=SetVisible, default=1};
	__property TPopupButtonPosition Position = {read=FPosition, write=SetPosition, default=0};
	__property int Tag = {read=FTag, write=FTag, default=0};
};


class DELPHICLASS TPopupButtons;
class PASCALIMPLEMENTATION TPopupButtons : public Classes::TCollection
{
	typedef Classes::TCollection inherited;
	
public:
	TPopupButton* operator[](int Index) { return Items[Index]; }
	
private:
	TAdvSmoothPopup* FOwner;
	Classes::TNotifyEvent FOnChange;
	HIDESBASE TPopupButton* __fastcall GetItem(int Index);
	HIDESBASE void __fastcall SetItem(int Index, const TPopupButton* Value);
	
protected:
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	
public:
	__fastcall TPopupButtons(TAdvSmoothPopup* AOwner);
	__property TPopupButton* Items[int Index] = {read=GetItem, write=SetItem/*, default*/};
	HIDESBASE TPopupButton* __fastcall Add(void);
	HIDESBASE TPopupButton* __fastcall Insert(int Index);
	HIDESBASE void __fastcall Delete(int Index);
	HIDESBASE void __fastcall Clear(void);
public:
	/* TCollection.Destroy */ inline __fastcall virtual ~TPopupButtons(void) { }
	
};


typedef void __fastcall (__closure *TButtonClick)(System::TObject* Sender, int Index);

typedef void __fastcall (__closure *TPopupEvent)(System::TObject* Sender);

class PASCALIMPLEMENTATION TAdvSmoothPopup : public Classes::TComponent
{
	typedef Classes::TComponent inherited;
	
private:
	Advgdip::TGPRectF FHeaderCaptionRect;
	Advgdip::TGPRectF FFooterCaptionRect;
	bool FClosing;
	int FFocusedElement;
	TPopupButton* FHoveredElement;
	TPopupButton* FDownElement;
	Controls::TWinControl* FPrevParent;
	Controls::TAlign FPrevAlign;
	TAdvSmoothPopupFormPlaceHolder* frmC;
	TAdvSmoothPopupForm* frm;
	Graphics::TColor FBorderColor;
	Graphics::TColor FColor;
	Graphics::TColor FShadowColor;
	int FHeaderHeight;
	int FWidth;
	int FFooterHeight;
	int FHeight;
	double FShadowIntensity;
	Controls::TControl* FControl;
	int FArrowSize;
	TAdvSmoothPopupArrowPosition FArrowPosition;
	bool FAnimation;
	TPopupButtons* FButtons;
	Gdippicturecontainer::TGDIPPictureContainer* FPictureContainer;
	Imglist::TCustomImageList* FImageList;
	Graphics::TColor FButtonDownColor;
	Graphics::TColor FButtonDisabledColor;
	Graphics::TColor FButtonColor;
	Graphics::TFont* FButtonFont;
	Graphics::TColor FButtonHoverColor;
	bool FCloseOnDeactivate;
	int FShadowSize;
	TButtonClick FOnButtonClick;
	System::UnicodeString FFooterCaption;
	System::UnicodeString FHeaderCaption;
	Graphics::TFont* FFooterFont;
	Graphics::TFont* FHeaderFont;
	System::Byte FOpacity;
	TPopupEvent FOnShow;
	TPopupEvent FOnClose;
	bool FVisible;
	int FAnimationFactor;
	void __fastcall SetColor(const Graphics::TColor Value);
	void __fastcall SetBorderColor(const Graphics::TColor Value);
	void __fastcall SetShadowColor(const Graphics::TColor Value);
	void __fastcall SetHeaderHeight(const int Value);
	void __fastcall SetHeight(const int Value);
	void __fastcall SetWidth(const int Value);
	void __fastcall SetFooterHeight(const int Value);
	void __fastcall SetShadowIntensity(const double Value);
	void __fastcall SetControl(const Controls::TControl* Value);
	void __fastcall SetArrowPosition(const TAdvSmoothPopupArrowPosition Value);
	void __fastcall SetArrowSize(const int Value);
	void __fastcall SetAnimation(const bool Value);
	void __fastcall SetButtons(const TPopupButtons* Value);
	void __fastcall SetButtonColor(const Graphics::TColor Value);
	void __fastcall SetButtonDisabledColor(const Graphics::TColor Value);
	void __fastcall SetButtonDownColor(const Graphics::TColor Value);
	void __fastcall SetButtonFont(const Graphics::TFont* Value);
	void __fastcall SetButtonHoverColor(const Graphics::TColor Value);
	void __fastcall SetCloseOnDeactivate(const bool Value);
	void __fastcall SetShadowSize(const int Value);
	void __fastcall SetFooterCaption(const System::UnicodeString Value);
	void __fastcall SetHeaderCaption(const System::UnicodeString Value);
	void __fastcall SetFooterFont(const Graphics::TFont* Value);
	void __fastcall SetHeaderFont(const Graphics::TFont* Value);
	void __fastcall SetOpacity(const System::Byte Value);
	void __fastcall SetAnimationFactor(const int Value);
	
protected:
	void __fastcall Changed(void);
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation AOperation);
	void __fastcall CloseWithAnimation(void);
	void __fastcall ShowWithAnimation(void);
	void __fastcall ButtonsChanged(System::TObject* Sender);
	void __fastcall FontChanged(System::TObject* Sender);
	void __fastcall DoButtonClick(System::TObject* Sender, int AIndex);
	void __fastcall InitializeRects(void);
	void __fastcall FormDeactivate(System::TObject* Sender);
	void __fastcall FormClose(System::TObject* Sender, Forms::TCloseAction &Action);
	int __fastcall GetHeight(void);
	int __fastcall GetWidth(void);
	
public:
	__fastcall virtual TAdvSmoothPopup(Classes::TComponent* AOwner);
	__fastcall virtual ~TAdvSmoothPopup(void);
	void __fastcall PopupAt(int X, int Y);
	void __fastcall PopupAtControl(Controls::TWinControl* Control, TAdvSmoothPopupDirection Direction);
	void __fastcall ClosePopup(void);
	void __fastcall SetComponentStyle(Advstyleif::TTMSStyle AStyle);
	void __fastcall DrawPopup(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &R);
	void __fastcall DrawButtons(Advgdip::TGPGraphics* g, const Advgdip::TGPRectF &R);
	void __fastcall DrawHeaderAndFooter(Advgdip::TGPGraphics* g);
	TPopupButton* __fastcall ButtonAtXY(int pX, int pY);
	void __fastcall SetGlobalColor(Graphics::TColor AColor);
	int __fastcall GetMaxMarginSize(void);
	void __fastcall SetDefaultStyle(void);
	
__published:
	__property int AnimationFactor = {read=FAnimationFactor, write=SetAnimationFactor, default=1};
	__property bool Visible = {read=FVisible, nodefault};
	__property System::Byte Opacity = {read=FOpacity, write=SetOpacity, default=175};
	__property bool CloseOnDeactivate = {read=FCloseOnDeactivate, write=SetCloseOnDeactivate, default=1};
	__property bool Animation = {read=FAnimation, write=SetAnimation, default=1};
	__property Graphics::TColor BorderColor = {read=FBorderColor, write=SetBorderColor, default=2954758};
	__property Graphics::TColor ShadowColor = {read=FShadowColor, write=SetShadowColor, default=2954758};
	__property int ShadowSize = {read=FShadowSize, write=SetShadowSize, default=20};
	__property double ShadowIntensity = {read=FShadowIntensity, write=SetShadowIntensity};
	__property Graphics::TColor Color = {read=FColor, write=SetColor, default=2954758};
	__property int Height = {read=FHeight, write=SetHeight, default=300};
	__property int HeaderHeight = {read=FHeaderHeight, write=SetHeaderHeight, default=35};
	__property int FooterHeight = {read=FFooterHeight, write=SetFooterHeight, default=35};
	__property int Width = {read=FWidth, write=SetWidth, default=250};
	__property TAdvSmoothPopupArrowPosition ArrowPosition = {read=FArrowPosition, write=SetArrowPosition, default=10};
	__property int ArrowSize = {read=FArrowSize, write=SetArrowSize, default=17};
	__property Controls::TControl* Control = {read=FControl, write=SetControl};
	__property TPopupButtons* Buttons = {read=FButtons, write=SetButtons};
	__property Graphics::TFont* ButtonFont = {read=FButtonFont, write=SetButtonFont};
	__property Graphics::TColor ButtonColor = {read=FButtonColor, write=SetButtonColor, default=2954758};
	__property Graphics::TColor ButtonHoverColor = {read=FButtonHoverColor, write=SetButtonHoverColor, default=11227670};
	__property Graphics::TColor ButtonDownColor = {read=FButtonDownColor, write=SetButtonDownColor, default=7878928};
	__property Graphics::TColor ButtonDisabledColor = {read=FButtonDisabledColor, write=SetButtonDisabledColor, default=12632256};
	__property Gdippicturecontainer::TGDIPPictureContainer* PictureContainer = {read=FPictureContainer, write=FPictureContainer};
	__property Imglist::TCustomImageList* ImageList = {read=FImageList, write=FImageList};
	__property TButtonClick OnButtonClick = {read=FOnButtonClick, write=FOnButtonClick};
	__property System::UnicodeString HeaderCaption = {read=FHeaderCaption, write=SetHeaderCaption};
	__property System::UnicodeString FooterCaption = {read=FFooterCaption, write=SetFooterCaption};
	__property Graphics::TFont* FooterFont = {read=FFooterFont, write=SetFooterFont};
	__property Graphics::TFont* HeaderFont = {read=FHeaderFont, write=SetHeaderFont};
	__property TPopupEvent OnShow = {read=FOnShow, write=FOnShow};
	__property TPopupEvent OnClose = {read=FOnClose, write=FOnClose};
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
static const System::ShortInt REL_VER = 0x0;
static const System::ShortInt BLD_VER = 0x1;

}	/* namespace Advsmoothpopup */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothpopup;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothpopupHPP
