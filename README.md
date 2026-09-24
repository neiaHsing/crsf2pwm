# CRSF2PWM

用于 ELRS 接收机的 CRSF 串口转 PWM 项目，基于 AirMCU Arduino 核心。仓库保存当前的 8 通道固件源码、PB2 LED 测试程序，以及可重复使用的构建和烧录脚本。

## 当前状态

- `firmware/crsf2pwm`：完整 CRSF → PWM 程序，已使用 AirMCU 0.6.4 编译通过；8 路 PWM 的实物功能尚未在本仓库记录验证结果。
- `examples/arduino_led_test`：PB2 低电平点亮 LED 的最小 Arduino 程序。此前已通过 DAPLink 写入并恢复运行；实物是否点亮由作者补充确认。
- 完整固件此前按 AIR001 的 32 KB Flash / 4 KB RAM 配置编译。按 PY32F002A 标称 20 KB Flash / 3 KB RAM 限制编译时超限，不能把 AIR001 编译通过等同于 PY32F002A 完整兼容。
- 硬件设计、制作过程和实物照片由作者在 [`hardware/`](hardware/README.md) 补充。

## 目录

```text
firmware/crsf2pwm/          CRSF 协议解析、PWM 输出、失控处理、电压回传
examples/arduino_led_test/  PB2 LED 点亮测试
scripts/                   环境安装、编译、LED 测试固件烧录
docs/                      构建说明、代码配置、验证记录与来源说明
hardware/                  原理图、PCB、BOM、照片及实物说明（待作者补充）
build/                     编译产物（自动生成，不上传）
.local/                    下载缓存、SDK、历史实验（不上传）
```

## 开始使用

需要安装 [Arduino CLI](https://arduino.github.io/arduino-cli/)；脚本适用于 macOS/Linux 的 Bash 环境。

```sh
bash scripts/setup-arduino.sh
bash scripts/build.sh led
bash scripts/build.sh firmware
```

`led` 按 20 KB Flash / 3 KB RAM 限制构建，`firmware` 按 AIR001 的 32 KB / 4 KB 配置构建。两个配置都选择 HSI 8 MHz。构建成功只表示编译、链接通过，不代表外设与时序验证通过。

完整操作见 [构建与烧录](docs/build-and-flash.md)，引脚、LED 行为与待确认项见 [固件配置](docs/firmware.md)。

## 来源与授权

主程序来自作者提供的 `crsf2pwm_1.0` 源码包，目录整理保留了工作区现有代码。该包未附带许可证或可核实的原始仓库地址，本仓库暂不为这些代码新增开源许可证。第三方框架通过工具下载，不随仓库分发。详见 [来源说明](docs/provenance.md)。
