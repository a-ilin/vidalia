;-------------------------------------------
; $Id: $
;
; Copyright (c) 2006, Matt Edman, Justin Hipple           
; See LICENSE for licensing information
;-------------------------------------------
!include "MUI.nsh"

SetCompressor /SOLID lzma
!packhdr header.dat "upx --best header.dat"

;---------------------------------
; Global definitions
!define VIDALIA_NAME        "Vidalia"
!define VIDALIA_EXEC        "vidalia.exe"
!define VIDALIA_APPVERSION  "0.0.10"
!define VIDALIA_PRODVERSION "${VIDALIA_APPVERSION}.0" ; Product version must be x.x.x.x
!define VIDALIA_AUTHORS     "Matt Edman, Justin Hipple"
!define VIDALIA_INSTALLFILE "vidalia-${VIDALIA_APPVERSION}.exe"
!define VIDALIA_DESC "${VIDALIA_NAME} ${VIDALIA_APPVERSION}"

;--------------------------------
; Installer file details
VIAddVersionKey "ProductName"     "${VIDALIA_NAME}"
VIAddVersionKey "Comments"        "${VIDALIA_DESC}"
VIAddVersionKey "CompanyName"     "${VIDALIA_AUTHORS}"
VIAddVersionKey "LegalTrademarks" "${VIDALIA_AUTHORS}"
VIAddVersionKey "LegalCopyright"  "${VIDALIA_AUTHORS}"
VIAddVersionKey "FileVersion"     "${VIDALIA_APPVERSION}"
VIAddVersionKey "FileDescription" "${VIDALIA_DESC}"
VIProductVersion "${VIDALIA_PRODVERSION}"

;--------------------------------
; Basic installer information
Name              "${VIDALIA_NAME}"
Caption           "$(VidaliaSetupCaption)"
BrandingText      "${VIDALIA_DESC}"
OutFile           "${VIDALIA_INSTALLFILE}"
InstallDir        "$PROGRAMFILES\${VIDALIA_NAME}"
InstallDirRegKey  HKCU "Software" "${VIDALIA_NAME}"
SetOverWrite      ifnewer
AutoCloseWindow   false
ShowInstDetails   show
CRCCheck          on
XPStyle           on

;--------------------------------
; Install types
!ifndef NOINSTTYPES ; only if not defined
  InstType "Full"
  InstType "Base"
  ;InstType /COMPONENTSONLYONCUSTOM
!endif

;--------------------------------
; MUI Options
!define MUI_WELCOMEPAGE_TITLE "$(VidaliaWelcomeTitle)"
!define MUI_WELCOMEPAGE_TEXT  "$(VidaliaWelcomeText)"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE
!define MUI_FINISHPAGE_RUN "$INSTDIR\${VIDALIA_EXEC}"
!define MUI_FINISHPAGE_RUN_TEXT "$(VidaliaRunNow)"
!define MUI_FINISHPAGE_LINK "$(VidaliaLinkText)"
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
; Available languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "SimpChinese"
!include "vidalia_en.nsh"
!include "vidalia_de.nsh"
!include "vidalia_es.nsh"
!include "vidalia_fa.nsh"
!include "vidalia_fi.nsh"
!include "vidalia_fr.nsh"
!include "vidalia_pl.nsh"
!include "vidalia_pt-br.nsh"
!include "vidalia_ru.nsh"
!include "vidalia_zh-cn.nsh"

;--------------------------------
; Functions
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------
; Required Components
Section "${VIDALIA_DESC}" Vidalia
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR    
  File "..\..\release\${VIDALIA_EXEC}"
  File "..\..\release\mingwm10.dll"
  File "..\..\AUTHORS"
  File "..\..\CHANGELOG"
  File "..\..\LICENSE"
  File "..\..\COPYING"
  File "..\..\README"

  ; Include a prebuilt GeoIP cache
  SetOutPath "$APPDATA\Vidalia\geoip-cache"
  File "..\geoip-cache"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${VIDALIA_NAME}" "DisplayName" "${VIDALIA_DESC}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${VIDALIA_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${VIDALIA_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${VIDALIA_NAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe" 
SectionEnd

;--------------------------------
; Start Menu Shortcuts
; Optional section (can be disabled by the user)
Section "$(VidaliaShortcuts)" Shortcuts
  SectionIn 1
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\${VIDALIA_NAME}"
  CreateShortCut  "$SMPROGRAMS\${VIDALIA_NAME}\${VIDALIA_NAME}.lnk" "$INSTDIR\${VIDALIA_EXEC}" "" "$INSTDIR\${VIDALIA_EXEC}" 0  
  CreateShortCut  "$SMPROGRAMS\${VIDALIA_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

;--------------------------------
; Run At Startup
Section "$(VidaliaStartup)" RunAtStartup
  SectionIn 1
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${VIDALIA_NAME}" '"$INSTDIR\${VIDALIA_EXEC}"'
SectionEnd

;--------------------------------
; Uninstaller
Section "Uninstall"
  SetShellVarContext all

  ; Remove registry keys
  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run\" "${VIDALIA_NAME}"
  DeleteRegKey   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${VIDALIA_NAME}"
  DeleteRegKey   HKCU "Software\${VIDALIA_NAME}"

  ; Remove files and uninstaller
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${VIDALIA_NAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${VIDALIA_NAME}"
  RMDir /r "$INSTDIR"

  SetShellVarContext current
  RMDir /r "$APPDATA\Vidalia"
SectionEnd

;--------------------------------
; Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Vidalia} "$(VidaliaAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${Shortcuts} "$(VidaliaShortcutsDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${RunAtStartup} "$(VidaliaStartupDesc)"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

