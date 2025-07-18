# Build and run your own version of Epsilon

The numworks calculator supports running unofficial software.

You can create and run you very own epsilon version.

This unofficial software will have more limited permissions :
- Any reset or crash will irreversibly switch back to an official firmware
- Exam mode cannot be set

## Retrieve the source code

The code is hosted on <a href="https://github.com/numworks/epsilon">GitHub</a>. You can retrieve it using with
```shell
git clone https://github.com/numworks/epsilon.git
cd epsilon
```

## Install the SDK

> [!NOTE]
> For **Windows** users, we recommend using the [Msys2](https://www.msys2.org/) environment and use `MINGW64` terminal to install most of the required tools.

To build and run epsilon, setup your device using the following command in your terminal (Msys2, Terminal.app, xterm…)
```shell
chmod +x build/setup.sh & build/setup.sh
```

## Run Epsilon on your computer

Once the SDK has been installed, just open your terminal, navigate to the epsilon folder and type the following commands:

```shell
make PLATFORM=simulator clean
make -j8 PLATFORM=simulator epsilon.app.run
```

## Run Epsilon on your calculator as a third-party firmware

You can also update your NumWorks calculator with our own custom firmware.

> [!WARNING]
> Flashing a custom firmware will erase most of your calculator's data. Save your python scripts beforehand.

> [!NOTE]
> After a reset, your calculator will exclusively run an official NumWorks software.

### Prerequisites

To install your custom firmware :
- Your calculator must be running on an official firmware.
- Your custom firmware must have the same version number `APP_VERSION` (you can change it in [root's Makefile](/Makefile))

### Installation steps

Plug your calculator to the `The calculator is connected` menu.

![Calculator is connected screenshot](calculator_connected.png)

Build and flash the custom userland with the command
```shell
make -j8 custom_userland.flash
```

The software should jump on your custom firmware with an `UNOFFICIAL SOFTWARE` warning :

![Unofficial software screenshot](unofficial_software.png)

Congratulations, you're running your very own version of Epsilon!

<details>
<summary>Detailed steps</summary>

You calculator actually holds two copies of the firmware, on two slots: **A** and **B**.

This allows the update of one of the slots while running the firmware of the other slot.

While on a official firmware, you can jump on the inactive slot to run a custom firmware.

After a reset, the calculator jumps on the slots containing the most recent version of the official NumWorks firmware.

To know which slot and model you are currently running on, plug your calculator on the `The calculator is connected` menu and run
```shell
python3 build/device/dfu.py -l
```

The `custom_userland.flash` target detects the model and inactive slot of connected device. It then flashes the appropriate firmware and jumps at the expected address.

For example, with a `n0120` model, and an inactive slot B.
- Build the corresponding target:
```shell
make -j8 PLATFORM=n0120 userland.B.dfu
```
- Flash the built firmware and leave:
```shell
python3 build/device/dfu.py -s 0x90410000:leave -D output/release/n0120/userland.B.dfu
```

These two steps can be merged with the single target:
```shell
make -j8 PLATFORM=n0120 DFULEAVE=0x90410000 userland.B.flash
```

With an inactive slot A, the command would be:
```shell
make -j8 PLATFORM=n0120 DFULEAVE=0x90010000 userland.A.flash
```

</details>

## Troubleshooting

- If you encounter an error when flashing the device, make sure you've closed all web pages that could interact with the calculator (such as https://my.numworks.com/).

- If your device is not detected (`python3 build/device/dfu.py -l` failing), try with another cable and another port of your computer.

<img style="float: right;" src="official_upgrade_required.png">

- After a custom firmware installation, if the screen displays the warning `OFFICIAL UPGRADE REQUIRED`, it means the custom firmware is incompatible with you current official firmware version, you may need to clean your output folder to re-build with the proper version.

```shell
make clean PLATFORM=[MODEL]
```
