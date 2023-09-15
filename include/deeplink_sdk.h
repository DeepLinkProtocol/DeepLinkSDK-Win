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
};

enum QDESKOption {
    // 0x101 避免和render::Option冲突
    kOptionScreenOriginalSize = 0x101, // 开启或关闭"原始屏幕比例"，默认关闭
    kOptionFullScreen, // 开启或关闭"全屏"，默认关闭
    kOptionBlackScreen, // 开启或关闭"黑屏"，默认关闭
    kOptionVideoLossless, // 开启或关闭"无损画质"，默认关闭
    kOptionVideoBitrate, // 设置固定"视频码率", 单位Mbit/s， 0为auto;
};

// error code
enum QDESKErrCode
{
    //成功
    QDErrCode_Success = 0,
    //未初始化
    QDErrCode_UnInitialized =1000 , 
    //已经初始化
    QDErrCode_Initialized , 
    //参数错误
    QDErrCode_ParameterInvalid, 
    //会话不存在；
    QDErrCode_Session_NoExist, 
    //已经在串流中
    QDErrCode_Session_Exist, 
    //内部错误
    QDErrCode_Inner, 
    QDErrCode_License_Invalid = 1007,
    // 没显示器
    QDErrCode_No_Display = 1100,   
    // 设备不在线
    QDErrCode_DeviceNotExist , 
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
