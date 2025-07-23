# DeepLink SDK USER MANUAL

## 1. Overview

DeepLink is a remote control software based on blockchain technology. The SDK version provides a named pipe for connection and communication. The pipe name is "deeplink_sdk". Since version v1.0.3.1, the communication protocol has been changed to the following rules:

The communication data stream is divided into two parts: header and body. The header occupies 4 bytes, and its value is the byte length of the body. The body is in utf8-encoded JSON format. The data stream is stored in little-endian mode.

For example, sending/receiving JSON data "{"data":{"device_id":"112233","online_state":4,"password":"123456"},"method":"setDeviceInfo","version":1}", the length is 105 bytes, and the actual data flow is as follows:

| header | body |
| - | - |
| 0x69 0x00 0x00 0x00 | {"data":{"device_id":"112233","online_state":4,"password":"123456"},"method":"setDeviceInfo","version":1} |

1. 0x69 0x00 0x00 0x00 in the header is equal to 105 in decimal, an unsigned integer, and the int type occupies exactly 4 bytes;
2. the characters in the body are parsed as shown in the table above. The body can have \n, but not \0, otherwise it may cause JSON parsing errors.

## 2. Interface list

### 2.1. Authority verification

When sending data, deeplink will verify the ID and Token, where the ID represents the channel number, preferably including the country and agent identifiers, such as "SiChuanWangYu001" ("SiChuan" is the city name, "WangYu" is the name of the agent or Internet cafe, and "001" is the number or branch, etc.), and the Token field is deprecated after version 1.0.2.1.

>Some users have reported that saving the wallet token in plain text in `%appdata%/deeplink/config.ini` may have security risks of information leakage and theft. Therefore, starting from version 1.0.2.8, the use of the token field of this interface is restored. The token field of this interface is used to pass the wallet token to the DeepLink service. The DeepLink service will only save the token in memory, not in any file, which improves the security of the wallet of users who do not use deeplink_ui.exe.

Send data:

```json
{
    "method": "auth",
    "data": {
        "id": "deeplink_test",
        "token": "xxxxxxxxx"
    }
}
```

Reply data：

```json
{
    "method": "auth",
    "code": 0,
    "message": "ok"
}
```

### 2.2. Open remote connection

Send data:

```json
{
    "method": "openConnect",
    "data": {
        // device id of the remote device
        "device": "123456789",
        // device password of the remote device, which can be empty when using the wallet signature remotely
        "password": "123456",
        // Whether to open in full screen mode, the default is false (deeplink v0.0.0.5 added)
        "fullscreen": true,
        // Whether to enable microphone transmission, the default is false (deeplink v1.0.1.6 added). The controlled terminal needs to install VBCable Driver.
        "microphone": true,
        // Wallet address, random number and signature, required when using the wallet signature remotely
        "wallet": "",
        "nonce": "",
        "signature": "",
        // Wallet type, the old DBC wallet uses "subscan"
        "wallet_type": "evm",
        // Wallet role, 0 is the default value, meaningless, 1 is your own wallet, 2 is the renter
        "wallet_role": 0,
        // Short-term rental start time, millisecond timestamp, no rental, pass 0 or no
        "rent_start_time": 0,
        // The rental duration of the short-term rental, in seconds, if not rented, pass 0 or not
        "rent_time": 0,
        // After the short-term rental, it is transmitted to the controlled end to create a virtual display and display the DEE screen
        "width": 1920,
        "height": 1080,
        "fps": 60,
    }
}
```

>VBCable Driver: <https://vb-audio.com/Cable/>

After receiving this request, the DeepLink service will start a child process, use the NFT and other information obtained from the server to establish a remote connection in the child process, and transmit audio and video streams.

The remote video process will create a new pipe to transmit status information, see the reply below for the name of the pipe.

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

This pipe will be closed when the remote video process ends.

The remote window opened using the SDK cannot be automatically pinned to the foreground window of the desktop and may be blocked by other focus windows.
This is because the remote window process is created by the DeepLink service process, but the service process is a background process without a window interface and does not have the permission to set the foreground window.
The solution is to find a desktop application with an interface, and let this program find the handle of the remote window (window class name "QDesk <remote process id>") and call SetForegroundWindow to set the remote window as the foreground window.

```cpp
QString class_name = QString("QDesk %1").arg(remote_pid);
HWND hWnd = FindWindow(class_name, NULL);
if (hWnd != NULL) {
    SetForegroundWindow(hWnd);
}
```

https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setforegroundwindow?redirectedfrom=MSDN

It is recommended to call the above code in the client desktop application that accesses the DeepLink SDK and initiates the remote connection.

### 2.3. Get remote transmission status

After the remote video process starts, the following operations will be performed:

1. Initialize the environment;
2. Create a client instance;
3. Connect remote device;
4. Negotiate settings such as microphone adoption frequency and format;

The result of the remote connection will be notified to the user of the connected pipe through the "actionInfo" method.

```json
{
    "method": "actionInfo",
    "data": {
        // Refer to the enum QDESKAction declaration in the deeplink_sdk.h file
        "action": 2,
        // Refer to the enum QDESKErrCode error code declaration in the deeplink_sdk.h file.
        "result": 0,
        "message": "connect result 0"
    }
}
```

```json
{
    "method": "actionInfo",
    "data": {
        // Refer to the enum QDESKAction declaration in the deeplink_sdk.h file
        "action": 3,
        // Refer to the enum MicrophoneError error code declaration in the deeplink_sdk.h file.
        "result": 0,
        "message": "ok"
    }
}
```

Connection status such as signaling server and P2P will be notified to the user of the connection pipe through the "eventInfo" method.

```json
{
    "method": "eventInfo",
    "data": {
        // Refer to the enum QDESK_Event declaration in the deeplink_sdk.h file.
        "event": 2,
        "event_string": "P2P connect success"
    }
}
```

After the remote device is successfully connected, the process will actively send video transmission status information to the user who is connected to the pipeline, and sends it once per second.

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

### 2.4. Close remote connection

Send data:

```json
{
    "method": "closeConnect",
    "data": {
        // device id of the remote device
        "device": "123456789",
        // the pid of rmeote process
        "pid": 9176
    }
}
```

At least one of "device" and "pid" must be passed. If both are present, when the service traverses the child process list, the remote connection that meets any of the conditions will be closed.

Reply data:

```json
{
    "method": "closeConnect",
    "code": 0,
    "message": "ok"
}
```

Using this interface will forcibly end the process. It is recommended to use UI interface operations such as the close button of the remote window to end the remote gracefully.

### 2.5. Get notified when remote connection ends

Received data:

```json
{
    "method": "connectClosed",
    "data": {
        // device id of the remote device
        "device": "123456789",
        // the pid of rmeote process
        "pid": 2936
    }
}
```

Note: This message does not require a reply.

### 2.6. Set some parameters of the remote connection

Send data:

```json
{
    "method": "setConnectOption",
    "data": {
        "option": kOptionVideoBitrate,
        "value": "0"
    }
}
```

Reply data:

```json
{
    "method": "setConnectOption",
    "code": 0,
    "message": "ok"
}
```

For options, return values and error codes, please refer to [deeplink_sdk.h](../include/deeplink_sdk.h)

Note: This message should be communicated in the new pipeline created in the remote video process, refer to 2.3.Get remote transmission status.

### 2.7. Get notified when remote controlled is started

Received data:

```json
{
    "method": "remoteControlledStarted",
    "data": {
        // The identification id of the device initiating remote control
        "device": "123456789"
    }
}
```

Note: This message does not require a reply.

### 2.8. Close the remote controlled state

Send data:

```json
{
    "method": "closeRemoteControlled",
    "data": {
        // The identification id of the device initiating remote control
        "device": "123456789"
    }
}
```

Reply data:

```json
{
    "method": "closeRemoteControlled",
    "code": 0,
    "message": "ok"
}
```

### 2.9. Get notified when remote controlled is stopped

Received data:

```json
{
    "method": "remoteControlledStopped",
    "data": {
        // The identification id of the device initiating remote control
        "device": "123456789"
    }
}
```

Note: This message does not require a reply.

### 2.10. Enable/disable prompt window when remote controled is started

Send data:

```json
{
    "method": "setRemoteControlledPrompt",
    "data": {
        "value": "off"
    }
}
```

Reply data:

```json
{
    "method": "setRemoteControlledPrompt",
    "code": 0,
    "message": "ok"
}
```

Send data:

```json
{
    "method": "getRemoteControlledPrompt",
    "data": {}
}
```

Reply data:

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

### 2.11. The remote video process communicates with the controlled end

1. Either party can send data.

Send data:

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

Reply data:

```json
{
    "method": "sendData",
    "code": 0,
    "message": "ok"
}
```

2. Receive data notification.

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

Note: This message does not require a reply.

### 2.12 Set a custom port range

deeplink v1.0.3.4 added

Send data:
```json
{
    "method": "setPortRange",
    "data": {
        "start_port": 20000,
        "end_port": 20010
    }
}
```

Reply data:
```json
{
    "method": "setPortRange",
    "code": 0,
    "message": "ok"
}
```

## 3. Device info interface

In addition to the above-mentioned interfaces, there are also some interfaces used to obtain device information.

### 3.1. Get device info

Send data:

```json
{
    "method": "getDeviceInfo",
    "data": {}
}
```

Reply data:

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

### 3.2. Get application version

Send data:

```json
{
    "method": "getAppVersion",
    "data": {}
}
```

Reply data:

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

### 3.3. Device Information Change Notification

Received data:

```json
{
    "method": "deviceInfoChanged",
    "data": {
        "device_id": "123456789",
        "password": "123456"
    }
}
```

Note: This message does not require a reply.

### 3.4. Set the native verification code of the device

Added in version 1.0.1.5.

Send data:

```json
{
    "method": "setDevicePassword",
    "data": {
        // Only a combination of 6-16 letters and numbers is supported
        "password": "123456"
    }
}
```

Reply data:

```json
{
    "method": "setDevicePassword",
    "code": 0,
    "message": "ok"
}
```

### 5. Server connection status change notification

Received data:

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

Note: This message does not require a reply.

If you want to query manually, you can send the following request, and you can receive the reply data including the status code.

Send data:

```json
{
    "method": "getServerConnState",
    "data": {}
}
```

Reply data:

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

## 4. error code

In the reply data returned by the request, the error code "code" is defined as follows, which can refer to a type of error. Please analyze the specific error in conjunction with the "message" field in the reply.

| code | definition | illustrate |
| ---- | ---------------- | ---------------- |
| 0    | E_OK             | success |
| 1    | E_FAIL           | general error |
| 2    | E_INNER          | internal error, not public |
| 3    | E_POINTER        | pointer error |
| 4    | E_INVALARG       | parameter error |
| 5    | E_NOTIMPL        | function not implemented |
| 6    | E_OUTOFMEM       | not enough storage |
| 7    | E_BUFERROR       | buffer error |
| 8    | E_PERM           | permission error |
| 9    | E_TIMEOUT        | timeout |
| 10   | E_NOTINIT        | uninitialized |
| 11   | E_INITFAIL       | initialization failed |
| 12   | E_ALREADY        | initialized, already running |
| 13   | E_INPROGRESS     | already running, in progress |
| 14   | E_EXIST          | A resource such as a file object already exists |
| 15   | E_NOTEXIST       | Resource such as file object does not exist |
| 16   | E_BUSY           | device or resource busy |
| 17   | E_FULL           | device or resource is full |
| 18   | E_EMPTY          | resource is empty |
| 19   | E_OPENFAIL       | failed to open resource objects such as files |
| 20   | E_READFAIL       | failed to read/receive resource objects such as files |
| 21   | E_WRITEFAIL      | failed to write/send resource objects such as files |
| 22   | E_DELFAIL        | failed to delete/close resource objects such as files |
| 23   | E_CODECFAIL      | Encryption/decryption, encoding/decoding failed |
| 24   | E_CRC_FAIL       | CRC error |
| 25   | E_TOOMANY        | too many messages, buffers, content |
| 26   | E_TOOSMALL       | too few messages, buffers, content |
| 27   | E_NETNOTREACH    | no route, gateway error, network unreachable |
| 28   | E_NETDOWN        | the network is disconnected, the network is unavailable |
| 29   | E_JSONPARSEFAIL  | Json parsing error |
| ...  | ...              | ... |
