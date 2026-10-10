#!/bin/bash
set -euo pipefail

resource_dir="$HOME/Library/Application Support/REAPER"
architecture="$(uname -m)"
case "$architecture" in
    arm64)
        cache_name="reaper-clap-macos-aarch64.ini"
        ;;
    x86_64)
        cache_name="reaper-clap-macos-x86_64.ini"
        ;;
    *)
        printf 'Unsupported macOS architecture: %s\n' "$architecture" >&2
        exit 1
        ;;
esac

cache_file="$resource_dir/$cache_name"

if pgrep -x REAPER >/dev/null 2>&1; then
    printf 'Asking REAPER to quit. Save any open project when prompted.\n'
    osascript -e 'tell application "REAPER" to quit'

    for ((attempt = 0; attempt < 60; ++attempt)); do
        if ! pgrep -x REAPER >/dev/null 2>&1; then
            break
        fi
        sleep 1
    done

    if pgrep -x REAPER >/dev/null 2>&1; then
        printf 'REAPER is still running; refusing to edit its CLAP cache.\n' >&2
        exit 1
    fi
fi

if [[ -f "$cache_file" ]] && grep -Fqx '[shipment_evaluator.clap]' "$cache_file"; then
    backup_file="$cache_file.backup.$(date '+%Y%m%d-%H%M%S')"
    temporary_file="$(mktemp "$resource_dir/.reaper-clap-cache.XXXXXX")"
    trap 'rm -f "$temporary_file"' EXIT

    cp -p "$cache_file" "$backup_file"
    awk '
        $0 == "[shipment_evaluator.clap]" { skip = 1; next }
        /^\[/ { skip = 0 }
        !skip { print }
    ' "$cache_file" > "$temporary_file"
    chmod "$(stat -f '%Lp' "$cache_file")" "$temporary_file"
    mv "$temporary_file" "$cache_file"
    trap - EXIT

    printf 'Cleared only Shipment Evaluator from the CLAP cache.\n'
    printf 'Backup: %s\n' "$backup_file"
else
    printf 'No cached Shipment Evaluator entry found; leaving the CLAP cache unchanged.\n'
fi

open -a REAPER
printf 'REAPER restarted. Its CLAP scanner will rebuild the cleared entry on startup.\n'
printf 'Next, run scripts/replace-shipment-evaluator-on-master.lua from REAPER’s Action List.\n'
