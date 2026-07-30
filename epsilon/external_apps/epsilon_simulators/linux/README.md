# Prepare a Linux simulator

Follow next instructions from [Epsilon's root](https://github.com/numworks/epsilon/tree/master/epsilon).

## Setup

If this is the first time you build an epsilon simulator, you need to run the epsilon's setup script.
```bash
chmod +x tools/setup.sh && ./tools/setup.sh
```

## Simulator

Build a simulator and copy it there.
```bash
make PLATFORM=linux epsilon.bin
cp output/release/linux/epsilon.bin external_apps/epsilon_simulators/linux/
```
