object LoadCfgDlg: TLoadCfgDlg
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'Load Configuration'
  ClientHeight = 325
  ClientWidth = 523
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PopupMode = pmAuto
  Position = poScreenCenter
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PanelLoadCfg: TPanel
    Left = 0
    Top = 0
    Width = 523
    Height = 325
    Align = alClient
    BevelKind = bkFlat
    BevelWidth = 2
    BorderWidth = 4
    BorderStyle = bsSingle
    TabOrder = 0
    object infolb: TLabel
      Left = 32
      Top = 73
      Width = 145
      Height = 19
      Caption = 'Select configuration:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object LabelSelect: TLabel
      Left = 32
      Top = 14
      Width = 47
      Height = 19
      Caption = 'Select:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object LocalDir: TLabel
      Left = 345
      Top = 23
      Width = 3
      Height = 13
      Enabled = False
      Visible = False
    end
    object ExternalDir: TLabel
      Left = 263
      Top = 39
      Width = 3
      Height = 13
      Visible = False
    end
    object Panel2: TPanel
      Left = -4
      Top = 64
      Width = 517
      Height = 251
      BevelKind = bkFlat
      BevelOuter = bvLowered
      TabOrder = 6
    end
    object Panel1: TPanel
      Left = -4
      Top = -4
      Width = 517
      Height = 71
      BevelKind = bkFlat
      BevelOuter = bvLowered
      TabOrder = 5
      object BtnLoad: TBitBtn
        Left = 429
        Top = 12
        Width = 54
        Height = 46
        DoubleBuffered = True
        Glyph.Data = {
          EE000000424DEE000000000000007600000028000000100000000F0000000100
          04000000000078000000C40E0000C40E00001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
          8888000000000008888800333333333088880F033333333308880BF033333333
          30880FBF0333333333080BFBF000000000080FBFBFBFBF0888880BFBFBFBFB08
          88880FBF00000008888880008888888800088888888888888008888888880888
          080888888888800088888888888888888888}
        ParentDoubleBuffered = False
        TabOrder = 0
        OnClick = BtnLoadClick
      end
    end
    object okbt: TButton
      Left = 62
      Top = 265
      Width = 98
      Height = 33
      Caption = 'Load'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ModalResult = 1
      ParentFont = False
      TabOrder = 0
    end
    object cancelbt: TButton
      Left = 208
      Top = 265
      Width = 98
      Height = 33
      Caption = 'Cancel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ModalResult = 2
      ParentFont = False
      TabOrder = 1
    end
    object cfglistbox: TListBox
      Left = 32
      Top = 98
      Width = 449
      Height = 161
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ItemHeight = 23
      ParentFont = False
      ParentShowHint = False
      ShowHint = False
      TabOrder = 2
    end
    object localcb: TCheckBox
      Left = 149
      Top = 16
      Width = 148
      Height = 17
      Caption = 'Home Folder'
      Checked = True
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      State = cbChecked
      TabOrder = 3
      OnClick = localcbClick
    end
    object externalcb: TCheckBox
      Left = 149
      Top = 39
      Width = 132
      Height = 17
      Caption = 'External Disk'
      Color = clWhite
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 4
      OnClick = externalcbClick
    end
    object ButtonDelete: TButton
      Left = 353
      Top = 265
      Width = 98
      Height = 33
      Caption = 'Delete'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 7
      OnClick = ButtonDeleteClick
    end
  end
end
