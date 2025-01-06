# DeepLink 配置文件相关说明

DeepLink 除了 SDK 提供的接口外，还有一些配置文件。

## 应用配置

配置文件位于 `%appdata%/deeplink/config.ini`，此处仅列举一些可修改的常用配置，谨慎起见，没列出的请勿修改。修改后重启服务程序生效。

```ini
[host]
# 登录钱包时所用的 token
token=xxx.xxx.xxx

[app]
# 日志等级
log_level=2
# 检查更新的时间间隔，单位秒
update_check_interval=3600
# 被远程控制时是否在桌面右上角弹出提示
remote_control_prompt=1

[Setting]
# 是否开机启动 UI 程序
autostart=1
# 是否常驻(退出软件后仍可被远程)
backservice=1
# 自动更新
autoupdate=1
# 退出软件后进入托盘
system_tray_when_closing=1

[safe]
# 验证码更新设置
verify_update_type=0

[webview]
# 在 WebView 中禁用 GPU 加速
disable_gpu=0
```

## 远程视频相关参数配置

配置文件位于 `%appdata%/deeplink/store.dat`，此处仅列举一些常用的配置。

```ini
[display]
# 被控端分辨率
# 0 同被控端保持一致
# 1 同主控端保持一致
# 2 从当前分辨率中选取
resolution=0
width=1920
height=1080
# 被控端刷新率
fps=0
# 虚拟显示器创建类型
# 0 复制主控端edid信息, 或者尽量跟主控端接近;
# 1 3840x2160x60
# 2 2560x1440x60
# 3 1920x1080x60
vdisplay=0

[safe]
# 远程结束时自动锁屏
lockonclose=1
# 远程连接时自动锁屏
lockonconnect=0
# 隐私屏被关闭时锁屏
lockonprivacy=0

[net]
# 广域网带宽预测上限，单位 M
max_bandwidth_wan=10
# 设置特定端口，配合端口映射使用
minport=20000
maxport=21000

[setting]
# 0 Chinese
# 1 English
# 2 Korean
# 3 Japanese
language=0
# 右下角显示 FPS delay 等信息
showfps=1
# 1 use winapi sendinput
# 2 use hid driver
input_type=1
# 1:1
# 自适应
default_display_scale=0

[hotkey]
# 设置热键， 现在为win+某个字符
windowed_hk=z
minimize_hk=x
switchscreen_hk=c

[bitrate]
# 连接到 677276640 时，如果是广域网，限速 8m，局域网限速 40m，如果为 0 则由程序探测网速来动态决定
677276640=8|40

[lab]
# 全屏时不显示小箭头
hide_button_on_fullwnd=1
# 远程连接时，控制端静音 , 不传输音频数据
mute=1
# 远程连接时，被控端静音， 被控端api设置静音， 部分机器静音后无法获取音频数据， 所以这个功能还有问题
host_silent=1
# 远程连接时，声音不压缩， 有bug暂不能使用
pcm=1
# 禁用pacer, 在网络稳定的情况下，可以降低延时
Diablepacer=1
# 办公模式， 优先清晰度， 通过降低帧率保证清晰度
office_mode=1
# h264 1 , h265 2, av1 3
codec=1
# 开启bandwidth probe
probe=1
# 开启nvidia虚拟显示器， 默认走idd
usegvd=1
# 开启nv12 render, 比较弱的配置比如n100， 要支持240需要打开这个选项
mpo=1
# 使用d3d9
d3d9=1
```

## 程序配置

如需展示远程视频 fps、延迟等详细调试参数，或者记录更详细的远程日志，可以修改 `C:\Program Files (x86)\DeepLink\quicdesk.cfg` 的如下内容:

```ini
[log]
level=191
# log path 位于用户文件夹 %APPDATA% ，由实际用户名确定。
path=C:\Users\<username>\AppData\Roaming

[client]
debug_wnd=1
```

如果局域网下画面变动大的时候会变模糊，可以修改 `C:\Program Files (x86)\DeepLink\nvenc.ini` 的如下内容:

```ini
[encode]
param = -preset ll_hq -rc vbr_hq
vbv = 5
```

有些游戏使用绝对坐标会有问题，强制切换为相对坐标，光标带在视频中
%binpath% local.black
```ini
[black]
list=valorant.exe
```

注意: 以上的 `C:\Program Files (x86)\DeepLink` 指 DeepLink 的安装目录，视个人情况而定。
