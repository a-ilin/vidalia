;; $Id$
;; 
;; Vidalia installer and bundle Russian translation
;; 
;; Based on vidalia_en.nsh r1756
;;

; Bundle
LangString BundleSetupCaption         ${LANG_RUSSIAN} "Установка ${BUNDLE_NAME}"
LangString BundleWelcomeText          ${LANG_RUSSIAN} "Эта программа установит Tor, Vidalia, Privoxy, и Torbutton.\r\n\r\nTor это система анонимного использования сети Internet, которая поможет анонимизировать Вашу веб-деятельность, системы мгновенного обмена сообщениями, IRC, и многое другое. Vidalia это программа с оконным интерфейсом для настройки, управления и контроля Tor.\r\n\r\nPrivoxy это фильтрующий веб-прокси который помогает защитить вашу приватность, а также удаляет рекламу, баннеры, и всплывающие сообщения.\r\n\r\nTorbutton это расширение Firefox для быстрого включения/выключения режима анонимного использования браузера.\r\n\r\nЕсли вы ранее уже устанавливали Tor, Vidalia, Privoxy, или Firefox, пожалуйста убедитесь что эти программы остановлены и потом продолжите установку.\r\n\r\n$_CLICK"
LangString BundleWelcomeTitle         ${LANG_RUSSIAN} "Вас приветствует мастер установки ${BUNDLE_NAME}"
LangString BundleLinkText             ${LANG_RUSSIAN} "Инструкция по установке ${TOR_NAME}"
LangString BundleFinishText           ${LANG_RUSSIAN} "Установка завершена.\r\nПожалуйста прочитайте http://tor.eff.org/docs/tor-doc-win32.html чтобы узнать, как настроить другие приложения использовать Tor.\r\n\r\nЕсли вы установили Torbutton, вам придётся перезапустить Firefox."
LangString BundleRunNow               ${LANG_RUSSIAN} "Запустить установленные компоненты"


; Vidalia
LangString VidaliaGroupDesc           ${LANG_RUSSIAN} "Vidalia это программа с оконным интерфейсом для настройки, управления и контроля Tor."
LangString VidaliaUninstDesc          ${LANG_RUSSIAN} "Удалить ${VIDALIA_DESC}."
LangString VidaliaSetupCaption        ${LANG_RUSSIAN} "Установка ${VIDALIA_NAME}"
LangString VidaliaWelcomeText         ${LANG_RUSSIAN} "Этот мастер установит Vidalia, программу с оконным интерфейсом для настройки, управления и контроля Tor.\r\n\r\n$_CLICK"
LangString VidaliaWelcomeTitle        ${LANG_RUSSIAN} "Вас приветствует мастер установки ${VIDALIA_NAME}"
LangString VidaliaLinkText            ${LANG_RUSSIAN} "Страница проекта ${VIDALIA_NAME}"
LangString VidaliaAppDesc             ${LANG_RUSSIAN} "Vidalia это программа с оконным интерфейсом для настройки, управления и контроля Tor."
LangString VidaliaStartup             ${LANG_RUSSIAN} "Автозапуск"
LangString VidaliaStartupDesc         ${LANG_RUSSIAN} "Автоматически запускать ${VIDALIA_NAME} при загрузке системы."
LangString VidaliaShortcuts           ${LANG_RUSSIAN} "Ярлыки в Главное Меню"
LangString VidaliaShortcutsDesc       ${LANG_RUSSIAN} "Добавить ${VIDALIA_NAME} в Главное Меню."
LangString VidaliaRunNow              ${LANG_RUSSIAN} "Запустить ${VIDALIA_NAME}"


; Tor
LangString TorGroupDesc               ${LANG_RUSSIAN} "Tor это система анонимного использования сети Internet, которая поможет анонимизировать Вашу веб-деятельность, системы мгновенного обмена сообщениями, IRC, и многое другое."
LangString TorUninstDesc              ${LANG_RUSSIAN} "Удалить ${TOR_DESC}."
LangString TorAppDesc                 ${LANG_RUSSIAN} "Установить ${TOR_DESC}."
LangString TorAskOverwriteTorrc       ${LANG_RUSSIAN} "У вас уже есть файл настроек Tor.$\r$\nХотите ли вы перезаписать его файлом из дистрибутива?"
LangString TorOpenSSL                 ${LANG_RUSSIAN} "OpenSSL ${OPENSSL_APPVERSION}"
LangString TorOpenSSLDesc             ${LANG_RUSSIAN} "Установить ${OPENSSL_DESC}."
LangString TorDocumentation           ${LANG_RUSSIAN} "Документация"
LangString TorDocumentationDesc       ${LANG_RUSSIAN} "Установить документацию ${TOR_NAME}."
LangString TorShortcuts               ${LANG_RUSSIAN} "Ярлыки в Главное Меню"
LangString TorShortcutsDesc           ${LANG_RUSSIAN} "Добавить ${TOR_NAME} в Главное Меню."


; Privoxy
LangString PrivoxyGroupDesc           ${LANG_RUSSIAN} "Privoxy это фильтрующий веб-прокси который помогает защитить вашу приватность, а также удаляет рекламу, баннеры, и всплывающие сообщения."
LangString PrivoxyUninstDesc          ${LANG_RUSSIAN} "Удалить ${PRIVOXY_DESC}."
LangString PrivoxyAppDesc             ${LANG_RUSSIAN} "Установить ${PRIVOXY_DESC}."
LangString PrivoxyShortcuts           ${LANG_RUSSIAN} "Ярлыки в Главное Меню"
LangString PrivoxyShortcutsDesc       ${LANG_RUSSIAN} "Добавить ${PRIVOXY_NAME} в Главное Меню."
LangString PrivoxyStartup             ${LANG_RUSSIAN} "Автозапуск"
LangString PrivoxyStartupDesc         ${LANG_RUSSIAN} "Автоматически запускать ${PRIVOXY_NAME} при загрузке системы."


; Torbutton
LangString TorbuttonGroupDesc         ${LANG_RUSSIAN} "Torbutton это расширение Firefox для быстрого включения/выключения режима анонимного использования браузера."
LangString TorbuttonUninstDesc        ${LANG_RUSSIAN} "Удалить ${TORBUTTON_DESC}."
LangString TorbuttonAppDesc           ${LANG_RUSSIAN} "Установить ${TORBUTTON_DESC}."
LangString TorbuttonAddToFirefox      ${LANG_RUSSIAN} "Добавить в Firefox"
LangString TorbuttonAddToFirefoxDesc  ${LANG_RUSSIAN} "Добавить расширение ${TORBUTTON_DESC} в Firefox."
LangString TorbuttonFirefoxNotFound   ${LANG_RUSSIAN} "Не удалось найти Firefox на вашем компьютере. Torbutton не будет установлен."

