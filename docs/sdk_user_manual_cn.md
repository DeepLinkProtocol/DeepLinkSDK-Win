# DeepLink SDK 用户手册

## 1. 概述

DeepLink 是一款基于区块链技术的远程控制软件，SDK 版本提供命名管道以供连接通信，管道名为 "deeplink_sdk"，自 v1.0.3.1 版本开始，通信协议改成如下规则：

通信数据流分为 header 和 body 两部分，header 占 4 个字节，其值为 body 的字节长度，body 为 utf8 编码的 JSON 格式。数据流以小端模式存储。

例如发送/接收 JSON 数据 "{"data":{"device_id":"112233","online_state":4,"password":"123456"},"method":"setDeviceInfo","version":1}"，长度 105 个字节，实际数据流如下：

| header | body |
| - | - |
| 0x69 0x00 0x00 0x00 | {"data":{"device_id":"112233","online_state":4,"password":"123456"},"method":"setDeviceInfo","version":1} |

1. header 中的 0x69 0x00 0x00 0x00 等于十进制的 105，一个无符号的整数，且 int 类型正好占 4 个字节；
2. body 中的字符解析后形如上表，body 中可以有 \n，但不要有 \0，否则可能导致 JSON 解析错误。

## 2. 接口列表

### 2.1. 验证权限

发送数据，deeplink 会验证 ID 和 Token，其中 ID 表示渠道号，最好包含国家和代理商等标识，例如 "SiChuanWangYu001"("SiChuan" 是城市名，"WangYu" 为代理商或者网吧的名称，"001"为编号或者分店等)，而 Token 字段在 1.0.2.1 版本之后废弃。

>有客户反馈，在 `%appdata%/deeplink/config.ini` 中明文保存钱包的 Token 可能会有信息泄露、盗用的安全隐患，因此自 1.0.2.8 版本开始，恢复此接口的 Token 字段的使用，使用此接口的 Token 字段将钱包的 Token 传递给 DeepLink 服务，DeepLink 服务只会将 Token 保存在内存中，不会保存在任何文件中，提高了不使用 deeplink_ui.exe 的用户钱包的安全性。

```json
{
    "method": "auth",
    "data": {
        "id": "deeplink_test",
        "token": "xxxxxxxxx"
    }
}
```

回复数据：

```json
{
    "method": "auth",
    "code": 0,
    "message": "ok"
}
```

### 2.2. 发起远程连接

发送数据

```json
{
    "method": "openConnect",
    "data": {
        // 远程设备的识别码
        "device": "123456789",
        // 远程设备的验证码
        "password": "123456",
        // 是否以全屏方式打开，默认为 false (deeplink v0.0.0.5 增加)
        "fullscreen": true,
        // 是否开启麦克风的传输，默认为 false (deeplink v1.0.1.6 增加)，需要被控端安装 VBCable Driver。
        "microphone": true,
    }
}
```

>VBCable Driver: <https://vb-audio.com/Cable/>

DeepLink 服务在收到此请求后，会启动一个子进程，使用从服务器获取的 NFT 等信息在子进程中建立远程连接，传输音视频流。

远程视频进程会创建一个新的管道用来传输状态信息，管道名称见如下回复。

```json
{
    "method": "openConnect",
    "code": 0,
    "message": "ok",
    "data": {
        "remote_ipc": "deeplink_rvp_9176",
        "remote_pid": 9176
    }
}
```

这个管道会在远程视频进程结束时关闭。

### 2.3. 获取远程传输状态信息

远程视频进程启动后，会进行以下操作:

1. 初始化环境;
2. 创建 client 实例;
3. 连接远程设备;
4. 协商麦克风的采用频率和格式等设置;

远程连接的结果将通过 "actionInfo" 方法通知给连接管道的用户。

```json
{
    "method": "actionInfo",
    "data": {
        // 参考 deeplink_sdk.h 文件中的 enum QDESKAction 声明。
        "action": 2,
        // 参考 deeplink_sdk.h 文件中的 enum QDESKErrCode 错误码声明。
        "result": 0,
        "message": "connect result 0"
    }
}
```

```json
{
    "method": "actionInfo",
    "data": {
        // 参考 deeplink_sdk.h 文件中的 enum QDESKAction 声明。
        "action": 3,
        // 参考 deeplink_sdk.h 文件中的 enum MicrophoneError 错误码声明。
        "result": 0,
        "message": "ok"
    }
}
```

信令服务器和 P2P 等连接状态将通过 "eventInfo" 方法通知给连接管道的用户。

```json
{
    "method": "eventInfo",
    "data": {
        // 参考 deeplink_sdk.h 文件中的 enum QDESK_Event 声明。
        "event": 2,
        "event_string": "P2P connect success"
    }
}
```

远程设备连接成功后，进程会主动向连接管道的用户发送视频传输状态信息，每秒发送一次。

```json
{
    "method": "videoInfo",
    "data": {
        "timestamp": 1683603451,
        // Is it on the local LAN?
        "is_lan": 1,
        // Is it relay?
        "is_relay": 0,
        "rtt": 1,
        "lost_rate": 0,
        "est_bw": 102400,
        "capture_fps": 120,
        "encode_fps": 120,
        "receive_fps": 119,
        "present_fps": 60,
        // Unit bps
        "encode_bitrate": 64476160,
        // Encoder
        // 1 - AVC
        // 2 - HEVC
        // 0x20 - AV1
        // others - VP8
        "codec": 2,
        // resolution 1920 * 1080
        "width": 1920,
        "height": 1080,
        "fir_count": 0,
        "pli_count": 4,
        "nack_count": 0,
        "packets_lost": 0,
        "packets_sent": 7179
    }
}
```

### 2.4. 结束远程连接

发送数据
```json
{
    "method": "closeConnect",
    "data": {
        // 远程设备的识别码
        "device": "123456789",
        // 远程子进程的 pid
        "pid": 9176
    }
}
```

"device" 和 "pid" 至少要传递一个，如果两个都有，服务在遍历子进程列表时，满足其中任意一个条件的远程连接就会被关闭。

回复数据
```json
{
    "method": "closeConnect",
    "code": 0,
    "message": "ok"
}
```

使用此接口会强制结束进程，推荐使用远程窗口的关闭按钮等 UI 界面操作来优雅的结束远程。

### 2.5. 远程结束时会收到通知

收到数据
```json
{
    "method": "connectClosed",
    "data": {
        // 远程设备的识别码
        "device": "123456789",
        // 远程子进程的 pid
        "pid": 2936
    }
}
```

注意: 此消息不需要回复。

### 2.6. 设置远程连接的一些参数

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

有关 option 、返回值和错误码请参考 [deeplink_sdk.h](../include/deeplink_sdk.h)

注意: 此消息要在远程视频进程中创建的新管道中通信，参考 2.3.获取远程传输状态信息。

### 2.7. 被远程控制开始时会收到通知

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

### 2.8. 可以断开被远程控制的状态

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

### 2.9. 被远程控制结束时会收到通知

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

### 2.10. 启用/禁用被远程控制开始时的提示窗口

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

### 2.11. 远程视频进程与被控端互相通信

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

## 3. 设备信息接口

除了上述的接口外，还有些接口用来获取设备信息。

### 3.1. 获取设备信息

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

### 3.2. 获取应用的版本号

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

### 3.3. 设备信息变动通知

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

### 3.4. 设置设备的本机验证码

1.0.1.5 版本中增加。

发送数据
```json
{
    "method": "setDevicePassword",
    "data": {
        // 仅支持6-16位字母和数字组合
        "password": "123456"
    }
}
```

回复数据
```json
{
    "method": "setDevicePassword",
    "code": 0,
    "message": "ok"
}
```

### 3.5. 服务器连接状态变化通知

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

如果想要手动查询，可以发送如下的请求，可以收到包含状态码的回复数据。

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

## 4. 错误代码

在请求返回的回复数据中，错误码"code"定义如下，可指代一种类别的错误，具体的错误请结合回复中的"message"字段进行分析。

| code | 定义             | 说明             |
| ---- | ---------------- | ---------------- |
| 0    | E_OK             | 成功 |
| 1    | E_FAIL           | 一般性失败 |
| 2    | E_INNER          | 内部错误，不对外公开 |
| 3    | E_POINTER        | 指针错误 |
| 4    | E_INVALARG       | 参数错误 |
| 5    | E_NOTIMPL        | 功能未实现 |
| 6    | E_OUTOFMEM       | 内存不足 |
| 7    | E_BUFERROR       | 缓冲区错误 |
| 8    | E_PERM           | 权限错误 |
| 9    | E_TIMEOUT        | 超时 |
| 10   | E_NOTINIT        | 未初始化 |
| 11   | E_INITFAIL       | 初始化失败 |
| 12   | E_ALREADY        | 已初始化，已经在运行 |
| 13   | E_INPROGRESS     | 已在运行、进行状态 |
| 14   | E_EXIST          | 文件对象等资源已存在 |
| 15   | E_NOTEXIST       | 文件对象等资源不存在 |
| 16   | E_BUSY           | 设备或资源忙 |
| 17   | E_FULL           | 设备或资源忙 |
| 18   | E_EMPTY          | 资源为空 |
| 19   | E_OPENFAIL       | 文件等资源对象打开失败 |
| 20   | E_READFAIL       | 文件等资源对象读取/接收失败 |
| 21   | E_WRITEFAIL      | 文件等资源对象写入/发送失败 |
| 22   | E_DELFAIL        | 文件等资源对象删除/关闭失败 |
| 23   | E_CODECFAIL      | 加密/解密、编码/解码失败 |
| 24   | E_CRC_FAIL       | CRC校验错误 |
| 25   | E_TOOMANY        | 消息、缓冲区、内容太多 |
| 26   | E_TOOSMALL       | 消息、缓冲区、内容太多 |
| 27   | E_NETNOTREACH    | 无路由，网关错误，网络不可达 |
| 28   | E_NETDOWN        | 断网，网络不可用 |
| 29   | E_JSONPARSEFAIL  | Json 解析错误 |
| ...  | ...              | ... |
