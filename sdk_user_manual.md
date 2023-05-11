# DeepLink SDK USER MANUAL

DeepLink 接口文档

## 1.概述Overview

DeepLink是一款基于区块链技术的远程控制软件。

## 2.支持语言Language support

目前提供 C++ 接口，已进程间通信方式提供，使用 JSON 格式交换数据。

Currently, a C++ interface is provided, and inter-process communication is provided, using JSON format to exchange data.

## 3.工作模型

## 4.使用规范 Usage

### 1.连接命名管道Connect named pipe

命名管道暂定名称 "deeplink_sdk"

Named pipe tentative name "deeplink_sdk"

### 2.验证权限Authority verification

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
        "password": "123456"
    }
}
```

远程视频进程会创建一个新的管道用来传输状态信息，管道名称见如下回复

The remote video process will create a new pipe to transmit status information, see the reply below for the name of the pipe

```json
{
    "code": 0,
    "message": "ok",
    "data": {
        "remote_ipc": "connect001"
    }
}
```

### 4.获取远程传输状态信息Get remote transmission status

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
    "code": 0,
    "message": "ok"
}
```
