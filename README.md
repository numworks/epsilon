

<p align="center"><img src="https://github.com/Omega-Numworks/Omega-Design/blob/master/Omega-Banner.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Omega-Numworks/Omega/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Omega-Numworks/Omega.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
  </br>
  <a href="https://join.slack.com/t/omeganumworks/shared_invite/enQtODI0NjU5NDU2MDUwLTRkODc1NDgzNmJkMzNlZGEyYWU2YzNlOWY4NWZlZjk1ZTM0YzY0ZjJjYTgyYzYwNDA1ZWZiYmUyMjA2MGU0ZDM"><img alt="Slack" src="https://img.shields.io/badge/Chat-Slack-blue?labelColor=292929&logo=slack&style=for-the-badge" /></a>
  <a href="https://gitlab.com/joachim2lefournis/Omega/pipelines"><img alt="Pipeline" src="https://img.shields.io/gitlab/pipeline/joachim2lefournis/Omega/omega-master?labelColor=292929&logo=gitlab&style=for-the-badge" /></a>
</p>

## About

Omega is an extension to Numworks' Epsilon, the OS that runs on their calculator, which brings many features to it. Omega is for people who want to add features to the calculator, but cannot because they have been rejected by Numworks (and for reasons that are 100% understandable!). For example, there is no point in having a Dark Mode on the calculator, but it's fun, so we would add it into Omega!

## Some new features
- Adding symbolic calculation back into the calculator
- An app for RPN
- A periodic table app + all of the molar masses for the elements in the toolbox
- More steps for brightness (16 instead of 5)
- Different colors for the LED in exam mode
- 32 KB Python heap instead of 16 KB

The main new features are listed [here](https://github.com/Omega-Numworks/Omega/wiki/Main-features), and the complete changelog can be found [here](https://github.com/quentinguidee/Omega/wiki/Complete-changelog).

## Installation

First of all, follow **step 1** [here](https://www.numworks.com/resources/engineering/software/build/). Then:

<details>
  <summary><b>Model n0100</b></summary>

```
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make MODEL=n0100 clean
make MODEL=n0100 USERNAME="{Your name, max 15 characters}" -j4
make MODEL=n0100 epsilon_flash
```

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Model n0110</b></summary>

```
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make USERNAME="{Your name, max 15 characters}" -j4
make epsilon_flash
```

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Bin files</b></summary>
  
These can be used to distribute Omega (so that it can be flashed by anyone with [Webdfu_Numworks](https://ti-planet.github.io/webdfu_numworks/)).

```
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make MODEL=n0100 USERNAME="" -j8
make MODEL=n0100 USERNAME="" binpack -j8
make USERNAME="" -j8
make USERNAME="" binpack -j8
```

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.
  
</details>

<details>
  <summary><b>Web simulator</b></summary>
  
First, install emsdk :

```
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest-fastcomp
./emsdk activate latest-fastcomp
source emsdk_env.sh
```

Then, compile Omega :

```
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make PLATFORM=simulator TARGET=web USERNAME="{Your name, max 15 characters}" -j4
```

The simulator is now in `output/release/simulator/web/simulator.zip`

Important: Don't forget the `--recursive` tag, because Omega relies on submodules.
Also, you can change the number of processes that run in parallel during the build by changing the value of the `-j` flag.

</details>

## Contributing

To contribute, please refer to the [Wiki](https://github.com/Omega-Numworks/Omega/wiki/Contributing)

## Branches

* `omega-master` : stable releases
* `omega-dev` : beta releases
* `omega-hotfix` : developers only, branch to fix issues

## Related repositories

* [Omega CLI Installer](https://github.com/Omega-Numworks/Omega-CLI-Installer) `BETA`
* [Omega WEB Installer](https://github.com/Omega-Numworks/Omega-WEB-Installer) `BETA`
* [Omega RPN](https://github.com/Omega-Numworks/Omega-RPN)
* [Omega Atom](https://github.com/Omega-Numworks/Omega-Atom)

## Omega License

Omega is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).

---

<p align="center"><img src="docs/epsilon.svg?sanitize=true" alt="NumWorks Epsilon logo" height="70" ></p>

<!-- [![Build Status](https://github.com/numworks/epsilon/workflows/Continuous%20integration/badge.svg)](https://github.com/numworks/epsilon/actions?workflow=Continuous+integration) -->

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/numworks/epsilon/issues"><img alt="Issues" src="https://img.shields.io/github/issues/numworks/epsilon.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
</p>


Epsilon is a high-performance graphing calculator operating system. It includes eight apps that cover the high school mathematics curriculum.

You can try Epsilon straight from your browser in the [online simulator](https://www.numworks.com/simulator/).

## Diving in

We highly recommend you start by reading the [online documentation](https://www.numworks.com/resources/engineering/software/) for this project. You'll learn how to install the [SDK](https://www.numworks.com/resources/engineering/software/build/) and about the overall architecture of the Epsilon.

## Contributing

If you run into an issue, we would be very happy if you would file a bug on the [issue tracker](https://github.com/numworks/epsilon/issues).

We welcome contributions. For smaller changes just open a pull request straight away. For larger changes we recommend you open an issue first for discussion.

## License

NumWorks Epsilon is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode). NumWorks is a registered trademark.

