object Form446: TForm446
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = 'TMS DB AdvSmoothImageListBox Car List'
  ClientHeight = 735
  ClientWidth = 678
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010100000000000280100001600000028000000100000002000
    00000100040000000000C0000000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    00000000000000000000000000000000000000000000CCCCCCCCCCCCCCCC0000
    000000000000000000000000000000C000C000C0CCC000C000C000C0000C00C0
    00C0C0C0CCC000C000CC0CC0C000CCCCC0C000C00CCC00000000000000000000
    000000000000CCCCCCCCCCCCCCCC00000000000000000000000000000000FFFF
    0000FFFF0000FFFF000000000000FFFF0000FFFF0000DDD10000DDDE0000DD51
    0000DC97000005D80000FFFF0000FFFF000000000000FFFF0000FFFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 678
    Height = 735
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Car List'
      DesignSize = (
        670
        707)
      object DBGrid1: TDBGrid
        Left = 0
        Top = 47
        Width = 667
        Height = 156
        Anchors = [akLeft, akTop, akRight]
        DataSource = DataSource1
        TabOrder = 0
        TitleFont.Charset = DEFAULT_CHARSET
        TitleFont.Color = clWindowText
        TitleFont.Height = -11
        TitleFont.Name = 'Tahoma'
        TitleFont.Style = []
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 670
        Height = 41
        Align = alTop
        TabOrder = 1
        object Label6: TLabel
          Left = 265
          Top = 8
          Width = 88
          Height = 13
          Caption = 'Caption Template:'
        end
        object DBNavigator1: TDBNavigator
          Left = 8
          Top = 7
          Width = 240
          Height = 25
          DataSource = DataSource1
          TabOrder = 0
        end
        object RadioButton1: TRadioButton
          Left = 384
          Top = 0
          Width = 257
          Height = 17
          Caption = 
            '<b>Brand</b>:<br><#Brand><br><b>Model</b>:<br><#Model><br><b>Pri' +
            'ce</b>:<br><#Price>'
          TabOrder = 1
          OnClick = RadioButton1Click
        end
        object RadioButton2: TRadioButton
          Left = 384
          Top = 19
          Width = 217
          Height = 17
          Caption = '<#Brand>'
          Checked = True
          TabOrder = 2
          TabStop = True
          OnClick = RadioButton2Click
        end
      end
      object DBAdvSmoothImageListBox1: TDBAdvSmoothImageListBox
        Left = 0
        Top = 209
        Width = 667
        Height = 494
        SelectedItemIndex = 0
        Items = <>
        TopLayerItems = <>
        ItemAppearance.AutoSize = True
        ItemAppearance.ImageWidth = 150
        ItemAppearance.ImageHeight = 150
        ItemAppearance.TextVisible = True
        ItemAppearance.TextAlign = alTop
        ItemAppearance.TextWidth = 65
        ItemAppearance.TextHeight = 35
        ItemAppearance.ItemWidth = 200
        ItemAppearance.ItemHeight = 200
        ItemAppearance.Fill.Color = 16445163
        ItemAppearance.Fill.ColorTo = 15064023
        ItemAppearance.Fill.ColorMirror = 14866644
        ItemAppearance.Fill.ColorMirrorTo = 13748165
        ItemAppearance.Fill.GradientMirrorType = gtVertical
        ItemAppearance.Fill.BorderColor = clBlack
        ItemAppearance.Fill.Rounding = 0
        ItemAppearance.Fill.ShadowOffset = 0
        ItemAppearance.SelectedFill.Color = 11196927
        ItemAppearance.SelectedFill.ColorTo = 7257087
        ItemAppearance.SelectedFill.ColorMirror = 4370174
        ItemAppearance.SelectedFill.ColorMirrorTo = 8053246
        ItemAppearance.SelectedFill.GradientMirrorType = gtVertical
        ItemAppearance.SelectedFill.BorderColor = clBlack
        ItemAppearance.SelectedFill.Rounding = 0
        ItemAppearance.SelectedFill.ShadowOffset = 0
        ItemAppearance.DisabledFill.Color = 15921906
        ItemAppearance.DisabledFill.ColorTo = 11974326
        ItemAppearance.DisabledFill.ColorMirror = 11974326
        ItemAppearance.DisabledFill.ColorMirrorTo = 15921906
        ItemAppearance.DisabledFill.GradientMirrorType = gtVertical
        ItemAppearance.DisabledFill.BorderColor = clBlack
        ItemAppearance.DisabledFill.Rounding = 0
        ItemAppearance.DisabledFill.ShadowOffset = 0
        ItemAppearance.HoverFill.Color = 15465983
        ItemAppearance.HoverFill.ColorTo = 11332863
        ItemAppearance.HoverFill.ColorMirror = 5888767
        ItemAppearance.HoverFill.ColorMirrorTo = 10807807
        ItemAppearance.HoverFill.GradientMirrorType = gtVertical
        ItemAppearance.HoverFill.BorderColor = clBlack
        ItemAppearance.HoverFill.Rounding = 0
        ItemAppearance.HoverFill.ShadowOffset = 0
        ItemAppearance.Splitter.Fill.Color = 11196927
        ItemAppearance.Splitter.Fill.ColorTo = 7257087
        ItemAppearance.Splitter.Fill.ColorMirror = clNone
        ItemAppearance.Splitter.Fill.ColorMirrorTo = clNone
        ItemAppearance.Splitter.Fill.GradientType = gtHorizontal
        ItemAppearance.Splitter.Fill.BorderColor = clBlack
        ItemAppearance.Splitter.Fill.Rounding = 0
        ItemAppearance.Splitter.Fill.ShadowOffset = 0
        ItemAppearance.Splitter.TextFont.Charset = DEFAULT_CHARSET
        ItemAppearance.Splitter.TextFont.Color = clWindowText
        ItemAppearance.Splitter.TextFont.Height = -11
        ItemAppearance.Splitter.TextFont.Name = 'Tahoma'
        ItemAppearance.Splitter.TextFont.Style = []
        ItemAppearance.Splitter.ExpanderColor = 16445163
        ItemAppearance.Splitter.ExpanderDownColor = 7257087
        ItemAppearance.Splitter.ExpanderHoverColor = 11196927
        Header.Caption = 'Car List'
        Header.Font.Charset = DEFAULT_CHARSET
        Header.Font.Color = 9126421
        Header.Font.Height = -13
        Header.Font.Name = 'Tahoma'
        Header.Font.Style = []
        Header.Fill.Color = 16316406
        Header.Fill.ColorTo = 15261915
        Header.Fill.ColorMirror = clNone
        Header.Fill.ColorMirrorTo = clNone
        Header.Fill.BorderColor = 7630959
        Header.Fill.Rounding = 0
        Header.Fill.ShadowOffset = 0
        Header.Navigator.Visible = False
        Header.Navigator.Color = 16445163
        Header.Navigator.HintNext = 'Next Item'
        Header.Navigator.HintPrevious = 'Previous Item'
        Header.Navigator.HintNextPage = 'Next Page'
        Header.Navigator.HintPreviousPage = 'Previous Page'
        Header.Navigator.DisabledColor = clGray
        Header.Navigator.HoverColor = 11196927
        Header.Navigator.DownColor = 7257087
        Header.Navigator.BorderColor = clBlack
        Footer.Caption = 'Footer'
        Footer.Font.Charset = DEFAULT_CHARSET
        Footer.Font.Color = 9126421
        Footer.Font.Height = -13
        Footer.Font.Name = 'Tahoma'
        Footer.Font.Style = []
        Footer.Fill.Color = 16316406
        Footer.Fill.ColorTo = 15261915
        Footer.Fill.ColorMirror = clNone
        Footer.Fill.ColorMirrorTo = clNone
        Footer.Fill.BorderColor = 7630959
        Footer.Fill.Rounding = 0
        Footer.Fill.ShadowOffset = 0
        Footer.Visible = False
        Footer.Navigator.Visible = True
        Footer.Navigator.Color = 16445163
        Footer.Navigator.HintNext = 'Next Item'
        Footer.Navigator.HintPrevious = 'Previous Item'
        Footer.Navigator.HintNextPage = 'Next Page'
        Footer.Navigator.HintPreviousPage = 'Previous Page'
        Footer.Navigator.DisabledColor = clGray
        Footer.Navigator.HoverColor = 11196927
        Footer.Navigator.DownColor = 7257087
        Footer.Navigator.BorderColor = clBlack
        Fill.Color = 13287866
        Fill.ColorTo = 13287866
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        DefaultHTMLText.Location = cpTopLeft
        DefaultHTMLText.Font.Charset = DEFAULT_CHARSET
        DefaultHTMLText.Font.Color = clWindowText
        DefaultHTMLText.Font.Height = -11
        DefaultHTMLText.Font.Name = 'Tahoma'
        DefaultHTMLText.Font.Style = []
        ZoomMode = zmAspectRatio
        Anchors = [akLeft, akTop, akRight, akBottom]
        TabOrder = 2
        ParentShowHint = False
        ShowHint = True
        DataBinding.CaptionField = 'Brand'
        DataBinding.ImageField = 'Image'
        DataBinding.HintField = 'Model'
        DataSource = DataSource1
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Contact List'
      ImageIndex = 1
      DesignSize = (
        670
        707)
      object DBAdvSmoothImageListBox2: TDBAdvSmoothImageListBox
        Left = 3
        Top = 47
        Width = 664
        Height = 226
        SelectedItemIndex = 0
        Items = <>
        TopLayerItems = <>
        ItemAppearance.AutoSize = True
        ItemAppearance.ImageWidth = 100
        ItemAppearance.ImageHeight = 100
        ItemAppearance.TextVisible = True
        ItemAppearance.TextAlign = alTop
        ItemAppearance.TextHeight = 20
        ItemAppearance.ItemWidth = 200
        ItemAppearance.ItemHeight = 200
        ItemAppearance.Fill.Color = 15252637
        ItemAppearance.Fill.ColorTo = 14330996
        ItemAppearance.Fill.ColorMirror = 13471297
        ItemAppearance.Fill.ColorMirrorTo = 14661510
        ItemAppearance.Fill.GradientMirrorType = gtVertical
        ItemAppearance.Fill.BorderColor = clNavy
        ItemAppearance.Fill.Rounding = 0
        ItemAppearance.Fill.ShadowOffset = 0
        ItemAppearance.SelectedFill.Color = 16777184
        ItemAppearance.SelectedFill.ColorTo = 16704977
        ItemAppearance.SelectedFill.ColorMirror = 16704977
        ItemAppearance.SelectedFill.ColorMirrorTo = 16707037
        ItemAppearance.SelectedFill.GradientMirrorType = gtVertical
        ItemAppearance.SelectedFill.BorderColor = 14529170
        ItemAppearance.SelectedFill.Rounding = 0
        ItemAppearance.SelectedFill.ShadowOffset = 0
        ItemAppearance.DisabledFill.Color = 15921906
        ItemAppearance.DisabledFill.ColorTo = 11974326
        ItemAppearance.DisabledFill.ColorMirror = clNone
        ItemAppearance.DisabledFill.ColorMirrorTo = clNone
        ItemAppearance.DisabledFill.GradientMirrorType = gtVertical
        ItemAppearance.DisabledFill.BorderColor = 5800032
        ItemAppearance.DisabledFill.Rounding = 0
        ItemAppearance.DisabledFill.ShadowOffset = 0
        ItemAppearance.HoverFill.Color = 16777184
        ItemAppearance.HoverFill.ColorTo = 16704977
        ItemAppearance.HoverFill.ColorMirror = 16704977
        ItemAppearance.HoverFill.ColorMirrorTo = 16707037
        ItemAppearance.HoverFill.GradientMirrorType = gtVertical
        ItemAppearance.HoverFill.BorderColor = 14529170
        ItemAppearance.HoverFill.Rounding = 0
        ItemAppearance.HoverFill.ShadowOffset = 0
        ItemAppearance.Splitter.Fill.Color = 9758459
        ItemAppearance.Splitter.Fill.ColorTo = 1414638
        ItemAppearance.Splitter.Fill.ColorMirror = clNone
        ItemAppearance.Splitter.Fill.ColorMirrorTo = clNone
        ItemAppearance.Splitter.Fill.GradientType = gtHorizontal
        ItemAppearance.Splitter.Fill.BorderColor = 9841920
        ItemAppearance.Splitter.Fill.Rounding = 0
        ItemAppearance.Splitter.Fill.ShadowOffset = 0
        ItemAppearance.Splitter.TextFont.Charset = DEFAULT_CHARSET
        ItemAppearance.Splitter.TextFont.Color = clWindowText
        ItemAppearance.Splitter.TextFont.Height = -11
        ItemAppearance.Splitter.TextFont.Name = 'Tahoma'
        ItemAppearance.Splitter.TextFont.Style = []
        ItemAppearance.Splitter.ExpanderColor = 13627626
        ItemAppearance.Splitter.ExpanderDownColor = 1414638
        ItemAppearance.Splitter.ExpanderHoverColor = 9758459
        Header.Caption = 'Header'
        Header.Font.Charset = DEFAULT_CHARSET
        Header.Font.Color = clWhite
        Header.Font.Height = -13
        Header.Font.Name = 'Tahoma'
        Header.Font.Style = []
        Header.Fill.Color = 8569007
        Header.Fill.ColorTo = 4487779
        Header.Fill.ColorMirror = clNone
        Header.Fill.ColorMirrorTo = clNone
        Header.Fill.BorderColor = 5800032
        Header.Fill.Rounding = 0
        Header.Fill.ShadowOffset = 0
        Header.Visible = False
        Header.Navigator.Visible = False
        Header.Navigator.Color = 13627626
        Header.Navigator.HintNext = 'Next Item'
        Header.Navigator.HintPrevious = 'Previous Item'
        Header.Navigator.HintNextPage = 'Next Page'
        Header.Navigator.HintPreviousPage = 'Previous Page'
        Header.Navigator.DisabledColor = clGray
        Header.Navigator.HoverColor = 9758459
        Header.Navigator.DownColor = 1414638
        Header.Navigator.BorderColor = clBlack
        Footer.Caption = 'Footer'
        Footer.Font.Charset = DEFAULT_CHARSET
        Footer.Font.Color = clWhite
        Footer.Font.Height = -13
        Footer.Font.Name = 'Tahoma'
        Footer.Font.Style = []
        Footer.Fill.Color = 8569007
        Footer.Fill.ColorTo = 4487779
        Footer.Fill.ColorMirror = clNone
        Footer.Fill.ColorMirrorTo = clNone
        Footer.Fill.BorderColor = 5800032
        Footer.Fill.Rounding = 0
        Footer.Fill.ShadowOffset = 0
        Footer.Visible = False
        Footer.Navigator.Visible = True
        Footer.Navigator.Color = 13627626
        Footer.Navigator.HintNext = 'Next Item'
        Footer.Navigator.HintPrevious = 'Previous Item'
        Footer.Navigator.HintNextPage = 'Next Page'
        Footer.Navigator.HintPreviousPage = 'Previous Page'
        Footer.Navigator.DisabledColor = clGray
        Footer.Navigator.HoverColor = 9758459
        Footer.Navigator.DownColor = 1414638
        Footer.Navigator.BorderColor = clBlack
        Fill.Color = 16773087
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.BorderColor = clBlack
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        DefaultHTMLText.Location = cpTopLeft
        DefaultHTMLText.Font.Charset = DEFAULT_CHARSET
        DefaultHTMLText.Font.Color = clWindowText
        DefaultHTMLText.Font.Height = -11
        DefaultHTMLText.Font.Name = 'Tahoma'
        DefaultHTMLText.Font.Style = []
        Rows = 1
        ZoomMode = zmAspectRatio
        OnItemSelect = DBAdvSmoothImageListBox2ItemSelect
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        ParentShowHint = False
        ShowHint = True
        DataBinding.CaptionField = 'Name'
        DataBinding.ImageField = 'Image'
        DataBinding.HintField = 'Address'
        DataSource = DataSource2
      end
      object GroupBox1: TGroupBox
        Left = 3
        Top = 291
        Width = 664
        Height = 250
        Caption = 'Contact Information'
        TabOrder = 1
        object Label1: TLabel
          Left = 16
          Top = 24
          Width = 31
          Height = 13
          Caption = 'Name:'
        end
        object Label2: TLabel
          Left = 16
          Top = 56
          Width = 43
          Height = 13
          Caption = 'Address:'
        end
        object Label3: TLabel
          Left = 16
          Top = 88
          Width = 37
          Height = 13
          Caption = 'Picture:'
        end
        object Label4: TLabel
          Left = 72
          Top = 56
          Width = 3
          Height = 13
        end
        object Label5: TLabel
          Left = 72
          Top = 24
          Width = 3
          Height = 13
        end
      end
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 670
        Height = 41
        Align = alTop
        TabOrder = 2
        object Button1: TButton
          Left = 8
          Top = 7
          Width = 89
          Height = 25
          Caption = 'Load Contacts'
          TabOrder = 0
          OnClick = Button1Click
        end
        object Button2: TButton
          Left = 103
          Top = 7
          Width = 90
          Height = 25
          Caption = 'Clear Contacts'
          TabOrder = 1
          OnClick = Button2Click
        end
      end
    end
  end
  object ADOTable1: TADOTable
    ConnectionString = 
      'Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database.mdb;Persis' +
      't Security Info=False'
    CursorType = ctStatic
    TableName = 'Cars'
    Left = 928
    Top = 512
  end
  object DataSource1: TDataSource
    DataSet = ADOTable1
    Left = 896
    Top = 512
  end
  object ADOTable2: TADOTable
    ConnectionString = 
      'Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database.mdb;Persis' +
      't Security Info=False'
    CursorType = ctStatic
    TableName = 'Contacts'
    Left = 928
    Top = 552
  end
  object DataSource2: TDataSource
    DataSet = ADOTable2
    Left = 896
    Top = 552
  end
end
