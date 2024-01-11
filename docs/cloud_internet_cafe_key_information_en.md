# Key information about the cloud cybercafe

## Software and hardware environment

### hardware

#### Host: 

**Currently, all cloud cybercafes in China use NVIDIA + Intel solutions for graphics cards. AMD has plans to enter the cloud cybercafe market next year as part of their new strategy.**

AMD's stability and encoding/decoding performance are inferior to NVIDIA's.

#### Client: 

The current mainstream processors used are Intel N5105, Intel N100, and AMD 200GE.

- The Intel N5105 supports dual channels, has a TDP of 10W, and offers stronger performance compared to the N100. It can operate stably at 2K 240fps. Currently, it is being used by Shunwang.
- The Intel N100 is single-channel, with a TDP of 6W, and is one of Intel's newer chips. Currently, only our streaming solution can achieve 2K at 240fps with it. Other solutions have issues even at 2K 165fps. Shunwang, Yunxin, and Yuntian Changxiang are using it.
- The AMD 200GE has strong CPU and GPU performance, but it is more expensive and the chips are less available. The newer streaming solutions no longer recommend it. Yileyou is still using it.

*For MiniPC BIOS, the power-saving feature must be turned off, otherwise, it can lead to instability. The final manifestation of this instability is often an unstable frame rate, which can be due to CPU and GPU throttling, network packet loss, or lag.*

Currently, these MiniPCs are being used with their frequencies locked.

### software settings

#### Host

**Power management should always be set to High Performance mode.**

![Power Options High Performance](../assets/power_options.png)

**For NVIDIA power mode, select 'High Performance'.**

![NVIDIA Power Managerment Mode](../assets/nvidia_power_mode.png)

#### Client

Set the power mode to 'High Performance'.

Set the Intel GPU to 'High Performance', *If possible, obtaining Intel's frequency-locking driver is better as it can lock the frequency to a higher rate than what is achievable through BIOS frequency locking.*

![Intel Graphics Power Plan](../assets/intel_graphic_power_plan.png)

*Both systems should be properly registered to avoid the impact of the Windows watermark. The lower the configuration, the greater the impact of the watermark.*

## Testing

After the software and hardware have been set up, it is necessary to test for performance and stability.

### Network

Streaming does not have high network requirements, and testing is relatively simple. It only requires a basic test using fping.

<https://github.com/dexit/fping-windows/releases/tag/fping-4-2-win-binary>

`fping -b 1400 -l -p 10 192.168.3.49`, setting too short an interval can lead to many false positives. If there are reports with a 10ms interval, it would require investigation.

### Streaming

What we ultimately need is for the client machine and the host to have as similar an experience as possible, so it is important to focus on testing latency, stability, and image quality.

#### Image Quality

Since it mainly depends on the bitrate and encoding parameters, it's not very critical. It's optional to test; a simple test of the color deviation in pixels would suffice.

#### Latency

Latency affects the user's operational experience. In the network environment of a cloud cybercafe, this latency is generally very low and difficult for users to perceive. A simple testing approach is to display a stopwatch in milliseconds on the host machine, then take a photo for comparison to determine the screen latency. The total latency, including the delay from keyboard and mouse inputs, is typically calculated as half of the round-trip time (RTT).
  
#### Stability

The most critical aspect is stability. Both the network and other software can potentially affect stability.

<https://www.geeks3d.com/furmark/>


Both the host and client sides use 2K 240fps monitors. On the host side, open FurMark, and on the client side, stream it. Record the following data every second: Rtt, capturefps, receivefps, presentfps. Additionally, you can include other data that you find interesting.

It is recommended to run multiple devices simultaneously to record data and perform analysis, extracting data similar to the following:

![fps data](../assets/fps_data.png)

| Machines | Frame loss 0-3 | Frame loss 4-6 | Frame loss 10-15 | Frame loss 20+ | Average frame loss | Capture fps | Receive fps |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| First group |  |  |  |  |  |  |  |
| Second group |  |  |  |  |  |  |  |
| Third group |  |  |  |  |  |  |  |

When there is significant frame loss, it is necessary to compare RTT (Round-Trip Time) information. If the RTT is normal, debugging software should be added for further analysis.

## Commence online operations.

After ensuring that all tests are successful, it's essential to proceed with on-site testing at the cybercafe. Collect data while the cybercafe is operational, especially focusing on connection failure rates, various FPS (Frames Per Second) and RTT (Round-Trip Time) metrics after successful connections. This will allow for the timely detection, analysis, and resolution of any issues that may arise during real-world usage.
  
### Platform

There needs to be a platform that allows real-time monitoring of the operational process. Essential information to include is the boot times of the host and client, hardware and software details, distinguishable IP information, and streaming process details (start time, end time). For the host, keepalive information is required so that the platform can receive notifications in case of unexpected shutdowns, blue screens, network disconnections, and other similar situations.

*During actual operation, various issues may arise, and it's important to continuously improve the platform to address and resolve these issues.*

### Virtual Display

If the client is a specific cybercafe, it's advisable to avoid using Deeplink's built-in virtual display as it sacrifices compatibility and can affect the host's game frame rate. It's recommended to use the alternative solution provided by us in such cases.

[Internet cafe virtual display deployment scheme](./internet_cafe_virtual_display_deployment_scheme.md)

*Creating a virtual display can take time, and the time required may vary depending on the GPU and driver versions. It's advisable to aim for the virtual display to be created during system startup whenever possible to minimize delays.*
