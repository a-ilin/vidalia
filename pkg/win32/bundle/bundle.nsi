;---------------------------------
; Vidalia/Tor/Privoxy Bundle Installer
; See BUNDLE_LICENSE for licensing information

;
!include "MUI.nsh"

;---------------------------------
; Global Definitions
!define VIDALIA_NAME "Vidalia"
!define VIDALIA_EXEC "vidalia.exe"
!define VIDALIA_UNINST "uninstall.exe"
!define VIDALIA_VERSION "0.0.4"

!define TOR_NAME "Tor"
!define TOR_EXEC "tor.exe"
!define TOR_UNINST "Uninstall.exe"
!define TOR_VERSION "0.1.1.18-rc"

!define PRIVOXY_NAME "Privoxy"
!define PRIVOXY_EXEC "privoxy.exe"
!define PRIVOXY_UNINST "privoxy_uninstall.exe"
!define PRIVOXY_VERSION "3.0.3"

!define BUNDLE_REVISION "1"

!define APPNAME "Vidalia Bundle"
!define APPVERSION "${TOR_VERSION}-${VIDALIA_VERSION}"
!define PRODVERSION "${VIDALIA_VERSION}.${BUNDLE_REVISION}" ; Product version must be x.x.x.x
!define APPDESCRIPTION "${APPNAME} ${APPVERSION}"
!define INSTALLFILE "vidalia-bundle-${APPVERSION}.exe"


;--------------------------------
; Basic Installer Information
Name "Vidalia-Tor-Privoxy Bundle"
Caption "Vidalia-Tor-Privoxy Bundle"
BrandingText "${APPDESCRIPTION} (Rev. ${BUNDLE_REVISION})"
OutFile "${INSTALLFILE}"
CRCCheck on

VIAddVersionKey "ProductName" "${APPNAME}"
VIAddVersionKey "Comments" "${APPDESCRIPTION}"
VIAddVersionKey "FileVersion" "${APPVERSION}"
VIAddVersionKey "FileDescription" "${APPDESCRIPTION}"
VIProductVersion "${PRODVERSION}"

OutFile "${INSTALLFILE}"

InstallDir "$PROGRAMFILES"

SetCompressor /SOLID lzma
SetOverWrite ifnewer
AutoCloseWindow false
ShowInstDetails show
XPStyle on


;--------------------------------
; Install Types
!ifndef NOINSTTYPES ; only if not defined
  InstType "Full"
  InstType "Base"
  ;InstType /COMPONENTSONLYONCUSTOM
!endif


;--------------------------------
; MUI Options
!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${APPNAME} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of:\r\n\tVidalia\t${VIDALIA_VERSION}\r\n\tTor\t${TOR_VERSION}\r\n\tPrivoxy\t${PRIVOXY_VERSION}\r\n\r\nIf you have previously installed Vidalia, Tor, or Privoxy, please make sure they are not running before continuing this installation.\r\n\r\n$_CLICK"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE
!define MUI_FINISHPAGE_TEXT "Installation is complete.\r\n\r\nPlease see http://tor.eff.org/docs/tor-doc-win32.html to learn how to configure your applications to use Tor.\r\n"
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION CustomFinishFn
!define MUI_FINISHPAGE_RUN_TEXT "Run installed components now"

;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Tor
Var configdir
Var configfile
var bInstallTor
SectionGroup "!Tor ${TOR_VERSION}" TorGroup
    Section "Tor" Tor
    ;Files that have to be installed for tor to run and that the user
    ;cannot choose not to install
       SectionIn 1 2
       SetOutPath "$INSTDIR\Tor"
       File "tor\${TOR_VERSION}\tor.exe"
       File "tor\${TOR_VERSION}\tor_resolve.exe"
       File "tor\${TOR_VERSION}\${TOR_UNINST}"
       WriteIniStr "$INSTDIR\Tor\Tor Website.url" "InternetShortcut" "URL" "http://tor.eff.org"

       StrCpy $configfile "torrc"
       StrCpy $configdir $APPDATA\Tor
    ;   ;If $APPDATA isn't valid here (Early win95 releases with no updated
    ;   ; shfolder.dll) then we put it in the program directory instead.
    ;   StrCmp $APPDATA "" "" +2
    ;      StrCpy $configdir $INSTDIR
       SetOutPath $configdir
       ;If there's already a torrc config file, ask if they want to
       ;overwrite it with the new one.
       IfFileExists "$configdir\torrc" "" endiftorrc
          MessageBox MB_ICONQUESTION|MB_YESNO "You already have a Tor config file.$\r$\nDo you want to overwrite it with the default sample config file?" IDNO yesreplace
          Delete $configdir\torrc
          Goto endiftorrc
         yesreplace:
          StrCpy $configfile "torrc.sample"
       endiftorrc:
       File /oname=$configfile "tor\${TOR_VERSION}\torrc.sample"

       ; Write the installation path into the registry
       WriteRegStr HKCU SOFTWARE\Tor "Install_Dir" "$INSTDIR"

       ; Write the uninstall keys for Windows
       WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "DisplayName" "${TOR_NAME} ${TOR_VERSION}"
       WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "UninstallString" '"$INSTDIR\Tor\${TOR_UNINST}"'
       WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "NoModify" 1
       WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "NoRepair" 1
       
       IntOp $bInstallTor 0 + 1
    SectionEnd

    Section "OpenSSL 0.9.8a" TorOpenSSL
       SectionIn 1 2
       SetOutPath "$INSTDIR\Tor"
       File "tor\${TOR_VERSION}\libeay32.dll"
       File "tor\${TOR_VERSION}\ssleay32.dll"
    SectionEnd

    Section "Documents" TorDocs
       SectionIn 1 2
       SetOutPath "$INSTDIR\Tor\Documents"
       File "tor\${TOR_VERSION}\Documents\*.*"
    SectionEnd

    SubSection /e "Shortcuts" TorShortcuts
        Section "Start Menu" TorStartMenu
           SectionIn 1
           SetOutPath "$INSTDIR\Tor"
           IfFileExists "$SMPROGRAMS\Tor\*.*" "" +2
              RMDir /r "$SMPROGRAMS\Tor"
           CreateDirectory "$SMPROGRAMS\Tor"
           CreateShortCut "$SMPROGRAMS\Tor\Tor.lnk" "$INSTDIR\Tor\tor.exe"
           CreateShortCut "$SMPROGRAMS\Tor\Torrc.lnk" "Notepad.exe" "$configdir\torrc"
           CreateShortCut "$SMPROGRAMS\Tor\Tor Website.lnk" "$INSTDIR\Tor\Tor Website.url"
           CreateShortCut "$SMPROGRAMS\Tor\Uninstall.lnk" "$INSTDIR\Tor\${TOR_UNINST}"
           IfFileExists "$INSTDIR\Tor\Documents\*.*" "" endifdocs
              CreateDirectory "$SMPROGRAMS\Tor\Documents"
              CreateShortCut "$SMPROGRAMS\Tor\Documents\Tor Documentation.lnk" "$INSTDIR\Tor\Documents"
              CreateShortCut "$SMPROGRAMS\Tor\Documents\Version Specification.lnk" "$INSTDIR\Tor\Documents\version-spec.txt"
           endifdocs:
        SectionEnd

        Section "Desktop" TorDesktop
           SectionIn 1
           SetOutPath "$INSTDIR\Tor"
           CreateShortCut "$DESKTOP\Tor.lnk" "$INSTDIR\Tor\tor.exe"
        SectionEnd
    SubSectionEnd
SectionGroupEnd


;--------------------------------
; Vidalia
var bInstallVidalia
SectionGroup "Vidalia ${VIDALIA_VERSION}" VidaliaGroup
    Section "Vidalia" Vidalia
      SectionIn 1 2

      ; Set output path to the installation directory.
      SetOutPath "$INSTDIR\Vidalia"
      File "Vidalia\${VIDALIA_VERSION}\${VIDALIA_EXEC}"
      FILE "Vidalia\${VIDALIA_VERSION}\${VIDALIA_UNINST}"
      File "Vidalia\${VIDALIA_VERSION}\mingwm10.dll"
      File "Vidalia\${VIDALIA_VERSION}\README"
      File "Vidalia\${VIDALIA_VERSION}\CHANGELOG"
      File "Vidalia\${VIDALIA_VERSION}\LICENSE"
      File "Vidalia\${VIDALIA_VERSION}\COPYING"
      File "Vidalia\${VIDALIA_VERSION}\AUTHORS"
      File "BUNDLE_LICENSE"

      ; Write the uninstall keys for Windows
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "DisplayName" "${VIDALIA_NAME} ${VIDALIA_VERSION}"
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "UninstallString" '"$INSTDIR\Vidalia\${VIDALIA_UNINST}"'
      WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "NoModify" 1
      WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "NoRepair" 1
      
      IntOp $bInstallVidalia 0 + 1
    SectionEnd

    ;--------------------------------
    ; Start Menu Shortcuts
    ; Optional section (can be disabled by the user)
    Section "Start Menu Shortcuts" VidaliaShortcuts
      SectionIn 1 2
      CreateDirectory "$SMPROGRAMS\Vidalia"
      CreateShortCut "$SMPROGRAMS\Vidalia\Uninstall.lnk" "$INSTDIR\Vidalia\uninstall.exe" "" "$INSTDIR\Vidalia\${VIDALIA_UNINST}" 0
      CreateShortCut "$SMPROGRAMS\Vidalia\Vidalia.lnk" "$INSTDIR\Vidalia\${VIDALIA_EXEC}" "" "$INSTDIR\Vidalia\${VIDALIA_EXEC}" 0
    SectionEnd

    ;--------------------------------
    ; Run At Startup
    Section "Run At Startup" VidaliaRunAtStartup
      SectionIn 1 2
      WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${VIDALIA_NAME}" '"$INSTDIR\Vidalia\${VIDALIA_EXEC}"'
    SectionEnd    
SectionGroupEnd


;--------------------------------
; Privoxy
var bInstallPrivoxy
SectionGroup "Privoxy ${PRIVOXY_VERSION}" PrivoxyGroup
    Section "Privoxy" Privoxy
        SectionIn 1 2
        ; add files / whatever that need to be installed here.
        SetOutPath "$INSTDIR\Privoxy"
        File /r Privoxy\${PRIVOXY_VERSION}\*.*

        WriteRegStr HKEY_CLASSES_ROOT "PrivoxyActionFile\shell\open\command" "" 'Notepad.exe "%1"'
        WriteRegStr HKEY_CLASSES_ROOT ".action" "" "PrivoxyActionFile"
        WriteRegStr HKEY_CLASSES_ROOT "PrivoxyFilterFile\shell\open\command" "" 'Notepad.exe "%1"'
        WriteRegStr HKEY_CLASSES_ROOT ".filter" "" "PrivoxyFilterFile"
       
        ; Write the installation path into the registry
        WriteRegStr HKCU SOFTWARE\Privoxy "Install_Dir" "$INSTDIR"

        ; Write the uninstall keys for Windows
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "DisplayName" "${PRIVOXY_NAME} ${PRIVOXY_VERSION}"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "UninstallString" '"$INSTDIR\Privoxy\${PRIVOXY_UNINST}"'
        WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "NoModify" 1
        WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "NoRepair" 1
               
       
        IntOp $bInstallPrivoxy 0 + 1
    SectionEnd

    Section "Add to Start Menu" PrivoxyStartMenu
        SectionIn 1
        SetShellVarContext all ; (Add to "All Users" Start Menu if possible)
        RMDir /r "$SMPROGRAMS\Privoxy"
        CreateDirectory "$SMPROGRAMS\Privoxy"
        CreateShortCut "$SMPROGRAMS\Privoxy\Privoxy.lnk" "$INSTDIR\Privoxy\privoxy.exe"
        WriteINIStr "$SMPROGRAMS\Privoxy\Web-based Configuration.url" "InternetShortcut" "URL" "http://config.privoxy.org/"
        CreateShortCut "$SMPROGRAMS\Privoxy\Web-based Feedback.lnk" "$INSTDIR\Privoxy\doc\user-manual\contact.html"
        CreateDirectory "$SMPROGRAMS\Privoxy\Edit Config"
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Main Configuration.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\config.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Default Actions.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\default.action"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\User Actions.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\user.action"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Filters.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\default.filter"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Trust list.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\trust.txt"'
        CreateDirectory "$SMPROGRAMS\Privoxy\Documentation"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\User Manual.lnk" "$INSTDIR\Privoxy\doc\user-manual\index.html"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Frequently Asked Questions.lnk" "$INSTDIR\Privoxy\doc\faq\index.html"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Credits.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\AUTHORS.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\License.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\LICENSE.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\ReadMe file.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\README.txt"'
        WriteINIStr "$SMPROGRAMS\Privoxy\Documentation\Web Site.url" "InternetShortcut" "URL" "http://privoxy.org/"
        CreateShortCut "$SMPROGRAMS\Privoxy\Privoxy Uninstall.lnk" "$INSTDIR\Privoxy\${PRIVOXY_UNINST}"
    SectionEnd
 
    ;--------------------------------
    ; Run At Startup
    Section "Run At Startup" PrivoxyRunAtStartup
      SectionIn 1
      CreateShortCut "$SMSTARTUP\Privoxy.lnk" "$INSTDIR\Privoxy\privoxy.exe" "" "" 0 SW_SHOWMINIMIZED
    SectionEnd     
SectionGroupEnd

Function CustomFinishFn
    IntCmp $bInstallVidalia 1 run_vidalia check_tor check_tor
    run_vidalia:
        Exec '"$INSTDIR\Vidalia\${VIDALIA_EXEC}"'
        goto check_privoxy
    
    check_tor:
    IntCmp $bInstallTor 1 run_tor check_privoxy check_privoxy
    run_tor:
        Exec '"$INSTDIR\Tor\${TOR_EXEC}"'
    
    check_privoxy:
    IntCmp $bInstallPrivoxy 1 run_privoxy done done
    run_privoxy:
        Exec '"$INSTDIR\Privoxy\${PRIVOXY_EXEC}"'
    done:
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Tor} "Install ${TOR_NAME} ${TOR_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorGroup} "Install ${TOR_NAME} ${TOR_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorDocs} "Install ${TOR_NAME} documentation"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorOpenSSL} "Install OpenSSL 0.9.7e"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorShortcuts} "Add ${TOR_NAME} shortcuts to your Start menu"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorStartMenu} "Add ${TOR_NAME} to your Start menu"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorDesktop} "Add ${TOR_NAME} to your desktop"

  !insertmacro MUI_DESCRIPTION_TEXT ${Vidalia} "Install ${VIDALIA_NAME} ${VIDALIA_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaGroup} "Install ${VIDALIA_NAME} ${VIDALIA_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaShortcuts} "Add ${VIDALIA_NAME} to your Start menu"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaRunAtStartup} "Run ${VIDALIA_NAME} at startup"

  !insertmacro MUI_DESCRIPTION_TEXT ${Privoxy} "Install ${PRIVOXY_NAME} ${PRIVOXY_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${PrivoxyGroup} "Install ${PRIVOXY_NAME} ${PRIVOXY_VERSION}"
  !insertmacro MUI_DESCRIPTION_TEXT ${PrivoxyStartMenu} "Add ${PRIVOXY_NAME} to your Start menu"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

