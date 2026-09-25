# 源码与依赖来源

- 主程序：作者提供的 `crsf2pwm_1.0` 文件夹，经现有工作区整理导入。包括 `CrsfSerial`、CRC8、CRSF 协议定义与滤波辅助代码。
- LED 示例：本项目调试过程中编写，调用 Arduino API 驱动 PB2。
- [Air-duino / Arduino-AirMCU](https://github.com/Air-duino/Arduino-AirMCU)：Arduino 框架，固定 0.6.4。由脚本下载，遵循上游许可证。
- [OpenPuya / PY32F0xx_Firmware](https://github.com/OpenPuya/PY32F0xx_Firmware)：此前 HAL 点灯实验及 CMSIS 设备包来源。HAL 实验和工具包保存在本地，不作为本仓库的主程序分发。
- [pyOCD](https://github.com/pyocd/pyOCD)：SWD 烧录工具，历史使用版本 0.45.1。

本项目的固件、示例、脚本、文档及作者提供的硬件资料采用 [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) 许可协议，完整条款见 [LICENSE](../LICENSE)。第三方依赖及另有许可标注的材料遵循各自的许可证。
