# DeepLinkSDK

DeepLink 程序的 SDK。

## 概述

DeepLink 有普通版本和 SDK 版本两个版本，一般情况下，普通版本便能满足用户的远程需求，在云网吧等特殊场景或者需要二次开发时，可以下载使用 SDK 版本。

在此查看和下载 [最新的 SDK 版本](https://github.com/DeepLinkProtocol/DeepLinkSDK-Win/releases/latest)。

也可以在 https://download.deeplink.cloud 页面查看.

SDK 版本安装后会在注册表 HKLM 的 `"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\DeepLink"` 中写入名为 "InstallType"，值为 "SDK" 的注册表项。

## deeplink_ui

使用 qt 渲染网页的示例。

参考项目: <https://doc.qt.io/qt-6/qtwebengine-webenginewidgets-simplebrowser-example.html>

qt 与 JS 的交互请查看 [bridge 类](./deeplink_ui/bridge.h), [JS 示例](./deeplink_ui/index.html)

不要忘记 [qwebchannel.js](./deeplink_ui/qwebchannel.js)

## sdk_example

使用 Qt 实现的 SDK 调用示例。

[DeepLink SDK 用户手册](./docs/sdk_user_manual_cn.md)

## win32_sdk_example

使用纯 WIN32 API 实现的 SDK 调用示例。

## Notice

所有代码示例仅供参考。

- [DeepLink 配置文件相关说明](./docs/configuration_cn.md)
- [云网吧关键信息](./docs/cloud_internet_cafe_key_information_cn.md)
- [安装程序说明文档](./docs/installation_instructions.md)
