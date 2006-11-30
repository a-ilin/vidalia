;; $Id: $
;; 
;; Vidalia installer and bundle English translation
;; 
;; All translations should be derived from this file.
;;
;; To translate this file:
;;   
;;    1) Copy this file (vidalia_en.nsh), creating a new file with "en"
;;       replaced by the two-letter language code of your translated language.
;;       For example, if you are creating a German translation, your new file
;;       would be named "vidalia_de.nsh".
;;
;;    2) Replace all ${LANG_ENGLISH} tags with whatever language you are
;;       translating this file into. For example, if you are creating a German
;;       translation, all ${LANG_ENGLISH} tags would become ${LANG_GERMAN}.
;;
;;    3) Translate the quoted strings. You can move string replacement tags
;;       (e.g., ${BUNDLE_NAME}) around within the quoted strings, but you 
;;       should not alter the tags themselves.
;;
;; Your resulting file should use a non-Unicode character encoding, since NSIS 
;; does not support Unicode.
;; 


; Bundle
LangString BundleSetupCaption         ${LANG_CHINESE} "安装 ${BUNDLE_NAME}"
LangString BundleWelcomeText          ${LANG_CHINESE} "本向导将指导你如何安装 Tor、Vidalia、Privoxy 和 Torbutton。\r\n\r\n如果你曾经安装过 Tor、Vidalia 或 Firefox，在继续安装前请确保它们没有运行。\r\n\r\n$_CLICK"
LangString BundleWelcomeTitle         ${LANG_CHINESE} "欢迎来到 ${BUNDLE_NAME} 安装向导"
LangString BundleLinkText             ${LANG_CHINESE} "${TOR_NAME} 安装文档"
LangString BundleFinishText           ${LANG_CHINESE} "安装已经结束。r\n请访问 http://tor.eff.org/docs/tor-doc-win32.html 学习如何配置你的应用程序使用 Tor。\r\n\r\n如果安装了 Torbutton，你需要重新启动 Firefox。"
LangString BundleRunNow               ${LANG_CHINESE} "立刻运行安装的部件"

; Vidalia
LangString VidaliaGroupDesc           ${LANG_CHINESE} "安装 ${VIDALIA_DESC}。"
LangString VidaliaSetupCaption        ${LANG_CHINESE} "安装 ${VIDALIA_NAME}"
LangString VidaliaWelcomeText         ${LANG_CHINESE} "本向导将指导你如何安装 ${VIDALIA_DESC}。\r\n\r\n$_CLICK"
LangString VidaliaWelcomeTitle        ${LANG_CHINESE} "欢迎来到 ${VIDALIA_NAME} 安装向导"
LangString VidaliaLinkText            ${LANG_CHINESE} "${VIDALIA_NAME} 主页"
LangString VidaliaAppDesc             ${LANG_CHINESE} "安装 ${VIDALIA_DESC}。"
LangString VidaliaStartup             ${LANG_CHINESE} "启动时运行"
LangString VidaliaStartupDesc         ${LANG_CHINESE} "系统启动时自动运行 ${VIDALIA_NAME}。"
LangString VidaliaShortcuts           ${LANG_CHINESE} "添加到开始菜单"
LangString VidaliaShortcutsDesc       ${LANG_CHINESE} "把 ${VIDALIA_NAME} 添加到你的开始菜单。"
LangString VidaliaRunNow              ${LANG_CHINESE} "立刻运行 ${VIDALIA_NAME}"

; Tor
LangString TorGroupDesc               ${LANG_CHINESE} "安装 ${TOR_DESC}。"
LangString TorAppDesc                 ${LANG_CHINESE} "安装 ${TOR_DESC}。"
LangString TorAskOverwriteTorrc       ${LANG_CHINESE} "你已有一个 Tor 配置文件。$\r$\n你想用缺省的示例配置文件覆盖吗？"
LangString TorOpenSSL                 ${LANG_CHINESE} "OpenSSL ${OPENSSL_APPVERSION}"
LangString TorOpenSSLDesc             ${LANG_CHINESE} "安装 ${OPENSSL_DESC}。"
LangString TorDocumentation           ${LANG_CHINESE} "文档"
LangString TorDocumentationDesc       ${LANG_CHINESE} "安装 ${TOR_NAME} 文档。"
LangString TorShortcuts               ${LANG_CHINESE} "添加到开始菜单"
LangString TorShortcutsDesc           ${LANG_CHINESE} "把 ${TOR_NAME} 添加到你的开始菜单。"

; Privoxy
LangString PrivoxyGroupDesc           ${LANG_CHINESE} "安装 ${PRIVOXY_DESC}"
LangString PrivoxyAppDesc             ${LANG_CHINESE} "安装 ${PRIVOXY_DESC}。"
LangString PrivoxyShortcuts           ${LANG_CHINESE} "添加到开始菜单"
LangString PrivoxyShortcutsDesc       ${LANG_CHINESE} "把 ${PRIVOXY_NAME} 添加到你的开始菜单。"
LangString PrivoxyStartup             ${LANG_CHINESE} "启动时运行"
LangString PrivoxyStartupDesc         ${LANG_CHINESE} "系统启动时自动运行 ${PRIVOXY_NAME}。"

; Torbutton
LangString TorbuttonGroupDesc         ${LANG_CHINESE} "安装 ${TORBUTTON_DESC}。"
LangString TorbuttonAppDesc           ${LANG_CHINESE} "安装 ${TORBUTTON_DESC}。"
LangString TorbuttonAddToFirefox      ${LANG_CHINESE} "添加到 Firefox"
LangString TorbuttonAddToFirefoxDesc  ${LANG_CHINESE} "把 ${TORBUTTON_DESC} 扩展添加到 Firefox。"
LangString TorbuttonFirefoxNotFound   ${LANG_CHINESE} "你的系统中没有找到 Firefox，Torbutton 将不安装。"
