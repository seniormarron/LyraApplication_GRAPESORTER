object Form1: TForm1
  Left = 205
  Top = 135
  Caption = 'TMS TAdvSmoothSplashScreen Demo'
  ClientHeight = 352
  ClientWidth = 619
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 199
    Width = 107
    Height = 12
    Caption = 'Splashscreen Opacity:'
  end
  object Label2: TLabel
    Left = 16
    Top = 139
    Width = 122
    Height = 13
    Caption = 'Splashscreen Office style:'
  end
  object Label3: TLabel
    Left = 336
    Top = 93
    Width = 124
    Height = 13
    Caption = 'Splashscreen HTML Text:'
  end
  object Label4: TLabel
    Left = 17
    Top = 212
    Width = 3
    Height = 13
  end
  object ComboBox1: TComboBox
    Left = 144
    Top = 136
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = ComboBox1Change
    Items.Strings = (
      'Office 2003 Blue'
      'Office 2003 Silver'
      'Office 2003 Olive'
      'Office 2003 Classic'
      'Office 2007 Luna'
      'Office 2007 Obsidian'
      'Office 2007 Silver'
      'Windows XP'
      'Whidbey')
  end
  object Button1: TButton
    Left = 152
    Top = 163
    Width = 121
    Height = 25
    Caption = 'Show splash screen'
    TabOrder = 1
    OnClick = Button1Click
  end
  object TrackBar1: TTrackBar
    Left = 139
    Top = 194
    Width = 150
    Height = 45
    Max = 255
    Frequency = 15
    Position = 200
    TabOrder = 2
    OnChange = TrackBar1Change
  end
  object Memo1: TMemo
    Left = 336
    Top = 112
    Width = 265
    Height = 113
    Lines.Strings = (
      'This is a piece of <b>HTML</b>'
      '<FONT color="clBlue">TEXT</FONT>')
    TabOrder = 3
  end
  object AdvSmoothSplashScreen1: TAdvSmoothSplashScreen
    Version = '1.0.0.0'
    AutoShow = False
    AutoAdaptPictureSize = False
    BasicProgramInfo.ProgramName.Font.Charset = ANSI_CHARSET
    BasicProgramInfo.ProgramName.Font.Color = clWindowText
    BasicProgramInfo.ProgramName.Font.Height = -27
    BasicProgramInfo.ProgramName.Font.Name = 'Tahoma'
    BasicProgramInfo.ProgramName.Font.Style = [fsBold]
    BasicProgramInfo.ProgramName.Location = ilCustom
    BasicProgramInfo.ProgramName.PosX = 50
    BasicProgramInfo.ProgramName.PosY = 10
    BasicProgramInfo.ProgramName.HatchStyle = HatchStyleHorizontalBrick
    BasicProgramInfo.ProgramName.Picture.Data = {
      FFD8FFE000104A46494600010101006000600000FFE100664578696600004949
      2A000800000004001A010500010000003E0000001B0105000100000046000000
      28010300010000000200000031010200100000004E0000000000000060000000
      0100000060000000010000005061696E742E4E45542076332E303800FFDB0043
      0001010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      01FFDB0043010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      010101010101FFC00011080032003F03012200021101031101FFC4001F000001
      0501010101010100000000000000000102030405060708090A0BFFC400B51000
      02010303020403050504040000017D0102030004110512213141061351610722
      7114328191A1082342B1C11552D1F02433627282090A161718191A2526272829
      2A3435363738393A434445464748494A535455565758595A636465666768696A
      737475767778797A838485868788898A92939495969798999AA2A3A4A5A6A7A8
      A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3
      E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F0100030101010101010101
      010000000000000102030405060708090A0BFFC400B511000201020404030407
      05040400010277000102031104052131061241510761711322328108144291A1
      B1C109233352F0156272D10A162434E125F11718191A262728292A3536373839
      3A434445464748494A535455565758595A636465666768696A73747576777879
      7A82838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5
      B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3E4E5E6E7E8E9EA
      F2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FB3BC39F183C55E3EF
      8BBE1EF0978FE6F1678ABE126BDE39D42CE7F8749AE7C2AFEDAF1BF84AE1B508
      FC0B0DC786F45D6EC7E25EABA2EA5AD2F87F4D8A38741D267F12442E75092EDB
      4BBC169A97AF7C5AFD8B3E22695A77C4EF19FC1FF17F85FE0578335D82FB56BE
      F86DE27BFF00117C43F1668B676DA0432F8A75EB9D620D63C41E1C87C463FB3A
      EBFB07C292F88EE2D74DB2B3D1ED1FC41A6FDB3EC7A4FBE41F113C71A5EA173F
      137F681F859F0E6EF4E9752F067C27F863E17F00F8924D45FE1F5DEB7E201A16
      95ADEA3AE78923F07680BAA4B73E29B4D22CB55D01F4FBDF0469F1EA57F61AED
      CE99AF5F7F66FCF1F0EFE1EFED21F113C57F103E19DFF837E3CF8A7E186997FA
      EF98DADFC44F0DD9FC3DD6F4EF146AAF7FA1D86BFF0012DB56B3F1778A2EBC33
      E1B9B4FD2F52D1BC37E2DD4ACA68E6BD1E29B3BD93C88B55FA6CBF154D7B3AF8
      66E9C5C231AF2F6B4DC13E67670A69C6528C54A319382C472A9269461CD557C0
      63F0D5E3ED30F5F96725294E845D19F3497B386939CA33829370954829FD5DC9
      C657E69DA93F2AFD92BC33FB3F7C02F8ABF12BC7BE28FDA4B5CF889F176CF41D
      37C1FF0007B52F8AFA2786FE17E8DA0F86ACF48B383C4579E00F0E45E36F157D
      AEE6F6FF00FB3FC3B7BABA5D691A86956F15DE9B67A1C363ADCB7FAB765E3AFD
      AA3C5FF177C4DA0F81AFF5AB8B5FD99F5DD7EFB43F89FF0012CCDA9B7890E8F2
      C17F77A5DAEFD06D2E2C3C1DA06BDABD8D868163E21D7AEB4BD6B598EF60B8F0
      8E99A9DA5C26B76BE79AEFC19F117EC97F1674A93C4961A4F8B3C53A9786FC1B
      A7F86F4F83C6FE1CD72DB53D67C4DAC4D6BE37BCF01EA7E20B0F87DA5DB5ADA6
      B1FF0008A6962E7C4167A3F8909D45DCC7A8E8EF19B8EC74EF83975F103C5FAC
      786FE267C40F14FC12F156A9ACF89BC532F81120F0BF893C25ACF85AF8785B4C
      82D633E645A7DD6BD06AE7C4779E259A6B5D4EC27D3B59D02C92D6F6F6FE4BC9
      77C561A850856C4611D754B928BA389C542EE14AAB509CD422FD9494713FBBE5
      8CA29FB4715C92E55530A189AF5E787A188F633A9CF5D56C350A8A0EAD5A294E
      2A53D6A46F86BD4529467F05DA9AE6F65F397C61F04FECE7FB4078F3C4FF000D
      BC23E15F85BF12FC2FE1DD6BC09E21F12FC47D7EFAD759F89B2F84E5B9B19B57
      F06786E2B8F0A0BBD3AE34F6D0EEACACEFEDA7D334B960BEB66D4AFB725DBC9F
      147C79F871F0FB5DF89965736DF097E1F7ECF7E1C920B0F0AEA1A4EABAFDFDCE
      97369ADA844ADE3A9FC3DE119B53F0E26AAFA4CF1417535A268BA86A7134F241
      702F2292E1FF005E7F69CF823E27F18786FC35E32F0C6A5F0C7C0BA07C18BEF1
      FE8B71AAFC25D1B55D1BC5D71A147AB68BA7EB17FE1FD1FC3D1787ECB4B934EB
      5D0357FB1784357BBF11695ACEB730B69D9D6D91AF3E20F1F43F0AEF7C57F0A6
      7F01EAF71AA78A2E6C3584F17E93A878B7C5FF000F7C3FF14F4CD2BC17A84BA8
      6A5E33D46593C4128D646A56FA2DF594D7E2E75869A4B0D1EEA77D054456F947
      2BC356C0FD654AACB1942955AD5211C4429C2BC214E32AB0A349DE51C4C271E4
      A34D5674A509FB39CBDF496CB36AF4B1CB06E34D60B1156951837879D79E1E73
      9B54675E7EE46587A9197B4AD53D8AA91A90F6B04DC26DD4F8CFF1BFC6DE0EF1
      1783344F8951EB1F1A269B44F0CE97E09D33C63F0F12CAFF00C40757D0E0D2A6
      F1649F0FEE9BC617875EF19DCCBAB69DA38FECF5D4F4DD10490697690DFDD5EC
      55F617C20F006ADFD93E303F19F51F107C18F04EB1A27846DE0F0AE9F07C3FD6
      358BAD334FD5B529B439A6D474EF0FEAA3C0E961749676F7163A9787F53F16F8
      861934DFF8496EB4782D349B73C5FC2BB5F00788FC1FE07D6BC5B77E2FF0AFC4
      DBDF885F0BFC57ADCFE11F8ED61ABF82ADBE117F6ED8C971E18F15F87353B3F0
      7C7A3DA7823C156FA949AE7893C37E0FB9D4F4F9E6D34C3E3748AE2E2DF4BFA9
      7F6C2F14FECD33F84F49BAF85165F0FB53F8832EBF671F86B4AF877E2087484F
      1E2269730D62C75D9FC232BBEB32699A1A5E78823B8BF82F2F2197C3F166E228
      E4943272A938D1862275F95FBF2E685255684A318B517528CE4A50E5E5F6909C
      946338C924D5A5294A109D59E1A950F6B16A1170757D9E223524D4A6E9558C1A
      9DDBF6752309549536B67EEC7CE7E0A78B7C2177F16753F1C78A74AD513E0BD8
      7877569B4FD43C5BA7EA173F0B3C35F1134ED63474B08EC64F11DA8D326F15E9
      B64756BA83478CCD06832DADFDF269DA7EAD656B22FABFC478BE36D8E89E0AB2
      F82BF19FC1FE00D2BE357C43BAB8D1FE1DA68B79A645E19F09789FC2F79E293A
      8DF5E6916ED78DAA6A763E19D6FC6F79A34B65E1E9EC27D63FE11E9F53FB4599
      BA3E49F0375E8BE30F8766D43C55E24F0C699E05F865F126F7C6FA1F87BC55E1
      DBCD6F4DD4B51D4AFB5FBDD0347F135DDB6B32F86AEE1D1FC5D07887C71737F6
      3A249FD8F28F0968D7F1DC4A359B41ED3E39F03695E19F863F0FBF69EB9F125C
      F8F7C496FA7F887C67A3F8222BFD374FD33C31E13F15787B5DD4EDE4D2750369
      05D6A979E0EF0B6AD1F85B4FD4F5D9749867B6B95F363B3BABBDD27A3FD9B5AB
      E611A8E850C251AB5D51C2D0C3F32A12A74E9464A7150F73DA6264A339A4D38D
      493737EFA4B8279BE1E8E5CE9C6A6231388A38755B175B1118BC453AD5AAA87B
      3AAEADAAAA5864E70A6FE195382E58B70B8DF835A5FC12F853F17351B3FDAE75
      CF0EFC62D47C59E008ADBE1FF8EBE2578734A8BC35E097B4D4FCBF17F86F479F
      58BED5ECF44BCF17C52786B52B7D5A7BE3ABEA375A1C5A4DB5DC5776B616D7BF
      1EF887C47E0EF107897C49E35F887E26D0FF0068DF85DF0CFE22FC4BD2BE1278
      6BC5F61A7685A8C9A569B24FA7D826ABF1734AD72F561D4FC36E4691A5DFB786
      13C57A96A5756F7D7C63D4FCEB94E13E3C78FBE28EB51EBCDE3EF0C5CF837498
      A2BEF87F6D0C17BA2EA72AEB1757FA26A7245E2AB7D3F4FF00145E58E857979A
      747E1796EED974A92EA37D545BDF472AE9B6FAD79BFC41F0A7C3BF027C1DB5F0
      AF8A2EBC4DF0AFC4BE21F12786A4F88B7706ADAB785CF886D7C5BA8A788F556F
      13780F5881B49D3920596FE3F0B5CDCDAB3E9B0C1A6EFD6AE649644D43BB38CA
      717468D59E5F88A51F6B46D5A9CE2AA42586A747FDA5539CDCA318CEEA294694
      9CAD5396A2B2BF164999612A54A31CCA8D4A8E1592A5528CF9271C455AE9E1A5
      38C15F9A9AF8FF007AB979A9B9424A4DBF78F09FED6FE1B5F01F82FC09E03D2F
      C1BF0F4EABE24D66C2DB41D5AE7C79AFEA96577AD6A7AD6BF792DFEBFAB6969A
      3F897C4DA1D845763521A9EAE6D35DF1443F67B3FB479B6D337ABFC66F147823
      E1B7C3DF14EA5E326BCF8B1A4F8B34CB1F00D9FC44F12685E0BF1078EBE15CD7
      DE12BA8F43D234CBCF0FE9769776422BCD25AFF43BEF0AE9F1CF75AF5F59AF88
      2E43C361ACCBF8A9FB52587ECD3A3FC24F0EE93F01AF27F885E2AD17C5D77AAF
      8BF5DBBD7759BCF15691E1A9345BBB6D4A4B0D2F4BBC6B0B5D396EB50D5759D6
      7C5373A715B097EC8E93DBC170D6F3F887C3FF008E3E22F16F8774EF868DF15F
      C697DA5D8DDC97BE15D2EF6C25D7755D4359B9B66B18ED6FE7B231EAFACDD359
      C925A69B8B7F32592EE765BC8AEE758AEA28E1E94E74287328D2AF7AD09FB58C
      22A52A92551CA3394213A53508C7D9BD2CA339C68CB9651E9E5AB2A75F1293F6
      B879468D5A7ECDCAA469C2952941C5C539C6749CE72559372E794A319D5A7CD1
      7FAD9F085FC03E32BAF047853E14F86F581E1BD1FC2BACDEC9F13350F861A6F8
      13E23451E83AC0F0DDFF0086A6D44585C68DF11AFF005FBDD4669B52D5F57D3E
      5BED2A6D0EE669DEEAE6FCADD4DF14BE177C2AF0BF8A7C170EA7A2AF8BBC133D
      DDD695ADF853C3FE11D47FE1605CF93E17BEBBD06661F0EEFB48875CB0F3ECA1
      BEB9BFD43C03A96B309D32490EB1F649EF2E23F39F04EADF00BC03F0D744B2F0
      FF0088B57F845F153C33A7492FC49F1835F5A586B9F12E76D4AC357F1A585C5B
      78C6C6FF0046D23527FEC29F47F0AE9DADDB5B6A7A2DB69BA4DA5E6A915BDB6A
      8977EE3A77C64F85DE10F8A5E15F8A762BE2979AD3E18DEB5AD878BFE24FC343
      E359B5EF175EF87C58EACBE11F0CCB8D3BECBE1783C4969AACBE298AC359B797
      568ED6D748B5B55F3A4EFA94E382E556A09D7E69B8C9C62AABA7A24F95B8C5C5
      72B51F7B953D7DED17934E5531B52518FD6E71C2CE114E2A72E4A755A9B92BFB
      D28CEF28B9A716DC6EEF17770FED23FB2CDEFC2AF87FA6F81B5D9354D33C6DE3
      08A77D22CFE18EB5E38D6FC1FE28874A92DE07F06EAD33DA6973DC59D969D308
      B519756D3A2BB92C6E2EF55FB66B535D6B332707F0C35FF1BFC3ED32EFC37F18
      BC51ADEBFF00B3A5FC5E2FF871AE25BF8B2D574A5B98256B5D3F4B9ADB5BF0BD
      BF8BBC39E1ABED2F3A7694D737363A8436F696B3C2B676D6E24AFD1EFDA0BC1D
      F14343FD99B4AF8AB6FE2096F3E32693A67877C4FE21BCF8A9AE6A1ABF87F48B
      4F144BA545E2AD0E7D0FC3B7BE19F0D6991787E2BD8AE125F0E697A2ACD37876
      CDAEE59209EFE59BF2EAD3E2CEB3E2A8FE304BE21F8D1F0D93C57F0F3C19A45E
      F87C683F0C74DF12F86BC6BA5F881EE4DE7877C45E1FF12F89BC4DE28B8B83AF
      E876DA0595CF85BC49A5448357B3964D32F750BE8447C74788A8CED4ABD0A73C
      4469CA2A14546973B5514B9A8C39A30A2A0E3CFEEC69B92528DD5D72FA33E1DA
      B184E54EB545427284E52C42956F65CF4E2AD56AB552A5694D2941B94EA72CDC
      64DBD79F89D4F5D87E28FC66F1EF87BE13FC48F8716FE0CD6FC47E03D434FF00
      187887C6A1B5EF0978AFC5FE25FEC2D6E2B68AF2086C3C71A6F836F6C6C359FE
      CBD36FB48BB82CE5B1D3BED13C505FDFC5F66FEDEBE23F17F833C56FF14FC51E
      24F82FE2F974EB64F0C784EC34986FF5DF14E9BE26D6F5AD1EFECB55D3ED7C5F
      A4C767A69D374BD2F5E8BED9A45EEA7AB787B4CBFD42FF004DBA9E3B5BFD3AF3
      13C33FB29FECB9ACFC6EBEF077C34F8B76765F0DBC17E034B1F8D5A75AF888C1
      E24D6356BDBE96D7C2BE16D03507BD54D053C417706B9A9F89A6D1AD647B4D2B
      4FD2749D2E5D2E6D734FD562ABE0AFD977E1FEBB7BF11BC4375A85AF89CF83F5
      59BC1DF0965D5FC0561AAF85F5382E344B1D5FC43E28F19DACB7BA35CF8D34EF
      0EB6A36FA0CDE22D0AF3428EC754B5D4E61F69BCCE9B5853CDB0B9A61A53C753
      8CA51A72A183C3D0AD3AB41FB6AB1A908734E9D39FB3A524A6D55E54E57A719A
      4E1196F5329C565988A0B035EA457B5857C6D6C561E9D3AEA387A6E94DDE356A
      43DAD5A4F914A8F33517193849AA9287C19E11F8C9E29D4BC65E2BF1268BE185
      BEB8B6F0F38F897E27D1F4FB94BFD32CEF4581B4D3749D46DA2D6AF7467D5DF4
      686E2E1F52D07C4979AAA682EF2DB2B991E3F3EF8E9F01BE14782FC27F09FE3A
      F80BC41E12D43C61E34D507C40D6F43F0C78C74FB3B2D161D66F6E7C5BA1C5E2
      6F09CDA068761E15BA867D434DD0B4BD6F4FBEF0CE9BABDE68DACD9DCF87F4CB
      C4632FD29A7FC4BF86FF0009ACFC53E04F873AD68DE33F853E35F0BDBF89B55F
      1078F3C3FA9F83B54B6F88F7F63AADB5C5D69CC34FD1752B8B5D534D8344B9FE
      C49F4E9A4B017115B58F89CBB5F24FF1CFC31F0C7C43F0BFC35D5BC4119F06DF
      F807E297882EF43B6FB6F886F25D6D8E9173E24B3D4358D0F4580EAB6F1F872E
      61D2A4834BB5BCD59751B111472CD6B22DEA450635B115634B2CAD84C0466A75
      671A8EA28C2ACA14ED0F69286D4E14A9C95674A9D1A2E7CB67526A324FAE8E16
      33AF99D2C4E3A7414694654E345CE506EAA5374D4EF2E775A69D08D69D5AAA0E
      4A5ECE2E4A47EA3FECBFFB3BEA5F1A3E03E91F14B564F87E9F15B5EF165D7C5D
      D0EFFC59E034F15EAABA6F883C3BA4F876C345F10F8B6FF5D9EFB50B3B8B6B0B
      7F114FF678A4974EBFB98A0B79EF12D1CDD7CD1F05FF0067FD47C61F10AD3C29
      69F08BE05F8C7C79AB78B3E285B4BAAAF89E2DF0DB7872FB5A9AF2FEF6C1E2BD
      D6FC27A0C061FF008477438EDE391351912DD2EA0D3A374B4B777C1EF1E7C6AB
      1D335DF077C2CF8B91F87A0D334549EE750FED578A3F016943594D2CF84FC29A
      56A9E18F1769F2EAF3E96FE46876771E20D0A1B0B3B7BB92F9E1822B177FB27F
      656F847E2BF0959E85F14BC2BA7FC5A6D096CBC51A8F863C5FAFEB3A4D8F8FB5
      58F5D93C39A769ADAB784ECFFE258FA61B5BFF0019CB6E8FE1C81C41A8691A9C
      C5AF116787D99D4746D888D05565529CD50AB52853AD43DB28C2552167077517
      51FB494211939AF6495DA4BE6634B9A73C3D7AFEC614EAD3956A54ABCE85774A
      527184949548BE69AA6BD9C1D49C52FDEE9F11FAEDF1D218750F81FE3CB1BF8A
      2BDB2BAD06E2DAEACEEE35B9B5B9B79A38A19609EDE60F0CD0CB0931491488C8
      F1931B29538AFCCCF809F0BBE19787BE3AF84B58D03E1D781343D5E3F8AFE0FB
      88F54D1FC23E1FD33518E7FF00857B0DDF9C97D65A7C172B2FDAAEEEAE7CC594
      3FDA2E6E26DDE64D2331457CB65BFC2A9E93FF00D3733EBB32FE343FC347FF00
      4E40F90BE28E9DA7EA1FF0501FD99F4EBFB0B3BED3FC77F17AD97C7161796B05
      CD978C96E7E3FDC69D72BE2BB5991E0F112DC69ECD6338D5E3BC12D9936B2068
      094AFDF9FF00829E471A7EC37F1A6F95116F7C3BA6E997FE1FBC555175A15F2F
      89F4AB15BCD1AE0012E99762CAEAE6CC5C593C130B5B89EDC3F952C88C515C35
      3FE46B8DFF00AFB0FF00D354CEF7FF00225CB3FEBD3FFD4867F037F1AFC41AFD
      9783B54D22CF5CD5ED34A5F8BB1DBAE996DA95EC1A7AC16DE1FD3EDADE116514
      E96C2282DFF710C622D9143FBA4554F96BE91F025DDD49FB36F8655EE6E1D6CA
      CF4296CC3CD230B497FB2BC4B2F9B6A0B116F279B3CF2EF8B6379934AF9DD239
      628AF470FF00EF34BFEC1732FF00D368C311FEE957FEC2301FFA759FBCFF00F0
      533B1B2F0DFC24FD95AD7C3B676BA0DB59EB9A8699696FA2DBC5A5C16BA6CBF0
      B6F2FA5D3EDE2B1482382CA4BDD3B4FBC92D225581EEAC6CEE1A332DAC0E9DE7
      C14F10EBE9FB0FF833505D73585BF87C0FE0230DF2EA77A2F222FA868D6AE63B
      9138990BDA816CC55C1680084E63F968A2BDFC0FFBA2FF00AFB88FFD230E7C7E
      69FEF347FEBE51FCE47FFFD9}
    BasicProgramInfo.ProgramName.ColorEnd = 16671013
    BasicProgramInfo.ProgramName.GradientType = gtSolid
    BasicProgramInfo.ProgramVersion.Font.Charset = DEFAULT_CHARSET
    BasicProgramInfo.ProgramVersion.Font.Color = clWindowText
    BasicProgramInfo.ProgramVersion.Font.Height = -16
    BasicProgramInfo.ProgramVersion.Font.Name = 'Tahoma'
    BasicProgramInfo.ProgramVersion.Font.Style = [fsBold]
    BasicProgramInfo.ProgramVersion.Location = ilCustom
    BasicProgramInfo.ProgramVersion.PosX = 337
    BasicProgramInfo.ProgramVersion.PosY = 40
    BasicProgramInfo.ProgramVersion.HatchStyle = HatchStyleWideDownwardDiagonal
    BasicProgramInfo.ProgramVersion.ColorEnd = 8404992
    BasicProgramInfo.ProgramVersion.GradientType = gtSolid
    BasicProgramInfo.CopyRightFont.Charset = DEFAULT_CHARSET
    BasicProgramInfo.CopyRightFont.Color = 8404992
    BasicProgramInfo.CopyRightFont.Height = -11
    BasicProgramInfo.CopyRightFont.Name = 'MS Sans Serif'
    BasicProgramInfo.CopyRightFont.Style = []
    BasicProgramInfo.CopyRightLocation = ilBottomRight
    BasicProgramInfo.CopyRightPosX = 415
    BasicProgramInfo.CopyRightPosY = 267
    CloseOnTimeout = True
    Fill.Color = 16640730
    Fill.ColorTo = 14986888
    Fill.ColorMirror = clNone
    Fill.ColorMirrorTo = clNone
    Fill.GradientMirrorType = gtVertical
    Fill.BackGroundPictureLeft = 0
    Fill.BackGroundPictureTop = 0
    Fill.Picture.Data = {
      89504E470D0A1A0A0000000D49484452000000DB0000002308060000001D427F
      34000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
      00206348524D00007A26000080840000FA00000080E8000075300000EA600000
      3A98000017709CBA513C0000001874455874536F667477617265005061696E74
      2E4E45542076332E3336A9E7E2250000161949444154785EED9C6D70546596C7
      87A413020609185EC6377A14922004424842201D6824208B515050A3A246458D
      824E80A0EC0C6EC55954469995D96157105FB20535B82B3B714A67700B77E95D
      4BA2669D8C666AD06166992A3EE4031FF2211FF2A13FF4FE9ECBBD97F39C7BBB
      3BD9AD52A6AA5375AAEF3DFFFF7979CE799EFBDA9DEF7C27F797AB40AE02B90A
      E42A90AB40AE02B90AE42A90AB40AE02DF56057EFCD24F4A9136A413D985547D
      5BB9E4E2FE855580C912416248CD5F58EADF78BAD4A80919445242926C47BD64
      D82E41E2C8CC6F3CC15CC04BBB02CFBFF8D27B2FBCF852CA08DBBB2EED6CBFBD
      EC76BFF0E3226A74DEAB95FAAC3799C129467FCEC592ECAFFEF632CE45BEE42A
      F0DCDFBE901292B8E412BC44127AEE47CFC754ADCEFC68F78B9DC8262FC5104E
      E725927E2E8D4BA102CFFECD732921B9C596A629D428AE6A15584823E15C0A3D
      CFE5F00D57A063C7CEE88EA7FF3AFECCCE1FA684F4191DE25C16993F78C54F3F
      F383DDC87EF4CE7D08BA22B65BD0ED435E47B6A02B113625E09BD01F70F10EF6
      2BD20D11DBABC1DB5C7F5D7CEE65DFF84C7BDF031681D30CB7133136268F9DE8
      569BFC329513BC145EAB89E3DAEE33F14D1EDACE8D1383D7AE6AD5E5D6CA1917
      DB5523E054983A9A7A9ABACA586E9D77BAF998B147248EFFE96EBEA636D343F2
      3463DA08678FEBC3D4A413DD061DCBB305AB717BD4E6F595FD0E636F7C85D510
      BDA985C9D3D4DBC430358FFF7FA7EF91FFFC73D591FFFA73077200E9423AD135
      1D4E9CB5EA20E38015C36981BBC7B5D9CF673BFA68BA7CE06F70B87C7A1CB66B
      D0ED727DEC36B9487B9307D85E17DFCB7E73A6BC02B1B76DDF31B46DFBD3A9F4
      B2A3DD18C1DB7791B3E3A3ADDB3AEAD19D0DDAED1800AB409AC0CF87E0C9ADDB
      2EF8F4FEDAB76E8FA0DB0F3F99218F93F8B4161DFBABB13997C1C6E4D2AA077D
      215E4727B6C3E1B63B4C8EFBE1F98B156E7BE63A991AEE681D01A7CACE79C74E
      991FD81EDB875D2BF023A20F27EC1A76EC4D3F2627BF61C6DDA1EB817E40F85C
      C7FE0995833F21B1AF01EFCB50F3D3666E8C76D1BDF9E11F67BEF5EF7F3AD9F5
      1FFF930A13B03370027EDFFCF04F1D608319EC8EBE71E28F25321FF873251FBF
      55E88EA689DB8E7D31F88934780FF696FFB4637FF2A9AD83482A8338F722E089
      8B9C766C8CA4B36BEF071FCAE477CB93DFF79F7AC2DD9F250737BFF633DE40B0
      9F8B5D728476D6030AEC8E8ED0EEE4E62D4F394754F86D596C924F7D7FDB0638
      997282B395BC65DDDA4FCAE690DB1985F7287C40E07E3DB07B6B646332B1DBAD
      B3958AD71DE2C7392050F358B6BE5EB06D4FC21DF143A103C7BFAA78ED833F0C
      1CFAB73FA432099CA183C7BF9AEBD5E3E0075FBF9CCDC6E0AF7DF075FF815F7F
      E55F41A08B4B3BF0B3E9FC103309DE9B39AFAF5F1FD1C1E589CD4F9520F1B6C7
      B7A484F4191DE25FF2B53DBE3981C091B2A5EFF1279EDC8D5D7710737803E07B
      91FD6C0F49CE636D9BF799041F7DECF122B68705761EFE4662C7F86CC6F74981
      25BD4161637C8A7CB6BCEAE65C7F21A7CD4981FB47F3C7DA9ED8A8723539B6B9
      F10C76D6CEF309FFC92C9C2AF2695771BBDCB8CE251DDBD1C79F301CAB9E1607
      ACF722BE79881A380B9ADC4A959D195FD2D4C8C5672AFC885B43ECACDE9C75C7
      646AB11A9BF754EDDF97934361B26E26FE397C44DD3E9D93BD40BF93F15E6DE6
      9089277B4C7FCE6333A223FE3FBCFFFB5E2425A4FF1F7F757ACBABBF3ABD09DD
      7B0A73E6CDFEF77F1F57FA216CDA5FFDF5E97AEC3680F585D9195BF4DAD6C41E
      C47E3FB2D76C2B5B8327C1DE4676B3DDAFF0C1112D368FF4F0A647534212DA18
      2CA138EF3DF2689B7F1D0DD6ADF0739B1E79CCBFA700DBA4F02E13035D54EA1F
      7AF811FFD2C8E026067EF6C0E9E7B35DE46BE5F3D0439BAC4B0CB8316C7A91E3
      6C977A76F8EF93F1C0ACF78AEC9B7C863D0EFC81D6071F96E38CAB717486D42A
      2307FB5D6ACC4EEEC45AA77C3B3D411F73F18D0A5F27EAE18DAB57D6DDAD6151
      A6FE86C43C4EDD2B10FFDE951C74FFACCB5FB7972D6A5C7EBFD24DC657BAFBE3
      FBDEED4F09E9FFFB5FFECEBA8F057B4FE08ECF7DDDFDC7A5DD4F7FF9BB661903
      1F25E0033EA7BB7FF8EF7EF1A5E3179D8E3988BD7F62016F5639A57EFA6E7FAB
      E7DFF53D2439A35A6CF7DDDF9A1212586C6009C5B12629589BC2AD7775605185
      3B8BED81D6878AD00F4B6CE37D0F9CB977E3FDFBEEB9F7BE16241A3610F8FB95
      CD1076C7B06BC7A60609DC508395AA1CAC4B382F0E9C23CAB73F56F471E523B0
      D8B271C0AB947F67E292FF6EA13FE36DA377CECC7C1E10F8F0FD0F3CE84F4AEA
      1841A2E6D31B87A9013293714BBFA6CF567FD57852F80D3C2022F6BB9287CFD5
      F88E6B813324F23E966D12BEF4CE6FF7BCFCCE6F5342020F647EF2AF5F14EF3D
      F6C5166403DB913DFFDC17C16E58D8F897D3321EB8E51B1BE7E0843EAE62EE97
      762686C2CFEA7180272427DB382DBCA5E59E9490C062034B288EB508C05A15EE
      1F094C20B0A8C29DC5E6620714267349DD75D7DD7D77DED9D2BEE18EBBFC8974
      F7DDF7CEC52699CE0E9B216C5EBFE3CE16FFA10ADCB8E2EF0D2B129C0EC9C357
      8BC855FB082CB69038619CB35E0CFC1F37FEF93C21E2FAF544EF4C5A53078177
      EBDC196B13633E00AF1719C85053ABBF8A173A71F1773A538FD2608179A4737E
      FEE8E7C75E38FA9B9490ACDFB8811B55366F87F511CE46C9239673D0421757F6
      81FE283C300EF084E48C6AB1DD7EFB8694908073B084E2588B0DAC55E1D66203
      8B2ADC5F6CEB37DC1959BFFE8E3DE0438A23734ADD76DBFA8FD6AEBDCD7F4288
      4D1C7E5F169B216CE2A618F00C5FFA0C14D9F088D3966E2C23F13142CE3E2F06
      F1866EBD755D84CF4157779A9A14B33D6CF6D19F630C665FE6EEBF44C7B618CE
      890C7570FCA4EB6FB6DEBBB53B9BA9CE219889B925DB247CEEF07F279094279D
      877BAD4BC8307BB85169C3B63F9724FFB9C3BDEB14CFE937BA7898DEB6BD9893
      C951E7A1F3CE364E0B6F6EBE352524E01C2CA138D662036B55B8B5D8C0A20A0F
      14C84C9A5B6E59DB8CEC85DB8324954DEAE69B6F09DC2B6037179B76B8EF2283
      2136CED11A7D95C4F0157A44446FE2CB7AF84FD6D0C7151658B0FF170E31370A
      BF07DC7C4F7A3AF04D024F325EFF3E146C8FCAE93CF87EA40D899BBA66EA6FB6
      DEBBB9F48B5C46F74020C34CDCD5F5D9312475513E0D7D0FFB83373F891A31AE
      9EFDA7CF4A95CD47612176757DDA6EF33E730E50E8E24A1FE8A1C203EB013C21
      39A35A6C2B57DE941212700E96501C6BB181B52ADC5A6C605185FB8BAD69C5CA
      CEA6A65589152B565A36AB57AF295EB56AF5EE30BB153736D563F32172E4C6E5
      2BACA3E14D37FD551C9B0169076F3AFE8AD00D0BFDF91B6F6CB25E7C2F5FBE22
      027E56DAE2CF7FD083DEF896B50A346A241C723171CE0B5FFD62DBB96C65BF33
      0D6E4D2E38A7056F987CAD7B2E6A559AA9BFD97AEFE6D265D573C5CAD047FBF4
      B0855A7711D379A8E3FD3D7DA827F2CCEB3D313EADCBC49D6F7CD289A47C79BD
      277030C56E8B87B3EDBCF363FFDC45BB9E24FAC00BFE9D6FF47C64F97EE313E7
      DE1B5D5CE9033D5478603D80272467548B6D69E3B294907E6D0C96501C6BB181
      B52ADC5A386051853B8BAD31B674BAD43736345A0F561A6330ECDC9CA33EBA77
      3D3D8CDED892983FC11A163744C0FA855D72F9F21B9D4585EE1D2B5E6CE93BF0
      1DCCD8E1EB1515CF7ACF05165778A051014E6CA97F60917585D7A57C393D882F
      5BEE4C9C65C1585E8FAC17D3D89C157E061B9634F8071F774C87541C6BF264C2
      BC7C972D8DEB3E0C50737FC111A7945EBD2A7C9DF76CB7BF76AAA8E3B5537D3B
      0E9D4AF19944FC4B6074738DDE13B021F8FE4B74B65B8C8DE0380F33D8DFABEC
      4E6C3F78AAC48B894D87C4D9F6EF45D98E2B2CD0438507161B78427246B5D816
      D5D60D21294FEA6AEB06EA6A6A4FD72CAC718E50E8131267DB5A6CECB72ADC5A
      6C86AF7067022EAE5F5C847ED08A4DDCDA9ADA2EE46DF218B6ECEA16C5DD7CF6
      AA7C87C8F798B1E3B3DF8EB5E81DAF18F58BEA2BC02C9FC4388F4D82CF732AC7
      647D5DBD13CFFB038F2B4EA0518BEA1655294E0AFF67919E9AEA1AFF080C6F9D
      E6B1DFE7C5726B638F9F1E91937576C0E6B8AA5F7FEDC29A0E643731FB02B930
      5E3526BFEFA6CFE9260EF91E0DF13568C616328ED39E9FAD073EDEB0EDE0C729
      4FD8F731C3417F54E2661BCE20725EEBD977FAC1E774C4C2E10F2109E44C889D
      BF808D0F85077AA8F0404DC0139233AAC5367F6EE5512415224E22E88F49AC6A
      DE7CFF9EC1C55B95ADF508B7AA72DE74853B8BCDFC2D98373F0E369826BE9F53
      55E5FCDD9E4DF5FCAA627C9EC86633BF725E3F5CEB1283781BB01BCA629B84D7
      A68B884D4CD9057E8E543D7F4104CE9934FE9DC962FEC8AB28240FEB0929B81E
      A3BF182FD6AFAA26D3782ED469DE4999CFBCB995FE9500FAA4C03E4C3771DC9A
      7767ADF9DCCAF3D4CEBF8C64F2D723660179625D06A32F464E288EE47BDB7EFF
      4D8EF06366418EC0CEEA11FCB8B2095CBA822705275013B0F7A58F512DB63915
      B38B67CF2AEBAA98396B08491961BFF786F20AE7287A43597933BA61577F583B
      87371DEC8C6B7BC6EC6B0EFE0EBBF8D0ECB272EB9A9FF8A5C4D809A7C78BE372
      07D11DC39F3F49A55FF4EB0C5E31AB6CC0CB9BCFA4937B59793B7EAD7B32CF16
      BB28F83E785ECECE98F1738EDC0E8087DEA81B7FD89C74639DF3EA13528F0A78
      274C2E2ED7E4D4ADF341D72138C65F54FA229726F041D7C7B0E9435863B1ABC2
      D77115EF23F8ADC48CF0590FE6D4C8E465749E1FF67FE6F9275EE03D57C8D84C
      CDBB455EA676C3A677A687F82ED1364CCC4E73C6417ADA0F7C1CA82DBA08D20A
      7ED29CA1B6B1308DB867B86EB0D0FEA32F85B31BE9F76C5CBB017447C0AB742E
      E8CCE2EE7379E7D8B76AEE2EE49FB9F830DC404DB069463FEC7202EB21E3E2A3
      48110A56557EFDCC9B91B546D85F4A012F33867C4E43D7ECEAAB4316D258B015
      AEED0AF86335077FD52E7E33937AAAB568CACA0BB19909A7D18BE372D7A0AB61
      125C1136006CCC22AF817B9397379FB7B8B95FC724CC0FB52B2B1F8FED1C91B3
      33666415B9CD235EE82368EA948FEF068FEBD527500FECE12D36B9B85C93539D
      B1975C74D70BCE2AFC8DB7F0596553C0D7B83E9A4D1FD2D4E1727CD5AB7831F8
      D75083317C4EF26A64F2323ACF0FFB959E7FC61E78A11DD26B53F35A9197A99B
      3918379A1EE2BB2064B195333957208DE64C16B200C6A0BF065982DCBCEDE0A9
      B546D85E83D422A1FD67D21782CD46967B36AEDD6A740B908921B944D02F7379
      ABD8B66AEE2EB64AC7DF854515A809BA6906737D04D643C6C5366FCEDC4A6465
      885C670CD1CF931897255641C1AE54B6DF9501B9F418ABF0391ECE65CE24B078
      9AF87E4EF8B8DEB3E132259FFDEA6C36E08BE116CA5C883715FDF22CB64DF0AE
      D245C3A644D97D4F738837064E431AFF93C418F242F22893FEC0F5189DAF76A9
      F15C9E693C6E9D16CA7C2AE7CCF5CFF8E89B049676E2B835B7E6419A312EA376
      255E8E2C8889C84A21B5327F266E3E58B5E248BEB7EDF7DFD8C32F41968DC0CE
      EA11FC49CA261AB2F89B0427501373C6943E322E2E0D7293BBB876616D4C481D
      DB0B172EA8BEDC70D9AE54B875E6029BAA70EBCC053656E1B38C5F1E58E411BB
      5E610B7990508654A05F22311E6238472A6CA2CA6631FCD9C68E0703D5964D4D
      AD7FD9C24DFE38ED93FD45B5D535667CB5CA670337FED691117CA2E25C1BAC65
      DD658A63EA5A4B8CF9D4D35FF88C65B2E631AE2ACF9FA94D48AE3172B22E8DB1
      B9C1F64315162CBC1A99412D1684E4B248E6ACF0CA7413A7AEA6AE3CC497E99D
      AE5B8C9CFC09CA6232977AE6FECA136BF2F2A0C19CF5246EB6CD7DDE22AD87EB
      F483CF0244E38BE15722352176FE99D19CED141EE8A1C2033571E3F8398F6AB1
      F1C8BB5AC86C6D0C364B71ACB305D8150AB74EFB60850A9F6162F0A8BF40EA79
      9C6C9D11630DB162892F8D2DBDC6B18B2DBDCED7373456F0E8D9CF6749FD9231
      3C869E2DEC16F0383DCFD861FF3D2B5E43E375F01DCCD89957082A9E75A6019B
      A0C661E5EBE666731A1A9DB1EA3F7299A17C393DE031BB73D5B0B471A935768F
      8B9D7520433F47F899CF38FC4B55674CC457719C039DF79709F3383CD2BF4CF1
      2AA9B97F20224E845E5D2B38F33C5B1EC9E7F1287E36528D2CE071BEFF708DC7
      E7E6B580D17B321FBE7FF6677BB2B1F1F143A79CFE771C3A7595B29BB5FDE0C7
      FEB88931CDC20F9DBAC1CB07FD04651BE8A1F6AD7B073E4B7246B5D87811395B
      4860A5835DAB38D66524D844855B6704B002855FE925C80BD029BC0C9DC10BE6
      1299342F67F378915A1A6607779CB141AEE245B4B358BC3F5E848FC76696B483
      17C1DF1874E5425F86AD7FEF62ECCD3EF84C69BB6AE54DFEC304F4C6B7AC95F5
      54D6F81809C7CD45E6789DF0EB5C4DB02FC72E716BF1C2935839E3B7BE844DAD
      F233F5375BEFDD5CBEABEA197A4F4BAC89A6277C5AF7414F1F3A358617DAE3F9
      B40ED2BC182EE585F46C21817B33B0C902776A83DD4CA1AB302FCDF584079F21
      7D63E35C11A01BAF62067AA8F0C07A00BF567246B5D8D6AC698E0A093C49049B
      AE38D6E0C08A156E35032CA2F0C000F96AD198E65BD68EE36B41257C05C9C49B
      A16C4C8E811B5EEC0AB09980CD14E49A101BE70617AC506153C28A046792E461
      37CEE3A12F523EAC0384E18D84834FEDC7AF1F9833E1DC1A787D9920E2CE60BC
      FE012624DF6BC02721C5D473ACA96BA6FE66EBBD3BA62B052F30F94635D90499
      AF544DD9F5D627514F7EF8D62781872B86CE57B52246CCF6B35D9FE6491BB603
      F3D5F0D05FAE78CE9C4457A4F4811E66F36F624ACEA8C6BFEEB6F5D3844CD6C6
      609315C77AAA06364EE1FE0435BEC0F215EE1CA1BC3FBEB45A0C3E5571644ED3
      D6ADBB7D32DFF3F3CF44B7AFBFA3802FE0EABC6C1B7CF2255EE7688AEF42E53F
      F4E80C677CBAB18CC4C74838E43D41C4986AC625C67F055FB296FB535CDC1F1B
      F67E7D1D8CDA64A89DAEABD5DF6CBD776B579AB137F6FC31794EBDEDF60D81A7
      7C7A5E751EFE6C3232EDA2F45A571A6193186EBE6DF39935973C1B38458AE7F4
      1B5D61985EC65278603DE8BC47B5D8EEBCEBEE122181490856AC38D6A51B58A1
      C2ADCB05B03C85FB8DE0E71BE314267329E1672313F8F988F550809F74187F13
      33D84DC4663C3FCBF10F0A7023926FE2861509FD58E5D73FDA6A1FEC07DEE38D
      9073B988E1BC5E9135268702813BB5627F82A703776CE41F632D306336F58227
      FDDBF5A497D24E8D7542584D64EC8CBDCA328FB4EFE77FFEF978A4C497A3BFB1
      E655582E70F36C9BCF4317359C428BF7F3CF9D877AE8224A1FE8A1C203EB01BC
      587246B5D8F831E07821817764606325871F135A45018B28DCBA1C001BA362F8
      8B11FD388515F1A3C442E7C78FF76C0C3DD2C12F5436E3E08F450A90D086A1D7
      3904C6698A16E2DBF70796A7E2062E7BB2714CEDC27CA02B10FA22B1ED5C3EA9
      BC02070A7EF419A815F5CB3335C9D45F858DE7C79F013FBAFFF8CC0F13D5CBD0
      FACA89F9D2BFF415F0A3CEF14242DF8BBE7CEC8B7C239E2DFC71C226F48B0BE0
      A1BED1E7A998811E2A3CE01F7CACE48C6AB13DF8D0A68890C080C1F225879FC9
      5B131A2C2F930FB0310A77ECF1A3F581D82616FFF6209F9FDCFB9340E763FCCB
      011BAE6317CC538F2330B1946F2B9F74E390B1B371426AE5E41062E7F5241437
      B5F3E28A9C430F34597A237B1F3135E35F2B5875D1B9E9BAA6E965D6B3D42BBF
      F8720CFF1A212224D07FB03C813B7961972FEDF83709813E820BCE97FE3306F4
      196386E0263FCBBFEDBB3FF07066548B2F47CE552057815C057215C855205781
      5C057215C855E06205FE174728CD8B7EC9E8720000000049454E44AE426082}
    Fill.PicturePosition = ppBottomLeft
    Fill.PictureLeft = -10
    Fill.PictureTop = -10
    Fill.OpacityMirror = 154
    Fill.OpacityMirrorTo = 190
    Fill.BorderColor = 14668485
    Fill.BorderOpacity = 181
    Fill.BorderWidth = 2
    Fill.Rounding = 10
    Fill.RoundingType = rtNone
    Fill.ShadowColor = 16753536
    Fill.ShadowOffset = 0
    Fill.Angle = 0
    TimeOut = 5000
    ProgressBar.BackGroundFill.ColorMirror = clNone
    ProgressBar.BackGroundFill.ColorMirrorTo = clNone
    ProgressBar.BackGroundFill.BackGroundPictureLeft = 0
    ProgressBar.BackGroundFill.BackGroundPictureTop = 0
    ProgressBar.BackGroundFill.PictureLeft = 0
    ProgressBar.BackGroundFill.PictureTop = 0
    ProgressBar.BackGroundFill.BorderColor = clNone
    ProgressBar.BackGroundFill.Rounding = 0
    ProgressBar.BackGroundFill.ShadowOffset = 0
    ProgressBar.BackGroundFill.Angle = 0
    ProgressBar.ProgressFill.ColorMirror = clNone
    ProgressBar.ProgressFill.ColorMirrorTo = clNone
    ProgressBar.ProgressFill.BackGroundPictureLeft = 0
    ProgressBar.ProgressFill.BackGroundPictureTop = 0
    ProgressBar.ProgressFill.PictureLeft = 0
    ProgressBar.ProgressFill.PictureTop = 0
    ProgressBar.ProgressFill.BorderColor = clNone
    ProgressBar.ProgressFill.Rounding = 0
    ProgressBar.ProgressFill.ShadowOffset = 0
    ProgressBar.ProgressFill.Angle = 0
    ProgressBar.Font.Charset = DEFAULT_CHARSET
    ProgressBar.Font.Color = clWindowText
    ProgressBar.Font.Height = -11
    ProgressBar.Font.Name = 'Tahoma'
    ProgressBar.Font.Style = []
    ProgressBar.ProgressFont.Charset = DEFAULT_CHARSET
    ProgressBar.ProgressFont.Color = clWindowText
    ProgressBar.ProgressFont.Height = -11
    ProgressBar.ProgressFont.Name = 'Tahoma'
    ProgressBar.ProgressFont.Style = []
    ProgressBar.ValueFormat = '%.0f%%'
    ProgressBar.Step = 10.000000000000000000
    ProgressBar.Maximum = 100.000000000000000000
    Items = <>
    ListItems = <>
    ListItemsSettings.HTMLFont.Charset = DEFAULT_CHARSET
    ListItemsSettings.HTMLFont.Color = clWindowText
    ListItemsSettings.HTMLFont.Height = -11
    ListItemsSettings.HTMLFont.Name = 'Tahoma'
    ListItemsSettings.HTMLFont.Style = []
    Width = 450
    Height = 225
    TopLayerItems = <
      item
        Top = 75
        Left = 20
        HTMLText.Location = cpTopLeft
        HTMLText.Font.Charset = DEFAULT_CHARSET
        HTMLText.Font.Color = clWindowText
        HTMLText.Font.Height = -11
        HTMLText.Font.Name = 'Tahoma'
        HTMLText.Font.Style = []
        Fill.Color = clWhite
        Fill.ColorTo = clWhite
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.BackGroundPictureLeft = 0
        Fill.BackGroundPictureTop = 0
        Fill.PictureLeft = 0
        Fill.PictureTop = 0
        Fill.Opacity = 157
        Fill.OpacityTo = 111
        Fill.BorderColor = 10066329
        Fill.BorderOpacity = 99
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Angle = 0
        Width = 405
      end>
    Left = 192
    Top = 80
  end
end
