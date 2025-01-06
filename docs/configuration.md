# DeepLink configuration file related instructions

In addition to the interfaces provided by the SDK, DeepLink also has some configuration files.

## Application configuration

The configuration file is located at `%appdata%/deeplink/config.ini`. This section lists only some of the commonly modifiable configurations. For caution, please do not modify the settings that are not listed. Restart the service program to apply the changes after modification.

```ini
[host]
# The token used for logging into the wallet.
token=xxx.xxx.xxx

[app]
# Log level.
log_level=2
# Interval for checking updates, in seconds.
update_check_interval=3600
# Whether to pop up a notification in the top right corner of the desktop when being remotely controlled.
remote_control_prompt=1

[Setting]
# Whether to start the UI program on system boot.
autostart=1
# Whether to remain active (can be remotely controlled even after exiting the software).
backservice=1
# Automatic updates.
autoupdate=1
# Enter the tray after exiting the software.
system_tray_when_closing=1

[safe]
# Verification code update settings.
verify_update_type=0

[webview]
# Disable GPU acceleration in WebView.
disable_gpu=0
```

## Configuration of parameters related to remote video.

The configuration file is located at `%appdata%/deeplink/store.dat`. This section lists only some of the commonly used configurations.

```ini
[display]
# Resolution of the controlled device.
# 0 Keep consistent with the controlled device.
# 1 Keep consistent with the controlling device.
# 2 Select from the current resolution options.
resolution=0
width=1920
height=1080
# Refresh rate of the controlled device.
fps=0
# Type of virtual display creation.
# 0 Copy the EDID information of the controlling device, or closely match it as much as possible.
# 1 3840x2160x60
# 2 2560x1440x60
# 3 1920x1080x60
vdisplay=0

[safe]
# Automatically lock the screen when remote access ends.
lockonclose=1
# Automatically lock the screen when initiating a remote connection.
lockonconnect=0
# Lock the screen when the privacy screen is turned off.
lockonprivacy=0

[net]
# Upper limit for wide area network (WAN) bandwidth prediction, in megabits (M).
max_bandwidth_wan=10
# Set a specific port and use it with port mapping
minport=20000
maxport=21000

[setting]
# 0 Chinese
# 1 English
# 2 Korean
# 3 Japanese
language=0
# Display FPS delay and other information in the lower right corner
showfps=1
# 1 use winapi sendinput
# 2 use hid driver
input_type=1
# 1:1
# Adaptive
default_display_scale=0

[hotkey]
# Set hotkeys, currently set to Win + a specific character.
windowed_hk=z
minimize_hk=x
switchscreen_hk=c

[bitrate]
# When connected to 677276640, if it is a wide area network, the speed limit is 8m,
# and the speed limit is 40m for a local area network. If it is 0, it is dynamically
# determined by the program detecting the network speed
677276640=8|40

[lab]
# Do not display the small arrow when in full screen mode.
hide_button_on_fullwnd=1
# During remote connection, mute the controlling device and do not transmit audio data.
mute=1
# During remote connection, mute the controlled device. The controlled device's API is set to mute. Some machines cannot obtain audio data after being muted, so this function has some issues.
host_silent=1
# During remote connection, the sound is not compressed. There is a bug, so this feature cannot be used temporarily.
pcm=1
# Disable pacer. In stable network conditions, this can reduce latency.
Diablepacer=1
# Office mode: prioritize clarity by reducing the frame rate to ensure image sharpness.
office_mode=1
# h264 1 , h265 2, av1 3
codec=1
# Enable bandwidth probe.
probe=1
# Enable NVIDIA virtual display, default to using IDD.
usegvd=1
# Enable NV12 render. For weaker configurations, like N100, this option needs to be enabled to support 240.
mpo=1
# Use Direct3D9 (D3D9).
d3d9=1
```

## Program configuration.

To display detailed debugging parameters such as remote video FPS and latency, or record more detailed remote logs, modify the following contents in `C:\Program Files (x86)\DeepLink\quicdesk.cfg`:

```ini
[log]
level=191
# The log path is located in the user folder %APPDATA% and is determined by the actual username.
path=C:\Users\<username>\AppData\Roaming

[client]
debug_wnd=1
```

If the image becomes blurry when there are significant changes in the picture over a local area network, you can modify the following contents in `C:\Program Files (x86)\DeepLink\nvenc.ini`:

```ini
[encode]
param = -preset ll_hq -rc vbr_hq
vbv = 5
```

Some games will have problems using absolute coordinates. Forced to switch to relative coordinates, the cursor will be in the video

%binpath% local.black
```ini
[black]
list=valorant.exe
```

Note: The above-mentioned `C:\Program Files (x86)\DeepLink` refers to the installation directory of DeepLink, which may vary depending on individual circumstances.
