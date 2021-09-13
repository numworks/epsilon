<p align="center"><img src="https://github.com/Laporte12974/UpsilonDesign/blob/89a15953ae128aef8aa7d066dcaaf8d5c70f02a5/UPSILogo.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Lauryy06/Upsilon/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Lauryy06/Upsilon.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
  <br/>
  <a href="https://discord.gg/Q9buEMduXG"><img alt="Discord" src="https://img.shields.io/discord/663420259851567114?color=blue&labelColor=292929&label=chat%20-%20discord&logo=discord&style=for-the-badge" /></a>
</p>

> Vous ne comprenez pas l'anglais ? vous êtes francophone ? Regardez le [*LISEZ-MOI* français](./README.fr.md) !

## About

Upsilon is a fork of Omega, an user-made OS that runs on the Numworks calculator, which brings many features to it, but was discontinued because of a policy change from Numworks. Upsilon is for the people who want to see a future for user-made OSes for Numworks, even after the closure and archiving of Omega.

### Some new features
- Enhancements for the Kandinsky python module
- A support for wallpapers
- Exernal apps
- A custom theme
- Operator overload for python
- Improvements for the Periodic table application
- *And everything that has been added to Omega before its termination!* [See Omega's changelog here](https://github.com/Omega-Numworks/Omega/wiki/Changelog) | [Main Omega features + screenshots](https://github.com/Omega-Numworks/Omega/wiki/Main-features).

## Installation

### Manual

*As of today, only the manual installation is available.*

First of all, follow **step 1** [here](https://www.numworks.com/resources/engineering/software/build/). Then:

<details>
  <summary><b>Model n0100</b></summary>

(note: you can change the `EPSILON_I18N=en` flag to `fr`, `nl`, `pt`, `it`, `de`, `es` or `hu`).

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make MODEL=n0100 clean
make MODEL=n0100 EPSILON_I18N=en OMEGA_USERNAME="{Your name, max 15 characters}" -j4
make MODEL=n0100 epsilon_flash
```

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Model n0110</b></summary>

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make clean
make OMEGA_USERNAME="{Your name, max 15 characters}" -j4
make epsilon_flash
```

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Bin files</b></summary>
  
These can be used to distribute Upsilon (so that it can be flashed by anyone with [Webdfu_Numworks](https://ti-planet.github.io/webdfu_numworks/)).

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Omega
git checkout omega-master
make clean
make MODEL=n0100 OMEGA_USERNAME="" -j8
make MODEL=n0100 OMEGA_USERNAME="" binpack -j8
make OMEGA_USERNAME="" -j8
make OMEGA_USERNAME="" binpack -j8
```

Important: Don't forget the `--recursive` tag, because Upsilon relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Web simulator</b></summary>
  
First, install emsdk :

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest-fastcomp
./emsdk activate latest-fastcomp
source emsdk_env.sh
```

Then, compile Upsilon :

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make clean
make PLATFORM=simulator TARGET=web OMEGA_USERNAME="{Your name, max 15 characters}" -j4
```

The simulator is now in `output/release/simulator/web/simulator.zip`

Important: Don't forget the `--recursive` tag, because Upsilon relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.

</details>

<details>
  <summary><b>3DS Simulator</b></summary>
  
You need devkitPro and devkitARM installed and in your path (instructions [here](https://devkitpro.org/wiki/Getting_Started))

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout --recursive omega-dev
make PLATFORM=simulator TARGET=3ds -j
```
You can then put epsilon.3dsx on a SD card to run it from the HBC or use 3dslink to launch it over the network:

```bash
3dslink output/release/simulator/3ds/epsilon.3dsx -a <3DS' IP ADDRESS>
```

</details>

If you need help, you can join our Discord server here : https://discord.gg/Q9buEMduXG

<a href="https://discord.gg/Q9buEMduXG"><p align="center"><img alt="Omega Banner Discord" src="https://user-images.githubusercontent.com/12123721/86287349-54ef5800-bbe8-11ea-80c1-34eb1f93eebd.png" /></p></a>
---

## Contributing

To contribute, please refer to [Omega's Wiki](https://github.com/Omega-Numworks/Omega/wiki/Contributing), the same rules apply here.

## Related repositories

Here are the main links toward Omega's different websites and repositories, that have been used for the creation of Upsilon.

* [Omega Themes](https://github.com/Omega-Numworks/Omega-Themes)
* [Omega Website](https://github.com/Omega-Numworks/Omega-Website)
* [Omega RPN `APP`](https://github.com/Omega-Numworks/Omega-RPN)
* [Omega Atomic `APP`](https://github.com/Omega-Numworks/Omega-Atomic)
* [Omega Design](https://github.com/Omega-Numworks/Omega-Design)
* [Omega Discord Bot](https://github.com/Omega-Numworks/Omega-Discord-Bot)
* [Omega App Template `BETA`](https://github.com/Omega-Numworks/Omega-App-Template)
* [External Apps](https://github.com/Omega-Numworks/External-Apps)

## About Epsilon

Upsilon is a fork of Omega, after the project's discontinuation.

Omega is a fork of Epsilon, a high-performance graphing calculator operating system. It includes eight apps that cover the high school mathematics curriculum.

You can try Epsilon straight from your browser in the [online simulator](https://www.numworks.com/simulator/).

## License

NumWorks is a registered trademark of NumWorks SAS, 24 Rue Godot de Mauroy, 75009 Paris, France.
Nintendo and Nintendo 3DS are registered trademarks of Nintendo of America Inc, 4600 150th Ave NE, Redmond, WA 98052, USA.
NumWorks SAS and Nintendo of America Inc aren't associated in any shape or form with this project.

* NumWorks Epsilon is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Omega is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Upsilon is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
