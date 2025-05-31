program Demo;

uses
  Forms,
  UDemo in 'UDemo.pas' {Form181};

{$R *.res}

begin
  Application.Initialize;
  
  Application.CreateForm(TForm181, Form181);
  Application.Run;
end.
