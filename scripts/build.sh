#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
if [[ -f "$PROJECT_ROOT/.local/build-env.sh" ]]; then
  source "$PROJECT_ROOT/.local/build-env.sh"
fi
profile="${1:-firmware}"
case "$profile" in
  firmware) sketch="$PROJECT_ROOT/firmware/crsf2pwm"; flash=32768; ram=4096 ;;
  led) sketch="$PROJECT_ROOT/examples/arduino_led_test"; flash=20480; ram=3072 ;;
  py32-check) sketch="$PROJECT_ROOT/firmware/crsf2pwm"; flash=20480; ram=3072 ;;
  *) echo 'Usage: bash scripts/build.sh {firmware|led|py32-check}' >&2; exit 2 ;;
esac
props=(--build-property "upload.maximum_size=$flash" --build-property "upload.maximum_data_size=$ram")
# Optional overrides for an existing ARM GCC/CMSIS installation.
if [[ -n "${ARM_GCC_BIN:-}" ]]; then
  props+=(--build-property "compiler.path=${ARM_GCC_BIN%/}/")
fi
if [[ -n "${CMSIS_INCLUDE:-}" ]]; then
  [[ -f "$CMSIS_INCLUDE/core_cm0plus.h" ]] || { echo 'CMSIS_INCLUDE must contain core_cm0plus.h' >&2; exit 1; }
  props+=(--build-property "compiler.arm.cmsis.c.flags=\"-I$CMSIS_INCLUDE\"" --build-property 'compiler.arm.cmsis.ldflags=')
fi
"$ARDUINO_CLI" compile --fqbn 'AirM2M:AirMCU:Air001Dev:ClockSourceAndFrequency=HSI8M_HCLK8M' \
  "${props[@]}" --build-path "$PROJECT_ROOT/build/$profile" "$sketch"
