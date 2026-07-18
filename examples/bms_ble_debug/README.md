# BMS BLE Debug Tool

Standalone ESP32 sketch for checking the BEXEL / LWS-1608 smart BMS BLE
protocol before relying on it from the cluster firmware.

Upstream protocol/code:
https://github.com/swallowlikeyoshi/bexel-v2-bluetooth-protocol

## Run

```powershell
cd C:\Users\hyun1\OneDrive\문서\HEVEN\github\cluster\examples\bms_ble_debug
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -t upload
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" device monitor
```

Expected serial output after connecting to `LWS-1608`:

```text
connected, polling...
Pack 57.40V  I 0mA  SOC 49%  SOH 100%  T 23C  39691mAh  cyc 0
Cells: 4099 4099 ...
```

This sketch is read-only. It polls summary frame `0x2A` and cell-voltage frame
`0x24`; it does not change BMS settings.
