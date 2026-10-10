# The Shipment Evaluator CLAP Plugin

A CLAP utility plugin inspired by the [ReaScript Shipment Evaluator](https://www.egill.rocks/blog/rea-scripts/shipment-evaluator).
It does not have audio inputs or outputs and does not process audio.

When the host transport is playing, the Evaluation Meter advances by 0.025 per
processing block until it reaches 11. Stopping playback resets the meter. At 11,
the GUI displays:

> ♬ ♫ ♪ ♩  →  SHIP IT !  ←  ♩ ♪ ♫ ♬

The plugin provides an embedded Cocoa GUI on macOS. The needle follows the
transport-driven meter; it is a display, not an audio control.

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

Copy `build/shipment_evaluator.clap` to
`~/Library/Audio/Plug-Ins/CLAP/`, then rescan plugins in your DAW.
