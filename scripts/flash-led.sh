#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
if [[ $# -ne 2 ]]; then
  echo 'Usage: bash scripts/flash-led.sh /path/to/Puya.PY32F0xx_DFP.pack PROBE_UID' >&2
  exit 2
fi
pack="$1"
probe_uid="$2"
hex="$PROJECT_ROOT/build/led/arduino_led_test.ino.hex"
PYOCD="${PYOCD:-pyocd}"
[[ -f "$pack" ]] || { echo "Pack not found: $pack" >&2; exit 1; }
[[ -f "$hex" ]] || { echo 'Build the LED example first: bash scripts/build.sh led' >&2; exit 1; }
command -v "$PYOCD" >/dev/null || { echo 'Install pyOCD or set PYOCD to its executable.' >&2; exit 1; }
echo 'Writing the LED example to PY32F002Ax5; affected flash pages will be erased.'
"$PYOCD" load --no-wait --uid "$probe_uid" --frequency 100k --pack "$pack" \
  --target PY32F002Ax5 --erase sector "$hex"
