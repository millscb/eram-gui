# Native C++ architecture decision

The repository's existing ARCHITECTURE.md proposes a React/Vite client. This implementation intentionally uses **C++20 + Qt 6 Widgets**, per the project's current native-desktop requirement; the prior document is retained rather than overwritten.

## Dependency direction

```text
main / Qt Widgets UI
       |
       v
    eram_core (pure C++ state and simulation)
```

The GUI must never own canonical simulation state. Widgets observe or query the simulation model. The core library has no Qt headers or dependency, enabling a separate CLI runner, deterministic replay harness, unit tests, and alternative UI frontends later. Synthetic scenario fixtures are the only data source.

## Existing specification mapping

- OMCWS shell: MainWindow, QMdiArea, channel/banner clock
- System summary: all-UP synthetic groups
- Processors: static synthetic inventory
- Events: in-memory event list with acknowledgement
- R-position: QPainter vector display over normalized coordinates
- Toolbars and MCA: deferred to later milestones
- Live operational interfaces: **explicitly out of scope**

## Simulation contracts

The initial core uses normalized display coordinates, a 600-NM synthetic display width, simple heading/groundspeed kinematics, and an explicit `step(seconds)` method. No live feed, actual radar site data, geo-accuracy, or aircraft separation guarantee is provided. Future scenario engines should include seeded deterministic inputs and versioned data models rather than networking directly from widgets.

Qt is deliberately confined to the GUI layer; unit tests run without a display server.
