# Prepare a MacOS simulator

Follow next instructions from epsilon's root.

## Setup

If this is the first time you build an epsilon simulator, you need to run the epsilon's setup script.
```bash
chmod +x build/setup.sh & build/setup.sh
```

## Simulator

Build a simulator and copy it there.
```bash
make PLATFORM=macos epsilon.app
cp -r output/release/macos/epsilon.app external_apps/epsilon_simulators/macos/
```
