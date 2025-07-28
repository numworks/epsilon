winget install --accept-package-agreements python
winget install ezwinports.make

$emsdkVersion = if (Test-Path ../.emsdk-version) {
    Get-Content ../.emsdk-version | Out-String | ForEach-Object { $_.Trim() }
} else {
    "latest"
}

git clone https://github.com/emscripten-core/emsdk.git
./emsdk/emsdk install $emsdkVersion
./emsdk/emsdk activate $emsdkVersion
