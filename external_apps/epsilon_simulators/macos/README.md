# Prepare a MacOS simulator

From Epsilon's root, build a simulator and copy it there.
```bash
make PLATFORM=macos epsilon.app
cp -r output/release/macos/epsilon.app external_apps/epsilon_simulators/macos/
```
