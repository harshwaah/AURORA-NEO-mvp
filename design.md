# AURORA NEO — Design System Source of Truth

## 1. Product Overview
AURORA NEO is a neonatal monitoring dashboard for NICU workflows, paired with an ESP32 incubator prototype. It is used by clinicians (represented in UI as **Dr. Shravan**) to quickly assess ward-level status and then drill into a single bed for live telemetry.

Current product workflow:
1. **Home Screen (`index.html`)**: scan all beds in Ward A, identify priority patients.
2. **Live Monitor Screen (`monitor.html`)**: monitor Bed 04 (Akhi) in real time across vitals, trends, events, and control-state context.

Why this interface exists:
- Give fast, low-friction situational awareness.
- Reduce cognitive load via clear card hierarchy and strict visual grouping.
- Keep clinically relevant data visible without dense hospital UI clutter.

Primary use cases:
- At-a-glance ward triage (stable/watch/critical).
- Continuous live monitoring of one connected incubator.
- Tracking trends and actuator state changes over session time.

---

## 2. Design Philosophy
The implemented design language is:
- **Medical but warm**: soft cream base and rounded geometry reduce sterile harshness.
- **Calm under pressure**: muted neutrals, sparse alerts, and soft motion avoid panic-inducing visuals.
- **Information-first**: oversized metric typography and card zoning prioritize scanning speed.
- **Premium SaaS clarity**: glass nav, elevated cards, and controlled micro-interactions convey reliability.
- **Action-aware, not action-heavy**: interface shows control logic/state but does not overwhelm with many mutable controls.

The dominant UX intent is confidence + readability, not maximal data density.

---

## 3. Visual Identity
### Core palette (implemented)
- Background: `#fdf9ed`
- Surface white: `#ffffff` / `#fffbff`
- Primary mauve: `#7e5b64`
- Primary container pink: `#ffd1dc`
- Secondary steel: `#516870`
- Secondary container: `#cee6f0`
- Tertiary lavender gray: `#626374`
- Tertiary container: `#e6e6fa`
- On-surface: `#393831`
- Muted text: `#66645c`
- Outline: `#838178`, variant `#bcb9af`
- Error/critical: `#c12048`, strong critical accent `#f74b6d`

### Semantic monitoring colors
- Success/live green: `#10b981`, `#059669`, bg `#ecfdf5`
- Warning amber: `#f59e0b`, `#d97706`, bg `#fff7ed`
- Critical red: `#f74b6d`, `#c12048`, bg `#fef2f2`
- Analytics purple channel (BPM): `#7c3aed` / `#5b21b6`
- Analytics blue channel (SpO2): `#3b82f6` / `#1d4ed8`

### Surfaces, depth, effects
- Glass nav: `rgba(253,249,237,0.88~0.9)` + `blur(20px)`.
- Card shadows: subtle (e.g., `0 4px 20px rgba(0,0,0,0.03)` to `0 16px 40px rgba(126,91,100,0.1)`).
- Corner system: heavy rounding (`rounded-2xl`, `rounded-[2rem]`, pill badges/buttons).
- Gradients: accent line `linear-gradient(90deg,#7e5b64,#ffd1dc)`, hero card and radial decorative blob.
- Motion language: soft easing (`0.18s`–`0.22s` for hover, longer loops for ambient activity).

---

## 4. Typography System
### Font families
- Headline/display: **Plus Jakarta Sans** (400–800).
- Body/supporting: **Be Vietnam Pro** (300–600).
- Icons: Material Symbols Outlined.

### Hierarchy in implementation
- Hero greeting: `text-4xl` / `lg:text-5xl`, `font-extrabold`.
- Major live metric (temp): inline `font-size:9rem`.
- Primary vital numerics: `5.5rem`, `4.5rem` classes via inline style.
- Card headings: mostly `text-xs`/`text-sm`, uppercase, high tracking.
- Meta labels/captions: `text-[9px]`, `text-[10px]`, uppercase tracking for telemetry labels.
- Body: `text-sm` / `text-base` for explanatory copy.

### Styling rules
- Numeric values use tighter tracking and heavier weights.
- Unit labels are lighter and smaller than value.
- High-frequency data labels are uppercase microtext for scan grouping.
- Headline font reserved for identity, section headers, and key numeric values.

---

## 5. Layout System
### Structure
- Fixed top nav on both screens (`h-16`, z-layered).
- Home: fixed left sidebar on `lg+`; main content shifts with `lg:ml-72`.
- Monitor: centered max-width content (`max-w-[1520px]`), tabbed content areas.

### Grid rules
- Home cards: `grid-cols-1 md:grid-cols-2 xl:grid-cols-3`, `gap-5`; featured card spans 2 columns on `md+`.
- Monitor bento: `grid-cols-12`, `gap-5`; primary temp block `lg:col-span-8`, HR `lg:col-span-4`, others split by `md` spans.

### Spacing scale (observed)
- Major page padding: `px-6 lg:px-10`, `py-8`.
- Card padding: mostly `p-7` / `p-8`.
- Internal rhythm: 2, 3, 4, 5, 6, 8 spacing steps.

### Sizing philosophy
- High-priority card gets larger area + larger type.
- Related measures grouped by color channel and proximity.
- Compact cards carry either one actionable status or one metric family.

---

## 6. Navigation
### Home screen
- Top nav tabs: Dashboard (active), Analytics (placeholder), Archive (placeholder).
- Sidebar: Patient List (active), Global Alerts (placeholder), System Logs (placeholder), Settings, Support.
- Key path: **Bed 04 featured card → “Open Live Monitor” (`monitor.html`)**.
- Mobile bottom nav appears on small screens with Ward/Data/Alerts/Profile plus center FAB.

### Monitor screen
- Top tab buttons switch in-page content:
  - Monitor (`dashboard`)
  - Analytics (`analytics`)
  - Events (`history`)
  - Controls (`settings`)
- Back arrow returns to `index.html`.

Interaction behavior:
- Tab switches are JS class toggles (`.tab-content.active`, `.active-tab`).
- No route change for monitor tabs; all state is in single page runtime.

---

## 7. Screen Documentation
### Home Screen (`index.html`)
**Purpose**: Ward-level triage and entry to live bed.

**Hierarchy**:
1. Greeting + ward status sentence.
2. Featured Bed 04 live card (largest, animated, gradient surface).
3. Supporting patient cards with stable/watch/critical states.

**Components**: glass header, desktop sidebar, greeting block, accent divider, patient grid cards, mobile bottom nav, demo mode toggle.

**Interactions**:
- Hover lift on cards.
- Bed 04 CTA navigates to monitor screen.
- Demo mode writes `localStorage['aurora_demo']`.
- Greeting changes by time-of-day.

**Responsive**:
- Sidebar hidden below `lg`.
- Grid compresses from 3 → 2 → 1 columns.
- Mobile nav fixed at bottom for small viewports.

**Current limitations**:
- Most non-Bed 04 items are visual placeholders (no drill-down routes).
- No keyboard focus styling overrides beyond browser defaults.

### Live Monitor Screen (`monitor.html`)
**Purpose**: single-bed real-time telemetry + trend analysis + event context + threshold/control transparency.

**Hierarchy**:
1. Connection/system health badges in top bar.
2. Monitor tab bento with oversized live values.
3. Secondary tabs (analytics/events/controls) for depth.

**State model**:
- `DEMO_MODE=true`: synthetic sinusoidal data, immediate live status.
- `DEMO_MODE=false`: polls ESP32 `/data` every 3000ms.
- Connection transitions: “Connecting...” → “Live Monitoring” or “Connection Lost”.

**Interactions**:
- Tab switcher via buttons.
- Real-time value transitions and animated indicators.
- Event log prepends new events with animated entry.

**Current limitations**:
- Controls tab is informational/read-only.
- Temperature is intentionally simulated in UI (`SIMULATED_TEMP` oscillation) in all modes.
- Failure handling is badge/event based; no retry UX beyond interval polling.

---

## 8. Component Library
### Sidebar (desktop home)
- Anatomy: ward identity block, nav stack, primary action button, utility links, demo toggle.
- Shape: rounded container (`2rem`).
- States: active item has filled pink container.

### Top Bar (both screens)
- Fixed/glass, subtle border, identity left, utilities right.
- Monitor top bar adds connection and system health badges.

### Greeting
- Dynamic salutation based on hour.
- Large headline + short ward status sentence.

### Patient Card
- Variants: featured live, stable summary, watch oxygen alert, critical alert.
- Shared patterns: large rounding, micro-label uppercase, soft shadow, footer timestamp.

### Status Badge
- Pill-shaped with semantic color fill/text.
- Used for patient state and actuator state.

### Metric Cards (monitor tab)
- Title + backend mapping code chip + numeric display + support visualization.
- Numeric-first hierarchy with contextual thresholds/status lines.

### Graph Cards (analytics)
- Min/Max/Avg header row + sparkline area/line + placeholder text before enough samples.

### Alert/Event Items
- Dot marker + message + semantic mini badge + timestamp.
- New entries animated with fade/slide.

### Buttons
- Rounded pills/circles.
- Primary mauve fills for key actions.
- Hover opacity / scale transitions.

### Tabs
- Text buttons with active bottom border color (`#7e5b64`).

### Progress Indicators
- SpO2 horizontal fill, peltier/humidifier activity bars.

### Connection Badge
- Live (green) vs lost (amber) styling changed via inline `cssText`.

### Empty/Loading/Error states
- Loading: “Awaiting data”, “Connecting...”, placeholders on charts.
- Error/offline: “Connection Lost”, system badge switches to disconnected.

---

## 9. Dashboard Cards (Monitor)
### Heart Rate
- Data: `bpm`
- Display: large purple numeric + ECG animation + status text.
- Logic: normal 100–180, watch 80–99 or 181–200, critical <80 or >200.

### SpO2
- Data: `spo2`
- Display: blue numeric + fill bar + status line.
- Logic: stable >=95, watch 90–94, critical <90.

### Body Temperature
- Data label maps to `temp`, but rendered from simulated value around `36.8` with ±`0.12` oscillation.
- Display: largest number on page, pink temperature panel.

### Ambient Temperature
- Data: `amb`
- Neutral card style, medium emphasis.

### Humidity
- Data: `hum`
- Neutral card style with low/high annotation (55/65).

### Peltier
- Data: `peltier` string ON/OFF.
- ON: green emphasis, spinning fan icon, larger progress fill.
- OFF: gray standby styling.

### Humidifier
- Data: `humidifier` string ON/OFF.
- ON: blue emphasis and high fill; OFF standby gray.

### Analytics
- Uses last 40 readings for BPM/SpO2 trend paths and aggregate stats.

### Events
- Logs session start, connection status, parameter load, stream start, threshold alerts, actuator transitions.

### Controls
- Displays fixed firmware threshold constants and live actuator states.

---

## 10. Information Hierarchy
1. Brand/context and connection state (top bar).
2. Largest live metric zones (temperature, HR, SpO2).
3. Supporting environmental/actuator cards.
4. Analytics/event/control details behind tabs.

Eye movement is guided by:
- Scale contrast (9rem temp value).
- Color channel coding by metric family.
- Bento spans (large block anchors scan).
- Uppercase micro-labels that preframe numeric meaning.

---

## 11. Icons
- Icon set: **Google Material Symbols Outlined**.
- Default variation: outline, weight 400; filled used selectively for emphasis.
- Sizes: mostly 14–36px depending on role.
- Usage:
  - Clinical/entity: `child_care`, `clinical_notes`.
  - System/status: `sensors`, `check_circle`, `wifi_off`, `warning`.
  - Metric metaphors: `favorite`, `water_drop`, `thermostat`, `humidity_mid`, `mode_fan`.
- Conventions: semantic color pairing with adjacent badges/text.

---

## 12. Motion Design
Implemented animations:
- `card-lift`: hover translateY with shadow elevation.
- `pulse`: status/live dots and urgent timestamp pulse.
- `live-glow`: featured live card soft glow loop.
- `heartbeat`: heart icon beat loop.
- `ecg-line`: horizontal ECG travel loop.
- `spin-slow`: active peltier fan rotation.
- `chart-path`: initial stroke-draw for chart lines.
- `event-item` fade in on insertion.
- `value-transition`: smoother metric text state shifts.

Timing language:
- Micro interactions around `0.18s`–`0.22s`.
- Ambient loops 1.4s to 3s+.
- Progress widths use 700ms–1000ms transitions.

---

## 13. Design Tokens
### Colors
- See Section 3 list (core + semantic).

### Spacing
- Primary rhythm: 2, 3, 4, 5, 6, 7, 8 (Tailwind scale equivalents).
- Card padding defaults: 28px/32px style (`p-7`, `p-8`).

### Radius
- Primary large card: `2rem`.
- Secondary modules: `1rem`–`1.5rem`.
- Pill controls/badges: full radius (`9999px`).

### Shadows
- Ambient card: low alpha black or primary-tinted.
- Hover state adds stronger, soft spread shadows.

### Typography sizes (key)
- Display: 9rem (temp), 5.5rem (bpm), 4.5rem (spo2/amb/hum).
- Section/title: text-sm/text-lg.
- Labels: 9–10px uppercase tracking.

### Motion durations
- 180ms, 200ms, 220ms, 400ms, 700ms, 1000ms, periodic loops 1400ms/2000ms/3000ms.

### Z-index
- Header `z-50`, sidebar `z-40`, mobile nav fixed high, floating FAB bottom-right.

### Opacity conventions
- Muted metadata uses 0.4–0.7 alpha tones.
- Glass backgrounds use ~0.88–0.92 alpha.

---

## 14. Accessibility
Current implementation characteristics:
- Strong text contrast on light surfaces for key values.
- Large numeric typography improves readability at distance.
- Color is often paired with text labels/status words.
- Touch targets are generally generous (pill/circle buttons).

Current gaps:
- No explicit keyboard focus styling system.
- No ARIA landmarks/advanced semantic annotations for dynamic telemetry.
- No reduced-motion conditional handling for animation-heavy elements.

Guidance: preserve current visual system while adding non-visual support progressively.

---

## 15. Responsive Behavior
### Desktop (lg/xl)
- Home: fixed left sidebar + 3-column patient grid.
- Monitor: full bento proportions with top tab bar visible.

### Tablet (md)
- Home grid becomes 2 columns.
- Monitor secondary cards reorganize by md spans.

### Mobile
- Home sidebar removed, bottom nav appears.
- Home grid single column.
- Monitor top tabs hidden under `lg` (content still switchable if buttons surfaced externally).

### Large monitors
- Monitor constrained by `max-w-[1520px]` to avoid over-stretched readability.

---

## 16. Interaction Model
- **Hover**: cards/buttons elevate or tint subtly.
- **Click**: direct navigation (home↔monitor), tab reveal, demo toggle.
- **Loading**: placeholders (`--`, “Awaiting data”, chart placeholders).
- **Polling**: fetch interval every 3000ms in non-demo.
- **Refresh model**: periodic overwrite of live value nodes + rolling history arrays.
- **Offline behavior**: connection badge/system badge switch to lost/disconnected; warning event added on first failure.
- **Recovery**: next successful fetch restores live state and updates UI seamlessly.

---

## 17. Current Backend Mapping
Endpoint contract in the current implementation: `GET BASE_URL + "/data"` with `BASE_URL = "http://192.168.1.45"` (CORS), polled every `POLL_MS = 3000`.

Expected payload fields:
- `temp` (body temp)
- `hum` (humidity)
- `amb` (ambient temp)
- `bpm` (heart rate)
- `spo2` (oxygen saturation)
- `peltier` (`"ON"|"OFF"`)
- `humidifier` (`"ON"|"OFF"`)

UI mapping (exact):
- `bpm` → `#val-bpm`, status `#bpm-status`, analytics history `histData.bpm`, trend chart + min/max/avg.
- `spo2` → `#val-spo2`, `#spo2-bar`, `#spo2-status`, analytics history `histData.spo2`, trend chart + min/max/avg.
- `amb` → `#val-amb`, analytics history `histData.amb`, min/max/avg panel.
- `hum` → `#val-hum`, analytics history `histData.hum`, min/max/avg panel.
- `peltier` → `#peltier-badge`, `#peltier-bar`, `#peltier-icon` spin class, `#ctrl-peltier`, event counts.
- `humidifier` → `#humidifier-badge`, `#humidifier-bar`, `#ctrl-humidifier`, event counts.
- `temp` is documented in UI as mapping reference but **not consumed in runtime update path**; displayed temp is simulated (`SIMULATED_TEMP` + oscillation every 2s).

Error handling:
- Fetch failure triggers `setConn('lost')` and initial warning event.
- Successful fetch sets live badge and adds first-stream success event.
- Runtime tolerates missing/invalid numeric fields by rendering `--` fallback.

---

## 18. Design Consistency Rules
1. Keep base background `#fdf9ed` and soft-surface palette.
2. Preserve rounded-heavy card geometry (2rem primary).
3. Keep Plus Jakarta Sans + Be Vietnam Pro pairing.
4. Maintain uppercase micro-label pattern for metric metadata.
5. Do not replace semantic color channels (purple HR, blue SpO2, green success).
6. Preserve large-number-first hierarchy for live monitoring.
7. Keep subtle, calm motion style; avoid aggressive animation.
8. Reuse existing card modules before inventing new shells.
9. Keep tab model single-page and visually consistent.
10. Keep event semantics (info/success/warning/critical) color coded and labeled.

---

## 19. Future Design Guidelines
- Add new screens by reusing glass top-nav + card rhythm + typography stack.
- Add new monitoring cards within existing bento/grid spans and semantic channeling.
- Add new backend metrics with explicit “Mapping: <code>field</code>” label pattern.
- For new tabs/modules, follow current tab button style and `.tab-content` switching model.
- Extend analytics using same min/max/avg + sparkline card template.
- If introducing controls, keep current calm/readable control panels and avoid dense command UIs.
- Any new alert category must map to the existing event semantic system.

---

## 20. Final Design Principles (Immutable)
1. Design for clinical trust before visual novelty.
2. Keep caregiver scanning speed as the primary UX metric.
3. Prioritize readability over compactness.
4. Keep the interface calm even during critical states.
5. Use color semantically and sparingly.
6. Preserve typography hierarchy and numeric emphasis.
7. Keep layout rhythm consistent across screens.
8. Reuse existing components and interaction patterns.
9. Surface system state clearly at all times.
10. Keep motion subtle, informative, and non-distracting.
11. Ensure each metric has explicit meaning and context.
12. Avoid decorative additions that dilute medical clarity.
13. Maintain consistency between ward overview and live monitor language.
14. Keep backend-to-UI mapping transparent and stable.
15. Evolve by extension, not redesign.
