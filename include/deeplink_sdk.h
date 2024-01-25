#ifndef DEEPLINK_SDK_H
#define DEEPLINK_SDK_H

enum QDESK_Event {
    QDEvent_OnConnected =0, // qdesk server connect
    QDEvent_OnSignalConnected, // signal connect
    QDEvent_OnP2PConnected, //  p2p connect
    QDEvent_OnAuthSuccessed, //  auth sucess
    QDEvent_OnVideoStreamCreated, // video created
    QDEvent_OnVideoRendered, //  first idr rendered
    QDEvent_OnVideoStreamStop, // stream stop
    QDEvent_OnP2PDisConnected, // p2p disconnect
    QDEvent_OnDisConnected, // qdesk server disconnect
    QDEvent_OnReConnecting, //qdesk server reconnect
    QDEvent_OnAuthFailed, // auth failed
    QDEvent_OnP2PReConnecting, // p2p disconnect
    QDEvent_OnP2PTimeout, // p2p disconnect
    QDEvent_OnP2PFailedTooMany, // p2p failed too many times;
};

enum QDESKOption {
    // 0x101 避免和render::Option冲突
    kOptionScreenOriginalSize = 0x101, // 开启或关闭"原始屏幕比例"，默认关闭
    kOptionFullScreen, // 开启或关闭"全屏"，默认关闭
    kOptionBlackScreen, // 开启或关闭"黑屏"，默认关闭
    kOptionVideoLossless, // 开启或关闭"无损画质"，默认关闭
    kOptionVideoBitrate, // 设置固定"视频码率", 单位Mbit/s， 0为auto;
    kOptionSound,
    kOptionInput, // 开启或关闭"输入"，默认开启，暂不实现
    kOptionRemoteDeviceMute, // 开启或关闭"被控设备静音"，默认关闭。

    kOptionCursor, // 设置"光标"，默认本地光标
    kOptionControlPanelWnd, // 显示或隐藏"控制面板窗口"，默认显示
    kOptionFpsWnd, // 显示或隐藏"FPS窗口"，默认显示
    kOptionLanguage, // 设置"语言"，"1" 中文, "2" 英文, 默认中文
};

// error code
enum QDESKErrCode
{
    QDErrCode_Success = 0,
    QDErrCode_UnInitialized =1000, // 未初始化
    QDErrCode_Initialized, // 已经初始化
    QDErrCode_ParameterInvalid, // 参数错误
    QDErrCode_Session_NotExist, // 未开始串流
    QDErrCode_Session_Exist, // 已经在串流中
    QDErrCode_DeviceNotExist, // 设备不在线
    QDErrCode_WEBConnectTimeout, // 业务服务器连接失败;
    QDErrCode_SignalTimeout, // 连接信令超时;
    QDErrCode_P2PTimeout, // 打洞超时;
    QDErrCode_RelayFailed, // 中转失败;
    QDErrCode_AuthFailed, // 认证失败
    QDErrCode_AuthTimeout, // 认证超时
    QDErrCode_No_Display, // 没显示器
    QDErrCode_MaxConnection, // 设备达到最大串流数量，目前限制为4；
    QDErrCode_InvaildType, // 连接方式错误
    QDErrCode_Inner, // 内部错误
    // for offline;
    QDErrCode_InvaildCompany = 0x200, // 企业信息错误;
    QDErrCode_License_Invalid, // license错误;
    QDErrCode_AuthLicenseDeviceOverLimit,
};

enum QDESKAction {
    // Initialize the environment
    kActionInitialize = 0,
    // Create a client instance
    kActionCreateClient,
    // Connect remote device
    kActionConnect,
    // Microphone
    kActionMicrophone,
};

enum MicrophoneError {
    Normal = 0,
    CreateContextError,
    ConnectBackendError,
    SelectDeviceError,
    GetDeviceError,
    CreateStreamError,
    OpenStreamError,
    StartStreamError,
    RingBufferError,
    OpusError,
    UnknownError,
};

#endif // DEEPLINK_SDK_H
