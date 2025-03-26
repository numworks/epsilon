# Build and run your own version of Epsilon

## Install the SDK

> [!NOTE]
> For **Windows** users, we recommend using the [Msys2](https://www.msys2.org/) environment and use `MINGW64` terminal to install most of the required tools.

To build and run epsilon, setup your device using the following command in your terminal (Msys2, Terminal.app, xterm…)
```shell
chmod +x build/setup.sh & build/setup.sh
```

## Retrieve the source code

The code is hosted on <a href="https://github.com/numworks/epsilon">GitHub</a>. You can retrieve it using with
```
git clone https://github.com/numworks/epsilon.git
```

## Run Epsilon on your computer

Once the SDK has been installed, just open your terminal, navigate to the epsilon folder and type the following commands:

```
make PLATFORM=simulator clean
make -j8 PLATFORM=simulator epsilon.app.run
```

## Run Epsilon on your calculator as a third-party firmware

You can also update your NumWorks calculator with our own custom firmware.

> [!WARNING]
> Flashing a custom firmware will erase most of your calculator's data. Save your python scripts beforehand.

> [!NOTE]
> After a reset, your calculator will exclusively run an official NumWorks software.

### Calculator's slots

You calculator actually holds two copies of the firmware, on two slots :
- Slot **A**: Its userland is at the address `0x90010000`.
- Slot **B**: Its userland is at the address `0x90410000`.

This allows the update of one of the slots while running the firmware of the other slots.

On a reboot, the calculator jumps on the slots containing the most recent version of the official NumWorks firmware.

To know which slot you are currently running on, plug you calculator on the `The calculator is connected` menu and run `python3 build/device/dfu.py -l` to display the active slot of connected device.

<details>
<summary>Other method</summary>

In the `About` settings menu, press `OK` three time on the software version line. If the last figure is 8 or greater, you are running on the slot A.

Example:
| Value | Last figure | Slot |
|-|-|-|
| 0008000C | C | A |
| 00080004 | 4 | B |

</details>

### Prerequisites

To install your custom firmware :
- Your calculator must be running on a [slot](#calculators-slots) containing an official firmware.
- Your custom firmware must have the same version number `APP_VERSION` (you can change it in [root's Makefile](/Makefile))

### Installation steps

Plug your calculator to the `The calculator is connected` menu.

![Calculator is connected screenshot](calculator_connected.png)

Find out your calculator's model, and its [active slot](#calculators-slots).

Build and flash the custom userland on the other slot with the command
```bash
make -j8 PLATFORM=[MODEL] userland.[INACTIVE_SLOT].flash
```

**Examples**
For example, on a N0120 running on slot A, run :
```bash
make -j8 PLATFORM=n0120 userland.B.flash
```

The software should jump on your custom firmware with an `UNOFFICIAL SOFTWARE` warning :

![Unofficial software screenshot](unofficial_software.png)

Congratulations, you're running your very own version of Epsilon!

<details>
<summary>Detailed steps</summary>

When using the target `userland.B.flash`, you actually build the `userland.B.dfu` firmware and then flash it at the expected address.

The individual steps are
```bash
make -j8 PLATFORM=n0120 userland.B.dfu
python3 build/device/dfu.py -s 0x90410000:leave -D output/release/n0120/userland.B.dfu
```
With slot B, the address to jump on changes to :
```bash
make -j8 PLATFORM=n0120 userland.A.dfu
python3 build/device/dfu.py -s 0x90010000:leave -D output/release/n0120/userland.A.dfu
```
</details>

## Troubleshooting

- If you encounter an error when flashing the device, make sure you've closed all web pages that could interact with the calculator (such as https://my.numworks.com/).

- If your device is not detected (`python3 build/device/dfu.py -l` failing), try with another cable and another port of your computer.

<img style="float: right;" src="official_upgrade_required.png">

- After a custom firmware installation, if the screen displays the warning `OFFICIAL UPGRADE REQUIRED`, it means the custom firmware is incompatible with you current official firmware version, you may need to clean your output folder to re-build with the proper version.

```bash
make clean PLATFORM=[MODEL]
```
