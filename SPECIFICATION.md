# ERAM GUI Simulator: Functional & UI Specification

## 1. System Overview

This specification defines the functional requirements, visual layout, and finite-state machine (FSM) transitions for a frontend-only graphical user interface simulator of the Federal Aviation Administration (FAA) En Route Automation Modernization (ERAM) system. 

The simulator focuses on two distinct operational console interfaces:
1. Operational Monitor and Control Workstation (OMCWS): Used in the System Operations Center (SOC) for facility maintenance, processor health monitoring, and system-level configuration.
2. Air Traffic Operational Workstation (R-Position and D-Position): Used by Radar and Radar Associate air traffic controllers for sector traffic separation and flight data management.

All functional subsystems, communication links, processors, and surveillance feeds in this simulation are statically set to an operational UP (green) state.

---

## 2. Global Styling and Visual Tokens

ERAM displays use standardized visual cues originating from Motif/X11 conventions for OMCWS and custom phosphor-emulating vector palettes for Sector displays.

### 2.1 Color Palette

* OMCWS Background: `#0B132B` (Deep Navy / Charcoal)
* OMCWS Window Frame / Bevel: `#1C2541` (Slate Navy with raised borders)
* OMCWS Title Bar Active: `#3A506B` (Muted Steel Blue)
* Sector Situation Display Background: `#000511` (Near Black / Dark Navy)
* Status UP / Normal: `#00FF66` (Bright Phosphor Green, 4px horizontal bar)
* Status Degraded: `#FFD700` (Amber Triangle)
* Status Down / Failed: `#FF3333` (Red X)
* Status Diagnostic: `#FFFF00` (Yellow background block with black text `DGN`)
* Menu Button Closed: `#2A4D69` (Subdued Air Force Blue)
* Menu Button Open / Active: `#E05A47` (Burnt Coral)
* Toggle Button Suppressed / Off: `#1A1D20` (Dark Charcoal / Black)
* Toggle Button Active / On: `#4A5568` (Cool Gray)
* Increment / Decrement Button: `#1B998B` (Medium Spring Green)
* Command Button Deactivated: `#008B8B` (Dark Teal)
* Command Button Activated: `#E05A47` (Burnt Coral)
* Radar Target / Vector Lines: `#39FF14` (Classic CRT Radar Green)
* Primary Text: `#F0F4F8` (Off-white Monospace)
* Secondary Text / Inactive: `#94A3B8` (Slate Gray)

### 2.2 Typography

* Primary Font Family: Monospace (`Consolas`, `Monaco`, `Courier New`, or fixed-width sans-serif `Liberation Sans`)
* Sector Character Grid: Fixed aspect ratio character cells
* Font Size Tokens:
  * Small (Sub-labels, timestamps): 11px / 1.1
  * Medium (Standard data blocks, menu buttons): 13px / 1.2
  * Large (Sector identifiers, critical headers): 16px / 1.2

---

## 3. Operational Monitor and Control Workstation (OMCWS)

### 3.1 Shell Layout

```
+---------------------------------------------------------------------------------------+
| CHANNEL BANNER: ChA : ACTIVE   Release: F40PGA00   UTC: 16:30:00   Site: ZDC (Washington)|
+---------------------------------------------------------------------------------------+
| TOOLBAR: [Summary v] [Processors v] [Events v] [Surveillance v] [External v] [Net] ...|
+---------------------------------------------------------------------------------------+
| WORKSPACE CANVAS (MDI Desktop Container for Floating Draggable Windows)               |
|                                                                                       |
|   +-- [X] System Summary --+       +-- [X] Processors --------------------+           |
|   | Surveillance: [UP]     |       | Control Room:                        |           |
|   | Flight Data:  [UP]     |       |   [101R: UP] [101D: UP] [102R: UP]   |           |
|   | Weather:      [UP]     |       | Equipment Room:                      |           |
|   | Strategic Alt:[UP]     |       |   [SDRP1: UP] [FDP1: UP] [CPRP1: UP] |           |
|   +------------------------+       +--------------------------------------+           |
+---------------------------------------------------------------------------------------+
```

### 3.2 Top Channel Banner

* Position: Fixed top, full width, height 28px.
* Information Fields:
  * Channel Identifier: `Channel A : ACTIVE` (Green text or normal background)
  * Secondary Channel: `Channel B : BACKUP IN SYNC`
  * Software Release: `Release: F40PGA00`
  * Time Display: Continuous UTC digital clock (`HH:MM:SS UTC`)
  * Facility ID: 3-letter ARTCC identifier (e.g., `ZTL`, `ZLC`, `ZDC`)

### 3.3 Menu Hierarchy & Dropdown Trees

The OMCWS toolbar contains drop-down buttons and single-action action buttons:

1. `Summary ▾`:
   * `Processors Summary` -> Opens Processors Summary window
   * `System Summary` -> Opens System Summary window
2. `Processors ▾`:
   * `Processors` -> Opens main Processors matrix window
   * `Processor Status` -> Opens detailed hardware/software component view
   * `Processor Details` -> Opens processor inventory and diagnostics
   * `Flight Strip Printer Settings` -> Opens printer mapping dialog
3. `Events ▾`:
   * `Events` -> Opens current real-time Events window
   * `Event History` -> Opens searchable event audit log
4. `Surveillance ▾`:
   * `Surveillance` -> Opens overall Radar Site and SBS interface matrix
   * `Radar Status` -> Opens operational radar status overview
   * `Radar Details` -> Opens specific sensor configuration modal
   * `Radio Station Details` -> Opens ADS-B and ground radio station parameters
5. `External Interfaces ▾`:
   * `External Interfaces` -> Opens external boundary facility status (ARTCCs, TRACONs, Weather, IPOP, CPDLC, TDLS, NAP)
   * `FDIO Assignments` -> Opens Flight Data Input/Output mappings
   * `Service-B Users` -> Opens teletype user configurations
6. `Networks` (Single Button):
   * Immediately opens the Network LAN status window (SwOPS LAN, Storage LAN, Support LAN, ECG LAN)
7. `External Comm ▾`:
   * `Modem Status`
   * `Line Monitoring`
8. `Ports ▾`:
   * `Port Summary`
   * `Port Reconfiguration`
9. `Subsystem ▾` (or `SWIM ▾`):
   * `SWIM Terminal Data Distribution System (STDDS)`
   * `National Application Processing (NAP)`
10. `Data Management ▾`:
    * `Adaptation Data Management`
    * `Recording & Playback (SAR)`
11. `Print Management ▾`:
    * `Queue Manager`
    * `Printer Assignments`
12. `Tech Manuals ▾`:
    * `TI 6110.110 Reference`
    * `System Specifications`

### 3.4 Key OMCWS Windows

#### 3.4.1 System Summary Window
* High-level service group boxes:
  * `Surveillance Services`: `Target Processing [UP]`, `Multi-Radar Tracking [UP]`, `ADS-B Processing [UP]`
  * `Flight Data Services`: `Trajectory Modeling [UP]`, `Flight Plan Pairing [UP]`, `National Exchange [UP]`
  * `Weather Services`: `NEXRAD Interface [UP]`, `WARP Radar [UP]`, `RAP Real-time [UP]`
  * `Strategic Alert Services`: `Conflict Alert (CA) [UP]`, `Mode C Intruder (MCI) [UP]`, `CPRP Probe [UP]`
  * `Channel Resources`: `Channel A Sync [UP]`, `Channel B Standby [UP]`
  * `National Applications`: `NAP ZLC [UP]`, `NAP ZTL [UP]`

#### 3.4.2 Processors Window
* Organized into logical group boxes:
  * `Control Room`: Sectors `01R` through `36R`, `01D` through `36D`. Each tile shows sector number and a solid green bar.
  * `Equipment Room`: `SDR1`, `SDR2`, `FDP1`, `FDP2`, `CPRP1`, `CPRP2`, `MCRP1`, `MCRP2`, `ECGIP1`, `ECGIP2`.
  * `System Operations Center (SOC)`: `401M` through `406M` (OMCWS consoles).
  * `Support & Test`: `DRAPL`, `ESSPL`, `STPL`, `SIMPL`.
* Tile Interaction: Clicking any processor tile opens the `Processor Status` dialog for that specific unit.

#### 3.4.3 Events Window
* Toolbar controls: `View: [Non-Surveillance | Surveillance | Combined]`, `Ack Selected`, `Auto Ack [ON]`, `Delete`, `Filter...`
* Tabular Columns:
  1. `Time`: UTC timestamp (`HH:MM:SS`)
  2. `Severity`: `INFO` (Green/White), `WARN` (Amber), `CRIT` (Red)
  3. `Subsystem`: `SURV`, `FLTS`, `EDSM`, `MIPS`, `IFPA`, `ESRM`
  4. `Unit`: Target processor or sector ID
  5. `Message`: Description (e.g., `CHANNEL A SYNCHRONIZATION VERIFIED`, `BEACON CODE BLOCK NOMINAL`)

---

## 4. Sector Controller Workstation (R-Position & D-Position)

### 4.1 Situation Display (RPD Canvas)

* Geometry: Square 2048x2048 coordinate canvas centered on sector airspace.
* Map Layers:
  * Sector boundaries (solid thin green or cyan lines).
  * High/Low jet routes (dashed gray lines).
  * Navigational fixes and VOR diamonds with 3/5-letter identifiers.
* Target Display:
  * Primary target symbol: Small circle or square.
  * Secondary (beacon) target symbol: Slash or target chevron.
  * History dots: 4 decaying intensity dots along the past trajectory.
  * Velocity vector: Projected line indicating 1, 2, or 3 minutes of forward flight.
* Full Data Block (FDB) 3-Line Structure:
  * Line 1: Aircraft Identification (e.g., `AAL1244`)
  * Line 2: Assigned Altitude + Current Mode C Flight Level (e.g., `350C 334` -> Assigned FL350, Climbing through FL334)
  * Line 3: Computer Identification Number (`CID`) + Groundspeed (`480` kts) + Sector indicator
  * Leader Line: Line connecting target symbol to data block (orientation positions 1 through 8).

### 4.2 Tear-Off Toolbar System

The tear-off toolbar can float anywhere or dock along the display perimeter:

* Master Toggle: `TOOLBAR` button. When clicked:
  * Button background changes from Blue (`#2A4D69`) to Burnt Coral (`#E05A47`).
  * Reveals tear-off management menu: `MASTER TOOLBAR`, `MCA TOOLBAR`, `HORIZ TOOLBAR`, `LEFT TOOLBAR`, `RIGHT TOOLBAR`, `MASTER LOWER`, `MCA RAISE`.
* Primary Toolbar Row Buttons:
  * `VIEWS`: Replaces toolbar row with sub-views (`ALTIM SET`, `AUTO HO INHIB`, `CFR`, `CODE`, `CONFLCT ALERT`, `CPDLC ADV`, `DEPT LIST`, `HOLD LIST`, `INBND LIST`, `MRP LIST`, `SAA FILTER`, `WX REPORT`).
  * `DRAW`: Opens graphic annotation tools (Box, Circle, Line, Freehand Text `A`, Font Size, Color palette `100`/`90`, `DEL`, `DEL ALL`).
  * `ATC TOOLS`: Opens spacing aids, J-rings (distance circles around target), Halo lines, Vector lines.
  * `RANGE`: Range selection buttons (`75`, `150`, `225`, `300` nmi, `+`, `-`, Range Slider).
  * `ALT LIM`: Altitude filtering sliders (Low / High altitude cutoff limits).
  * `RADAR FILTER`: Target selection filters (All, VFR, En Route Only, Terminal).
  * `AB SETTING`: Active border controls (`WIDTH 1`, `AB OPEN`, `AB CLOSED`).

### 4.3 Message Composition Area (MCA)

The MCA is positioned at the bottom or docked corner of the radar display:

* `Preview Area`:
  * Displays user keyboard input as it is typed (e.g., `QZ AAL1244 370` for altitude clearance).
  * Monospace uppercase text with solid blinking cursor.
* `Feedback Area`:
  * Displays system response upon pressing Enter.
  * Positive Acceptance: Green checkmark icon with text `ACCEPTED`.
  * Rejection: Red `X` icon with syntax error description (e.g., `INVALID ALTITUDE LIMIT`, `FLIGHT PLAN NOT FOUND`).

### 4.4 D-Position Tabular Views (DVu)

Tabular flight data lists displayed on the auxiliary 24-inch flat panel:

* `Departure List (DEP)`: Callsign, Proposed Departure Time, Assigned Altitude, Route, Beacon Code.
* `Inbound List (INB)`: Callsign, Fix Estimate, Assigned Flight Level, Destination.
* `Holding List (HOLD)`: Callsign, Holding Fix, Inbound Course, EFC Time.
* Status Bar: Channel health indicators (`CH: A - ACTIVE`, `CH: B - BACKUP`), `CA ON UP`, `MCI ON UP`, `CPDLC UP`.

---

## 5. Finite-State Machine (FSM) Transitions

### 5.1 OMCWS Window Manager State Machine

```
State 0: IDLE (Desktop clean, only top banner and toolbar active)
  |
  +-- User clicks Dropdown (e.g., "Summary v")
  |     |
  |     v
  |   State 1: DROPDOWN_OPEN (Dropdown menu displayed, captures focus)
  |     |
  |     +-- Click outside -> State 0
  |     +-- Click Item (e.g., "System Summary") -> State 2
  |
  +-- Click direct button (e.g., "Networks") -> State 2
  |
  v
State 2: WINDOW_ACTIVE (Window mounted to DOM, z-index elevated)
  |
  +-- Click Minimize -> State 3: WINDOW_MINIMIZED (Hidden, docked icon highlighted)
  +-- Click Close [X] -> State 0: WINDOW_CLOSED (Unmounted)
  +-- Click Header Drag -> State 4: WINDOW_DRAGGING (Follows mouse cursor)
  +-- Click Another Window -> State 5: WINDOW_INACTIVE (Z-index lowered)
```

### 5.2 Tear-Off Toolbar State Machine

```
State 0: DOCKED_COLLAPSED (Standard top toolbar row)
  |
  +-- Click [TOOLBAR] button
  |     |
  v     v
State 1: MENU_EXPANDED (Button turns Burnt Coral, 2x5 tear-off menu visible)
  |
  +-- Select "HORIZ TOOLBAR" -> State 2: DETACHED_FLOATING (Can be dragged anywhere)
  +-- Select "VIEWS" -> State 3: SUBMENU_VIEWS_ACTIVE (Toolbar buttons swapped for Views)
  |     |
  |     +-- Click "RETURN" -> State 1
  |     +-- Click "DEPT LIST" -> Opens floating Departure List
  |
  +-- Select "DRAW" -> State 4: SUBMENU_DRAW_ACTIVE (Toolbar buttons swapped for Draw tools)
```

### 5.3 Button Visual State Machine

Every button in the ERAM GUI conforms to one of five deterministic interaction models:

1. Menu Button:
   * Closed: Background Blue (`#2A4D69`), Border Raised.
   * Open: Background Burnt Coral (`#E05A47`), Border Sunken.
2. Toggle Button:
   * Suppressed / Inactive: Background Dark Charcoal (`#1A1D20`), Text Dull White.
   * Displayed / Active: Background Cool Gray (`#4A5568`), Text Bright White.
3. Increment / Decrement Button:
   * Normal: Background Spring Green (`#1B998B`).
   * Left-Click (Pick): Decrements value by 1.
   * Right-Click / Enter: Increments value by 1.
4. Command Button:
   * Idle: Background Dark Teal (`#008B8B`).
   * Triggered: Background Burnt Coral (`#E05A47`) for 300ms, then returns to Idle.
5. Press-and-Hold Button:
   * Released: Black rectangle with gray cutout.
   * Depressed: Gray rectangle with black cutout (active while mouse is down).
