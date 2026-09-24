#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
command -v "$ARDUINO_CLI" >/dev/null || { echo 'Please install Arduino CLI first.' >&2; exit 1; }
mkdir -p "$ARDUINO_DIRECTORIES_DATA" "$ARDUINO_DIRECTORIES_DOWNLOADS" "$ARDUINO_DIRECTORIES_USER"
"$ARDUINO_CLI" core update-index --additional-urls "$AIRMCU_INDEX"
"$ARDUINO_CLI" core install "AirM2M:AirMCU@$AIRMCU_VERSION" --additional-urls "$AIRMCU_INDEX"
