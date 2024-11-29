# DeepLinkSDK

SDK of the Deeplink program

## Overview

DeepLink has two versions: the regular version and the SDK version. Generally, the regular version can meet the remote needs of users. In special scenarios such as cloud Internet cafes or when secondary development is required, you can download and use the SDK version.

View and download the [latest SDK version](https://github.com/DeepLinkProtocol/DeepLinkSDK-Win/releases/latest) here.

You can also view it on the https://download.deeplink.cloud page.

After the SDK version is installed, a registry key named "InstallType" with a value of "SDK" will be written in the registry HKLM `"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\DeepLink"`.

## deeplink_ui

An example of rendering a web page using qt.

Reference project: <https://doc.qt.io/qt-6/qtwebengine-webenginewidgets-simplebrowser-example.html>

For interaction between qt and JS, please check [bridge class](./deeplink_ui/bridge.h), [JS example](./deeplink_ui/index.html)

Do not forget [qwebchannel.js](./deeplink_ui/qwebchannel.js)

## sdk_example

Example of a provider implemented with QT.

[DeepLink SDK USER MANUAL](./docs/sdk_user_manual.md)

## win32_sdk_example

Example of a provider implemented with pure WIN32 API.

## Notice

All code samples are for reference only.

- [Configuration file documentation](./docs/configuration.md)
- [Cloud Internet Cafe key information](./docs/cloud_internet_cafe_key_information.md)
- [Installation Instructions](./docs/installation_instructions.md)
