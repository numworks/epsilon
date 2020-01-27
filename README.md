
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

<p align="center"><img src="https://github.com/Omega-Numworks/Omega-Design/blob/master/Omega-Banner.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Omega-Numworks/Omega/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Omega-Numworks/Omega.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
  </br>
  <a href="https://discord.gg/hjH3gtd"><img alt="Discord" src="https://img.shields.io/discord/663420259851567114?color=blue&labelColor=292929&label=chat%20-%20discord&logo=discord&style=for-the-badge" /></a>
  <a href="https://gitlab.com/joachim2lefournis/Omega/pipelines"><img alt="Pipeline" src="https://img.shields.io/gitlab/pipeline/joachim2lefournis/Omega/omega-master?labelColor=292929&logo=gitlab&style=for-the-badge" /></a>
</p>

## About

Omega is a fork of Numworks' Epsilon, the OS that runs on their calculator, which brings many features to it. Omega is for the people who want to add features to the calculator, but cannot because they have been rejected by Numworks (for reasons that are 100% understandable!).

## Some new features
- Adding symbolic calculation back into the calculator
- An app for RPN
- A periodic table app + all of the molar masses for the elements in the toolbox
- More steps for brightness (16 instead of 5)
- 32 KB Python heap instead of 16 KB
- And more...

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

* [Omega Themes](https://github.com/Omega-Numworks/Omega-Themes)
* [Omega Website](https://github.com/Omega-Numworks/Omega-Website)
* [Omega RPN `APP`](https://github.com/Omega-Numworks/Omega-RPN)
* [Omega Atom `APP`](https://github.com/Omega-Numworks/Omega-Atom)
* [Omega Converter `APP`](https://github.com/Omega-Numworks/Omega-Converter)
* [Omega Design](https://github.com/Omega-Numworks/Omega-Design)
* [Omega CLI Installer `BETA`](https://github.com/Omega-Numworks/Omega-CLI-Installer)
* [Omega App Template `BETA`](https://github.com/Omega-Numworks/Omega-App-Template)

## About Epsilon

Omega is a fork of Epsilon, a high-performance graphing calculator operating system. It includes eight apps that cover the high school mathematics curriculum.

You can try Epsilon straight from your browser in the [online simulator](https://www.numworks.com/simulator/).

## License

NumWorks is a registered trademark. Omega is not affiliated with NumWorks.

* NumWorks Epsilon is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Omega is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/quentinguidee"><img src="https://avatars0.githubusercontent.com/u/12123721?v=4" width="100px;" alt=""/><br /><sub><b>Quentin</b></sub></a><br /><a href="https://github.com/Omega-Numworks/Omega/commits?author=quentinguidee" title="Code">💻</a> <a href="https://github.com/Omega-Numworks/Omega/commits?author=quentinguidee" title="Documentation">📖</a> <a href="#design-quentinguidee" title="Design">🎨</a> <a href="#maintenance-quentinguidee" title="Maintenance">🚧</a> <a href="#projectManagement-quentinguidee" title="Project Management">📆</a> <a href="#question-quentinguidee" title="Answering Questions">💬</a> <a href="https://github.com/Omega-Numworks/Omega/pulls?q=is%3Apr+reviewed-by%3Aquentinguidee" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="https://M4xi1m3.github.io/"><img src="https://avatars0.githubusercontent.com/u/57234558?v=4" width="100px;" alt=""/><br /><sub><b>M4x1m3</b></sub></a><br /><a href="https://github.com/Omega-Numworks/Omega/commits?author=M4xi1m3" title="Code">💻</a> <a href="https://github.com/Omega-Numworks/Omega/issues?q=author%3AM4xi1m3" title="Bug reports">🐛</a> <a href="https://github.com/Omega-Numworks/Omega/commits?author=M4xi1m3" title="Documentation">📖</a> <a href="#ideas-M4xi1m3" title="Ideas, Planning, & Feedback">🤔</a> <a href="#question-M4xi1m3" title="Answering Questions">💬</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!