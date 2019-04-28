xcopy "GUI\GLL_Res"   "D:\Programming\Libraries\NGin\NGin_Resource"    /e /i
ROBOCOPY ".\GUI"      "D:\Programming\Libraries\NGin\Include\GUI"      *.h /E
ROBOCOPY ".\Maths"    "D:\Programming\Libraries\NGin\Include\Maths"    *.h /E
ROBOCOPY ".\Network"  "D:\Programming\Libraries\NGin\Include\Network"  *.h /E
ROBOCOPY ".\rapidxml" "D:\Programming\Libraries\NGin\Include\rapidxml" *.hpp /E
ROBOCOPY ".\Scripts"  "D:\Programming\Libraries\NGin\Include\Scripts"  *.h /E

copy "x64\Release\NGin_x64_Release.lib"   "D:\Programming\Libraries\NGin\NGin_x64_Release.lib"
copy "x64\Debug\NGin_x64_Debug.lib"   "D:\Programming\Libraries\NGin\NGin_x64_Debug.lib"