<p align="center"><img src="https://github.com/Omega-Numworks/Omega-Design/blob/master/Omega-Themes.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Omega-Numworks/Omega-Themes/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Omega-Numworks/Omega-Themes.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
</p>

## About

Omega-Themes `BETA` is the theme engine of [Omega](https://github.com/Omega-Numworks/Omega), an extension to Numworks' Epsilon. This engine allows you to change the theme of Omega easily before installing the OS.

## Installation

While compiling Omega, add the `THEME_NAME` flag :

```
make THEME_NAME=the_name_of_the_theme -j4
```

There are 4 themes:
* Omega Light (`THEME_NAME=omega_light`)
* Omega Dark (`THEME_NAME=omega_dark`)
* Epsilon Light (`THEME_NAME=epsilon_light`)
* Epsilon Dark (`THEME_NAME=epsilon_dark`)

## 3rd party themes

To make your own theme, you can use our 3rd party theme system :
* Create a new repository with your theme (there is an example [here](https://github.com/Omega-Numworks/Omega-Theme-Example)). Note: You can put several themes in the same repository.
* It's done!

To install your new theme, use these flags during the compilation:

```
make THEME_REPO={your repository url} THEME_NAME={Your theme name}
```

Example:
```
make THEME_REPO=https://github.com/Omega-Numworks/Omega-Theme-Example.git THEME_NAME=omega_blue
```

## License

Omega-Themes is released under a [CC BY-NC-SA License](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode). NumWorks is a registered trademark.
