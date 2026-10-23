# Signatouch AM62L — Production Flash Station

GUI front-end for the DFU flashing sequence that `windows-flash.ps1` runs from the
command line. Same eight steps, same images, same order — with checks, timeouts and
traceability added.

| File | What it is |
| --- | --- |
| `Flash Station.bat` | **What the operator double-clicks.** |
| `FlashStation.ps1` | The application. |
| `flash-station.config.json.sample` | Optional settings; copy to `flash-station.config.json` to use. |
| `image-manifest.json` | Approved SHA-256 of each image. Created by the **Create/refresh manifest** button. |
| `bin\images\version.txt` | Which build these images are. Read at start-up, shown on the banner, recorded per unit. |
| `logs\` | One `.log` per unit, plus `flash-results.csv`. Created automatically. |

Nothing needs installing. Windows PowerShell 5.1 (in the box on Windows 10/11) is enough.

---

## Setting up a station (engineering, once)

1. Copy the whole `IR` folder to a **local** drive on the station, e.g. `C:\FlashStation`.
   See *Do not run from OneDrive* below.
2. Install the WinUSB driver with `zadig-2.9.exe` — same as today, this hasn't changed.
3. Stage the correct `bin\loader\*` and `bin\images\*` files for the build being run,
   and update `bin\images\version.txt` to match.
4. Launch `Flash Station.bat` and press **Create/refresh manifest**.
   This records the SHA-256 of the staged images as the approved set. From then on the
   app refuses to flash if any image is missing, truncated, or has been swapped.
5. Optionally copy `flash-station.config.json.sample` → `flash-station.config.json`
   and set `StationName`, timeouts, and whether a serial number is mandatory.
6. Right-click `Flash Station.bat` → *Send to* → *Desktop (create shortcut)*.

Repeat step 4 whenever the released image changes — otherwise every unit gets a
"HASH MISMATCH" and the station correctly refuses to run.

## Fastboot flashing (v1.2.0)

The station now writes the eMMC with `fastboot.exe` instead of eight DFU transfers. Steps 1-3 still DFU-boot
the board, but step 3 loads `bin\loader\u-boot-flasher.img` (a U-Boot built with a fastboot gadget; it runs
from RAM and never goes on a unit) whose bootcmd sets the eMMC boot config and runs `fastboot usb 0`.
There is no uEnv.txt stage any more. Then:

| Step | fastboot command | Writes |
|---|---|---|
| 4 | `fastboot flash mmc0boot0 bin\images\boot0.img` | eMMC boot0: tiboot3 + tispl + u-boot.img from the build, one file |
| 5 | `fastboot flash mmc0 bin\images\emmc-<product>.simg` | the whole eMMC user area from the sparse image |
| 6 | `fastboot reboot` | boots the new image (non-critical) |

Why: U-Boot's DFU moves data in 4 KB control transfers, about 6 MB/s on this board, 107 s for the 627 MB
image. Fastboot uses bulk transfers, and the sparse image leaves out every empty block, so about half the
bytes are sent and none of the empty ones are written. Measured over WSL/usbipd, which adds seconds to every
re-enumeration: 48 s for the whole flash with the uEnv stage, versus 2 min 15 s before.

Setup, once per station PC:

1. Put `fastboot.exe`, `AdbWinApi.dll` and `AdbWinUsbApi.dll` from Google's Android platform-tools next to
   `FlashStation.ps1` (https://dl.google.com/android/repository/platform-tools-latest-windows.zip).
2. Driver. Windows has no driver for the board's USB gadget (0451:6165, the same VID:PID in every stage:
   ROM DFU, SPL DFU, U-Boot DFU and fastboot). dfu-util needs WinUSB on it; fastboot.exe additionally
   only enumerates WinUSB devices registered under Android's interface GUID
   `{F72FE0D4-CBCB-407D-8814-9ED673D0DD6B}`, which Zadig's dialog cannot set. So, on a PC that has
   never had Zadig bind this device:
   - plug the board in (any stage; USB boot mode is fine), open Zadig as admin, Options > List All
     Devices, Device > Load Preset Device... > `zadig-am62l-fastboot.cfg` (carries the GUID), driver
     target WinUSB, Install Driver;
   - then, with the board in fastboot mode (run the station once; it stops at step 4), check in an admin
     PowerShell: `.\Fix-FastbootDriver.ps1`. It prints the registered GUID per device instance, writes
     the Android GUID where it is missing, restarts the device and runs `fastboot devices`, which must
     list the board. Run it again whenever `fastboot devices` is empty but DFU works - Windows keeps
     the registration per device instance, so a board on a different USB port of the same PC is a new
     instance and may need it once more.
   On a PC where Zadig already bound plain WinUSB (every station before v1.2.0) only the second bullet
   is needed; that is what fixed the first station.
3. `bin\images\version.txt` names the sparse image: `[signatouch_image_name]="emmc-signatouch.simg"`.
   With `"FlashMethod": "dfu"` in `flash-station.config.json` the original sequence is used and the raw
   `.wic` of the same name must be present instead.
4. Press "Create/refresh manifest" after staging (or let `flash-tools/stage-station.sh` write it): the
   hashed file set is now tiboot3, tispl, u-boot-flasher.img, boot0.img and the .simg.

How to tell which problem you have: `dfu-util -l` lists the board but `fastboot devices` is blank means
the GUID (bullet 2, second part); neither lists it means no WinUSB binding at all (Zadig); the station
reaching step 4 at all proves the DFU side.

The previous script is kept as `FlashStation-1.1.0-dfu.ps1`.

## Operator instructions

1. Type your **Operator ID** (required by default).
2. Scan or type the **unit serial** if your work order calls for one.
3. Connect the board in DFU mode.
4. Press **START** (or `F5`; a barcode scanner's Enter key also starts the run).
5. Watch the banner:
   - **blue FLASHING…** — in progress, do not unplug.
   - **green PASS** — done, disconnect and fit the next board.
   - **amber PASS (with warnings)** — the unit programmed, but something was odd. Report it.
   - **red FAIL** — tag the unit, set it aside, tell your supervisor if it repeats.
6. **Repeat mode** flashes continuously: after a PASS, remove the board and the next one
   starts automatically as soon as it enumerates.

`Esc` aborts. An aborted board is in an unknown state and must be re-flashed.

---

## Image version (`bin\images\version.txt`)

Engineering edits this file when a new build is staged. The station reads it at
start-up and on **Re-check images**, and the version appears in four places:

- on the right-hand end of the status banner, as **image ver 1.0** — impossible to
  miss while a board is being flashed;
- bottom right, alongside the image-set fingerprint and manifest state;
- in the header of every per-unit log file;
- in the `ImageVersion` column of `flash-results.csv`.

The current file works as-is:

```
#version 1.0 is the initial release
[image]="1.0"
```

**The format**, if you want to extend it — lines starting with `#` are comments and
are echoed into the log as notes. Everything else is a key/value pair, and any of
these shapes is accepted:

```
[image]="1.0"          <- what you are using now
image = 1.0
release: 2026.08
build = 4417
```

The version reported is the `image` key. If there is no `image` key it falls back to
`version`, `release`, then `build`, and finally to the first value in the file.
**Every** key found is written to the unit log, so extra fields are free — this is a
perfectly good version.txt:

```
# Signatouch AM62L production image
# Cut from release/1.0 by engineering, 2026-08-28
[image]="1.0"
[kernel]="6.1.83-ti"
[yocto]="scarthgap"
[built]="2026-08-28"
[jira]="SIG-412"
```

If `version.txt` is missing or has no readable version, the station still runs but
logs a warning and displays **image ver ?** — an unidentifiable build is flagged,
not silently accepted.

Note that `version.txt` is *not* part of the image-set fingerprint, because it is not
flashed to the board. `ImageVersion` is what engineering *declares*; `ImageSet` is the
hash of what was *actually written*. Keeping them separate means an edited version.txt
can never disguise a changed binary.

---

## What it checks (the "warnings" you asked about — it is both timeouts *and* checks)

**Before any board is touched (pre-flight, at launch and on "Re-check images"):**

- `dfu-util.exe` is present.
- Every image in the recipe exists and is non-zero.
- Every image's SHA-256 matches `image-manifest.json`; size is cross-checked too.
- No manifest, or an image missing from it → loud warning telling engineering to act.
- `dfu-util -l` actually responds — i.e. the WinUSB/Zadig driver is working.
- Free disk space for logs.

If pre-flight fails, **START is disabled**. The station cannot flash a bad image set.

**During the run, per step:**

| Check | Default | Behaviour |
| --- | --- | --- |
| Wait for the DFU interface to enumerate | 90 s (180 s on step 1) | Timeout → FAIL, naming the interface and pointing at cable / power / boot switches / Zadig |
| Transfer duration | 60–180 s, 2700 s for `emmc.wic` | Timeout → the `dfu-util` process is **killed** and the unit FAILs |
| More than one DFU device on the PC | — | FAIL immediately (prevents flashing the wrong board) |
| `dfu-util -l` stops responding | 10 s | FAIL |
| `dfu-util` confirmed the download | — | No "File downloaded successfully" → FAIL |
| `dfu-util` exit code | — | Non-zero *after* a confirmed download → recorded as a **warning**, not a failure (see below) |

Every step must confirm its own download. There is no path through the code that
reports PASS without all eight steps confirming.

### Two real bugs in `windows-flash.ps1` that this fixes

1. **It always reported success.** `$completeCount--` inside `Flash-One` wrote to a
   *function-local* copy of the variable, so failures never decremented it. The trailing
   `$completeCount++` therefore always made the total ≥ 1 and printed
   `Flash compelte.` even when every single step had timed out.
2. **`u-boot` matched `u-boot.img`.** `$dfuOutput -match $label` is an unanchored
   substring match, so the step waiting for alt `u-boot` was satisfied by the
   `u-boot.img` interface from the earlier stage. The GUI parses the `name="…"` fields
   and compares them exactly.

It also checks `dfu-util`'s result, which the original script never looked at at all.

### Why a non-zero exit code alone is not a failure

With `-R`, `dfu-util` resets the USB device after writing. On Windows/WinUSB that reset
frequently reports an error *after* the data has already been transferred correctly.
Failing on exit code alone would scrap good boards. So the rule is:

- **No "File downloaded successfully" → FAIL.** The data did not land.
- **Success confirmed but exit code non-zero → PASS with a warning**, logged with the
  exit code and shown in the amber banner, so a station that starts doing this
  constantly gets noticed.

---

## Traceability

`logs\<timestamp>_<serial>.log` — full transcript per unit, including `dfu-util` output.

`logs\flash-results.csv` — append-only, one row per unit, safe to open in Excel:

```
Timestamp, Station, Operator, WorkOrder, Serial, DfuSerial, UsbPath, Result,
DurationSec, FailedStep, Reason, ImageVersion, ImageSet, AppVersion, LogFile
```

If an existing `flash-results.csv` was written by an older build with different
columns, it is renamed to `flash-results-archived-<timestamp>.csv` and a fresh file
is started, rather than appending misaligned rows to it.

`ImageSet` is a 12-character fingerprint of the exact set of image files used, so a
field failure can be traced back to precisely what was loaded.

---

## Configuration

Copy `flash-station.config.json.sample` to `flash-station.config.json`. Any key you
leave out keeps its built-in default.

| Key | Default | Meaning |
| --- | --- | --- |
| `StationName` | computer name | Shown in the header, recorded in the CSV |
| `RequireOperatorId` | `true` | Block START until an operator ID is entered |
| `RequireSerial` | `false` | Block START until a serial is scanned |
| `VerifyHashes` | `true` | SHA-256 every image at pre-flight |
| `BeepOnResult` | `true` | Sound on PASS / FAIL |
| `Steps[]` | see sample | Per-step `WaitSec`, `XferSec`, `PostDelaySec`, `Critical` |

Steps are matched on `Alt` + `File`. **Use forward slashes** (`bin/loader/tiboot3.bin`) —
they are normalised internally, and it avoids JSON's backslash-escaping trap.
Setting `Critical: false` on a step lets the run continue past a failure there and
finish as PASS-with-warnings; everything is critical by default.

---

## Do not run from OneDrive

This folder currently sits under `Harvia Group\Harvia US - …`, which is OneDrive-synced.
For a production station, copy it to a local path such as `C:\FlashStation`:

- A synced folder can turn images into cloud placeholders, so a 1.3 GB `emmc.wic` read
  may stall or fail mid-flash.
- Sync can change images underneath a running station with no change control.
- `logs\flash-results.csv` is append-only and will generate sync conflict copies.

Keep the OneDrive copy as the master that engineering publishes from, and copy a
release down to each station.

## Troubleshooting

| Symptom | Cause |
| --- | --- |
| `dfu-util -l did not respond` at pre-flight | WinUSB driver not bound — re-run Zadig |
| `Timed out … waiting for … 'bootloader'` | Board not in DFU mode, no power, wrong USB port/cable, or boot switches wrong |
| `HASH MISMATCH` | Staged image is not the approved one. Do not flash. Escalate to engineering |
| Banner shows `image ver ?` | `bin\images\version.txt` is missing or has no readable version field |
| `2 DFU devices are connected` | Another board is plugged into this PC — only one at a time |
| `Transfer … exceeded its … limit` | Board dropped off the USB bus mid-write; usually a cable or power issue |
| Nothing happens on double-click | PowerShell script execution blocked by policy — the `.bat` already passes `-ExecutionPolicy Bypass`; if IT blocks that, they must allow it |
