# SecurityAccess Seed & Key DLL

Tester-side "Seed & Key" DLL for the SecurityAccess (0x27) service
implemented in `04_CAPL/UDS_Service_27.cin` (ES95486-02 4.3, ASK variant:
subFunction 0x11 requestSeed / 0x12 sendKey, 8-byte Seed and Key).

## What this is

CANoe's Diagnostics/ISO-TP "Security Access" configuration can call an
external DLL to compute the Key for a Seed it received, instead of the user
typing the Key in by hand every time. The exported function CANoe calls is
`GenerateKeyExOpt` - this is a real, documented Vector interface (see
`GenerateKeyExOpt.h`'s header comment for the two sources this was verified
against), not something invented for this project.

The Key algorithm itself (`Key[i] = (Seed[i] + 0x2A) & 0xFF`, i.e. add the
fixed constant 0x2A (42) to each Seed byte, wrapping around mod 256) **is**
project-specific - ES95486-02 explicitly leaves the actual Seed/Key
algorithm up to each ECU vendor, it only standardizes the service's
request/response byte layout. It was deliberately kept this simple (plain
addition, no bit tricks) for readability. See the header comment in
`04_CAPL/UDS_Service_27.cin` for the full algorithm definition. This same
formula is implemented in both `UDS_Service_27.cin` (CAPL, ECU side) and
`GenerateKeyExOpt.cpp` (this DLL, tester side) - they were cross-checked
against a third, independent Python implementation (`verify_algorithm.py`)
before delivery.

## Files

| File | Purpose |
|---|---|
| `GenerateKeyExOpt.h` / `.cpp` | The DLL source itself |
| `build_mingw.bat` | Builds the DLL with MinGW-w64 g++ (free, recommended) |
| `build_msvc.bat` | Builds the DLL with MSVC cl.exe (needs Visual Studio / Build Tools) |
| `self_test.cpp` | Standalone test program - links the .cpp directly, no DLL/CANoe needed |
| `verify_algorithm.py` | Independent Python re-implementation used to cross-check the algorithm before this code was written |

**This project's cloud sandbox could not compile an actual Windows .dll**
(no Windows cross-compiler available there) - the `.cpp`/`.h` source is
fully written and was compiled + run successfully as a native (non-DLL)
test program to confirm the algorithm logic is correct and warning-free,
but the actual `.dll` file must be built locally using one of the two
`build_*.bat` scripts below.

## Building the DLL (pick one)

### Option A - MinGW-w64 (free, no Visual Studio needed)

1. If you don't have it: install MinGW-w64. Easiest is
   [WinLibs](https://winlibs.com/) - download a "UCRT runtime" x86_64
   build, unzip anywhere, add its `bin\` folder to your PATH.
2. Double-click `build_mingw.bat` in this folder (or run it from a normal
   cmd/PowerShell window).
3. This produces `SecurityAccessDLL.dll` in this folder.

### Option B - MSVC (if you already have Visual Studio)

1. Open an **"x64 Native Tools Command Prompt for VS"** (Start Menu, under
   the Visual Studio folder) - a plain cmd/PowerShell will NOT have
   `cl.exe` on PATH.
2. `cd` into this folder and run `build_msvc.bat`.
3. This produces `SecurityAccessDLL.dll` in this folder.

## Verifying the build (optional but recommended)

Before wiring the DLL into CANoe, you can sanity-check it standalone:

```
cl /EHsc self_test.cpp GenerateKeyExOpt.cpp /Fe:self_test.exe   REM MSVC
g++ -O2 -o self_test.exe self_test.cpp GenerateKeyExOpt.cpp     REM MinGW
self_test.exe
```

Expected output ends with `ALL CHECKS PASSED`. If it doesn't, do not wire
the DLL into CANoe yet - something in the local build differs from the
delivered source.

## Wiring it into CANoe

This has been **confirmed live** in CANoe - the DLL was built locally,
wired in, and the request-seed / send-key exchange against
`UDS_Service_27.cin`'s SecurityAccess (0x27) implementation was verified
working end-to-end.

Location (in the "Diagnostics/ISO TP Configuration" dialog already used
for this project's addressing/frame-type settings): the per-ECU
"Diagnostic Layer" node in that same dialog tree has a Security Access tab
where you point to a DLL path and select `GenerateKeyExOpt` as the
function. Configure:

- DLL path: the `SecurityAccessDLL.dll` built above
- Security level / variant: `0x11` (only level this project implements)

With this wired up, a request-seed / send-key exchange in the Diagnostics
Console computes the Key automatically instead of requiring it to be
pasted in by hand.
