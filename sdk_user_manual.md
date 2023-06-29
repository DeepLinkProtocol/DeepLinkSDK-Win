# DeepLink SDK USER MANUAL

DeepLink 接口文档

## 1.概述 Overview

DeepLink是一款基于区块链技术的远程控制软件。

## 2.支持语言 Language support

目前提供 C++ 接口，已进程间通信方式提供，使用 JSON 格式交换数据。

Currently, a C++ interface is provided, and inter-process communication is provided, using JSON format to exchange data.

## 3.工作模型

## 4.使用规范 Usage

### 1.连接命名管道 Connect named pipe

命名管道暂定名称 "deeplink_sdk"

Named pipe tentative name "deeplink_sdk"

### 2.验证权限 Authority verification

发送数据，deeplink会验证 id 和 token，不符合条件的连接会被拒绝并断开。

Send data, deeplink will verify the id and token, and the connection that does not meet the conditions will be rejected and disconnected.

```json
{
    "method": "auth",
    "data": {
        "id": "deeplink_test",
        "token": "0123456789"
    }
}
```

回复数据 Reply data：

```json
{
    "method": "auth",
    "code": 0,
    "message": "ok"
}
```

### 3.发起远程连接 Open remote connection

发送数据

```json
{
    "method": "openConnect",
    "data": {
        // 远程设备的识别码
        "device": "123456789",
        // 远程设备的验证码
        "password": "123456",
        // 以全屏方式打开 (deeplink v0.0.0.5 增加)
        "fullscreen": true
    }
}
```

远程视频进程会创建一个新的管道用来传输状态信息，管道名称见如下回复

The remote video process will create a new pipe to transmit status information, see the reply below for the name of the pipe

```json
{
    "method": "openConnect",
    "code": 0,
    "message": "ok",
    "data": {
        "remote_ipc": "connect001"
    }
}
```

### 4.获取远程传输状态信息 Get remote transmission status

远程视频进程主动向连接管道的用户发送视频传输状态信息，每秒发送一次

The remote video process actively sends video transmission status information to the user connected to the pipeline, and sends it once per second

```json
{
    "method": "videoInfo",
    "data": {
        "timestamp": 1683603451,
        "rtt": 1,
        "lost_rate": 0,
        "est_bw": 5162,
        "encode_fps": 32,
        "encode_bitrate": 10080
    }
}
```

### 5.结束远程连接 Close remote connection

发送数据
```json
{
    "method": "closeConnect",
    "data": {
        // 远程设备的识别码
        "device": "123456789"
    }
}
```

回复数据
```json
{
    "method": "closeConnect",
    "code": 0,
    "message": "ok"
}
```

### 6. 设置远程连接的一些参数 Set some parameters of the remote connection

发送数据
```json
{
    "method": "setConnectOption",
    "data": {
        "option": kOptionVideoBitrate,
        "value": "0"
    }
}
```

回复数据
```json
{
    "method": "setConnectOption",
    "code": 0,
    "message": "ok"
}
```

有关 option 、返回值和错误码请参考 [deeplink_sdk.h](./include/deeplink_sdk.h)

For options, return values and error codes, please refer to [deeplink_sdk.h](./include/deeplink_sdk.h)

注意: 此消息要在远程视频进程中创建的新管道中通信，参考 4.获取远程传输状态信息。

Note: This message should be communicated in the new pipeline created in the remote video process, refer to 4.获取远程传输状态信息 Get remote transmission status.

### 7. 被远程控制开始时会收到通知 Get notified when remote controlled is started

收到数据
```json
{
    "method": "remoteControlledStarted",
    "data": {
        // 发起远程的设备的识别码
        "device": "123456789"
    }
}
```

注意: 此消息不需要回复。

Note: This message does not require a reply.

### 8. 可以断开被远程控制的状态 Close the remote controlled state

发送数据
```json
{
    "method": "closeRemoteControlled",
    "data": {
        // 发起远程的设备的识别码
        "device": "123456789"
    }
}
```

回复数据
```json
{
    "method": "closeRemoteControlled",
    "code": 0,
    "message": "ok"
}
```

### 9. 被远程控制结束时会收到通知 Get notified when remote controlled is stopped

收到数据
```json
{
    "method": "remoteControlledStopped",
    "data": {
        // 发起远程的设备的识别码
        "device": "123456789"
    }
}
```

注意: 此消息不需要回复。

Note: This message does not require a reply.

### 10. 启用/禁用被远程控制开始时的提示窗口 Enable/disable prompt window when remote controled is started

发送数据
```json
{
    "method": "setRemoteControlledPrompt",
    "data": {
        "value": "off"
    }
}
```

回复数据
```json
{
    "method": "setRemoteControlledPrompt",
    "code": 0,
    "message": "ok"
}
```

发送数据
```json
{
    "method": "getRemoteControlledPrompt",
    "data": {}
}
```

回复数据
```json
{
    "method": "getRemoteControlledPrompt",
    "code": 0,
    "message": "ok",
    "data": {
        "value": "on"
    }
}
```

### 11. 远程视频进程与被控端互相通信 The remote video process communicates with the controlled end

1. 任何一方都可以发送数据。

发送数据
```json
{
    "method": "sendData",
    "to_device": "xxxxxx",
    "data": {
        "int": 666,
        "double": 3.1415926,
        "bool": true,
        "string": "this is a test string"
    }
}
```

回复数据
```json
{
    "method": "sendData",
    "code": 0,
    "message": "ok"
}
```

2. 接收数据通知。

```json
{
    "method": "receivedData",
    "from_device": "xxxxxx",
    "data": {
        "int": 666,
        "double": 3.1415926,
        "bool": true,
        "string": "this is a test string"
    }
}
```

注意: 此消息不需要回复。

Note: This message does not require a reply.

## 5. 设备信息接口 Device info interface

除了上述的接口外，还有些接口用来获取设备信息。

In addition to the above-mentioned interfaces, there are also some interfaces used to obtain device information.

### 1. 获取设备信息 Get device info

发送数据
```json
{
    "method": "getDeviceInfo",
    "data": {}
}
```

回复数据
```json
{
    "method": "getDeviceInfo",
    "code": 0,
    "message": "ok",
    "data": {
        "device_id": "123456789",
        "password": "123456"
    }
}
```

### 2. 获取应用的版本号 Get application version

发送数据
```json
{
    "method": "getAppVersion",
    "data": {}
}
```

回复数据
```json
{
    "method": "getAppVersion",
    "code": 0,
    "message": "ok",
    "data": {
        "version": "1.0.1.2"
    }
}
```

### 3. 设备信息变动通知 Device Information Change Notification

收到数据
```json
{
    "method": "deviceInfoChanged",
    "data": {
        "device_id": "123456789",
        "password": "123456"
    }
}
```

注意: 此消息不需要回复。

Note: This message does not require a reply.

### 4. 服务器连接状态变化通知 Server connection status change notification

收到数据
```json
{
    "method": "serverConnStateChanged",
    "data": {
        "state": 1
    }
}
```

|   | state        | 状态 |
| - | ------------ | ---- |
| 0 | DisConnected | 未连接 |
| 1 | Connected    | 已连接 |
| 2 | Registered   | 已注册设备 |
| 3 | Onlined      | 上线(此时才会启动 P2P 远程控制监听) |
| 4 | Logined      | 已登录钱包 |

注意: 此消息不需要回复。

Note: This message does not require a reply.

如果想要手动查询，可以发送如下的请求，可以收到包含状态码的回复数据。

If you want to query manually, you can send the following request, and you can receive the reply data including the status code.

发送数据
```json
{
    "method": "getServerConnState",
    "data": {}
}
```

回复数据
```json
{
    "method": "getServerConnState",
    "code": 0,
    "message": "ok",
    "data": {
        "state": 1
    }
}
```
