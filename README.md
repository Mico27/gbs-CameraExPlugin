# GBS Camera Ex Plugin

An engine plugin for **GB Studio 4.3.0+** that extends the stock camera.

| Feature | What it adds |
|---|---|
| **Lock onto another actor** | The camera can follow any actor in the scene, not just the player — picked from the scene or from a variable holding an actor index. |
| **Lock onto several actors** | The camera follows the *median* position of up to 8 actors, either the midpoint of their bounding box or their average position. |
| **Smooth transitions** | The camera eases towards its target instead of snapping. Changing the lock target, the camera offset or the deadzone all make the camera glide to its new position. |
| **Direct line movement** | Camera travel can follow the true straight line between two points, alongside the stock horizontal-first / vertical-first / 45° diagonal routes. |

The plugin adds **~2.0 KB of ROM**, **14 bytes of WRAM** and **no SRAM**.

---

## Events

All events appear in the stock **Camera** group.

| Event | Description |
|---|---|
| **Camera Lock On Actor** | Follow an actor picked from the scene. |
| **Camera Lock On Actor By Index** | Follow the actor at a given scene index (player = 0), so the target can come from a variable. |
| **Camera Lock On Multiple Actors** | Follow the median position of 2–4 actors, with a midpoint / average choice. |
| **Camera Add Follow Target** | Append one more actor to the follow list without clearing it — use it to go past four targets, or to build the list in a loop. Up to 8 targets are kept. |
| **Camera Lock On Player** | Clear the follow list and go back to the player. |
| **Camera Set Smoothing** | Change the smoothing speed and path without touching the lock target. |
| **Camera Move To (Extended)** | Move the camera to a position along a chosen path, with an optional re-lock once it arrives. |

Every lock event also carries the stock **Lock Axis** and **Prevent Backtracking**
fields, so it fully replaces `Camera Lock` rather than sitting beside it.

### Path modes

| Mode | Behaviour |
|---|---|
| **Direct line** | Travels along the true straight line between the two points. The minor axis advances proportionally to the major one, so a 3:1 move really looks 3:1. |
| **Diagonal** | Each axis advances by the full speed independently — a 45° move until one axis arrives, then straight. This is the stock `Camera Move To` behaviour. |
| **Horizontal first** | Finish X, then Y. |
| **Vertical first** | Finish Y, then X. |

### Smoothing

Smoothing is a **speed in pixels per frame**, or *Instant*. Instant reproduces the
stock camera exactly: every frame the camera snaps to the edge of the deadzone
around its target. With a speed set, the camera moves at most that far per frame
towards the same position, along the chosen path.

Because smoothing is applied to the *final* camera position rather than to any one
event, it covers every case at once:

- **Lock target changes** → the camera glides to the new actor.
- **Camera offset changes** (`Camera Set Property`) → the camera glides to the new offset.
- **Deadzone changes** (`Camera Set Property`) → the camera glides as the deadzone widens or narrows.

Smoothing and the follow list are **reset on every scene load**, exactly like the
stock camera deadzone. The values they reset to come from the engine settings
below, so a project-wide smooth camera needs no per-scene setup.

---

## Engine settings

Found under **Settings → Engine → Camera Ex**.

| Field | Default | Meaning |
|---|---|---|
| `Enable smooth camera transitions` | on | Compiles the eased follow code. Turn off to save ROM if you only need the extra lock targets. |
| `Enable multiple lock targets` | on | Compiles the median-of-several-actors code. Turn off to save ROM if you only ever lock onto one actor. |
| `Default smoothing speed` | 0 | Slider, shown in pixels per frame (stored as subpixels, 32 = 1 pixel). 0 = instant, the stock behaviour. Re-applied on every scene load. |
| `Default smoothing path` | Direct line | Dropdown: Direct line / Diagonal / Horizontal first / Vertical first. Re-applied on every scene load. |

---

## Engine files

The plugin overrides two stock files and adds two of its own:

```
engine/
  include/camera.h        ← override: stock content + the Camera Ex externs
  include/camera_ex.h     ← new: follow list, smoothing state, camera_ex_reset()
  include/vm_camera_ex.h  ← new: VM entry points
  src/core/camera.c       ← override: stock globals, camera_update() delegates
  src/core/camera_ex.c    ← new: follow target resolution, path stepping
  src/core/vm_camera_ex.c ← new: VM entry points
```

`camera.c` is deliberately thin — it only holds the stock globals and forwards
`camera_update()` to `camera_ex_update()`. All the logic lives in `camera_ex.c`,
which is byte-identical across every engineAlt variant.

### Compatibility with other plugins

`ScreenScrollPlugin` and `ContinuousScenePlugin` also modify `camera.c` / `camera.h`
(they widen the camera coordinates to signed and skip the update while a scene
transition is scrolling). `engineAlt` variants fold those changes in:

| Installed alongside | Variant used |
|---|---|
| — | `engine` |
| `ScreenScrollPlugin` | `engineAlt/ScreenScrollPlugin` |
| `ContinuousScenePlugin` | `engineAlt/ContinuousScenePlugin` |
| `MetaTilePlugin` | no variant needed — it does not touch `camera.c` |
| `ContinuousScenePlugin` + `MetaTilePlugin` | `engineAlt/ContinuousScenePlugin` |

`plugin.json` sets `"order": 10` so Camera Ex is applied **after** those plugins and
its `camera.c` wins. GB Studio will log that it superseded their camera patch — that
is expected, the variant already contains their changes.

---

## Example project

`CameraExPluginExample/` runs every event from the scene init script on a 40×36 tile
map with four NPCs: lock onto a single actor, glide to another in a direct line,
lock by index, follow the midpoint and then the average of several actors, change
smoothing at runtime, and move the camera along each of the four paths.

Build it with:

```bash
node "C:/Users/micka/Documents/gb-studio/out/cli/gb-studio-cli.js" make:rom CameraExPluginExample/CameraExPluginExample.gbsproj CameraExPluginExample/build/CameraExPluginExample.gb
```

---

## Development

Event files are sandboxed by GB Studio and cannot `require` a sibling module, so the
shared field definitions and compile helpers are inlined into every event file.
They are generated — edit `tools/genEvents.js` and re-run it rather than editing one
copy:

```bash
node tools/genEvents.js src/CameraExPlugin/events
```

The `engineAlt` variants differ from `engine/` only in `camera.c` and `camera.h`;
after changing `camera_ex.c`, `camera_ex.h`, `vm_camera_ex.c` or `vm_camera_ex.h`,
copy them across:

```bash
node tools/syncAlt.js
```

---

## Memory footprint

Measured against a stock GB Studio 4.3.0 build:

| | Bytes |
|---|---|
| ROM — `camera.c` (was 468 stock, now 71) | −397 |
| ROM — `camera_ex.c` | 2011 |
| ROM — `vm_camera_ex.c` | 397 |
| **ROM total added** | **2011** |
| WRAM — follow list (8) + state (4) + engine field defaults (2) | 14 |
| SRAM | 0 |

## License

MIT

---

<!-- BANK0:BEGIN -->
## Bank 0 (HOME) Usage

Bank 0 is the 16 KB non-switchable ROM bank that the GB Studio engine core,
the interrupt handlers and the GBDK runtime all share. Banked ROM is cheap
(add another bank), bank 0 is not, so it is usually the first thing a project
runs out of.

| | Bytes |
|---|---|
| Bank 0 used by this plugin | **0** |
| Bank 0 free with this plugin installed | **1,451** of 16,384 (91% used) |

**This plugin costs nothing in bank 0.** All of its code lives in a switchable
ROM bank; nothing it adds is resident in bank 0.

<details><summary>How this was measured</summary>

GB Studio 4.3.2, DMG target, default engine settings. Each module's bank 0
contribution is the `A _HOME size` record that SDCC writes into its `.rel`
object, summed over the engine sources this plugin provides. Stock sizes come
from building projects whose only plugin ships no engine C, so every module in
them is the untouched engine; two such builds were compared and agreed on all
73 shared modules.

The "free" figure is a stock project with this plugin and nothing else. Your
own number will differ: other plugins, and any engine settings that change what
the core compiles, move it independently of this plugin.

</details>
<!-- BANK0:END -->
