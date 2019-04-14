[Setup]
AppName=NewMixer
AppVersion=0.3.0
; DisableDirPage=yes
DefaultDirName={pf}\NewMixer
DefaultGroupName=NewMixer
OutputBaseFilename=NewMixer-Win-0.3.0
OutputDir=.
LicenseFile=..\NewMixer_License.txt
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations=yes

[Files]
Source: "..\..\Builds\VisualStudio2017\x64\Release\App\NewMixer.exe"; DestDir: "{app}"

[Icons]
Name: "{group}\NewMixer"; Filename: "{app}\NewMixer.exe"

[Registry]
Root: HKCR; Subkey: ".chow";                        ValueData: "NewMixer";             Flags: uninsdeletevalue; ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "NewMixer";                     ValueData: "Program NewMixer";     Flags: uninsdeletekey;   ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "NewMixer\DefaultIcon";         ValueData: "{app}\NewMixer.exe,0";                          ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "NewMixer\shell\open\command";  ValueData: """{app}\NewMixer.exe"" ""%1""";                 ValueType: string;  ValueName: ""

