# ERAM GUI Simulator: Software Architecture & Data Models

## 1. Architecture Overview

The ERAM GUI Simulator is architected as a lightweight, zero-backend, client-side web application. It models the visual hierarchies and finite-state machine (FSM) transitions of the FAA's ERAM system without requiring real-time radar data feeds or live server infrastructure.

```
+-------------------------------------------------------------------------+
|                               UI Layer                                  |
|  +-------------------------+  +---------------------------------------+ |
|  |     OMCWS Desktop       |  |       Sector Controller Display       | |
|  | - Channel Banner        |  | - RPD Situation Canvas (2048x2048)   | |
|  | - Motif Toolbar         |  | - Tear-Off Toolbar Engine             | |
|  | - MDI Window Manager    |  | - Message Composition Area (MCA)      | |
|  | - Diagnostic Modals     |  | - DVu Tabular Flight Lists            | |
|  +-------------------------+  +---------------------------------------+ |
+-------------------------------------------------------------------------+
|                              State Layer                                |
|  +------------------+  +-------------------+  +-----------------------+ |
|  |   Window Store   |  |   Toolbar Store   |  |     System Store      | |
|  | (Z-index, bounds,|  | (FSM states,      |  | (Static ALL-UP nodes, | |
|  |  minimize, close)|  |  tear-off offsets)|  |  Channel A/B, clocks) | |
|  +------------------+  +-------------------+  +-----------------------+ |
+-------------------------------------------------------------------------+
|                              Data Layer                                 |
|  +--------------------------------------------------------------------+ |
|  | Static Mock Schemas: Processors, Services, Radar Targets, Events   | |
|  +--------------------------------------------------------------------+ |
+-------------------------------------------------------------------------+
```

---

## 2. Recommended Tech Stack

* Framework: React 18+ with TypeScript
* Bundler: Vite
* Styling: Tailwind CSS (with custom tokens for ERAM phosphor green, burnt coral, and Motif slate)
* Window Dragging & Sizing: Vanilla Pointer Events hook or `react-rnd`
* 2D Vector Rendering: HTML5 2D Canvas or SVG for the 2048x2048 R-Position Situation Display
* State Management: Zustand (lightweight store separation without boilerplate)

---

## 3. Directory Layout

A standard, maintainable project structure for `millscb/eram-gui`:

```
eram-gui/
├── README.md
├── SPECIFICATION.md
├── ARCHITECTURE.md
├── package.json
├── tsconfig.json
├── vite.config.ts
├── src/
│   ├── main.tsx
│   ├── App.tsx
│   ├── styles/
│   │   ├── theme.css
│   │   └── tokens.ts
│   ├── types/
│   │   ├── system.ts
│   │   ├── window.ts
│   │   ├── toolbar.ts
│   │   └── radar.ts
│   ├── data/
│   │   ├── mockProcessors.ts
│   │   ├── mockServices.ts
│   │   ├── mockEvents.ts
│   │   ├── mockSectors.ts
│   │   └── mockTargets.ts
│   ├── stores/
│   │   ├── useSystemStore.ts
│   │   ├── useWindowStore.ts
│   │   ├── useToolbarStore.ts
│   │   └── useMcaStore.ts
│   ├── components/
│   │   ├── common/
│   │   │   ├── Button.tsx
│   │   │   ├── StatusIndicator.tsx
│   │   │   └── GroupBox.tsx
│   │   ├── omcws/
│   │   │   ├── ChannelBanner.tsx
│   │   │   ├── MainToolbar.tsx
│   │   │   ├── MdiContainer.tsx
│   │   │   ├── WindowChrome.tsx
│   │   │   └── windows/
│   │   │       ├── SystemSummaryWindow.tsx
│   │   │       ├── ProcessorsWindow.tsx
│   │   │       ├── EventsWindow.tsx
│   │   │       ├── SurveillanceWindow.tsx
│   │   │       ├── ExternalInterfacesWindow.tsx
│   │   │       └── NetworksWindow.tsx
│   │   └── sector/
│   │       ├── SectorDisplay.tsx
│   │       ├── SituationCanvas.tsx
│   │       ├── TearOffToolbar.tsx
│   │       ├── MessageCompositionArea.tsx
│   │       └── TabularListView.tsx
```

---

## 4. TypeScript Domain Models

### 4.1 System & Processor Status Types

```typescript
export type HealthStatus = 'UP' | 'DOWN' | 'DEGRADED' | 'MANUAL_INTERVENTION' | 'DIAGNOSTIC';

export interface ProcessorNode {
  id: string;             // e.g. "SDR1", "101R", "FDP2"
  name: string;           // Display label
  category: 'CONTROL_ROOM' | 'EQUIPMENT_ROOM' | 'SOC' | 'SUPPORT';
  status: HealthStatus;
  channel: 'A' | 'B' | 'COMMON';
  ipAddress?: string;
  vlan?: string;
}

export interface SubsystemService {
  id: string;
  name: string;
  group: 'SURVEILLANCE' | 'FLIGHT_DATA' | 'WEATHER' | 'STRATEGIC_ALERT' | 'CHANNELS' | 'NATIONAL';
  status: HealthStatus;
  details?: string;
}

export interface SystemEvent {
  id: string;
  timestamp: string;      // UTC ISO format or "HH:MM:SS"
  severity: 'INFO' | 'WARN' | 'CRIT';
  subsystem: 'SURV' | 'FLTS' | 'EDSM' | 'MIPS' | 'IFPA' | 'ESRM';
  unit: string;
  message: string;
  acknowledged: boolean;
}
```

### 4.2 Window Management Types

```typescript
export interface WindowState {
  id: string;
  title: string;
  isOpen: boolean;
  isMinimized: boolean;
  x: number;
  y: number;
  width: number;
  height: number;
  zIndex: number;
  componentKey: string;
}
```

### 4.3 Radar & Flight Data Types

```typescript
export interface AircraftTarget {
  callsign: string;        // e.g., "AAL1244"
  cid: string;             // Computer ID, e.g. "042"
  assignedAltitude: string;// e.g., "350"
  actualAltitude: string;  // e.g., "334"
  altitudeStatus: 'C' | 'D' | 'L'; // Climbing, Descending, Level
  groundspeed: number;     // e.g., 480
  beaconCode: string;      // e.g., "2411"
  x: number;               // Normalized canvas X (0.0 to 1.0)
  y: number;               // Normalized canvas Y (0.0 to 1.0)
  heading: number;         // 0 to 359 degrees
  leaderLinePosition: 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8;
  historyDots: Array<{ x: number; y: number }>;
}
```

---

## 5. State Management Implementation (Zustand Stores)

### 5.1 System Store (`useSystemStore.ts`)

```typescript
import { create } from 'zustand';
import { ProcessorNode, SubsystemService, SystemEvent } from '../types/system';
import { mockProcessors, mockServices, mockEvents } from '../data/mockData';

interface SystemStore {
  channelMode: 'ACTIVE' | 'BACKUP_IN_SYNC' | 'DEGRADED';
  activeChannel: 'A' | 'B';
  softwareRelease: string;
  siteId: string;
  processors: ProcessorNode[];
  services: SubsystemService[];
  events: SystemEvent[];
  acknowledgeEvent: (id: string) => void;
  acknowledgeAllEvents: () => void;
}

export const useSystemStore = create<SystemStore>((set) => ({
  channelMode: 'ACTIVE',
  activeChannel: 'A',
  softwareRelease: 'F40PGA00',
  siteId: 'ZDC',
  processors: mockProcessors, // Pre-populated with status: 'UP'
  services: mockServices,     // Pre-populated with status: 'UP'
  events: mockEvents,
  acknowledgeEvent: (id) =>
    set((state) => ({
      events: state.events.map((e) => (e.id === id ? { ...e, acknowledged: true } : e)),
    })),
  acknowledgeAllEvents: () =>
    set((state) => ({
      events: state.events.map((e) => ({ ...e, acknowledged: true })),
    })),
}));
```

### 5.2 Window Manager Store (`useWindowStore.ts`)

```typescript
import { create } from 'zustand';
import { WindowState } from '../types/window';

interface WindowStore {
  windows: Record<string, WindowState>;
  topZIndex: number;
  openWindow: (id: string, title: string, componentKey: string, defaultBounds?: Partial<WindowState>) => void;
  closeWindow: (id: string) => void;
  minimizeWindow: (id: string) => void;
  bringToFront: (id: string) => void;
  updatePosition: (id: string, x: number, y: number) => void;
  updateSize: (id: string, width: number, height: number) => void;
}

export const useWindowStore = create<WindowStore>((set) => ({
  windows: {},
  topZIndex: 100,
  openWindow: (id, title, componentKey, defaultBounds) =>
    set((state) => {
      const nextZ = state.topZIndex + 1;
      const existing = state.windows[id];
      if (existing) {
        return {
          topZIndex: nextZ,
          windows: {
            ...state.windows,
            [id]: { ...existing, isOpen: true, isMinimized: false, zIndex: nextZ },
          },
        };
      }
      return {
        topZIndex: nextZ,
        windows: {
          ...state.windows,
          [id]: {
            id,
            title,
            componentKey,
            isOpen: true,
            isMinimized: false,
            x: defaultBounds?.x ?? 80 + Object.keys(state.windows).length * 24,
            y: defaultBounds?.y ?? 80 + Object.keys(state.windows).length * 24,
            width: defaultBounds?.width ?? 640,
            height: defaultBounds?.height ?? 420,
            zIndex: nextZ,
          },
        },
      };
    }),
  closeWindow: (id) =>
    set((state) => ({
      windows: { ...state.windows, [id]: { ...state.windows[id], isOpen: false } },
    })),
  minimizeWindow: (id) =>
    set((state) => ({
      windows: { ...state.windows, [id]: { ...state.windows[id], isMinimized: !state.windows[id].isMinimized } },
    })),
  bringToFront: (id) =>
    set((state) => {
      const nextZ = state.topZIndex + 1;
      return {
        topZIndex: nextZ,
        windows: {
          ...state.windows,
          [id]: { ...state.windows[id], zIndex: nextZ, isMinimized: false },
        },
      };
    }),
  updatePosition: (id, x, y) =>
    set((state) => ({
      windows: { ...state.windows, [id]: { ...state.windows[id], x, y } },
    })),
  updateSize: (id, width, height) =>
    set((state) => ({
      windows: { ...state.windows, [id]: { ...state.windows[id], width, height } },
    })),
}));
```

---

## 6. Static "All UP / Green" Initial Dataset

This mock dataset guarantees that all components display green operational status out-of-the-box:

```typescript
export const mockProcessors: ProcessorNode[] = [
  // Equipment Room
  { id: 'SDR1', name: 'SDRP 1', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'A' },
  { id: 'SDR2', name: 'SDRP 2', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'B' },
  { id: 'FDP1', name: 'FDP 1', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'A' },
  { id: 'FDP2', name: 'FDP 2', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'B' },
  { id: 'CPR1', name: 'CPRP 1', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'A' },
  { id: 'CPR2', name: 'CPRP 2', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'B' },
  { id: 'MCR1', name: 'MCRP 1', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'A' },
  { id: 'MCR2', name: 'MCRP 2', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'B' },
  { id: 'ECG1', name: 'ECGIP 1', category: 'EQUIPMENT_ROOM', status: 'UP', channel: 'COMMON' },

  // Control Room Sectors
  { id: '101R', name: 'Sector 01 Radar', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },
  { id: '101D', name: 'Sector 01 Data', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },
  { id: '102R', name: 'Sector 02 Radar', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },
  { id: '102D', name: 'Sector 02 Data', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },
  { id: '103R', name: 'Sector 03 Radar', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },
  { id: '103D', name: 'Sector 03 Data', category: 'CONTROL_ROOM', status: 'UP', channel: 'COMMON' },

  // SOC Consoles
  { id: '401M', name: 'OMCWS 01', category: 'SOC', status: 'UP', channel: 'A' },
  { id: '402M', name: 'OMCWS 02', category: 'SOC', status: 'UP', channel: 'B' },
  { id: '403M', name: 'OMCWS 03', category: 'SOC', status: 'UP', channel: 'A' }
];

export const mockServices: SubsystemService[] = [
  { id: 'SURV_TGT', name: 'Target Report Processing', group: 'SURVEILLANCE', status: 'UP' },
  { id: 'SURV_MRT', name: 'Multi-Radar Tracker', group: 'SURVEILLANCE', status: 'UP' },
  { id: 'SURV_ADSB', name: 'ADS-B / SBS Ingestion', group: 'SURVEILLANCE', status: 'UP' },
  { id: 'FLT_TRAJ', name: 'Trajectory Modeling Engine', group: 'FLIGHT_DATA', status: 'UP' },
  { id: 'FLT_FDM', name: 'Flight Plan Database Sync', group: 'FLIGHT_DATA', status: 'UP' },
  { id: 'WX_NEXRAD', name: 'NEXRAD Radar Processor', group: 'WEATHER', status: 'UP' },
  { id: 'WX_WARP', name: 'WARP Weather Gateway', group: 'WEATHER', status: 'UP' },
  { id: 'ALT_CA', name: 'Conflict Alert (CRAD)', group: 'STRATEGIC_ALERT', status: 'UP' },
  { id: 'ALT_MCI', name: 'Mode C Intruder Monitoring', group: 'STRATEGIC_ALERT', status: 'UP' },
  { id: 'COMM_NAP', name: 'National Application Process', group: 'NATIONAL', status: 'UP' },
  { id: 'COMM_CPDLC', name: 'Data Comm CPDLC Link', group: 'NATIONAL', status: 'UP' }
];

export const mockEvents: SystemEvent[] = [
  {
    id: 'EVT-001',
    timestamp: '16:20:12',
    severity: 'INFO',
    subsystem: 'ESRM',
    unit: 'MCRP1',
    message: 'CHANNEL A RESYNCHRONIZATION COMPLETED SUCCESSFULLY',
    acknowledged: true
  },
  {
    id: 'EVT-002',
    timestamp: '16:22:05',
    severity: 'INFO',
    subsystem: 'SURV',
    unit: 'SDRP1',
    message: 'RADAR SITE QVR CHANNEL 1 ONLINE AND ACCEPTING TARGETS',
    acknowledged: true
  },
  {
    id: 'EVT-003',
    timestamp: '16:25:40',
    severity: 'INFO',
    subsystem: 'IFPA',
    unit: 'FDP1',
    message: 'IPOP EDDS CONNECTION ESTABLISHED ON PRIMARY INTERFACE A2',
    acknowledged: false
  }
];

export const mockTargets: AircraftTarget[] = [
  {
    callsign: 'AAL1244',
    cid: '042',
    assignedAltitude: '350',
    actualAltitude: '334',
    altitudeStatus: 'C',
    groundspeed: 480,
    beaconCode: '2411',
    x: 0.42,
    y: 0.38,
    heading: 85,
    leaderLinePosition: 2,
    historyDots: [
      { x: 0.41, y: 0.38 },
      { x: 0.40, y: 0.38 },
      { x: 0.39, y: 0.39 }
    ]
  },
  {
    callsign: 'DAL892',
    cid: '115',
    assignedAltitude: '370',
    actualAltitude: '370',
    altitudeStatus: 'L',
    groundspeed: 465,
    beaconCode: '5201',
    x: 0.65,
    y: 0.55,
    heading: 260,
    leaderLinePosition: 4,
    historyDots: [
      { x: 0.66, y: 0.54 },
      { x: 0.67, y: 0.54 },
      { x: 0.68, y: 0.53 }
    ]
  },
  {
    callsign: 'UAL405',
    cid: '078',
    assignedAltitude: '240',
    actualAltitude: '280',
    altitudeStatus: 'D',
    groundspeed: 390,
    beaconCode: '1344',
    x: 0.31,
    y: 0.72,
    heading: 140,
    leaderLinePosition: 7,
    historyDots: [
      { x: 0.30, y: 0.70 },
      { x: 0.29, y: 0.69 },
      { x: 0.28, y: 0.68 }
    ]
  }
];
```
