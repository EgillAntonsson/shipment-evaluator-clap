#!/bin/bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "$0")/.." && pwd)"
build_dir="$project_dir/build"
plugin_path="$build_dir/shipment_evaluator.clap"
install_dir="${CLAP_PLUGIN_DIR:-/Library/Audio/Plug-Ins/CLAP}"
destination="$install_dir/shipment_evaluator.clap"

cmake -S "$project_dir" -B "$build_dir"
cmake --build "$build_dir" --parallel

if [[ ! -d "$plugin_path" ]]; then
    printf 'Build did not produce %s\n' "$plugin_path" >&2
    exit 1
fi

mkdir -p "$install_dir"
rm -rf "$destination"
ditto "$plugin_path" "$destination"
printf 'Installed %s\n' "$destination"
