# Prepare a web simulator

## Setup

### First time setup

#### Install EMSDK

From the [external_apps folder](/external_apps), depending on your system, run
- Windows:
```shell
./setup_web.ps1
```
- Others:
```shell
chmod +x setup_web.sh & ./setup_web.sh
```
Follow the setup's next steps to set `EMSDK` in your path.

#### Epsilon setup

If not already done, you need to run the epsilon's setup script from [Epsilon's root](/).
```bash
chmod +x build/setup.sh & build/setup.sh
```

### Second time setup

Make sure your environment variable `EMSDK` has been set to the emsdk clone you installed the first time.

## Simulator

From [Epsilon's root](/), build a simulator and copy it there:
```bash
make PLATFORM=web WEB_EXTERNAL_APPS=1 epsilon.html
cp output/release/web/epsilon.html external_apps/epsilon_simulators/web/
```
