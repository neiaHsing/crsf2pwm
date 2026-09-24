# 构建与烧录

## Arduino 环境

`bash scripts/setup-arduino.sh` 安装 AirMCU **0.6.4** 及其工具依赖。索引使用 Air-duino 官方 GitHub 发布地址。工具与缓存保存在 `.local/arduino/`，不会写入 Git。

```sh
bash scripts/build.sh firmware   # 完整工程，AIR001 32 KB / 4 KB
bash scripts/build.sh led        # PB2 LED，20 KB / 3 KB
bash scripts/build.sh py32-check # 完整工程的 PY32 容量检查，当前预期失败
```

产物位于 `build/<配置>/`，包括 `.hex`、`.bin`、`.elf` 和 `.map`。源码目录名与 `.ino` 文件名保持一致，可直接作为 Arduino sketch 打开。使用 Arduino IDE 时安装相同核心并选择 Air001 Dev Chip、HSI 8 MHz；LED 的 20 KB/3 KB 链接限制由本仓库脚本显式设置，IDE 默认配置没有这一限制。

如已有 ARM GCC 和 CMSIS，可复用本机安装：

```sh
ARM_GCC_BIN=/path/to/toolchain/bin \
CMSIS_INCLUDE=/path/to/CMSIS/Core/Include \
bash scripts/build.sh led
```

`CMSIS_INCLUDE` 应包含 `core_cm0plus.h`。这两个变量可选；正常安装官方包后无需设置。`ARDUINO_CLI`、`ARDUINO_DIRECTORIES_DATA`、`ARDUINO_DIRECTORIES_DOWNLOADS`、`ARDUINO_DIRECTORIES_USER` 可覆盖工具与缓存位置。

本机路径也可放入 `.local/build-env.sh`，构建脚本会加载它；该文件不上传 Git。当前作者工作区已配置本机 ARM GCC/CMSIS，直接执行 `build.sh` 即可。

## DAPLink / SWD

AirMCU 0.6.4 的默认 Arduino 上传配置是 AirISP 串口下载。当前 DAPLink 工作流采用 Arduino 编译、pyOCD 经 SWD 下载。

安装 [pyOCD](https://pyocd.io/docs/installing.html)，从 [Puya SDK](https://github.com/OpenPuya/PY32F0xx_Firmware) 获取 `Packs/MDK/Puya.PY32F0xx_DFP.1.2.6.pack` 或合适的设备包，然后：

```sh
pyocd list
bash scripts/flash-led.sh /path/to/Puya.PY32F0xx_DFP.1.2.6.pack YOUR_PROBE_UID
```

脚本只下载 LED 示例，目标明确设为 `PY32F002Ax5`，SWD 速率为 100 kHz。历史操作中降低到此速率后写入成功。脚本会覆盖对应 Flash 页中的现有程序。烧录成功后仍需观察 LED，不能仅凭工具的写入日志认定实物测试通过。

连接 SWDIO、SWCLK 和公共 GND，按调试器说明连接目标供电/电平参考。通常不用按 BOOT0 或 NRST；供电、BOOT0 与复位的实际电路由硬件说明补充。

## 当前限制

完整固件的 AIR001 配置不能直接作为 PY32F002A 的容量保证。`py32-check` 保留了严格的 20 KB/3 KB 限制；不会为了通过编译放宽它。当前工作仅整理工程，没有完成完整 CRSF 固件向 PY32F002A 的移植或容量优化。
