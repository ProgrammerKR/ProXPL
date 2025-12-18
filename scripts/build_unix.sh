#!/usr/bin/env bash
# --------------------------------------------------
#   Project: ProX Programming Language (ProXPL)
#   Author:  ProgrammerKR
#   Created: 2025-12-16
#   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j $(nproc)

echo "Build complete: $BUILD_DIR"
