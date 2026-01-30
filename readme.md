# LocCounter

WATCH OUT!! THIS IS STILL SLOW BY ABOUT A FACTOR OF 500x IF WINDOWS ANTIVIRUS IS WATCHING THE FOLDER IT SCANS.
Disable the AV or add a exception for raw performance with this tool.

Counts lines in files and directories. Windows-only C++23 CLI using fast Win32 APIs.

## Build

- **CMake:** `cmake -B build && cmake --build build --config Release`
- **PowerShell:** `.\build.ps1`

## Run

```text
LocCounter [path]
```

If no path is given, uses the current directory. Prints per-file line counts and a total.
Tracks time taken and logs to a timestamp.log file in the current directory.
