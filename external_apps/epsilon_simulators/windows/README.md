# Prepare a Windows simulator

Follow next instructions from epsilon's root.

## Setup

If this is the first time you build an epsilon simulator, you need to run the epsilon's setup script.
```bash
chmod +x build/setup.sh & build/setup.sh
```

## Simulator

Build a simulator and copy the necessary files [here](./).
```bash
make PLATFORM=windows epsilon.exe
cp output/release/windows/epsilon.exe output/release/windows/libepsilon.a external_apps/epsilon_simulators/windows/
```
