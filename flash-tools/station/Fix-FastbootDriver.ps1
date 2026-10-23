<#  Register the Android device interface GUID on the AM62L U-Boot USB gadget.

    fastboot.exe only enumerates WinUSB devices that carry Android's interface
    GUID {F72FE0D4-CBCB-407D-8814-9ED673D0DD6B}. Zadig installs WinUSB with a
    random GUID (its dialog has no field for it, and the preset-file GUID did
    not take on this device), so "fastboot devices" stays empty while
    dfu-util works. This writes the GUID into the device's Device Parameters
    and restarts the device so WinUSB re-registers its interface.

    Run from an ADMIN PowerShell, with the board plugged in (any stage):
        Set-ExecutionPolicy -Scope Process Bypass -Force; .\Fix-FastbootDriver.ps1
    It touches only USB\VID_0451&PID_6165 instances. DFU is unaffected by the GUID.
#>
$guid = '{F72FE0D4-CBCB-407D-8814-9ED673D0DD6B}'
$devs = Get-PnpDevice | Where-Object { $_.InstanceId -like 'USB\VID_0451&PID_6165*' }
if (-not $devs) { Write-Host 'No AM62L USB device (0451:6165) known to Windows.'; exit 1 }
foreach ($d in $devs) {
    $key = 'HKLM:\SYSTEM\CurrentControlSet\Enum\' + $d.InstanceId + '\Device Parameters'
    if (-not (Test-Path $key)) { New-Item -Path $key -Force | Out-Null }
    $svc = (Get-PnpDeviceProperty -InstanceId $d.InstanceId -KeyName DEVPKEY_Device_Service -ErrorAction SilentlyContinue).Data
    Set-ItemProperty -Path $key -Name DeviceInterfaceGUIDs -Value @($guid) -Type MultiString
    Write-Host ("{0}  [{1}, driver {2}]  -> DeviceInterfaceGUIDs = {3}" -f $d.InstanceId, $d.Status, $svc, $guid)
    if ($d.Status -eq 'OK') {
        Disable-PnpDevice -InstanceId $d.InstanceId -Confirm:$false -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 2
        Enable-PnpDevice  -InstanceId $d.InstanceId -Confirm:$false -ErrorAction SilentlyContinue
        Write-Host '  device restarted'
    }
}
Start-Sleep -Seconds 2
$fb = Join-Path $PSScriptRoot 'fastboot.exe'
if (Test-Path $fb) { Write-Host '--- fastboot devices:'; & $fb devices }
