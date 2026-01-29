try {
    if (Test-Path -Path build) {
        Remove-Item -Path build -Recurse
    }
    mkdir build
    Set-Location build
    cmake ..
    cmake --build . --config Release
    Set-Location ..
}
catch {
    Write-Error "Error: $($_.Exception.Message)"
    Set-Location ..
    exit 1
}