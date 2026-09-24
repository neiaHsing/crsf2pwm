# CRSF2PWM

**将 ELRS 接收机的 CRSF 串口数据转换为模型舵机、电调使用的 PWM 控制信号。**

这个项目源于一个模型车改装需求：把手里的穿越机 ELRS 串口接收机用于模型车，通过一块单片机转接板连接舵机和电调。固件基于 AirMCU Arduino 核心，当前源码配置了 8 路 PWM 输出，并包含连接状态指示、失控处理和电池电压回传逻辑。

信号路径：遥控器 → ELRS 接收机 → CRSF 串口 → 转接板 → PWM → 舵机 / 电调。

> 文档初稿，待作者复核。硬件设计、实物照片和实际测试结果将由作者补充。当前仓库记录了编译和 LED 示例下载结果，完整 CRSF 转 PWM 功能仍待实物验证。

## 功能与进度

| 项目 | 当前情况 |
| --- | --- |
| CRSF 通道解析 | 已包含，串口速率 420000 baud |
| PWM 输出 | 源码配置为 8 路、50 Hz；独立通道与时序待实测 |
| 通道映射与失控处理 | 可在源码中配置 |
| 状态 LED | PB2，低电平点亮 |
| 电池电压回传 | 已包含 ADC 采样和 CRSF 电池数据帧，待校准 |
| Arduino 构建 | 完整固件与 LED 示例均已编译通过 |
| DAPLink 下载 | Arduino LED 示例已有成功写入记录 |
| 硬件资料与实物测试 | 待作者补充 |

## MCU 与构建限制

完整程序沿用 AIR001 的 Arduino 配置；仓库同时保存了在 PY32F002A 上进行 LED 测试的流程。

**目前不能将完整程序直接视为适用于 PY32F002A 的已验证固件。** AIR001 开发板配置按 32 KB Flash / 4 KB RAM 链接，PY32F002A 容量检查按 20 KB Flash / 3 KB RAM 链接。后者当前无法容纳完整程序，且外设、引脚复用与定时器支持仍需逐项确认。

2026-09-25 的构建结果如下，使用 AirMCU 0.6.4、ARM GCC 14.2.1、HSI 8 MHz：

| 构建配置 | 程序大小 | 全局变量 | 结果 |
| --- | ---: | ---: | --- |
| 完整固件 `firmware` | 25,204 B | 2,176 B | 在 32 KB / 4 KB 配置下通过 |
| LED 示例 `led` | 10,748 B | 1,116 B | 在 20 KB / 3 KB 配置下通过 |
| 完整固件 `py32-check` | — | — | Flash 超出 4,952 B，RAM 超出 640 B |

RAM 链接检查还包含堆栈预留，不能只看全局变量大小。详细历史和验证边界见 [验证记录](docs/validation.md)。

## 目录结构

```text
crsf2pwm/
├── firmware/crsf2pwm/          # 主固件、CRSF 协议解析、引脚配置
├── examples/arduino_led_test/  # PB2 LED 常亮测试
├── scripts/                   # 环境安装、编译、LED 固件下载
├── docs/                      # 构建、配置、验证与来源说明
└── hardware/                  # 硬件资料（由作者补充）
    ├── schematic/             # 原理图
    ├── pcb/                   # PCB 与生产文件
    ├── bom/                   # 物料清单
    └── photos/                # 实物与测试照片
```

构建产物保存到 `build/`，工具和缓存保存到 `.local/`，均不纳入版本控制。

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
bash scripts/build.sh firmware   # 完整固件，AIR001 32 KB / 4 KB
bash scripts/build.sh py32-check # 完整固件的 PY32 容量检查，当前预期失败
```

生成的 `.hex`、`.bin`、`.elf` 等文件位于 `build/<配置>/`。

### 3. 通过 DAPLink 下载 LED 示例

安装 [pyOCD](https://pyocd.io/docs/installing.html)，并从 [Puya SDK](https://github.com/OpenPuya/PY32F0xx_Firmware) 获取对应的 CMSIS 设备包。历史使用的是 `Puya.PY32F0xx_DFP.1.2.6.pack`。

```sh
pyocd list
bash scripts/flash-led.sh /path/to/Puya.PY32F0xx_DFP.1.2.6.pack YOUR_PROBE_UID
```

将设备包路径和 `YOUR_PROBE_UID` 替换为实际值。此脚本会覆盖目标程序，下载对象为 PB2 LED 示例，目标设置为 `PY32F002Ax5`，SWD 频率为 100 kHz。按当前 LED 接法，示例启动后应使 LED 常亮，需结合实物确认。

AirMCU 的默认 Arduino 上传方式为 AirISP 串口下载；这里使用 Arduino 编译、pyOCD 经 DAPLink/SWD 下载。更多说明见 [构建与烧录](docs/build-and-flash.md)。

## 当前引脚配置

下表描述的是源码配置，最终 PCB 接线与 MCU 适配情况以硬件验证为准。

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
- [固件配置说明](docs/firmware.md)：当前失控值、LED 行为及外设待确认项。

完整固件的 LED 用于指示 CRSF 连接状态，与常亮测试示例的行为不同。默认失控配置也需要根据模型车实际油门、转向通道调整。

## 硬件与实物

本部分由作者撰写，入口为 [硬件说明](hardware/README.md)。后续计划补充：

- 原理图、PCB 文件与供电设计。
- BOM、元件选择、焊接和装配过程。
- 接收机、舵机、电调的接线与实物照片。
- LED、PWM 时序、失控行为和电压回传的测试结果。

## 后续工作

- [ ] 补充硬件资料与实物记录。
- [ ] 验证各路 PWM 的独立输出和脉宽。
- [ ] 验证 CRSF 连接、断连与失控处理。
- [ ] 校准电池电压采样与回传。
- [ ] 完成 PY32F002A 的容量优化和外设适配验证。
- [ ] 补齐原始源码出处与许可证信息。

## 来源与授权

主程序整理自项目作者提供的 `crsf2pwm_1.0` 源码包。该包未附带许可证或可核实的原始仓库地址，目前保留已有代码，待补齐来源与授权信息；暂未添加统一开源许可证。

Arduino 核心、CMSIS 设备包和烧录工具通过各自上游获取。详见 [源码与依赖来源](docs/provenance.md)。
