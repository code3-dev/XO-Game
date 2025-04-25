; XO Game Installer Script
; NSIS Modern User Interface

!include "MUI2.nsh"

; Application information
!define PRODUCT_NAME "XO Game"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Hossein Pira"
!define PRODUCT_WEB_SITE "https://github.com/code3-dev"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\XOGame.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Set compression
SetCompressor lzma

; Interface Settings
!define MUI_ABORTWARNING
; Use app.ico for installer
!define MUI_ICON "app.ico"
!define MUI_UNICON "app.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\XOGame.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Main install settings
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "build\Release\XOGame_Setup.exe"
InstallDir "$PROGRAMFILES\XO Game"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "build\Release\XOGame.exe"
  File "README.txt"
  File "LICENSE.txt"
  File "app.ico"
  
  ; Create Start Menu shortcut
  CreateDirectory "$SMPROGRAMS\XO Game"
  CreateShortCut "$SMPROGRAMS\XO Game\XO Game.lnk" "$INSTDIR\XOGame.exe" "" "$INSTDIR\app.ico"
  CreateShortCut "$SMPROGRAMS\XO Game\README.lnk" "$INSTDIR\README.txt"
  CreateShortCut "$DESKTOP\XO Game.lnk" "$INSTDIR\XOGame.exe" "" "$INSTDIR\app.ico"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\XO Game\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\XOGame.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\app.ico"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\XOGame.exe"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\LICENSE.txt"
  Delete "$INSTDIR\app.ico"

  Delete "$SMPROGRAMS\XO Game\Uninstall.lnk"
  Delete "$SMPROGRAMS\XO Game\XO Game.lnk"
  Delete "$SMPROGRAMS\XO Game\README.lnk"
  Delete "$DESKTOP\XO Game.lnk"

  RMDir "$SMPROGRAMS\XO Game"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd