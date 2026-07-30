# Prepare a Windows simulator

Follow next instructions from [Epsilon's root](https://github.com/numworks/epsilon/tree/master/epsilon).

## Setup

If this is the first time you build an epsilon simulator, you need to run the epsilon's setup script.
```bash
chmod +x tools/setup.sh && ./tools/setup.sh
```

## Simulator

Build a simulator and copy the necessary files [here](./).
```bash
make PLATFORM=windows epsilon.exe
cp output/release/windows/epsilon.exe output/release/windows/libepsilon.a external_apps/epsilon_simulators/windows/
```
