# CRSF2PWM

## 项目介绍
**将 ELRS 接收机的 CRSF 串口数据转换为模型舵机、电调使用的 PWM 控制信号。**

固件基于 AirMCU Arduino 核心，可运行在 PY32F002A 上，提供 8 路 PWM 输出、连接状态指示、失控保护和电池电压回传功能。

信号路径：遥控器 → ELRS 接收机 → CRSF 串口 → 转接板 → PWM → 舵机 / 电调。


## 项目特点
1. 八通道PWM输出
2. 使用国产单片机，成本低于5元
3. 支持电压回传


## 硬件简介

主控芯片采用PY32F002A国产芯片，集成多路I2C、SPI、USART 等通讯外设，1路12bit ADC,多个定时器，完美满足项目应用。

设计8路pwm输出，其中4路可以通过焊接排针连接，另外4路做冗余设计焊盘处理。

电路板31mm * 13mm 体积小，适合各种狭窄空间。


## 目录结构

```text
crsf2pwm/
├── firmware/crsf2pwm/          # 主固件、CRSF 协议解析、引脚配置
├── examples/arduino_led_test/  # PB2 LED 常亮测试（测试焊接正确性）
├── scripts/                   # 环境安装、编译、LED 固件下载
├── docs/                      # 构建、配置、验证与来源说明
└── hardware/                  # 硬件资料
    ├── schematic/             # 原理图
    ├── pcb/                   # PCB 与生产文件
    ├── bom/                   # 物料清单
    └── photos/                # 实物与测试照片
```


## 快速开始

### 1. 获取源码并准备环境

先安装 [Arduino CLI](https://arduino.github.io/arduino-cli/)。以下脚本适用于 macOS/Linux 的 Bash 环境：

```sh
git clone https://github.com/neiaHsing/crsf2pwm.git
cd crsf2pwm
bash scripts/setup-arduino.sh
```

安装脚本使用 Air-duino 官方索引，安装 AirMCU **0.6.4** 及其依赖。已有 ARM GCC/CMSIS 的用户可以参考 [工具链路径配置](docs/build-and-flash.md) 复用本机环境。

### 2. 编译

```sh
bash scripts/build.sh led        # LED 示例，20 KB Flash / 3 KB RAM
bash scripts/build.sh firmware   # 完整固件
```

生成的 `.hex`、`.bin`、`.elf` 等文件位于 `build/<配置>/`。

### 3. 通过 DAPLink 下载 LED 示例

安装 [pyOCD](https://pyocd.io/docs/installing.html)，并从 [Puya SDK](https://github.com/OpenPuya/PY32F0xx_Firmware) 获取 CMSIS 设备包 `Puya.PY32F0xx_DFP.1.2.6.pack`。

```sh
pyocd list
bash scripts/flash-led.sh /path/to/Puya.PY32F0xx_DFP.1.2.6.pack YOUR_PROBE_UID
```

将设备包路径和 `YOUR_PROBE_UID` 替换为实际值。此脚本会覆盖目标程序，下载对象为 PB2 LED 示例，目标设置为 `PY32F002Ax5`，SWD 频率为 100 kHz。示例启动后，PB2 输出低电平，LED 常亮。

这里使用 Arduino 编译、pyOCD 经 DAPLink/SWD 下载。更多说明见 [构建与烧录](docs/build-and-flash.md)。

## 引脚配置

PY32F002A 的引脚配置如下：

| 功能 | MCU 引脚 |
| --- | --- |
| CRSF RX（接收机 TX → MCU） | PF1 |
| CRSF TX（MCU → 接收机 RX） | PA0 |
| 状态 LED | PB2 |
| 电池电压 ADC | PA7 |
| PWM 1–4 | PA5、PA6、PB0、PB1 |
| PWM 5–8 | PB3、PF0、PA1、PB6 |

修改入口：

- [`target.h`](firmware/crsf2pwm/target.h)：引脚、LED 极性和分压比例。
- [`crsf2pwm.ino`](firmware/crsf2pwm/crsf2pwm.ino)：通道映射、失控输出、PWM 频率与电压修正系数。
- [固件配置说明](docs/firmware.md)：失控值、LED 行为及外设配置。

完整固件的 LED 用于指示 CRSF 连接状态，与常亮测试示例的行为不同。默认失控配置也需要根据模型车实际油门、转向通道调整。

## 硬件与实物

硬件资料入口为 [硬件说明](hardware/README.md)，目录按以下内容组织：

- `hardware/schematic/`：原理图。
- `hardware/pcb/`：PCB 与生产文件。
- `hardware/bom/`：物料清单。
- `hardware/photos/`：实物与测试照片。

## 许可证

本项目采用 [CC BY-NC-SA 4.0（署名—非商业性使用—相同方式共享 4.0 国际）](https://creativecommons.org/licenses/by-nc-sa/4.0/) 许可协议，完整条款见 [LICENSE](LICENSE)。分享或修改本项目时，请注明出处、提供许可链接并标明修改；不得用于商业目的，改编内容须按相同许可协议发布。

主程序整理自项目作者提供的 `crsf2pwm_1.0` 源码包。Arduino 核心、CMSIS 设备包和烧录工具遵循各自的上游许可证。详见 [源码与依赖来源](docs/provenance.md)。
