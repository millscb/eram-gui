# ERAM GUI Simulator (C++ / Qt 6)

An **offline, synthetic-data desktop simulator** inspired by the graphical interfaces described in [SPECIFICATION.md](SPECIFICATION.md). The initial C++ milestone provides an OMCWS dashboard, processor list, event acknowledgement, a sector situation display, and a GUI-independent simulation model.

**Training and demonstration only. Not an FAA system, not operational air-traffic-control software, and not connected to live radar, flight data, or safety-critical services.**

## Requirements

- C++20 compiler (MSVC 2022, GCC 11+, or recent Clang)
- CMake 3.21+
- Qt 6.4+ with the **Widgets** component (including Qt development packages)
- Ninja for the supplied CMake presets

## Build

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Run `build/debug/eram_gui` on Linux/macOS, or `build/debug/eram_gui.exe` on Windows. Run from an environment with a working Qt deployment; Windows packaged builds require the Qt runtime plugins (e.g. deploy with `windeployqt`). Alternatively, without Ninja:

```sh
cmake -S . -B build/local -DCMAKE_BUILD_TYPE=Debug
cmake --build build/local
ctest --test-dir build/local --output-on-failure
```

### Ubuntu setup

```sh
sudo apt update
sudo apt install build-essential cmake ninja-build qt6-base-dev
```

## Project layout

- `src/core/` - Pure C++ simulation state, events, channel switching and synthetic aircraft motion, no Qt dependency
- `src/gui/` - Qt 6 Widgets OMCWS shell and painted R-position viewport
- `tests/` - Headless CTest simulation regression checks
- `SPECIFICATION.md` - Existing original UI and state-machine requirements
- `ARCHITECTURE.md` - Original **web/React proposal** retained for reference; see `docs/NATIVE_ARCHITECTURE.md` for the adopted C++ design

## Milestone 0.1

The starter implements: desktop MDI windows, processor and event views, active/backup simulated channels, a UTC clock, synthetic targets and deterministic time stepping. Status is initially all-UP as required by the specification. The GUI uses the Motif-inspired dark palette.

Not yet implemented: full OMCWS menu hierarchy, advanced window state persistence, realistic geospatial charting, D-position tabular views, detailed MCA command parser, tear-off toolbars, radar filters, conflict alerts, flight-plan modeling, and scenario serialization. None of the displayed trajectories should be interpreted as operational predictions.

## Next milestones

1. Factor OMCWS windows into individual models and reusable status widgets.
2. Implement toolbar finite-state machine, dock/tear-off panels, radar range and layer controls.
3. Add scenario loading/saving, flight plans, aircraft histories, and event timeline.
4. Build D-position lists and a non-operational MCA training command grammar.
5. Add deterministic simulation replay, performance profiling, GUI integration tests, and CI packaging.
