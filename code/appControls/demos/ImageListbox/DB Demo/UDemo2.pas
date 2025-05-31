unit UDemo2;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, DB, ADODB, Grids, DBGrids, AdvSmoothImageListBox,
  DBAdvSmoothImageListBox, DBCtrls, StdCtrls, ExtCtrls, ComCtrls,
  AdvSmoothFillPreview, GDIPFill;

type
  TForm446 = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    DBGrid1: TDBGrid;
    Panel1: TPanel;
    DBNavigator1: TDBNavigator;
    DBAdvSmoothImageListBox1: TDBAdvSmoothImageListBox;
    ADOTable1: TADOTable;
    DataSource1: TDataSource;
    TabSheet2: TTabSheet;
    DBAdvSmoothImageListBox2: TDBAdvSmoothImageListBox;
    ADOTable2: TADOTable;
    DataSource2: TDataSource;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Panel2: TPanel;
    Button1: TButton;
    Button2: TButton;
    RadioButton1: TRadioButton;
    Label6: TLabel;
    RadioButton2: TRadioButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure DBAdvSmoothImageListBox2ItemSelect(Sender: TObject;
      itemindex: Integer);
    procedure RadioButton2Click(Sender: TObject);
    procedure RadioButton1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form446: TForm446;
  fp: TAdvSmoothFillPreview;

implementation

{$R *.dfm}

procedure TForm446.Button1Click(Sender: TObject);
begin
  ADOTable2.Active := true;
  DBAdvSmoothImageListBox2.SelectedItemIndex := 0;
  DBAdvSmoothImageListBox2ItemSelect(Self, 0);
end;

procedure TForm446.Button2Click(Sender: TObject);
begin
  ADOTable2.Active := false;
  Label4.Caption := '';
  Label5.Caption := '';
  fp.Visible := false;
end;

procedure TForm446.DBAdvSmoothImageListBox2ItemSelect(Sender: TObject;
  itemindex: Integer);
var
  blobf: TBlobField;
  aField: TField;
  s: TStream;
begin
  with DBAdvSmoothImageListBox2.Items[itemindex] do
  begin
    label5.Caption := Caption.Text;
    label4.Caption := Hint;
    DataSource2.DataSet.Locate('ID', itemindex + 1, []);
    aField := DataSource2.DataSet.Fieldbyname(DBAdvSmoothImageListBox2.DataBinding.ImageField);
    if Assigned(aField) and (aField.DataType = ftBlob) then
    begin
      blobf := aField as TBlobField;
      s := DataSource2.DataSet.CreateBlobStream(blobf, bmRead);
      try
        s.Position := 0;
        fp.Fill.PicturePosition := ppStretched;
        fp.Fill.Picture.LoadFromStream(s);
        fp.Visible := true;
      finally
        s.Free;
      end;
    end;
  end;
end;

procedure TForm446.FormCreate(Sender: TObject);
var
  i: integer;
begin
  ADOTable1.ConnectionString := 'Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database.mdb;Persist Security Info=False';
  ADOTable1.Active := true;
  DBAdvSmoothImageListBox2.PageMode := true;
  for I := 0 to DBGrid1.Columns.Count - 1 do
  begin
    DBGrid1.Columns[I].Width := 125;
  end;

  ADOTable2.ConnectionString := 'Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database.mdb;Persist Security Info=False';
  ADOTable2.Active := true;
  DBAdvSmoothImageListBox2.PageMode := false;

  fp := TAdvSmoothFillPreview.Create(GroupBox1);
  fp.Parent := GroupBox1;
  fp.Top := label3.Top + label3.Height;
  fp.Left := label4.Left;
  fp.Width := 137;
  fp.Height := 137;
  fp.Visible := false;
  DBAdvSmoothImageListBox2ItemSelect(Self, 0);
  for I := 0 to DBAdvSmoothImageListBox1.Items.Count - 1 do
  begin
    DBAdvSmoothImageListBox1.Items[i].Caption.Location := cpCenterLeft;
  end;
end;

procedure TForm446.RadioButton1Click(Sender: TObject);
begin
  DBAdvSmoothImageListBox1.DataBinding.CaptionTemplate := RadioButton1.Caption;
  DBAdvSmoothImageListBox1.ItemAppearance.TextAlign := alLeft;
end;

procedure TForm446.RadioButton2Click(Sender: TObject);
begin
  DBAdvSmoothImageListBox1.DataBinding.CaptionTemplate := RadioButton2.Caption;
  DBAdvSmoothImageListBox1.ItemAppearance.TextAlign := alTop;
end;

end.
