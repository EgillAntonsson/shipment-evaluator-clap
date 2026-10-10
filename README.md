# The Shipment Evaluator CLAP Plugin

See the demonstration video and more info on [The Shipment Evaluator](https://www.egill.rocks/blog/clap/shipment-evaluator).

## Disclaimer

Use this plugin at your own risk: it goes to 11 and provides a powerful urge to
`♬ ♫ ♪ ♩ →  SHIP IT  ← ♩ ♪ ♫ ♬`. Be careful when you use it—not too soon, not
too late.

The plugin always gives the same evaluation. It is a visual encouragement and
does not alter or analyze audio; its stereo audio ports pass the input through
unchanged so the plugin can sit in a DAW track's FX chain.

While the DAW is playing, the Evaluation Meter advances toward 11. At 11, the
plugin displays the “SHIP IT!” result. Stopping playback resets the meter.

## Build

Requirements:

- macOS with Xcode Command Line Tools
- CMake 3.16 or later
- A C++17/Objective-C++17 compiler
- The [CLAP headers](https://github.com/free-audio/clap)

Set `CLAP_PATH` to the CLAP checkout if it is not already in a standard include
directory, then configure and build:

```sh
export CLAP_PATH="$HOME/clap"
cmake -S . -B build
cmake --build build
```

Alternatively, provide `CLAP_INCLUDE_DIR` as the directory containing
`clap/clap.h`:

```sh
cmake -S . -B build -DCLAP_INCLUDE_DIR=/path/to/clap/include
cmake --build build
```

The resulting macOS `shipment_evaluator.clap` bundle is written to `build/`.

## Install

On macOS, copy the `build/shipment_evaluator.clap` bundle to
`/Library/Audio/Plug-Ins/CLAP/`, then rescan plugins in your DAW. Alternatively,
copy it to another directory your DAW scans for CLAP plugins, such as
`~/Library/Audio/Plug-Ins/CLAP/`.

To build and install it to the standard system-wide macOS CLAP directory in one
step, run (the destination can be overridden with `CLAP_PLUGIN_DIR`):

```sh
./scripts/install-macos.sh
```

## Known issues and REAPER troubleshooting

The plugin does not change or analyze audio. When REAPER is playing, its meter
should advance toward 11; stopping playback resets it.

If REAPER marks the plugin unavailable after an install or update, the
`scripts/restart-reaper-rescan.sh` helper gracefully quits REAPER, backs up the
CLAP cache, removes only the Shipment Evaluator cache entry, and restarts REAPER
so it can rescan this plugin. Save any open project when prompted.

After REAPER restarts and completes its scan, run the replacement ReaScript once
to remove any stale Shipment Evaluator instance from the Master track, add a
fresh instance, and open its window:

1. In REAPER, open **Actions → Show action list**.
2. Choose **New action → Load ReaScript**, then select
   `scripts/replace-shipment-evaluator-on-master.lua`.
3. Run the newly added action and confirm the replacement.

The FX replacement is recorded as one undoable project change. The helper only
changes the Master track; it does not remove instances from other tracks.

The initial REAPER compatibility issue was caused by a missing required
`on_main_thread` callback. The plugin now supplies that callback and is built as
a signed macOS `.clap` bundle. If the meter remains static after a fresh
instance is added, verify that the track or Master FX chain is processing during
playback; the plugin's diagnostic logs include transport flags, meter progress,
and GUI refresh activity.

Add the plugin to a track, press play, and get the urge to
`♬ ♫ ♪ ♩ →  SHIP IT  ← ♩ ♪ ♫ ♬`.
