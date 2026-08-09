# gbs-CameraExPlugin

**Version 1.0.0 — Requires GB Studio ≥ 4.3.0**

An engine plugin that extends the stock GB Studio camera.

| Feature | What it adds |
|---|---|
| **Lock onto another actor** | The camera can follow any actor in the scene, not just the player — picked from the scene or from a variable holding an actor index. |
| **Lock onto several actors** | The camera follows the *median* position of up to 8 actors, either the midpoint of their bounding box or their average position. |
| **Smooth transitions** | The camera eases towards its target instead of snapping. Changing the lock target, the camera offset or the deadzone all make the camera glide to its new position. |
| **Direct line movement** | Camera travel can follow the true straight line between two points, alongside the stock horizontal-first / vertical-first / 45° diagonal routes. |


https://github.com/user-attachments/assets/d4908bc2-fdb5-4a8c-b5f6-5edab3a762cc


---

## Table of Contents

1. [Concepts](#concepts)
2. [Project Setup](#project-setup)
3. [Engine Settings](#engine-settings)
4. [Size Limits and Restrictions](#size-limits-and-restrictions)
5. [Events Reference](#events-reference)
6. [Media](#media)
7. [Memory Footprint](#memory-footprint)
8. [License](#license)

---

## Concepts

### Path modes

| Mode | Behaviour |
|---|---|
| **Direct line** | Travels along the true straight line between the two points. The minor axis advances proportionally to the major one, so a 3:1 move really looks 3:1. |
| **Diagonal** | Each axis advances by the full speed independently — a 45° move until one axis arrives, then straight. This is the stock *Camera Move To* behaviour. |
| **Horizontal first** | Finish X, then Y. |
| **Vertical first** | Finish Y, then X. |

### Smoothing

Smoothing is a **speed in pixels per frame**, or *Instant*. Instant reproduces the stock camera exactly: every frame the camera snaps to the edge of the deadzone around its target. With a speed set, the camera moves at most that far per frame towards the same position, along the chosen path.

Because smoothing is applied to the *final* camera position rather than to any one event, it covers every case at once:

- **Lock target changes** → the camera glides to the new actor.
- **Camera offset changes** (*Camera Set Property*) → the camera glides to the new offset.
- **Deadzone changes** (*Camera Set Property*) → the camera glides as the deadzone widens or narrows.

### Follow list

Locking onto multiple actors builds a follow list of up to 8 targets. *Camera Lock On Multiple Actors* replaces the list; *Camera Add Follow Target* appends to it, which is how you go past four targets or build the list in a loop. *Camera Lock On Player* clears it.

---

## Project Setup

1. For a project-wide smooth camera, set **Default smoothing speed** and **Default smoothing path** under [Engine Settings](#engine-settings) — no per-scene setup is then needed.
2. To follow something other than the player, call one of the **Camera Lock On …** events from any script.
3. Adjust smoothing at runtime with **Camera Set Smoothing**, or move the camera manually with **Camera Move To (Extended)**.

Smoothing and the follow list are **reset on every scene load**, exactly like the stock camera deadzone. The values they reset to come from the engine settings.

---

## Engine Settings

Found under **Settings → Engine → Camera Ex**.

| Setting | Default | Description |
|---|---|---|
| **Enable smooth camera transitions** | on | Compiles the eased follow code. Turn off to save ROM if you only need the extra lock targets. |
| **Enable multiple lock targets** | on | Compiles the median-of-several-actors code. Turn off to save ROM if you only ever lock onto one actor. |
| **Default smoothing speed** | 0 | Slider in pixels per frame. 0 = instant, the stock behaviour. Re-applied on every scene load. |
| **Default smoothing path** | Direct line | Direct line / Diagonal / Horizontal first / Vertical first. Re-applied on every scene load. |

---

## Size Limits and Restrictions

- The follow list holds a maximum of **8 targets**; further *Camera Add Follow Target* calls are ignored.
- **Camera Lock On Multiple Actors** takes 2–4 actors from its own fields; use *Camera Add Follow Target* to go beyond four.
- Smoothing and the follow list **reset on every scene load** — re-apply them in the scene's init script if a scene needs different values from the engine defaults.
- This plugin also modifies the stock camera, as do **ScreenScrollPlugin** and **ContinuousScenePlugin**. Compatible variants are included and selected automatically when those plugins are installed alongside it; GB Studio will log that Camera Ex superseded their camera patch, which is expected. **MetaTilePlugin** needs no variant.

---

## Events Reference

All events appear in the stock **Camera** group. Every lock event also carries the stock **Lock Axis** and **Prevent Backtracking** fields, so it fully replaces *Camera Lock* rather than sitting beside it.

| Event | Description |
|---|---|
| **Camera Lock On Actor** | Follow an actor picked from the scene. |
| **Camera Lock On Actor By Index** | Follow the actor at a given scene index (player = 0), so the target can come from a variable. |
| **Camera Lock On Multiple Actors** | Follow the median position of 2–4 actors, with a midpoint / average choice. |
| **Camera Add Follow Target** | Append one more actor to the follow list without clearing it. Up to 8 targets are kept. |
| **Camera Lock On Player** | Clear the follow list and go back to the player. |
| **Camera Set Smoothing** | Change the smoothing speed and path without touching the lock target. |
| **Camera Move To (Extended)** | Move the camera to a position along a chosen path, with an optional re-lock once it arrives. |

---

## Media

An example project is included. It runs every event from the scene init script on a 40×36 tile map with four NPCs: lock onto a single actor, glide to another in a direct line, lock by index, follow the midpoint and then the average of several actors, change smoothing at runtime, and move the camera along each of the four paths.

---

<!-- SETTINGCOST:BEGIN -->
### What each engine setting costs

Every setting here changes what gets compiled. Figures are what you **get back by
turning the setting off**; rows marked *off by default* show what turning it **on**
costs instead, and sliders show the cost per step. A dash means that budget does not
move.

| Setting | Bank 0 | WRAM | Banked ROM |
|---|---|---|---|
| Enable smooth camera transitions | — | — | **39 B** |
| Enable multiple lock targets | — | — | **686 B** |

Turning off every on-by-default switch above frees **725 B** of banked ROM — the full
span between this plugin at its fullest and stripped to nothing. Treat it as a
ceiling rather than a recipe: you keep whatever your game actually uses.

<details><summary>How these were measured</summary>

GB Studio 4.3.0-e1. This plugin's `engine/src/**/*.c` was compiled with the
toolchain and flags GB Studio itself uses (`lcc -msm83:gb -Wf--max-allocs-per-node 3000
-DHUGE_TRACKER -DRUMBLE_ENABLE=0x08u`) against a merged include tree, and the SDCC object
files' area records were read: `_HOME` is bank 0, `_DATA`/`_INITIALIZED`/`_BSS` are WRAM,
and `_CODE*`/`_CONST`/`_LIT`/`_INITIALIZER` are banked ROM.

Two caveats. Only this plugin's own engine sources are measured, so a setting that also
changes a struct shared with stock engine files can move a few more bytes in files the
plugin does not ship. And each setting is toggled on its own: a handful measure slightly
*negative* because enabling their code lets the compiler drop a fallback path elsewhere,
and settings that gate other settings only show their own contribution.

</details>
<!-- SETTINGCOST:END -->

## Memory Footprint

Measured against a stock GB Studio 4.3.0 build:

| | Cost |
|---|---|
| WRAM | +14 bytes (follow list 8, state 4, engine field defaults 2) |
| ROM | +2,011 bytes |
| SRAM | 0 bytes |

Turning off either engine setting reduces the ROM cost.

---

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

## Changelog

Grouped by the date each change was merged into the official
[gb-studio-plugins](https://github.com/gb-studio-dev/gb-studio-plugins) repository.

Only bug fixes, new features and feature changes are listed. Engine version
bumps, patch regeneration, packaging fixes and documentation edits are omitted.

### 2026-08-02

- Initial release: lock the camera onto any actor or onto several actors at once, smooth eased following, and direct-line camera paths.
