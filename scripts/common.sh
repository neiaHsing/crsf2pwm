#!/usr/bin/env bash
# Shared paths are derived from this repository, never a particular user's home.
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ARDUINO_CLI="${ARDUINO_CLI:-arduino-cli}"
export ARDUINO_DIRECTORIES_DATA="${ARDUINO_DIRECTORIES_DATA:-$PROJECT_ROOT/.local/arduino/data}"
export ARDUINO_DIRECTORIES_DOWNLOADS="${ARDUINO_DIRECTORIES_DOWNLOADS:-$PROJECT_ROOT/.local/arduino/downloads}"
export ARDUINO_DIRECTORIES_USER="${ARDUINO_DIRECTORIES_USER:-$PROJECT_ROOT/.local/arduino/user}"
AIRMCU_INDEX='https://github.com/Air-duino/Arduino-pack-json-ci/releases/download/Nightly/package_air_index.json'
AIRMCU_VERSION='0.6.4'
