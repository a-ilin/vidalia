;-------------------------------------------
; vidalia.nsi
;
;  Copyright (c) 2006, Matt Edman, Justin Hipple           
;  See LICENSE for licensing information 
;  + Russian translation
;-------------------------------------------
!include "MUI.nsh"

;---------------------------------
; Global Definitions
!define APPVERSION "0.0.8"
!define PRODVERSION "${APPVERSION}.0" ; Product version must be x.x.x.x
!define APPNAME "Vidalia"
!define APPEXEC "vidalia.exe"
!define APPDESCRIPTION "${APPNAME} ${APPVERSION}"
!define AUTHOR "Matt Edman, Justin Hipple"
!define INSTALLFILE "vidalia-${APPVERSION}.exe"


;--------------------------------
; Basic Installer Information
Name "${APPNAME}"
Caption "$(LSetup)"
BrandingText "${APPDESCRIPTION}"
OutFile "${INSTALLFILE}"
CRCCheck on

;--------------------------------

VIAddVersionKey "ProductName" "${APPNAME}"
VIAddVersionKey "Comments" "${APPDESCRIPTION}"
VIAddVersionKey "CompanyName" "${AUTHOR}"
VIAddVersionKey "LegalTrademarks" "${AUTHOR}"
VIAddVersionKey "LegalCopyright" "й ${AUTHOR}"
VIAddVersionKey "FileVersion" "${APPVERSION}"
VIAddVersionKey "FileDescription" "${APPDESCRIPTION}"
VIProductVersion "${PRODVERSION}"

OutFile "${INSTALLFILE}"

InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKCU "Software" "${APPNAME}"

SetOverWrite ifnewer
AutoCloseWindow false
ShowInstDetails show
SetCompressor /SOLID lzma
!packhdr header.dat "upx --best header.dat"
XPStyle on

;!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
; Install Types
!ifndef NOINSTTYPES ; only if not defined
  InstType "Full"
  InstType "Base"
  ;InstType /COMPONENTSONLYONCUSTOM
!endif


;--------------------------------
; MUI Options
!define MUI_WELCOMEPAGE_TITLE "$(LWelcomeTitle)"
!define MUI_WELCOMEPAGE_TEXT "$(LWelcomeText)"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APPEXEC}"
!define MUI_FINISHPAGE_RUN_TEXT "$(LRunNowText)"
!define MUI_FINISHPAGE_LINK "$(LProjectText)"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.vidalia-project.net"

;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

LangString LSetup ${LANG_ENGLISH} "${APPDESCRIPTION} Setup"
LangString LSetup ${LANG_RUSSIAN} "Установка ${APPDESCRIPTION}"

LangString LWelcomeTitle ${LANG_ENGLISH} "Welcome to the ${APPNAME} Setup Wizard"
LangString LWelcomeTitle ${LANG_RUSSIAN} "Вас приветствует мастер установки ${APPNAME}"

LangString LWelcomeText ${LANG_ENGLISH} "This wizard will guide you through the installation of ${APPDESCRIPTION}.\r\n\r\n$_CLICK"
LangString LWelcomeText ${LANG_RUSSIAN} "Эта программа установит ${APPDESCRIPTION} на Ваш компьютер.\r\n\r\n$_CLICK"

LangString LRunNowText ${LANG_ENGLISH} "Run ${APPNAME} now"
LangString LRunNowText ${LANG_RUSSIAN} "Запустить ${APPNAME} сейчас"

LangString LAppSec ${LANG_ENGLISH} "${APPNAME} (required)"
LangString LAppSec ${LANG_RUSSIAN} "${APPNAME} (обязательно)"

LangString LMainMenuSec ${LANG_ENGLISH} "Add to Start Menu"
LangString LMainMenuSec ${LANG_RUSSIAN} "Ярлыки в Главное меню"

LangString LStartupSec ${LANG_ENGLISH} "Run At Startup"
LangString LStartupSec ${LANG_RUSSIAN} "Автозапуск"

LangString LAppDesc ${LANG_ENGLISH} "${APPNAME} Executable"
LangString LAppDesc ${LANG_RUSSIAN} "Исполняемый файл ${APPNAME}"

LangString LMainMenuDesc ${LANG_ENGLISH} "Add ${APPNAME} to your Start Menu"
LangString LMainMenuDesc ${LANG_RUSSIAN} "Добавить ярлыки в Главное Меню"

LangString LStartupDesc ${LANG_ENGLISH} "Automatically start ${APPNAME} at startup"
LangString LStartupDesc ${LANG_RUSSIAN} "Автоматически запускать ${APPNAME}  при загрузке системы"

LangString LProjectText ${LANG_ENGLISH} "{APPNAME} project site"
LangString LProjectText ${LANG_RUSSIAN} "Сайт проекта ${APPNAME}"

;--------------------------------
; Functions

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------
; Required Components
Section "$(LAppSec)" Vidalia
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR    
  File "..\..\release\${APPEXEC}"
  File "..\..\release\mingwm10.dll"
  File "..\..\AUTHORS"
  File "..\..\CHANGELOG"
  File "..\..\LICENSE"
  File "..\..\COPYING"
  File "..\..\README"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME} ${APPVERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe" 
SectionEnd

;--------------------------------
; Start Menu Shortcuts
; Optional section (can be disabled by the user)
Section "$(LMainMenuSec)" Shortcuts
  SectionIn 1 2
  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPEXEC}" "" "$INSTDIR\${APPEXEC}" 0  
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


;--------------------------------
; Run At Startup
Section "$(LStartupSec)" RunAtStartup
  SectionIn 1 2
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}" '"$INSTDIR\${APPEXEC}"'
SectionEnd


;--------------------------------
; Uninstaller
Section "Uninstall"
  SetShellVarContext all

  ; Remove registry keys
  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run\" "${APPNAME}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKCU "Software\${APPNAME}"

  ; Remove files and uninstaller
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir /r "$INSTDIR"
  RMDir /r "$APPDATA\Vidalia"
SectionEnd

;--------------------------------
; Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Vidalia} "$(LAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${Shortcuts} "$(LMainMenuDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${RunAtStartup} "$(LStartupDesc)"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
