# NG (1.10.984) port notes — EXPERIMENTAL, untested in-game

Base: shad0wshayd3-FO4/BakaFullscreenPipboy (GPL-3.0) + vendored CommonLibF4. Modified for Fallout 4 NG.
Install target: F4SE for NG + "Address Library for F4SE Plugins" (NG, file version-1-10-984-0.bin) + MCM.

## What changed
1. `lib/commonlibf4/lib/commonlib-shared/src/REL/IDDB.cpp` (+ IDDB.h): on NG only,
   - AE-only Address Library IDs are remapped to NG RVAs found by static analysis of Fallout4.exe 1.10.984 (table `g_ngRemap`).
   - Lookups must match the ID exactly (the stock loader silently returns the next-higher ID's offset when an ID is missing).
2. `RE/M/MemoryManager.h`: on NG the MemoryManager object is at RVA 0x2e62980 (AE resolves it through a function ID that NG lacks).
3. `RE/U/UIAdvanceMenusFunctionCompleteEvent.h`: on NG, return nullptr instead of creating the event source if the game has not.
4. `src/Hooks/Hooks.h`: the two `AddMenuToPipboy<true>` call sites are 5 bytes earlier on NG (0x173, 0x312 vs 0x178, 0x317).
5. `res/commonlibf4-plugin.cpp.in`: plugin declares compatibility with 1.10.984 and 1.10.980 (and the AE version).
6. `xmake.lua`: Papyrus target removed (reuse the .pex files from the original mod download).

## Verification done (static, against your Fallout4.exe 1.10.984)
- 30 of 31 hook groups: every site is a real call and all sites of a group call the same function. 1 group fixed (item 4).
- Singleton addresses were derived from call-site tracing; method cross-checked against TaskQueueInterface::Singleton (known NG ID -> same RVA).
- UIAdvanceMenusFunctionCompleteEvent event-source slot 0x2ff18c8 and BSTGlobalEvent pointer 0x2e64a40 come from the game's own lazy-init code (EventSource vtable 0x2339fa0, verified by RTTI).

## Known risks (not verifiable without running the game)
- CommonLibF4 here uses AE struct layouts. If any class the plugin touches has a different layout on NG, expect wrong behaviour or crashes.
- Event-source pointer address is inferred from startup init code.

## Build
Push this folder to a GitHub repo, run the "Build NG DLL" workflow (Actions tab), download the artifact.
Copy the DLL over Data/F4SE/Plugins/BakaFullscreenPipboy.dll of the original 2.1.0 install (keep its other files).
Logs: Documents\My Games\Fallout4\F4SE\BakaFullscreenPipboy.log


## Diagnostic build
Logs lines starting with NGDIAG in BakaFullscreenPipboy.log.
Flag files (create empty, in Data\F4SE\Plugins\): `BakaFullscreenPipboy.skipinstall` skips Hooks::Install; `BakaFullscreenPipboy.nosink` skips the per-frame event listener.


Change log: v-diag2 corrected the event-source slot (earlier value 0x2ff1938 belonged to a different singleton and made RegisterSink spin).

## Crash recorder (diagnostic)
src/CrashDiag.cpp installs a vectored exception handler. On a serious exception (access violation etc.) it appends the faulting
module+offset and a call stack to Data\F4SE\Plugins\BakaFullscreenPipboy_crash.log. Remove before a final release.
