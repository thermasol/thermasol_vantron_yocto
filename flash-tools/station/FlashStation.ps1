<#
.SYNOPSIS
    Signatouch AM62L Production Flash Station (GUI)

.DESCRIPTION
    Windows Forms front-end for the DFU flashing sequence that windows-flash.ps1
    performs from the command line. Intended for manufacturing operators:

      * Startup PRODUCT MENU - SignaTouch vs SignaTouch IR. The flash sequence
        is identical; only the eMMC image differs. bin\images\version.txt names
        each product's image file and version. Each product keeps its own logs
        folder, results CSV, image manifest and shift counters.
      * One big START button, one big PASS / FAIL banner.
      * Pre-flight verification of dfu-util.exe and every binary image
        (existence, size, SHA-256 against the product's image manifest).
      * Per-step "wait for device" timeout AND per-step transfer timeout,
        plus dfu-util result checking. Any failure stops the run and reports
        FAIL - a run can never be silently reported as good.
      * Live progress, per-step status list, scrolling colour-coded log.
      * Per-unit log file + append-only CSV traveller for traceability.
      * Shift counters, operator / work-order / serial capture, repeat mode.

.NOTES
    Run with:  "Flash Station.bat"   (recommended - sets ExecutionPolicy + STA)
    or:        powershell.exe -STA -ExecutionPolicy Bypass -File .\FlashStation.ps1

    -Product 'signatouch' or 'signatouch-ir' skips the startup product menu
    (useful for a station dedicated to one product).
#>

param(
    [string]$ConfigFile = '',
    [string]$Product    = ''
)

$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
[System.Windows.Forms.Application]::EnableVisualStyles()

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------
if ($PSScriptRoot) { $Root = $PSScriptRoot }
else               { $Root = Split-Path -Parent $MyInvocation.MyCommand.Definition }

$DfuExe       = Join-Path $Root 'dfu-util.exe'
$FastbootExe  = Join-Path $Root 'fastboot.exe'      # Android platform-tools: fastboot.exe + AdbWinApi.dll + AdbWinUsbApi.dll
$LogRoot      = Join-Path $Root 'logs'
$VersionFile  = Join-Path $Root 'bin\images\version.txt'
$ConfigPath   = if ($ConfigFile) { $ConfigFile } else { Join-Path $Root 'flash-station.config.json' }

# Product-dependent paths - assigned by Set-Product once a product is chosen.
$LogDir       = $null
$ResultsCsv   = $null
$ManifestFile = $null

$AppVersion = '1.2.0'

# ---------------------------------------------------------------------------
# Products - the flash sequence is identical, only the eMMC image differs.
# bin\images\version.txt names each product's image and version:
#
#   [signatouch_IR_image_name]="emmc-signatouch-ir.simg"
#   [signatouch_IR_image_version]="1.0"
#   [signatouch_image_name]="emmc-signatouch.simg"
#   [signatouch_image_version]="1.0"
#
# The image is the .wic as an Android sparse file (fastboot path). With
# FlashMethod "dfu" in the config the raw .wic of the same name is used.
#
# Each product keeps its own logs folder, results CSV, image manifest and
# shift counters, so the two production travellers never mix.
# ---------------------------------------------------------------------------
$Products = @(
    @{ Id         = 'signatouch-ir'
       Display    = 'SignaTouch IR'
       NameKey    = 'signatouch_IR_image_name'
       VersionKey = 'signatouch_IR_image_version'
       LogSubdir  = 'SignaTouch-IR'
       Manifest   = 'image-manifest-signatouch-ir.json'
       Accent     = [System.Drawing.Color]::FromArgb(20, 110, 190) }   # blue
    @{ Id         = 'signatouch'
       Display    = 'SignaTouch'
       NameKey    = 'signatouch_image_name'
       VersionKey = 'signatouch_image_version'
       LogSubdir  = 'SignaTouch'
       Manifest   = 'image-manifest-signatouch.json'
       Accent     = [System.Drawing.Color]::FromArgb(106, 64, 176) }   # purple
)
$CurrentProduct = $null

# ---------------------------------------------------------------------------
# Flash recipe - mirrors windows-flash.ps1 step for step.
#
#   Tool         = 'dfu' (dfu-util, the default) or 'fastboot' (fastboot.exe)
#   Alt          = dfu: alt-setting name to wait for, and to flash to
#                  fastboot: partition name for "fastboot flash" (mmc0boot0 =
#                  eMMC boot0, mmc0 = eMMC user area); Command='reboot' steps
#                  have no Alt/File
#   File         = image, relative to this folder
#   Role         = 'image' marks the step whose File is the product's eMMC
#                  image named in version.txt (replaced at pre-flight)
#   Reboot       = dfu: pass -R (board detaches / resets after the transfer)
#   Critical     = a failure here fails the unit
#   WaitSec      = how long to wait for that alt-setting to enumerate
#   XferSec      = how long the transfer itself may run before it is killed
#   PostDelaySec = settle time after the step (the Start-Sleep 5 in the script)
# ---------------------------------------------------------------------------
# Fastboot flash (the default). Steps 1-3 DFU-boot a fastboot-capable U-Boot
# (bin\loader\u-boot-flasher.img, the u-boot-vt-flasher Yocto recipe; it never
# goes on a unit) whose bootcmd sets the eMMC boot partition config and runs
# "fastboot usb 0". The eMMC is then written in two fastboot commands: boot0 as
# one file, and the whole user area from the sparse image, which skips every
# zero block and moves over bulk USB instead of DFU's 4 KB control transfers.
$FastbootSteps = @(
    @{ Id=1; Title='ROM boot   - R5 SPL (tiboot3.bin)';          Tool='dfu';      Alt='bootloader'; File='bin\loader\tiboot3.bin';        Reboot=$true;  Critical=$true; WaitSec=180; XferSec=120; PostDelaySec=5 }
    @{ Id=2; Title='SPL boot   - A53 SPL (tispl.bin)';           Tool='dfu';      Alt='bootloader'; File='bin\loader\tispl.bin';          Reboot=$true;  Critical=$true; WaitSec=90;  XferSec=120; PostDelaySec=0 }
    @{ Id=3; Title='SPL boot   - flasher U-Boot (fastboot)';     Tool='dfu';      Alt='u-boot.img'; File='bin\loader\u-boot-flasher.img'; Reboot=$true;  Critical=$true; WaitSec=90;  XferSec=120; PostDelaySec=0 }
    @{ Id=4; Title='eMMC boot0 - tiboot3 + tispl + u-boot';      Tool='fastboot'; Alt='mmc0boot0';  File='bin\images\boot0.img';          Reboot=$false; Critical=$true; WaitSec=90;  XferSec=120; PostDelaySec=0 }
    @{ Id=5; Title='eMMC user  - (set by product)';              Tool='fastboot'; Alt='mmc0';       File='bin\images\emmc.simg'; Role='image'; Reboot=$false; Critical=$true; WaitSec=90; XferSec=900; PostDelaySec=0 }
    @{ Id=6; Title='Reboot into the new image';                  Tool='fastboot'; Alt='';           File=''; Command='reboot';          Reboot=$false; Critical=$false; WaitSec=30;  XferSec=30;  PostDelaySec=0 }
)
# The original all-DFU sequence, selected with "FlashMethod": "dfu" in
# flash-station.config.json. Same result, ~2 minutes slower per board.
$DfuSteps = @(
    @{ Id=1; Title='ROM boot   - R5 SPL (tiboot3.bin)';   Tool='dfu'; Alt='bootloader'; File='bin\loader\tiboot3.bin'; Reboot=$true;  Critical=$true; WaitSec=180; XferSec=120;  PostDelaySec=5 }
    @{ Id=2; Title='SPL boot   - A53 SPL (tispl.bin)';    Tool='dfu'; Alt='bootloader'; File='bin\loader\tispl.bin';   Reboot=$true;  Critical=$true; WaitSec=90;  XferSec=120;  PostDelaySec=0 }
    @{ Id=3; Title='SPL boot   - U-Boot (u-boot.img)';    Tool='dfu'; Alt='u-boot.img'; File='bin\loader\u-boot.img';  Reboot=$true;  Critical=$true; WaitSec=90;  XferSec=120;  PostDelaySec=0 }
    @{ Id=4; Title='SPL boot   - U-Boot env (uEnv.txt)';  Tool='dfu'; Alt='uEnv.txt';   File='bin\loader\uEnv.txt';    Reboot=$true;  Critical=$true; WaitSec=90;  XferSec=60;   PostDelaySec=0 }
    @{ Id=5; Title='eMMC boot0 - tiboot3';                Tool='dfu'; Alt='tiboot3';    File='bin\loader\tiboot3.bin'; Reboot=$false; Critical=$true; WaitSec=120; XferSec=120;  PostDelaySec=0 }
    @{ Id=6; Title='eMMC boot0 - tispl';                  Tool='dfu'; Alt='tispl';      File='bin\loader\tispl.bin';   Reboot=$false; Critical=$true; WaitSec=90;  XferSec=120;  PostDelaySec=0 }
    @{ Id=7; Title='eMMC boot0 - u-boot';                 Tool='dfu'; Alt='u-boot';     File='bin\images\u-boot.img';  Reboot=$false; Critical=$true; WaitSec=90;  XferSec=180;  PostDelaySec=0 }
    @{ Id=8; Title='eMMC user  - (set by product)';       Tool='dfu'; Alt='emmc';       File='bin\images\emmc.wic'; Role='image'; Reboot=$false; Critical=$true; WaitSec=90;  XferSec=2700; PostDelaySec=0 }
)
$DefaultSteps = $FastbootSteps
# The Role='image' step's File above is a placeholder so a config override
# still matches on alt+file; pre-flight replaces it with the product's image
# named in version.txt before any image checks run.

# ---------------------------------------------------------------------------
# Config (optional flash-station.config.json overrides)
# ---------------------------------------------------------------------------
$Config = [ordered]@{
    StationName         = $env:COMPUTERNAME
    ProductName         = 'Signatouch AM62L'
    RequireOperatorId   = $true
    RequireSerial       = $false
    VerifyHashes        = $true
    BeepOnResult        = $true
    PollIntervalMs      = 250
    DeviceListTimeoutMs = 10000
    FlashMethod         = 'fastboot'   # 'fastboot' (default) or 'dfu' (the original sequence)
}

if (Test-Path $ConfigPath) {
    try {
        $raw = Get-Content -Raw -Path $ConfigPath | ConvertFrom-Json
        foreach ($p in $raw.PSObject.Properties) {
            if ($Config.Contains($p.Name)) { $Config[$p.Name] = $p.Value }
        }
        if ([string]$Config.FlashMethod -eq 'dfu') { $DefaultSteps = $DfuSteps }
        if (($raw.PSObject.Properties.Name -contains 'Steps') -and $raw.Steps) {
            # Key on alt + path with the separators normalised, so a config may use
            # "bin/loader/tiboot3.bin" and dodge JSON backslash escaping entirely.
            $norm = { param($a, $f) ("{0}|{1}" -f $a, ([string]$f -replace '[\\/]+', '/')).ToLowerInvariant() }
            $override = @{}
            foreach ($s in $raw.Steps) { $override[(& $norm $s.Alt $s.File)] = $s }
            foreach ($step in $DefaultSteps) {
                $k = & $norm $step.Alt $step.File
                if ($override.ContainsKey($k)) {
                    $o = $override[$k]
                    foreach ($f in 'WaitSec','XferSec','PostDelaySec','Critical') {
                        if ($o.PSObject.Properties.Name -contains $f) { $step[$f] = $o.$f }
                    }
                }
            }
        }
    } catch {
        [void][System.Windows.Forms.MessageBox]::Show(
            ("{0} could not be read, so the BUILT-IN DEFAULTS are being used.`r`n`r`nCheck the JSON syntax - remember that Windows paths need forward slashes (bin/loader/tiboot3.bin) or doubled backslashes.`r`n`r`n{1}" -f (Split-Path -Leaf $ConfigPath), $_.Exception.Message),
            'Configuration warning', 'OK', 'Warning')
    }
}

# ---------------------------------------------------------------------------
# Run state
# ---------------------------------------------------------------------------
$S = [ordered]@{
    Phase        = 'Idle'          # Idle | WaitAlt | Xfer | PostDelay | AwaitRemove | AwaitNext
    StepIndex    = -1
    Steps        = $DefaultSteps
    Proc         = $null
    OutFile      = $null
    ErrFile      = $null
    PhaseStart   = [datetime]::Now
    RunStart     = [datetime]::Now
    StepStart    = [datetime]::Now
    UnitLog      = $null
    Serial       = ''
    DfuSerial    = ''
    DevPath      = ''
    LastPct      = -1
    LastPoll     = [datetime]::MinValue
    Counters     = @{}             # product id -> @{ Pass = n; Fail = n }
    Warnings     = 0
    PreflightOk  = $false
    ImageSetId   = '(unverified)'
    ImageVersion = ''
    ImageFields  = $null
    FailedStep   = ''
    FailReason   = ''
}

# ===========================================================================
# Small helpers
# ===========================================================================
function Format-Bytes([long]$b) {
    if     ($b -ge 1GB) { '{0:N2} GB' -f ($b / 1GB) }
    elseif ($b -ge 1MB) { '{0:N1} MB' -f ($b / 1MB) }
    elseif ($b -ge 1KB) { '{0:N1} KB' -f ($b / 1KB) }
    else                { "$b B" }
}

function Format-Span([TimeSpan]$t) {
    # Floor, not [int] - [int] rounds, which made 3m59s display as "04:59".
    '{0:00}:{1:00}' -f [int][Math]::Floor($t.TotalMinutes), $t.Seconds
}

function Get-FileTail {
    param([string]$Path, [int]$Max = 8192)
    if (-not $Path -or -not (Test-Path -LiteralPath $Path)) { return '' }
    try {
        $fs = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open,
                                     [System.IO.FileAccess]::Read,
                                     [System.IO.FileShare]::ReadWrite)
        try {
            $len   = $fs.Length
            $start = [Math]::Max(0, $len - $Max)
            [void]$fs.Seek($start, [System.IO.SeekOrigin]::Begin)
            $n = [int]($len - $start)
            if ($n -le 0) { return '' }
            $buf = New-Object byte[] $n
            [void]$fs.Read($buf, 0, $n)
            return [System.Text.Encoding]::ASCII.GetString($buf)
        } finally { $fs.Dispose() }
    } catch { return '' }
}

function Read-ImageVersion {
    <#  Parses bin\images\version.txt.

        Lines starting with # are comments. Everything else is a key/value pair,
        written in any of these shapes:

            [signatouch_IR_image_name]="emmc-signatouch-ir.wic"
            [signatouch_IR_image_version]="1.0"
            key = value
            key: value

        With -Product, ImageName and Version are filled from that product's
        NameKey / VersionKey entries.                                        #>
    param([hashtable]$ForProduct)
    $r = @{ Found = $false; ImageName = ''; Version = ''; Fields = [ordered]@{}; Notes = @(); Error = '' }
    if (-not (Test-Path -LiteralPath $VersionFile)) { return $r }
    $r.Found = $true
    try {
        foreach ($line in (Get-Content -LiteralPath $VersionFile -ErrorAction Stop)) {
            $t = $line.Trim()
            if (-not $t) { continue }
            if ($t.StartsWith('#')) { $r.Notes += $t.TrimStart('#').Trim(); continue }
            $m = [regex]::Match($t, '^\[?\s*([A-Za-z0-9_.\- ]+?)\s*\]?\s*[:=]\s*(.*?)\s*$')
            if ($m.Success) {
                $k = $m.Groups[1].Value.Trim()
                $v = $m.Groups[2].Value.Trim().Trim('"').Trim("'")
                if ($k) { $r.Fields[$k] = $v }
            } else {
                $r.Notes += $t
            }
        }
        if ($ForProduct) {
            foreach ($k in $r.Fields.Keys) {
                if     ($k -ieq $ForProduct.NameKey    -and $r.Fields[$k]) { $r.ImageName = $r.Fields[$k] }
                elseif ($k -ieq $ForProduct.VersionKey -and $r.Fields[$k]) { $r.Version   = $r.Fields[$k] }
            }
        }
    } catch { $r.Error = $_.Exception.Message }
    return $r
}

function Get-DfuDevices {
    <#  Runs "dfu-util -l" and returns a hashtable, or $null if the tool
        itself did not respond (which is treated as a hard error).          #>
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName               = $DfuExe
    $psi.Arguments              = '-l'
    $psi.WorkingDirectory       = $Root
    $psi.UseShellExecute        = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError  = $true
    $psi.CreateNoWindow         = $true
    try { $p = [System.Diagnostics.Process]::Start($psi) } catch { return $null }
    $out = $p.StandardOutput.ReadToEnd()
    $err = $p.StandardError.ReadToEnd()
    if (-not $p.WaitForExit([int]$Config.DeviceListTimeoutMs)) {
        try { $p.Kill() } catch { }
        return $null
    }
    $text = "$out`n$err"
    $alts = New-Object System.Collections.Generic.List[string]
    foreach ($m in [regex]::Matches($text, 'name="([^"]*)"')) { [void]$alts.Add($m.Groups[1].Value) }
    $serial = ''
    $sm = [regex]::Match($text, 'serial="([^"]*)"')
    if ($sm.Success -and $sm.Groups[1].Value -ne 'UNKNOWN') { $serial = $sm.Groups[1].Value }
    $path = ''
    $pm = [regex]::Match($text, 'path="([^"]*)"')
    if ($pm.Success) { $path = $pm.Groups[1].Value }
    $devKeys = @()
    foreach ($m in [regex]::Matches($text, 'devnum=(\d+)')) { $devKeys += $m.Groups[1].Value }
    $devCount = ($devKeys | Sort-Object -Unique | Measure-Object).Count
    return @{ Alts = $alts; Serial = $serial; Path = $path; Devices = $devCount; Raw = $text }
}

function Stop-StrayDfu {
    Get-Process -Name 'dfu-util', 'fastboot' -ErrorAction SilentlyContinue | ForEach-Object {
        try { $_.Kill() } catch { }
    }
}

function Get-FastbootDevices {
    <#  Runs "fastboot devices" and returns a hashtable like Get-DfuDevices
        (Devices = count, Serial = first serial), or $null if the tool did
        not respond.                                                          #>
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName               = $FastbootExe
    $psi.Arguments              = 'devices'
    $psi.WorkingDirectory       = $Root
    $psi.UseShellExecute        = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError  = $true
    $psi.CreateNoWindow         = $true
    try { $p = [System.Diagnostics.Process]::Start($psi) } catch { return $null }
    $out = $p.StandardOutput.ReadToEnd()
    $err = $p.StandardError.ReadToEnd()
    if (-not $p.WaitForExit([int]$Config.DeviceListTimeoutMs)) {
        try { $p.Kill() } catch { }
        return $null
    }
    $serials = @()
    foreach ($ln in ($out -split "[`r`n]+")) {
        # "<serial>\tfastboot" from older tools, "????????????     Android Fastboot" from
        # platform-tools 37 (U-Boot's gadget has no serial string).
        $m = [regex]::Match($ln.Trim(), '^(\S+)\s+.*fastboot', 'IgnoreCase')
        if ($m.Success -and $ln -notmatch 'no permissions') { $serials += $m.Groups[1].Value }
    }
    $serial = if ($serials.Count -gt 0) { $serials[0] } else { '' }
    return @{ Alts = @(); Serial = $serial; Path = ''; Devices = $serials.Count; Raw = "$out`n$err" }
}

function Test-StepUsesFastboot {
    foreach ($st in $S.Steps) { if ($st.Tool -eq 'fastboot') { return $true } }
    return $false
}

# ===========================================================================
# UI
# ===========================================================================
$fontUI     = New-Object System.Drawing.Font('Segoe UI', 9)
$fontUIb    = New-Object System.Drawing.Font('Segoe UI', 9, [System.Drawing.FontStyle]::Bold)
$fontHead   = New-Object System.Drawing.Font('Segoe UI Semibold', 15, [System.Drawing.FontStyle]::Bold)
$fontBanner = New-Object System.Drawing.Font('Segoe UI', 30, [System.Drawing.FontStyle]::Bold)
$fontMono   = New-Object System.Drawing.Font('Consolas', 9)
$fontBtn    = New-Object System.Drawing.Font('Segoe UI', 14, [System.Drawing.FontStyle]::Bold)

$clrDark    = [System.Drawing.Color]::FromArgb(32, 38, 48)
$clrIdle    = [System.Drawing.Color]::FromArgb(90, 98, 110)
$clrBusy    = [System.Drawing.Color]::FromArgb(20, 110, 190)
$clrPass    = [System.Drawing.Color]::FromArgb(22, 140, 70)
$clrFail    = [System.Drawing.Color]::FromArgb(180, 30, 35)
$clrWarn    = [System.Drawing.Color]::FromArgb(200, 130, 0)

$form              = New-Object System.Windows.Forms.Form
$form.Text         = "Flash Station - $($Config.ProductName)  v$AppVersion"
$form.Size         = New-Object System.Drawing.Size(1280, 840)
$form.MinimumSize  = New-Object System.Drawing.Size(1060, 740)
$form.StartPosition= 'CenterScreen'
$form.Font         = $fontUI
$form.BackColor    = [System.Drawing.Color]::White
$form.KeyPreview   = $true

# ---- header ---------------------------------------------------------------
$header           = New-Object System.Windows.Forms.Panel
$header.Dock      = 'Top'
$header.Height    = 54
$header.BackColor = $clrDark

$lblTitle           = New-Object System.Windows.Forms.Label
$lblTitle.Text      = "  $($Config.ProductName)  -  PRODUCTION FLASH STATION"
$lblTitle.Font      = $fontHead
$lblTitle.ForeColor = [System.Drawing.Color]::White
$lblTitle.Dock      = 'Left'
$lblTitle.AutoSize  = $true
$lblTitle.Padding   = New-Object System.Windows.Forms.Padding(6, 13, 0, 0)
$header.Controls.Add($lblTitle)

$lblStation           = New-Object System.Windows.Forms.Label
$lblStation.Text      = "Station: $($Config.StationName)   "
$lblStation.ForeColor = [System.Drawing.Color]::Gainsboro
$lblStation.Dock      = 'Right'
$lblStation.TextAlign = 'MiddleRight'
$lblStation.AutoSize  = $false
$lblStation.Width     = 320
$lblStation.Font      = New-Object System.Drawing.Font('Segoe UI', 9)
$header.Controls.Add($lblStation)

# Product colour stripe - a full-width band under the header in the selected
# product's colour, so the active product is recognisable from across the room.
$stripe           = New-Object System.Windows.Forms.Panel
$stripe.Dock      = 'Top'
$stripe.Height    = 8
$stripe.BackColor = $clrIdle

# ---- operator / job entry -------------------------------------------------
$entry           = New-Object System.Windows.Forms.Panel
$entry.Dock      = 'Top'
$entry.Height    = 44
$entry.BackColor = [System.Drawing.Color]::FromArgb(244, 246, 249)

function New-EntryLabel([string]$text, [int]$x) {
    $l = New-Object System.Windows.Forms.Label
    $l.Text = $text; $l.Font = $fontUIb; $l.AutoSize = $true
    $l.Location = New-Object System.Drawing.Point($x, 14)
    return $l
}
function New-EntryBox([int]$x, [int]$w) {
    $t = New-Object System.Windows.Forms.TextBox
    $t.Location = New-Object System.Drawing.Point($x, 11)
    $t.Width = $w
    return $t
}

$entry.Controls.Add((New-EntryLabel 'Operator ID:' 10))
$txtOperator = New-EntryBox 92 130
$entry.Controls.Add($txtOperator)

$entry.Controls.Add((New-EntryLabel 'Work order:' 240))
$txtWorkOrder = New-EntryBox 320 150
$entry.Controls.Add($txtWorkOrder)

$entry.Controls.Add((New-EntryLabel 'Unit serial / barcode:' 490))
$txtSerial = New-EntryBox 620 190
$entry.Controls.Add($txtSerial)

$chkRepeat          = New-Object System.Windows.Forms.CheckBox
$chkRepeat.Text     = 'Repeat mode (auto-start next board)'
$chkRepeat.AutoSize = $true
$chkRepeat.Location = New-Object System.Drawing.Point(840, 13)
$entry.Controls.Add($chkRepeat)

# ---- status banner --------------------------------------------------------
$banner           = New-Object System.Windows.Forms.Panel
$banner.Dock      = 'Top'
$banner.Height    = 78
$banner.BackColor = $clrIdle

$lblBanner           = New-Object System.Windows.Forms.Label
$lblBanner.Dock      = 'Fill'
$lblBanner.Text      = 'READY'
$lblBanner.Font      = $fontBanner
$lblBanner.ForeColor = [System.Drawing.Color]::White
$lblBanner.TextAlign = 'MiddleCenter'
$banner.Controls.Add($lblBanner)

# Image version rides on the right-hand end of the status banner, so it is
# impossible to run a board without seeing which build is being loaded.
$lblVersion           = New-Object System.Windows.Forms.Label
$lblVersion.Dock      = 'Right'
$lblVersion.Width     = 250
$lblVersion.TextAlign = 'MiddleRight'
$lblVersion.Font      = New-Object System.Drawing.Font('Segoe UI', 13, [System.Drawing.FontStyle]::Bold)
$lblVersion.ForeColor = [System.Drawing.Color]::White
$lblVersion.Padding   = New-Object System.Windows.Forms.Padding(0, 0, 14, 0)
$lblVersion.Text      = ''
$banner.Controls.Add($lblVersion)
$lblVersion.BringToFront()

# ---- bottom action bar ----------------------------------------------------
$actions           = New-Object System.Windows.Forms.Panel
$actions.Dock      = 'Bottom'
$actions.Height    = 116
$actions.BackColor = [System.Drawing.Color]::FromArgb(244, 246, 249)

$btnStart              = New-Object System.Windows.Forms.Button
$btnStart.Text         = 'START  (F5)'
$btnStart.Font         = $fontBtn
$btnStart.Size         = New-Object System.Drawing.Size(230, 62)
$btnStart.Location     = New-Object System.Drawing.Point(12, 14)
$btnStart.BackColor    = $clrPass
$btnStart.ForeColor    = [System.Drawing.Color]::White
$btnStart.FlatStyle    = 'Flat'
$actions.Controls.Add($btnStart)

$btnAbort              = New-Object System.Windows.Forms.Button
$btnAbort.Text         = 'ABORT (Esc)'
$btnAbort.Font         = $fontBtn
$btnAbort.Size         = New-Object System.Drawing.Size(180, 62)
$btnAbort.Location     = New-Object System.Drawing.Point(252, 14)
$btnAbort.BackColor    = $clrFail
$btnAbort.ForeColor    = [System.Drawing.Color]::White
$btnAbort.FlatStyle    = 'Flat'
$btnAbort.Enabled      = $false
$clrDisabled           = [System.Drawing.Color]::FromArgb(188, 194, 202)
$btnAbort.BackColor    = $clrDisabled
$actions.Controls.Add($btnAbort)

$btnProduct          = New-Object System.Windows.Forms.Button
$btnProduct.Text     = 'Switch product (F2)'
$btnProduct.Size     = New-Object System.Drawing.Size(230, 26)
$btnProduct.Location = New-Object System.Drawing.Point(12, 82)
$actions.Controls.Add($btnProduct)

$btnVerify          = New-Object System.Windows.Forms.Button
$btnVerify.Text     = 'Re-check images'
$btnVerify.Size     = New-Object System.Drawing.Size(140, 28)
$btnVerify.Location = New-Object System.Drawing.Point(448, 14)
$actions.Controls.Add($btnVerify)

$btnLogs            = New-Object System.Windows.Forms.Button
$btnLogs.Text       = 'Open log folder'
$btnLogs.Size       = New-Object System.Drawing.Size(140, 28)
$btnLogs.Location    = New-Object System.Drawing.Point(448, 48)
$actions.Controls.Add($btnLogs)

$btnManifest          = New-Object System.Windows.Forms.Button
$btnManifest.Text     = 'Create/refresh manifest'
$btnManifest.Size     = New-Object System.Drawing.Size(140, 28)
$btnManifest.Location = New-Object System.Drawing.Point(448, 82)
$actions.Controls.Add($btnManifest)

$lblCounters           = New-Object System.Windows.Forms.Label
$lblCounters.Font      = New-Object System.Drawing.Font('Segoe UI', 12, [System.Drawing.FontStyle]::Bold)
$lblCounters.AutoSize  = $false
$lblCounters.Size      = New-Object System.Drawing.Size(260, 60)
$lblCounters.Location  = New-Object System.Drawing.Point(608, 16)
$lblCounters.Text      = "Shift totals`r`nPass 0    Fail 0"
$actions.Controls.Add($lblCounters)

$btnResetCount          = New-Object System.Windows.Forms.Button
$btnResetCount.Text     = 'Reset counters'
$btnResetCount.Size     = New-Object System.Drawing.Size(120, 26)
$btnResetCount.Location = New-Object System.Drawing.Point(608, 82)
$actions.Controls.Add($btnResetCount)

$lblImageSet           = New-Object System.Windows.Forms.Label
$lblImageSet.AutoSize  = $false
$lblImageSet.Size      = New-Object System.Drawing.Size(360, 76)
$lblImageSet.Location  = New-Object System.Drawing.Point(880, 16)
$lblImageSet.Text      = 'Image set: (not verified)'
$actions.Controls.Add($lblImageSet)

# ---- progress -------------------------------------------------------------
$progPanel        = New-Object System.Windows.Forms.Panel
$progPanel.Dock   = 'Bottom'
$progPanel.Height = 54

$lblAction          = New-Object System.Windows.Forms.Label
$lblAction.Dock     = 'Top'
$lblAction.Height   = 22
$lblAction.Font     = $fontUIb
$lblAction.Text     = 'Idle - press START when the board is connected in DFU mode.'
$lblAction.Padding  = New-Object System.Windows.Forms.Padding(10, 3, 0, 0)
$progPanel.Controls.Add($lblAction)

$bar          = New-Object System.Windows.Forms.ProgressBar
$bar.Dock     = 'Fill'
$bar.Style    = 'Continuous'
$bar.Minimum  = 0
$bar.Maximum  = 100
$progPanel.Controls.Add($bar)

# ---- centre split: steps + log -------------------------------------------
$split                  = New-Object System.Windows.Forms.SplitContainer
$split.Dock             = 'Fill'

$lv                        = New-Object System.Windows.Forms.ListView
$lv.Dock                   = 'Fill'
$lv.View                   = 'Details'
$lv.FullRowSelect          = $true
$lv.GridLines              = $true
$lv.HideSelection          = $true
$lv.Font                   = $fontUI
[void]$lv.Columns.Add('#', 30)
[void]$lv.Columns.Add('Step', 300)
[void]$lv.Columns.Add('Status', 120)
[void]$lv.Columns.Add('Time', 70)
$split.Panel1.Controls.Add($lv)

$log                = New-Object System.Windows.Forms.RichTextBox
$log.Dock           = 'Fill'
$log.ReadOnly       = $true
$log.Font           = $fontMono
$log.BackColor      = [System.Drawing.Color]::FromArgb(24, 26, 30)
$log.ForeColor      = [System.Drawing.Color]::Gainsboro
$log.WordWrap       = $true
$log.ScrollBars     = 'Vertical'
$split.Panel2.Controls.Add($log)

# ---- dock assembly ---------------------------------------------------------
# WinForms docks the LAST-added control closest to its edge, so add outward-in:
# the Fill control first, then each edge panel working towards the window frame.
$form.Controls.Add($split)        # Fill - centre
$form.Controls.Add($progPanel)    # Bottom, just under the centre
$form.Controls.Add($actions)      # Bottom, at the window edge
$form.Controls.Add($banner)       # Top, just above the centre
$form.Controls.Add($entry)        # Top
$form.Controls.Add($stripe)       # Top - product colour stripe under the header
$form.Controls.Add($header)       # Top, at the window edge

# ===========================================================================
# Product selection
# ===========================================================================
function Show-ProductDialog {
    <#  Modal startup menu: one big colour-coded button per product, with the
        image file and version it would flash shown underneath. Returns the
        chosen product hashtable, or $null if the operator closed the window. #>
    param([switch]$SwitchMode)

    $dlg                 = New-Object System.Windows.Forms.Form
    $dlg.Text            = 'Flash Station - Select product'
    $dlg.ClientSize      = New-Object System.Drawing.Size(820, 440)
    $dlg.FormBorderStyle = 'FixedDialog'
    $dlg.MaximizeBox     = $false
    $dlg.MinimizeBox     = $false
    $dlg.StartPosition   = 'CenterScreen'
    $dlg.BackColor       = [System.Drawing.Color]::White
    $dlg.Font            = $fontUI
    $dlg.TopMost         = $true

    $h           = New-Object System.Windows.Forms.Label
    $h.Dock      = 'Top'
    $h.Height    = 72
    $h.Text      = 'SELECT PRODUCT'
    $h.Font      = New-Object System.Drawing.Font('Segoe UI', 24, [System.Drawing.FontStyle]::Bold)
    $h.ForeColor = [System.Drawing.Color]::White
    $h.BackColor = $clrDark
    $h.TextAlign = 'MiddleCenter'
    $dlg.Controls.Add($h)

    $sub           = New-Object System.Windows.Forms.Label
    $sub.AutoSize  = $false
    $sub.SetBounds(0, 84, 820, 26)
    $sub.TextAlign = 'MiddleCenter'
    $sub.Font      = New-Object System.Drawing.Font('Segoe UI', 10)
    $sub.Text      = 'Both products flash identically - only the eMMC image differs. Pick the product on this work order.'
    $dlg.Controls.Add($sub)

    $script:__ProductChoice = $null
    $x = 55
    foreach ($p in $Products) {
        $ver = Read-ImageVersion -ForProduct $p

        $b           = New-Object System.Windows.Forms.Button
        $b.Text      = $p.Display
        $b.Font      = New-Object System.Drawing.Font('Segoe UI', 20, [System.Drawing.FontStyle]::Bold)
        $b.SetBounds($x, 130, 330, 170)
        $b.BackColor = $p.Accent
        $b.ForeColor = [System.Drawing.Color]::White
        $b.FlatStyle = 'Flat'
        $b.Tag       = $p
        $b.Add_Click({
            $script:__ProductChoice = $this.Tag
            $this.FindForm().DialogResult = [System.Windows.Forms.DialogResult]::OK
        })
        $dlg.Controls.Add($b)

        $d           = New-Object System.Windows.Forms.Label
        $d.AutoSize  = $false
        $d.SetBounds($x, 308, 330, 58)
        $d.TextAlign = 'TopCenter'
        if ($ver.ImageName) {
            $vtxt   = if ($ver.Version) { $ver.Version } else { '?' }
            $d.Text = ("image: {0}`r`nversion: {1}" -f $ver.ImageName, $vtxt)
        } else {
            $d.Text      = ("version.txt does not define`r`n{0}" -f $p.NameKey)
            $d.ForeColor = $clrFail
        }
        $dlg.Controls.Add($d)

        $x += 380
    }

    $hint           = New-Object System.Windows.Forms.Label
    $hint.AutoSize  = $false
    $hint.SetBounds(0, 386, 820, 24)
    $hint.TextAlign = 'MiddleCenter'
    $hint.ForeColor = [System.Drawing.Color]::Gray
    if ($SwitchMode) { $hint.Text = 'Close this window to keep the current product.' }
    else             { $hint.Text = 'Closing this window exits the Flash Station.' }
    $dlg.Controls.Add($hint)

    $result = $dlg.ShowDialog()
    $dlg.Dispose()
    if ($result -eq [System.Windows.Forms.DialogResult]::OK) { return $script:__ProductChoice }
    return $null
}

function Set-Product {
    <#  Makes $P the active product: per-product logs folder, results CSV and
        manifest paths, plus every place in the UI that names the product.
        Pre-flight (run right after) points the eMMC step at the product's
        .wic and validates it.                                               #>
    param([hashtable]$P)
    $script:CurrentProduct = $P
    $script:LogDir         = Join-Path $LogRoot $P.LogSubdir
    $script:ResultsCsv     = Join-Path $script:LogDir 'flash-results.csv'
    $script:ManifestFile   = Join-Path $Root $P.Manifest
    if (-not (Test-Path $script:LogDir)) { New-Item -ItemType Directory -Path $script:LogDir -Force | Out-Null }

    $form.Text        = "Flash Station - $($P.Display)  v$AppVersion"
    $lblTitle.Text    = "  $($P.Display)  -  PRODUCTION FLASH STATION"
    $stripe.BackColor = $P.Accent
    $lblVersion.Text  = $P.Display

    $S.UnitLog     = $null           # never append the new product's lines to the old product's unit log
    $S.PreflightOk = $false
    $S.ImageSetId  = '(unverified)'
    $lblImageSet.Text = 'Image set: (not verified)'
}

function Get-Counters {
    if (-not $CurrentProduct) { return @{ Pass = 0; Fail = 0 } }
    if (-not $S.Counters.ContainsKey($CurrentProduct.Id)) {
        $S.Counters[$CurrentProduct.Id] = @{ Pass = 0; Fail = 0 }
    }
    return $S.Counters[$CurrentProduct.Id]
}

# ===========================================================================
# Logging
# ===========================================================================
function Write-Log {
    param([string]$Message, [ValidateSet('INFO','OK','WARN','ERROR','STEP')][string]$Level = 'INFO')
    $ts   = (Get-Date).ToString('HH:mm:ss')
    $line = "{0}  {1,-5}  {2}" -f $ts, $Level, $Message
    switch ($Level) {
        'OK'    { $c = [System.Drawing.Color]::FromArgb(110, 220, 130) }
        'WARN'  { $c = [System.Drawing.Color]::FromArgb(255, 200, 90)  }
        'ERROR' { $c = [System.Drawing.Color]::FromArgb(255, 120, 120) }
        'STEP'  { $c = [System.Drawing.Color]::FromArgb(120, 190, 255) }
        default { $c = [System.Drawing.Color]::Gainsboro }
    }
    $log.SelectionStart  = $log.TextLength
    $log.SelectionLength = 0
    $log.SelectionColor  = $c
    $log.AppendText($line + "`r`n")
    $log.SelectionColor  = $log.ForeColor
    $log.ScrollToCaret()
    if ($S.UnitLog) {
        try { Add-Content -LiteralPath $S.UnitLog -Value ((Get-Date).ToString('yyyy-MM-dd HH:mm:ss') + "  $Level  $Message") } catch { }
    }
}

function Set-Banner {
    param([string]$Text, [System.Drawing.Color]$Color)
    $lblBanner.Text  = $Text
    $banner.BackColor = $Color
    $banner.Refresh()
}

function Set-Action([string]$Text) { $lblAction.Text = $Text }

function Update-Counters {
    $c    = Get-Counters
    $name = if ($CurrentProduct) { $CurrentProduct.Display } else { 'Shift' }
    $lblCounters.Text = "{0} shift totals`r`nPass {1}    Fail {2}" -f $name, $c.Pass, $c.Fail
    if ($c.Fail -gt 0) { $lblCounters.ForeColor = $clrFail } else { $lblCounters.ForeColor = $clrPass }
}

# ===========================================================================
# Step list
# ===========================================================================
function Reset-StepList {
    $lv.Items.Clear()
    foreach ($st in $S.Steps) {
        $it = New-Object System.Windows.Forms.ListViewItem([string]$st.Id)
        [void]$it.SubItems.Add($st.Title)
        [void]$it.SubItems.Add('Pending')
        [void]$it.SubItems.Add('')
        $it.ForeColor = [System.Drawing.Color]::Gray
        [void]$lv.Items.Add($it)
    }
}

function Set-StepStatus {
    param([int]$Index, [string]$Status, [System.Drawing.Color]$Color, [string]$Time = $null)
    if ($Index -lt 0 -or $Index -ge $lv.Items.Count) { return }
    $it = $lv.Items[$Index]
    $it.SubItems[2].Text = $Status
    if ($null -ne $Time) { $it.SubItems[3].Text = $Time }
    $it.ForeColor = $Color
    $it.EnsureVisible()
}

# ===========================================================================
# Pre-flight
# ===========================================================================
function Get-Sha256([string]$Path) {
    $sha = [System.Security.Cryptography.SHA256]::Create()
    try {
        $fs = [System.IO.File]::OpenRead($Path)
        try { return ([BitConverter]::ToString($sha.ComputeHash($fs))).Replace('-', '').ToLowerInvariant() }
        finally { $fs.Dispose() }
    } finally { $sha.Dispose() }
}

function Get-UniqueImageList {
    $seen = @{}
    $out  = New-Object System.Collections.Generic.List[string]
    foreach ($st in $S.Steps) {
        if (-not $st.File) { continue }
        if (-not $seen.ContainsKey($st.File)) { $seen[$st.File] = $true; [void]$out.Add($st.File) }
    }
    return $out
}

function Invoke-Preflight {
    param([switch]$Quiet)
    $ok = $true
    $P  = $CurrentProduct
    Write-Log 'Pre-flight checks...' 'STEP'
    Write-Log ("Product: {0}" -f $P.Display) 'STEP'

    # 1. image build identity - which product image is this station loading?
    $ver = Read-ImageVersion -ForProduct $P
    $S.ImageVersion = $ver.Version
    $S.ImageFields  = $ver.Fields
    if (-not $ver.Found) {
        Write-Log 'bin\images\version.txt is missing - the eMMC image for this product cannot be identified.' 'ERROR'
        $lblVersion.Text = "$($P.Display) - ver ?"
        $ok = $false
    } elseif ($ver.Error) {
        Write-Log "version.txt could not be read: $($ver.Error)" 'ERROR'
        $lblVersion.Text = "$($P.Display) - ver ?"
        $ok = $false
    } else {
        if (-not $ver.ImageName) {
            Write-Log ("version.txt does not define '{0}' - cannot tell which eMMC image belongs to {1}." -f $P.NameKey, $P.Display) 'ERROR'
            $ok = $false
        } else {
            Write-Log ("{0} eMMC image: {1}" -f $P.Display, $ver.ImageName) 'OK'
        }
        if (-not $ver.Version) {
            Write-Log ("version.txt does not define '{0}' - image version unknown." -f $P.VersionKey) 'WARN'
            $lblVersion.Text = "$($P.Display) - ver ?"
        } else {
            Write-Log ("IMAGE VERSION {0}" -f $ver.Version) 'OK'
            $lblVersion.Text = "$($P.Display)  v$($ver.Version)"
        }
    }
    foreach ($k in $ver.Fields.Keys) { Write-Log ("  version.txt  {0} = {1}" -f $k, $ver.Fields[$k]) 'INFO' }
    foreach ($n in $ver.Notes)       { Write-Log ("  version.txt  note: {0}" -f $n) 'INFO' }

    # Point the eMMC step at this product's image. Done every pre-flight, so
    # editing version.txt then pressing "Re-check images" takes effect.
    if ($ver.ImageName) {
        foreach ($st in $S.Steps) {
            if ($st.Role -eq 'image') {
                $name = $ver.ImageName
                # version.txt names the sparse image; the DFU sequence writes the raw .wic of the same name.
                if ($st.Tool -eq 'dfu'      -and $name -match '\.simg$') { $name = $name -replace '\.simg$', '.wic' }
                if ($st.Tool -eq 'fastboot' -and $name -match '\.wic$')  { $name = $name -replace '\.wic$', '.simg' }
                $newFile = 'bin\images\' + $name
                if ($st.File -ne $newFile) {
                    $st.File  = $newFile
                    $st.Title = ("eMMC user  - {0} ({1})" -f $name, $(if ($st.Tool -eq 'fastboot') { 'sparse image' } else { 'full image' }))
                    Reset-StepList
                }
            }
        }
    }

    # 2. dfu-util present (and fastboot.exe, when the sequence uses it)
    if (-not (Test-Path -LiteralPath $DfuExe)) {
        Write-Log "dfu-util.exe not found at $DfuExe" 'ERROR'
        $ok = $false
    } else {
        Write-Log ("dfu-util.exe found ({0})" -f (Format-Bytes (Get-Item $DfuExe).Length)) 'OK'
    }
    if (Test-StepUsesFastboot) {
        if (-not (Test-Path -LiteralPath $FastbootExe)) {
            Write-Log "fastboot.exe not found at $FastbootExe (copy fastboot.exe, AdbWinApi.dll and AdbWinUsbApi.dll from Android platform-tools next to this script)" 'ERROR'
            $ok = $false
        } else {
            Write-Log ("fastboot.exe found ({0})" -f (Format-Bytes (Get-Item $FastbootExe).Length)) 'OK'
        }
    }

    # 3. images present, correct size, correct hash
    $manifest = $null
    if (Test-Path -LiteralPath $ManifestFile) {
        try { $manifest = Get-Content -Raw -LiteralPath $ManifestFile | ConvertFrom-Json }
        catch { Write-Log "$($P.Manifest) is unreadable: $($_.Exception.Message)" 'WARN' }
    }
    if ($manifest -and ($manifest.PSObject.Properties.Name -contains 'product') -and
        $manifest.product -and $manifest.product -ne $P.Display) {
        Write-Log ("{0} was recorded for '{1}', not '{2}' - refresh the manifest for this product." -f $P.Manifest, $manifest.product, $P.Display) 'WARN'
    }

    $hashParts = New-Object System.Collections.Generic.List[string]
    foreach ($rel in Get-UniqueImageList) {
        $full = Join-Path $Root $rel
        if (-not (Test-Path -LiteralPath $full)) {
            Write-Log "MISSING image: $rel" 'ERROR'
            $ok = $false
            continue
        }
        $fi = Get-Item -LiteralPath $full
        if ($fi.Length -eq 0) {
            Write-Log "Image is zero bytes: $rel" 'ERROR'
            $ok = $false
            continue
        }
        if ($Config.VerifyHashes) {
            Set-Action "Hashing $rel ..."
            [System.Windows.Forms.Application]::DoEvents()
            $h = Get-Sha256 $full
            [void]$hashParts.Add("$rel=$h")
            if ($manifest -and $manifest.files -and ($manifest.files.PSObject.Properties.Name -contains $rel)) {
                $exp = $manifest.files.$rel
                if ($exp.sha256 -ne $h) {
                    Write-Log ("HASH MISMATCH {0}`r`n              expected {1}`r`n              actual   {2}" -f $rel, $exp.sha256, $h) 'ERROR'
                    $ok = $false
                } elseif ([long]$exp.size -ne $fi.Length) {
                    Write-Log ("SIZE MISMATCH {0}: expected {1}, actual {2}" -f $rel, $exp.size, $fi.Length) 'ERROR'
                    $ok = $false
                } else {
                    Write-Log ("{0}  {1}  sha256 OK" -f $rel.PadRight(28), (Format-Bytes $fi.Length)) 'OK'
                }
            } else {
                Write-Log ("{0}  {1}  (not in manifest)" -f $rel.PadRight(28), (Format-Bytes $fi.Length)) 'WARN'
            }
        } else {
            Write-Log ("{0}  {1}" -f $rel.PadRight(28), (Format-Bytes $fi.Length)) 'OK'
        }
    }

    if (-not $manifest -and $Config.VerifyHashes) {
        Write-Log ("No {0} - {1} images cannot be checked against a known-good release." -f $P.Manifest, $P.Display) 'WARN'
        Write-Log 'Engineering: press "Create/refresh manifest" once the correct images are staged.' 'WARN'
    }

    # image-set fingerprint for traceability
    if ($hashParts.Count -gt 0) {
        $sha  = [System.Security.Cryptography.SHA256]::Create()
        $bytes= [System.Text.Encoding]::UTF8.GetBytes(($hashParts -join ';'))
        $S.ImageSetId = (([BitConverter]::ToString($sha.ComputeHash($bytes))).Replace('-','').ToLowerInvariant()).Substring(0,12)
        $sha.Dispose()
        $lblImageSet.Text = "Product: $($P.Display)`r`nImage version: $(if ($S.ImageVersion) { $S.ImageVersion } else { 'UNKNOWN' })`r`nImage set:  $($S.ImageSetId)`r`nManifest: $(if (Test-Path -LiteralPath $ManifestFile) { 'present' } else { 'MISSING' })"
    }

    # 4. USB / driver sanity - can we talk to libusb at all?
    $d = Get-DfuDevices
    if ($null -eq $d) {
        Write-Log 'dfu-util -l did not respond. Check the WinUSB driver (Zadig) and USB cable.' 'ERROR'
        $ok = $false
    } elseif ($d.Devices -eq 0) {
        Write-Log 'No DFU device connected yet (that is fine - connect the board, then press START).' 'INFO'
    } else {
        Write-Log ("DFU device present: {0} device(s), alts: {1}" -f $d.Devices, ($d.Alts -join ', ')) 'OK'
    }

    # 5. free disk space for logs
    try {
        $drive = (Get-Item $Root).PSDrive
        if ($drive.Free -lt 200MB) { Write-Log ("Low free disk space on {0}: {1}" -f $drive.Name, (Format-Bytes $drive.Free)) 'WARN' }
    } catch { }

    $S.PreflightOk = $ok
    if ($ok) {
        Write-Log 'Pre-flight PASSED.' 'OK'
        if (-not $Quiet) { Set-Banner 'READY' $clrIdle }
        Set-Action 'Ready - connect the board in DFU mode and press START.'
    } else {
        Write-Log 'Pre-flight FAILED - do not flash until this is corrected.' 'ERROR'
        Set-Banner 'SETUP ERROR' $clrFail
        Set-Action 'Pre-flight failed - see the log. Flashing is blocked.'
    }
    $btnStart.Enabled = $ok
    return $ok
}

# Plain-text gate, not security: keeps production operators from accidentally
# re-approving a bad image set. Engineering knows the PIN.
$ManifestPin = '4242'

function Request-EngineeringPin {
    $dlg                 = New-Object System.Windows.Forms.Form
    $dlg.Text            = 'Engineering PIN required'
    $dlg.ClientSize      = New-Object System.Drawing.Size(360, 150)
    $dlg.FormBorderStyle = 'FixedDialog'
    $dlg.MaximizeBox     = $false
    $dlg.MinimizeBox     = $false
    $dlg.StartPosition   = 'CenterParent'
    $dlg.Font            = $fontUI

    $lbl          = New-Object System.Windows.Forms.Label
    $lbl.AutoSize = $false
    $lbl.SetBounds(14, 12, 332, 40)
    $lbl.Text     = "Refreshing the image manifest is an engineering task.`r`nEnter the engineering PIN to continue:"
    $dlg.Controls.Add($lbl)

    $txt              = New-Object System.Windows.Forms.TextBox
    $txt.PasswordChar = '*'
    $txt.SetBounds(14, 58, 332, 24)
    $dlg.Controls.Add($txt)

    $ok              = New-Object System.Windows.Forms.Button
    $ok.Text         = 'OK'
    $ok.DialogResult = [System.Windows.Forms.DialogResult]::OK
    $ok.SetBounds(180, 100, 80, 30)
    $dlg.Controls.Add($ok)

    $cancel              = New-Object System.Windows.Forms.Button
    $cancel.Text         = 'Cancel'
    $cancel.DialogResult = [System.Windows.Forms.DialogResult]::Cancel
    $cancel.SetBounds(266, 100, 80, 30)
    $dlg.Controls.Add($cancel)

    $dlg.AcceptButton = $ok
    $dlg.CancelButton = $cancel

    $result  = $dlg.ShowDialog($form)
    $entered = $txt.Text
    $dlg.Dispose()

    if ($result -ne [System.Windows.Forms.DialogResult]::OK) { return $false }
    if ($entered -ne $ManifestPin) {
        Write-Log 'Manifest refresh blocked - incorrect engineering PIN.' 'WARN'
        [void][System.Windows.Forms.MessageBox]::Show('Incorrect PIN. Manifest was not changed.', 'Engineering PIN', 'OK', 'Warning')
        return $false
    }
    return $true
}

function New-Manifest {
    if (-not (Request-EngineeringPin)) { return }
    $r = [System.Windows.Forms.MessageBox]::Show(
        ("This records the SHA-256 of the images CURRENTLY in bin\ as the approved set for {0}`r`n(written to {1}).`r`n`r`nOnly do this when engineering has confirmed the staged images are correct.`r`n`r`nContinue?" -f $CurrentProduct.Display, $CurrentProduct.Manifest),
        'Create / refresh image manifest', 'YesNo', 'Warning')
    if ($r -ne 'Yes') { return }

    $files = [ordered]@{}
    foreach ($rel in Get-UniqueImageList) {
        $full = Join-Path $Root $rel
        if (-not (Test-Path -LiteralPath $full)) {
            [void][System.Windows.Forms.MessageBox]::Show("Cannot create manifest - missing: $rel", 'Error', 'OK', 'Error')
            return
        }
        Set-Action "Hashing $rel ..."
        [System.Windows.Forms.Application]::DoEvents()
        $fi = Get-Item -LiteralPath $full
        $files[$rel] = [ordered]@{ sha256 = (Get-Sha256 $full); size = $fi.Length }
    }
    $obj = [ordered]@{
        generated   = (Get-Date).ToString('s')
        generatedBy = "$env:USERNAME on $env:COMPUTERNAME"
        product     = $CurrentProduct.Display
        imageVersion= $S.ImageVersion
        files       = $files
    }
    $obj | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $ManifestFile -Encoding UTF8
    Write-Log "Wrote $ManifestFile" 'OK'
    [void][System.Windows.Forms.MessageBox]::Show('Manifest written.', 'Done', 'OK', 'Information')
    [void](Invoke-Preflight)
}

# ===========================================================================
# CSV traveller
# ===========================================================================
function Write-Result {
    param([string]$Result, [string]$Reason = '')
    $dur = [int]((Get-Date) - $S.RunStart).TotalSeconds
    $header = 'Timestamp,Station,Product,Operator,WorkOrder,Serial,DfuSerial,UsbPath,Result,DurationSec,FailedStep,Reason,ImageVersion,ImageSet,AppVersion,LogFile'
    if (Test-Path -LiteralPath $ResultsCsv) {
        # An older station wrote a CSV without ImageVersion. Appending to it would
        # silently misalign every column, so archive it and start a fresh file.
        $existing = ''
        try { $existing = (Get-Content -LiteralPath $ResultsCsv -TotalCount 1 -ErrorAction Stop) } catch { }
        if ($existing -and $existing -ne $header) {
            $archive = Join-Path $LogDir ('flash-results-archived-{0}.csv' -f (Get-Date).ToString('yyyyMMdd-HHmmss'))
            try {
                Move-Item -LiteralPath $ResultsCsv -Destination $archive -Force
                Write-Log "flash-results.csv used an older column layout; archived it as $(Split-Path -Leaf $archive)." 'WARN'
            } catch {
                Write-Log "Could not archive the old flash-results.csv: $($_.Exception.Message)" 'WARN'
            }
        }
    }
    if (-not (Test-Path -LiteralPath $ResultsCsv)) {
        $header | Set-Content -LiteralPath $ResultsCsv -Encoding UTF8
    }
    $esc = {
        param($v)
        if ($null -eq $v) { $v = '' }
        '"' + ([string]$v).Replace('"', '""').Replace("`r", ' ').Replace("`n", ' ') + '"'
    }
    $row = @(
        (& $esc (Get-Date).ToString('s'))
        (& $esc $Config.StationName)
        (& $esc $CurrentProduct.Display)
        (& $esc $txtOperator.Text)
        (& $esc $txtWorkOrder.Text)
        (& $esc $S.Serial)
        (& $esc $S.DfuSerial)
        (& $esc $S.DevPath)
        (& $esc $Result)
        (& $esc $dur)
        (& $esc $S.FailedStep)
        (& $esc $Reason)
        (& $esc $S.ImageVersion)
        (& $esc $S.ImageSetId)
        (& $esc $AppVersion)
        (& $esc (Split-Path -Leaf ([string]$S.UnitLog)))
    ) -join ','
    try { Add-Content -LiteralPath $ResultsCsv -Value $row }
    catch { Write-Log "Could not append to flash-results.csv: $($_.Exception.Message)" 'WARN' }
}

# ===========================================================================
# Run control
# ===========================================================================
$timer          = New-Object System.Windows.Forms.Timer
$timer.Interval = [int]$Config.PollIntervalMs

function Set-Running([bool]$running) {
    $btnStart.Enabled     = (-not $running)
    $btnAbort.Enabled     = $running
    # Keep the colour in step with Enabled, or a disabled flat button still looks live.
    $btnStart.BackColor   = if ($running) { $clrDisabled } else { $clrPass }
    $btnAbort.BackColor   = if ($running) { $clrFail }    else { $clrDisabled }
    $btnVerify.Enabled    = (-not $running)
    $btnManifest.Enabled  = (-not $running)
    $btnProduct.Enabled   = (-not $running)
    $txtOperator.Enabled  = (-not $running)
    $txtWorkOrder.Enabled = (-not $running)
    $txtSerial.Enabled    = (-not $running)
}

function Start-Run {
    if ($S.Phase -ne 'Idle' -and $S.Phase -ne 'AwaitNext') { return }

    if ($Config.RequireOperatorId -and -not $txtOperator.Text.Trim()) {
        [void][System.Windows.Forms.MessageBox]::Show('Enter your Operator ID before flashing.', 'Operator ID required', 'OK', 'Warning')
        $txtOperator.Focus(); return
    }
    if ($Config.RequireSerial -and -not $txtSerial.Text.Trim()) {
        [void][System.Windows.Forms.MessageBox]::Show('Scan or enter the unit serial before flashing.', 'Serial required', 'OK', 'Warning')
        $txtSerial.Focus(); return
    }
    if (-not $S.PreflightOk) {
        if (-not (Invoke-Preflight)) { return }
    }

    Stop-StrayDfu

    $S.Serial     = $txtSerial.Text.Trim()
    $S.DfuSerial  = ''
    $S.DevPath    = ''
    $S.FailedStep = ''
    $S.FailReason = ''
    $S.Warnings   = 0
    $S.RunStart   = Get-Date
    $S.StepIndex  = 0
    $S.LastPct    = -1
    $S.LastPoll   = [datetime]::MinValue

    $stamp = (Get-Date).ToString('yyyyMMdd-HHmmss')
    $tag   = if ($S.Serial) { ($S.Serial -replace '[^A-Za-z0-9_\-]', '_') } else { 'noserial' }
    $S.UnitLog = Join-Path $LogDir "$stamp`_$tag.log"

    $log.Clear()
    Reset-StepList
    $bar.Style = 'Continuous'
    $bar.Value = 0
    Set-Running $true
    Set-Banner 'FLASHING...' $clrBusy

    Write-Log "Flash Station v$AppVersion   station=$($Config.StationName)   product=$($CurrentProduct.Display)" 'INFO'
    Write-Log "Operator=$($txtOperator.Text.Trim())  WorkOrder=$($txtWorkOrder.Text.Trim())  Serial=$($S.Serial)" 'INFO'
    Write-Log "Image version=$(if ($S.ImageVersion) { $S.ImageVersion } else { 'UNKNOWN' })  Image set=$($S.ImageSetId)" 'INFO'
    Write-Log '---------------------------------------------------------------' 'INFO'

    Enter-WaitAlt
    $timer.Start()
}

function Enter-WaitAlt {
    $st = $S.Steps[$S.StepIndex]
    $S.Phase      = 'WaitAlt'
    $S.PhaseStart = Get-Date
    $S.StepStart  = Get-Date
    $S.LastPoll   = [datetime]::MinValue
    Set-StepStatus $S.StepIndex 'Waiting...' $clrBusy
    Write-Log ("Step {0}/{1}  {2}" -f $st.Id, $S.Steps.Count, $st.Title) 'STEP'
    if ($st.Tool -eq 'fastboot') {
        Write-Log ("  waiting for a fastboot device (timeout {0}s)" -f $st.WaitSec) 'INFO'
        Set-Action ("Step {0}/{1} - waiting for the board in fastboot mode..." -f $st.Id, $S.Steps.Count)
    } else {
        Write-Log ("  waiting for DFU alt '{0}' (timeout {1}s)" -f $st.Alt, $st.WaitSec) 'INFO'
        Set-Action ("Step {0}/{1} - waiting for the board to present '{2}'..." -f $st.Id, $S.Steps.Count, $st.Alt)
    }
    $bar.Style = 'Marquee'
}

function Start-Transfer {
    $st   = $S.Steps[$S.StepIndex]
    if ($st.Tool -eq 'fastboot') {
        $exe = $FastbootExe
        if ($st.Command -eq 'reboot') {
            $dfuArgs = @('reboot')
            $sizeTxt = '-'
        } else {
            $full    = Join-Path $Root $st.File
            $dfuArgs = @('flash', $st.Alt, $st.File)
            $sizeTxt = Format-Bytes (Get-Item $full).Length
        }
        $toolName = 'fastboot.exe'
    } else {
        $exe  = $DfuExe
        $full = Join-Path $Root $st.File
        $dfuArgs = @("-a", $st.Alt, "-D", $st.File)
        if ($st.Reboot) { $dfuArgs += "-R" }
        $sizeTxt  = Format-Bytes (Get-Item $full).Length
        $toolName = 'dfu-util.exe'
    }

    $S.OutFile = Join-Path $env:TEMP ("dfu-out-{0}.txt" -f [guid]::NewGuid().ToString('N'))
    $S.ErrFile = Join-Path $env:TEMP ("dfu-err-{0}.txt" -f [guid]::NewGuid().ToString('N'))

    Write-Log ("  {0} {1}   [{2}]" -f $toolName, ($dfuArgs -join " "), $sizeTxt) 'INFO'
    try {
        $S.Proc = Start-Process -FilePath $exe -ArgumentList $dfuArgs -WorkingDirectory $Root `
                                -NoNewWindow -PassThru `
                                -RedirectStandardOutput $S.OutFile -RedirectStandardError $S.ErrFile
        # Touching .Handle makes PowerShell cache the process handle. Without this
        # the object is returned without one and .ExitCode reads back as $null for
        # the whole life of the process, so a real dfu-util failure code is lost.
        try { $null = $S.Proc.Handle } catch { }
    } catch {
        Complete-Run $false ("Could not launch {0}: {1}" -f $toolName, $_.Exception.Message)
        return
    }
    $S.Phase      = 'Xfer'
    $S.PhaseStart = Get-Date
    $S.LastPct    = -1
    $bar.Style    = 'Continuous'
    $bar.Value    = 0
    Set-StepStatus $S.StepIndex 'Flashing...' $clrBusy
    Set-Action ("Step {0}/{1} - writing {2} (max {3}s)" -f $st.Id, $S.Steps.Count, $(if ($st.File) { $st.File } else { $st.Command }), $st.XferSec)
}

function Stop-Transfer {
    if ($S.Proc -and -not $S.Proc.HasExited) {
        try { $S.Proc.Kill() } catch { }
        try { [void]$S.Proc.WaitForExit(3000) } catch { }
    }
    $S.Proc = $null
}

function Remove-TempOut {
    foreach ($f in @($S.OutFile, $S.ErrFile)) {
        if ($f -and (Test-Path -LiteralPath $f)) { try { Remove-Item -LiteralPath $f -Force } catch { } }
    }
    $S.OutFile = $null
    $S.ErrFile = $null
}

function Complete-Run {
    param([bool]$Passed, [string]$Reason = '')
    $timer.Stop()
    Stop-Transfer
    Remove-TempOut
    $S.Phase = 'Idle'
    $dur = (Get-Date) - $S.RunStart
    $bar.Style = 'Continuous'
    $failDialog = $null

    if ($Passed) {
        $bar.Value = 100
        $c = Get-Counters
        $c.Pass++
        if ($S.Warnings -gt 0) {
            Set-Banner ("PASS  (with {0} warning(s))" -f $S.Warnings) $clrWarn
            Write-Log ("UNIT PASS with {0} warning(s) in {1}" -f $S.Warnings, (Format-Span $dur)) 'WARN'
        } else {
            Set-Banner 'PASS' $clrPass
            Write-Log ("UNIT PASS in {0}" -f (Format-Span $dur)) 'OK'
        }
        Set-Action 'Flash complete. Disconnect the board and fit the next one.'
        Write-Result 'PASS' $Reason
        if ($Config.BeepOnResult) { [System.Media.SystemSounds]::Asterisk.Play() }
    } else {
        $c = Get-Counters
        $c.Fail++
        Set-Banner 'FAIL' $clrFail
        Write-Log ("UNIT FAIL after {0}: {1}" -f (Format-Span $dur), $Reason) 'ERROR'
        Set-Action "FAILED: $Reason"
        Write-Result 'FAIL' $Reason
        if ($Config.BeepOnResult) { [System.Media.SystemSounds]::Hand.Play() }
        $failDialog = ("FLASH FAILED{0}{0}{1}{0}{0}Step: {2}{0}{0}Tag the unit, set it aside, and tell your supervisor if this repeats." -f "`r`n", $Reason, $S.FailedStep)
    }

    # Bring the whole UI up to date BEFORE the modal, so the banner, the step
    # list and the shift tally are already correct behind the dialog.
    Update-Counters
    Set-Running $false
    $txtSerial.Clear()
    $txtSerial.Focus()
    $form.Refresh()

    if ($failDialog) {
        [void][System.Windows.Forms.MessageBox]::Show($failDialog, 'FLASH FAILED', 'OK', 'Error')
    }

    if ($Passed -and $chkRepeat.Checked) {
        $S.Phase      = 'AwaitRemove'
        $S.PhaseStart = Get-Date
        Set-Action 'Repeat mode: remove the flashed board...'
        $timer.Start()
    }
}

function Fail-Step {
    param([string]$Reason)
    $st = $S.Steps[$S.StepIndex]

    # A step explicitly marked Critical=false in the config is allowed to fail:
    # it is logged loudly and counted as a warning, and the run carries on.
    if (-not $st.Critical) {
        $S.Warnings++
        Write-Log ("  step {0} FAILED but is configured non-critical - continuing: {1}" -f $st.Id, $Reason) 'WARN'
        Set-StepStatus $S.StepIndex 'SKIPPED' $clrWarn (Format-Span ((Get-Date) - $S.StepStart))
        Advance-Step
        return
    }

    $S.FailedStep = ("{0} - {1}" -f $st.Id, $st.Title)
    Set-StepStatus $S.StepIndex 'FAILED' $clrFail (Format-Span ((Get-Date) - $S.StepStart))
    Complete-Run $false $Reason
}

# ---------------------------------------------------------------------------
# The state machine
# ---------------------------------------------------------------------------
$timer.Add_Tick({
    try {
        switch ($S.Phase) {

            'WaitAlt' {
                $st      = $S.Steps[$S.StepIndex]
                $elapsed = (Get-Date) - $S.PhaseStart
                $left    = [int]$st.WaitSec - [int]$elapsed.TotalSeconds
                $what    = if ($st.Tool -eq 'fastboot') { 'fastboot' } else { $st.Alt }
                Set-Action ("Step {0}/{1} - waiting for '{2}' ... {3}s left" -f $st.Id, $S.Steps.Count, $what, [Math]::Max(0, $left))

                if ($elapsed.TotalSeconds -ge $st.WaitSec) {
                    if ($st.Tool -eq 'fastboot') {
                        Fail-Step ("Timed out after {0}s waiting for the board in fastboot mode. Check that step 3 loaded u-boot-flasher.img (not the production u-boot.img) and that WinUSB (Zadig) is bound to the board's USB device (0451:6165)." -f $st.WaitSec)
                    } else {
                        Fail-Step ("Timed out after {0}s waiting for the board to present DFU interface '{1}'. Check USB cable, power, boot mode switches and the Zadig/WinUSB driver." -f $st.WaitSec, $st.Alt)
                    }
                    return
                }
                if (((Get-Date) - $S.LastPoll).TotalMilliseconds -lt 1000) { return }
                $S.LastPoll = Get-Date

                if ($st.Tool -eq 'fastboot') {
                    $d = Get-FastbootDevices
                    if ($null -eq $d) {
                        Fail-Step 'fastboot.exe stopped responding. Unplug and replug the board, then retry.'
                        return
                    }
                    if ($d.Devices -gt 1) {
                        Fail-Step ("{0} fastboot devices are connected to this PC. Only one board may be connected at a time." -f $d.Devices)
                        return
                    }
                    if ($d.Devices -eq 1) {
                        if ($d.Serial -and -not $S.DfuSerial) { $S.DfuSerial = $d.Serial; Write-Log "  device serial: $($d.Serial)" 'INFO' }
                        Write-Log ("  fastboot device found after {0:N1}s" -f $elapsed.TotalSeconds) 'OK'
                        Start-Transfer
                    }
                    return
                }

                $d = Get-DfuDevices
                if ($null -eq $d) {
                    Fail-Step 'dfu-util -l stopped responding. Unplug and replug the board, then retry. If it persists, re-run Zadig.'
                    return
                }
                if ($d.Devices -gt 1) {
                    Fail-Step ("{0} DFU devices are connected to this PC. Only one board may be connected at a time." -f $d.Devices)
                    return
                }
                if ($d.Alts -contains $st.Alt) {
                    if ($d.Serial -and -not $S.DfuSerial) { $S.DfuSerial = $d.Serial; Write-Log "  device serial: $($d.Serial)" 'INFO' }
                    if ($d.Path   -and -not $S.DevPath)   { $S.DevPath   = $d.Path;   Write-Log "  usb path: $($d.Path)" 'INFO' }
                    Write-Log ("  found '{0}' after {1:N1}s" -f $st.Alt, $elapsed.TotalSeconds) 'OK'
                    Start-Transfer
                }
            }

            'Xfer' {
                $st      = $S.Steps[$S.StepIndex]
                $elapsed = (Get-Date) - $S.PhaseStart

                $tail = Get-FileTail $S.OutFile
                if ($st.Tool -eq 'fastboot') {
                    # fastboot talks on stderr: "Sending sparse 'mmc0' 3/7 (262140 KB)" per piece.
                    $tail = $tail + "`n" + (Get-FileTail $S.ErrFile)
                    $pm   = [regex]::Matches($tail, "Sending(?: sparse)? '[^']*' (\d+)/(\d+)")
                    if ($pm.Count -gt 0) {
                        $last = $pm[$pm.Count - 1]
                        $tail = ("{0}%" -f [int](100 * [int]$last.Groups[1].Value / [Math]::Max(1, [int]$last.Groups[2].Value)))
                    }
                }
                $ms   = [regex]::Matches($tail, '(\d{1,3})%')
                if ($ms.Count -gt 0) {
                    $pct = [int]$ms[$ms.Count - 1].Groups[1].Value
                    if ($pct -ge 0 -and $pct -le 100) {
                        $bar.Value = $pct
                        if ($pct -ne $S.LastPct) {
                            $S.LastPct = $pct
                            Set-Action ("Step {0}/{1} - writing {2}  {3}%   elapsed {4}" -f $st.Id, $S.Steps.Count, $st.File, $pct, (Format-Span $elapsed))
                        }
                    }
                } else {
                    Set-Action ("Step {0}/{1} - writing {2}   elapsed {3} / max {4}s" -f $st.Id, $S.Steps.Count, $st.File, (Format-Span $elapsed), $st.XferSec)
                }

                if ($S.Proc -and $S.Proc.HasExited) {
                    $code = $null
                    try { $code = $S.Proc.ExitCode } catch { }
                    Start-Sleep -Milliseconds 120          # let the redirect flush
                    $out  = Get-FileTail $S.OutFile 16384
                    $err  = Get-FileTail $S.ErrFile 16384
                    $all  = "$out`n$err"
                    $S.Proc = $null

                    if ($st.Tool -eq 'fastboot') {
                        # "Finished. Total time: 12.345s" with every stage OKAY; any FAILED line is a failure.
                        $success = ($all -match 'Finished\.') -and ($all -notmatch 'FAILED') -and ($code -eq 0)
                    } else {
                        $success = ($all -match 'File downloaded successfully') -or ($all -match 'Download\s+done')
                    }

                    foreach ($ln in ($all -split "[`r`n]+")) {
                        $t = $ln.Trim()
                        if ($t -and $t -notmatch '^\[?=*\]?$' -and $t -notmatch '^Copyright' -and
                            $t -notmatch '^dfu-util 0' -and $t -notmatch 'ABSOLUTELY NO WARRANTY' -and
                            $t -notmatch 'Please report bugs' -and $t -notmatch 'Free Software') {
                            Write-Log "    | $t" 'INFO'
                        }
                    }

                    if (-not $success) {
                        Remove-TempOut
                        if ($st.Tool -eq 'fastboot') {
                            Fail-Step ("fastboot did not confirm {0} (exit code {1}). See the log above." -f $(if ($st.File) { $st.File } else { $st.Command }), $(if ($null -eq $code) { 'unknown' } else { $code }))
                        } else {
                            Fail-Step ("dfu-util did not confirm the download of {0} (exit code {1}). See the log above." -f $st.File, $(if ($null -eq $code) { 'unknown' } else { $code }))
                        }
                        return
                    }
                    if ($null -ne $code -and $code -ne 0 -and $st.Tool -ne 'fastboot') {
                        # Data went through but the trailing USB reset complained. Common with -R
                        # on Windows/WinUSB and not by itself a bad board - but it IS recorded.
                        # Only warn on a code we actually read: an unreadable code must not
                        # warn on every step and turn every good unit amber.
                        $S.Warnings++
                        Write-Log ("  dfu-util exit code {0} after a successful download (usually the -R USB reset). Recorded as a warning." -f $code) 'WARN'
                    }

                    $span = (Get-Date) - $S.StepStart
                    Set-StepStatus $S.StepIndex 'OK' $clrPass (Format-Span $span)
                    Write-Log ("  step {0} complete in {1}" -f $st.Id, (Format-Span $span)) 'OK'
                    Remove-TempOut

                    $bar.Value = [int](100 * ($S.StepIndex + 1) / $S.Steps.Count)

                    if ($st.PostDelaySec -gt 0) {
                        $S.Phase = 'PostDelay'
                        $S.PhaseStart = Get-Date
                        Write-Log ("  settle {0}s" -f $st.PostDelaySec) 'INFO'
                    } else {
                        Advance-Step
                    }
                    return
                }

                if ($elapsed.TotalSeconds -ge $st.XferSec) {
                    Stop-Transfer
                    Remove-TempOut
                    Fail-Step ("Transfer of {0} exceeded its {1}s limit and was aborted. The board may have dropped off the USB bus." -f $st.File, $st.XferSec)
                    return
                }
            }

            'PostDelay' {
                $st = $S.Steps[$S.StepIndex]
                $elapsed = (Get-Date) - $S.PhaseStart
                Set-Action ("Waiting {0}s for the board to restart..." -f $st.PostDelaySec)
                if ($elapsed.TotalSeconds -ge $st.PostDelaySec) { Advance-Step }
            }

            'AwaitRemove' {
                if (((Get-Date) - $S.LastPoll).TotalMilliseconds -lt 1500) { return }
                $S.LastPoll = Get-Date
                $d = Get-DfuDevices
                if ($null -ne $d -and $d.Devices -eq 0) {
                    $S.Phase = 'AwaitNext'
                    Set-Banner 'READY - CONNECT NEXT BOARD' $clrIdle
                    Set-Action 'Repeat mode: connect the next board, flashing starts automatically.'
                }
            }

            'AwaitNext' {
                if (-not $chkRepeat.Checked) { $timer.Stop(); $S.Phase = 'Idle'; return }
                if (((Get-Date) - $S.LastPoll).TotalMilliseconds -lt 1500) { return }
                $S.LastPoll = Get-Date
                $d = Get-DfuDevices
                if ($null -ne $d -and ($d.Alts -contains 'bootloader')) {
                    $timer.Stop()
                    $S.Phase = 'Idle'
                    Start-Run
                }
            }
        }
    } catch {
        $timer.Stop()
        Write-Log ("Internal error: {0}" -f $_.Exception.Message) 'ERROR'
        try { Complete-Run $false ("Internal error: {0}" -f $_.Exception.Message) } catch { }
    }
})

function Advance-Step {
    $S.StepIndex++
    if ($S.StepIndex -ge $S.Steps.Count) {
        Complete-Run $true
    } else {
        Enter-WaitAlt
    }
}

# ===========================================================================
# Wiring
# ===========================================================================
$btnStart.Add_Click({ Start-Run })

$btnAbort.Add_Click({
    if ($S.Phase -eq 'Idle') { return }
    $r = [System.Windows.Forms.MessageBox]::Show(
        'Abort the flash in progress? The board will be left in an unknown state and must be re-flashed.',
        'Abort', 'YesNo', 'Warning')
    if ($r -ne 'Yes') { return }
    $timer.Stop()
    Stop-Transfer
    Remove-TempOut
    if ($S.StepIndex -ge 0 -and $S.StepIndex -lt $lv.Items.Count) {
        Set-StepStatus $S.StepIndex 'ABORTED' $clrWarn
        $st = $S.Steps[$S.StepIndex]
        $S.FailedStep = ("{0} - {1}" -f $st.Id, $st.Title)
    }
    $S.Phase = 'Idle'
    Set-Banner 'ABORTED' $clrWarn
    Set-Action 'Aborted by operator. Re-flash this board before shipping it.'
    Write-Log 'Aborted by operator.' 'WARN'
    Write-Result 'ABORT' 'Operator abort'
    Set-Running $false
})

$btnVerify.Add_Click({ [void](Invoke-Preflight) })

$btnLogs.Add_Click({ Start-Process explorer.exe $LogDir })

$btnManifest.Add_Click({ New-Manifest })

$btnResetCount.Add_Click({
    $c = Get-Counters
    $c.Pass = 0
    $c.Fail = 0
    Update-Counters
    Write-Log ("Shift counters reset for {0}." -f $CurrentProduct.Display) 'INFO'
})

function Invoke-ProductSwitch {
    if ($S.Phase -ne 'Idle' -and $S.Phase -ne 'AwaitNext' -and $S.Phase -ne 'AwaitRemove') { return }
    # Stop the repeat-mode wait BEFORE the menu opens - a modal dialog still
    # pumps timer ticks, and a board plugged in mid-menu must not auto-start
    # a flash with the old product.
    $timer.Stop()
    $S.Phase = 'Idle'
    $sel = Show-ProductDialog -SwitchMode
    if (-not $sel) { return }
    if ($CurrentProduct -and $sel.Id -eq $CurrentProduct.Id) { return }
    Set-Product $sel
    Write-Log ("Product switched to {0}." -f $sel.Display) 'STEP'
    Reset-StepList
    Update-Counters
    [void](Invoke-Preflight)
}

$btnProduct.Add_Click({ Invoke-ProductSwitch })

$chkRepeat.Add_CheckedChanged({
    if (-not $chkRepeat.Checked -and ($S.Phase -eq 'AwaitRemove' -or $S.Phase -eq 'AwaitNext')) {
        $timer.Stop(); $S.Phase = 'Idle'
        Set-Action 'Repeat mode off.'
    }
})

# Barcode scanners send Enter - treat that as "start"
$txtSerial.Add_KeyDown({
    if ($_.KeyCode -eq 'Enter') {
        $_.SuppressKeyPress = $true
        if ($btnStart.Enabled) { Start-Run }
    }
})

$form.Add_KeyDown({
    if ($_.KeyCode -eq 'F5'     -and $btnStart.Enabled)   { Start-Run }
    if ($_.KeyCode -eq 'F2'     -and $btnProduct.Enabled) { Invoke-ProductSwitch }
    if ($_.KeyCode -eq 'Escape' -and $btnAbort.Enabled)   { $btnAbort.PerformClick() }
})

$form.Add_FormClosing({
    if ($S.Phase -ne 'Idle' -and $S.Phase -ne 'AwaitNext' -and $S.Phase -ne 'AwaitRemove') {
        $r = [System.Windows.Forms.MessageBox]::Show('A flash is still running. Close anyway?', 'Confirm', 'YesNo', 'Warning')
        if ($r -ne 'Yes') { $_.Cancel = $true; return }
    }
    $timer.Stop()
    Stop-Transfer
    Remove-TempOut
})

$form.Add_Shown({
    $form.Activate()
    try { $split.SplitterDistance = [int]($split.Width * 0.52) } catch { }
    Reset-StepList
    Update-Counters
    Write-Log "Flash Station v$AppVersion" 'STEP'
    Write-Log "Folder: $Root" 'INFO'
    [void](Invoke-Preflight)
    if ($Config.RequireOperatorId) { $txtOperator.Focus() } else { $txtSerial.Focus() }
})

# ---------------------------------------------------------------------------
# Startup: pick the product (menu, unless -Product preselects it), then run.
# ---------------------------------------------------------------------------
$startProduct = $null
if ($Product) {
    $want = ($Product -replace '[\s\-_]', '')
    foreach ($p in $Products) {
        if ((($p.Id -replace '[\s\-_]', '') -ieq $want) -or (($p.Display -replace '[\s\-_]', '') -ieq $want)) {
            $startProduct = $p; break
        }
    }
    if (-not $startProduct) {
        [void][System.Windows.Forms.MessageBox]::Show(
            ("Unknown -Product '{0}'. Valid values: {1}.`r`n`r`nThe product menu will be shown instead." -f $Product, (($Products | ForEach-Object { $_.Id }) -join ', ')),
            'Flash Station', 'OK', 'Warning')
    }
}
if (-not $startProduct) { $startProduct = Show-ProductDialog }
if (-not $startProduct) {
    # Operator closed the menu without choosing - do not open the station.
    $form.Dispose()
    return
}
Set-Product $startProduct

[void]$form.ShowDialog()
$form.Dispose()
