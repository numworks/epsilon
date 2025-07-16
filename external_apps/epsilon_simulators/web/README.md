# Prepare a web simulator

Follow next instructions from epsilon's root.

## Setup

If this is the first time you build an epsilon simulator, you need to run the epsilon's setup script.
```bash
chmod +x build/setup.sh & build/setup.sh
```

## Simulator

Build a simulator and copy it there.
```bash
make PLATFORM=web WEB_EXTERNAL_APPS=1 epsilon.html
cp output/release/web/epsilon.html external_apps/epsilon_simulators/web/
```
