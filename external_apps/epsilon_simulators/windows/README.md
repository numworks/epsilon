# Prepare a Windows simulator

From Epsilon's root, build a simulator and copy it there.
```bash
make PLATFORM=macos ION_SIMULATOR_EXTERNAL_APP=1 epsilon.exe
cp output/release/macos/epsilon.exe external_apps/epsilon_simulators/windows/
```
