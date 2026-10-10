# The Shipment Evaluator CLAP Plugin

See the demonstration video and more info on [The Shipment Evaluator](https://www.egill.rocks/blog/clap/shipment-evaluator).

## Disclaimer

Use this plugin at your own risk: it goes to 11 and provides a powerful urge to
`♬ ♫ ♪ ♩ →  SHIP IT  ← ♩ ♪ ♫ ♬`. Be careful when you use it—not too soon, not
too late.

The plugin always gives the same evaluation. It is a visual encouragement and
does not process or analyze audio.

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

The resulting `shipment_evaluator.clap` plugin is written to `build/`.

## Install

On macOS, copy `build/shipment_evaluator.clap` to
`~/Library/Audio/Plug-Ins/CLAP/`, then rescan plugins in your DAW. Alternatively,
copy it to another directory your DAW scans for CLAP plugins.

Add the plugin to a track, press play, and get the urge to
`♬ ♫ ♪ ♩ →  SHIP IT  ← ♩ ♪ ♫ ♬`.
