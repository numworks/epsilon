<p align="center"><img src="https://github.com/Laporte12974/UpsilonDesign/blob/89a15953ae128aef8aa7d066dcaaf8d5c70f02a5/UPSILogo.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/UpsilonNumworks/Upsilon/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Lauryy06/Upsilon.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
  <br/>
  <a href="https://discord.gg/sbGvhWETAd"><img alt="Discord" src="https://img.shields.io/discord/663420259851567114?color=blue&labelColor=292929&label=chat%20-%20discord&logo=discord&style=for-the-badge" /></a>
</p>

> Don't understand french ? speak english ? here's the [english README](./README.md) !

## À propos

Upsilon est un fork d'Omega, un fork d'Epsilon, l'OS de Numworks tournant sur les calculatrices du même nom, qui apporte beaucoup de fonctionnalités en plus, mais qui fut archivé et fermé pour des raisons légales après un changement de politique de Numworks. Upsilon est fait pour ceux qui aimeraient voir un futur pour les OS créées par les utilisateurs pour Numworks, même après l'arrèt du projet initial.

### Quelques fonctionnalités supplémentaires

- Un module python kandinsky amélioré
- Un support pour fonds d'écrans personnalisés
- Des applications externes
- Un thème Upsilon
- La surcharge des opérateurs en python
- Un tableau périodique légèrement amélioré
- L'utilisation possible du signe "=" dans les calculs
- *Ainsi que tout ce qui a été ajouté sur Omega, et bien plus...* [Changelogs complets d'Omega](https://github.com/Omega-Numworks/Omega/wiki/Changelog) | [Fonctionnalités principales d'Omega & captures d'écran](https://github.com/Omega-Numworks/Omega/wiki/Main-features).

## Installation

### Site web

Rendez-vous sur le [site d'Upsilon](https://getupsilon.web.app/) à la section "Installer".
Si votre calculatrice est reconnue, qu'elle contient une version d'Epsilon inférieure à 16 et que votre navigateur accepte WebUSB, la page vous proposera d'installer Upsilon.  
Ne débranchez votre calculatrice qu'une fois l'installation terminée.

### Manuelle

 *Vous pouvez vous référer à ce  [site internet](https://www.numworks.com/resources/engineering/software/build/)pour la première étape si vous avez des erreurs*

### 1. Installation du SDK

<br>

<details>

<summary><b>1.1 Linux</b></summary>

<br>

<details>

<summary>Debian ou Ubuntu</summary>

<br>

Il suffit juste d'installer les dépendances en tapant ces commandes dans un Terminal en mode super-utilisateur.

```bash
apt-get install build-essential git imagemagick libx11-dev libxext-dev libfreetype6-dev libpng-dev libjpeg-dev pkg-config gcc-arm-none-eabi binutils-arm-none-eabi
```

C'est fait! Vous pouvez aller à l'étape 2.

<br>

</details>

<details>

<summary>Fedora</summary>

<br>

Installez toutes les dépendances grâce à cette commande:

```bash
dnf install make automake gcc gcc-c++ kernel-devel git ImageMagick libX11-devel libXext-devel freetype-devel libpng-devel libjpeg-devel pkg-config arm-none-eabi-gcc-cs arm-none-eabi-gcc-cs-c++
```

<br>

</details>

<details>

<summary>Nix/Nixos</summary>

<br>

Installez toutes les dépendances grâce à cette commande:
```bash
nix-env -p gcc libpng libjpeg xorg.libX11 pkg-config freetype xorg.libXext python3 imagemagick python310Packages.lz4 python310Packages.pypng python310Packages.pypng gcc-arm-embedded
```

<br>

</details>

</details>

<details>

<summary><b>1.2 Mac</b></summary>

<br>

Il est recommandé d'utiliser [Homebrew](https://brew.sh/). Une fois intsallé, utilisez:

```bash
brew install numworks/tap/epsilon-sdk
```

Et toutes les dépendances seront installées.

<br>

Vous pouvez aller à l'étape 2.

<br>

</details>

<details>

<summary><b>1.3 Windows</b></summary>

[Git](http://git-scm.com) doit être installé.

<br>

<details>

<summary>Avec Msys2/Mingw (Supportés par Numwoks bien qu'il y ait beaucoup de bugs)</summary>

L'environnement de compilation [Msys2](https://www.msys2.org/) est recommandé par Numworks pour obtenir la plupart des outils requis facilement. C'est ici que vous allez copier-colletoutes lecommandes de ce tutoriel. Une fois installé, copier-coller ces deux commandes dans le terminal:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freetype mingw-w64-x86_64-pkg-config mingw-w64-x86_64-libusb git make python
echo "export PATH=/mingw64/bin:$PATH" >> .bashrc
```

Ensuite, vous devrez installer [GCC toolchain for ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). Quand il vouest demandde choisir u dossier d'installation, choisissez `C:\msys64\home\User\gcc-arm\`. Il vous faudra ensuite ajouter ce dossier à votre $PATH. Tapez juste:

```bash
echo "export PATH=$PATH:$HOME/gcc-arm/bin" >> .bashrc
```

Redémarrez votre terminal et vous pouvez aller à l'étape 2!

</details>

<details>

<summary>Avec WSL 2</summary>

WSL est un système qui virtualise un environnement GNU/Linux dans Windows.

Votre version de windows doit être >= 1903.

#### Installation de WSL

1. Apuyez simulatanément sur les touches "windows" et "x" puis cliquez sur "Powershell administrateur". Entrez ensuite ceci dans la nouvelle fenêtre:

```powershell
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
```

Cette commande active WSL

```powershell
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
```

Cette commande permet d'autoriser le démarrage des machines signées par Microsoft.

2. Redémarrez votre ordinateur.

3. Téléchargez [ce fichier](https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi) et suivez les instructions d'installation.

4. Ouvrez votre fenêtre powershell comme avant et tapez:

```powershell
wsl --set-default-version 2
```

5. téléchargez [Ubuntu](https://www.microsoft.com/store/apps/9n6svws3rx71) depuis le Microsoft store. Vous pouvez aussi installer [Debian](https://www.microsoft.com/store/productI9MSVKQC78PK6).

WSL est maintenant installé.

6. Installez maintenant la version pour ARM de GCC.
```bash
sudo apt-get install build-essential git imagemagick libx11-dev libxext-dev libfreetype6-dev libpng-dev libjpeg-dev pkg-config gcc-arm-none-eabi binutils-arm-none-eabi
```

### Installation d'usbipd pour connecter la calculatrice à WSL (facultatif)

Pour connecter la calculatrice, il faut installer cet [outil](https://github.com/dorssel/usbipd-win/releases/download/v1.3.0/usbipd-win_1.3.0.msi). Il permet de connecter deperiphériques USpar internet.Suivez les instructions pour installer.

#### Ubuntu

1. Dans un terminal WSL Ubuntu, tapez:

```bash
sudo apt install linux-tools-5.4.0-77-generic hwdata
```

2. Editez /etc/sudoers pour que l'on puisse utiliser la commande usbip. Sur Ubutu, cele est fait de cette manière:

```bash
sudo visudo
```

3. Ajoutez `/usr/lib/linux-tools/5.4.0-77-generic` au début du secure_path. Après édition, la ligne devrait ressembler à:
`Defaults secure_path="/usr/lib/linux-tools/5.4.0-77-generic:/usr/local/sbin:..."`

#### Debian

1.Si vous utiliser Debian, utilisez cette commande:

```bash
sudo apt install usbip hwdata usbutils
```

### Pour connecter la calculatrice à WSL

1. Ouvrez encore un powershell en mode administrateur et tapez:

```powershell
  usbipd wsl list
```

Ceci va lister les périphériques USB connectés à l'ordinateur. Reagrdez le BUSID de votre "Numworks Calculator".

2. Maintenant, lancez cette commande en remplçant <BUSID> par celui de votre caculatrice:

```powershell
usbipd wsl attach --busid <BUSID>
```

Le mot de passe de votre machine WSL vous sera demandé.

Vous pouvez aller à l'étape 2.

</details>

</details>

<br>

### 2. Récupérer le code source

Le code source est disponible dans une repository git. Récupérez-le de cette manière:

```bash
git clone --recursive https://github.com/UpsilonNumworks/Upsilon.git
cd Upsilon
git checkout upsilon-dev
```

<br>

### 3. Choisissez le système à compiler

<details>

<summary><b>Model n0100</b></summary>

(note: vous pouvez changer l'argument `EPSILON_I18N=en` avec `fr`, `nl`, `pt`, `it`, `de`, `es` or `hu`).

```bash
make MODEL=n0100 clean
make MODEL=n0100 EPSILON_I18N=en OMEGA_USERNAME="{Votre nom, maximum 15 caractères}" -j4
```

Maintenant, lancez soit:

```bash
make MODEL=n0100 epsilon_flash
```

pour directement flasher la calculatrice après avoir appuyé simultanément sur `reset` et `6` et avoir branché la calculatrice à l'ordinateur.

<br>

soit:

```bash
make MODEL=n0100 OMEGA_USERNAME="" binpack -j4
```

pour compiler les binpacks que vous pouvez distribuer et flasher depuis le [Ti-planet's webDFU](https://ti-planet.github.io/webdfu_numworks/n0100/).

</details>

<details>

<summary><b>Model n0110</b></summary>

Le bootloader vous permet d'installer firmware dans des "slots" séparés. Dans ce cas les applications externes ne pourront pas utiliser toute la mémoire mais la moitié. Si un seul slot est utilisé, le bootloader permettra d'utiliser toute la mémoire. Sans bootloader, les apps external peuvent utiliser toute la mémoire.

<details>
<summary>Bootloader</summary>

Votre calculatrice doit être flashé avec le bootloader d'[Upsilon](https://getupsilon.web.app) ou d'[Omega](https://getomega.dev).
Compilez avec:

```bash
make clean
make OMEGA_USERNAME="{Votre nom, max 15 caractères}" -j4
```

Ensuite lancez soit:

```bash
make epsilon.A_flash
```

pour flasher le slot actuel ou pour flasher par le flasher du booloader avec RESET, puis 4 (flash) et 1 (flash slots) pour flasher n'importe quel slot.

<br>

soit:

```bash
make OMEGA_USERNAME="{Votre nom, max 15 caractères}" binpack -j4
```

pour compiler les binpacks que vous pouvez distribuer et flasher depuis le [Ti-planet's webDFU](https://ti-planet.github.io/webdfu_numworks/n0100/). Vous les trouverez dans `output/release/device/bootloader/`.
</details>


<details>

<summary>Model n0110 sans bootloader (obsolète, utilisez le bootloader à la place pour la protection contre Epsilon)</summary>
Compilez avec:

```bash
make MODEL=n0110 clean
make MODEL=n0110 OMEGA_USERNAME="{Votre nom, max 15 caractères}" -j4
```

Ensuite lancez soit:

```bash
make MODEL=n0110 epsilon_flash
```

pour directement flasher la calculatrice après avoir appuyé simultanément sur `RESET` et `6` et avoir branché la calculatrice à l'ordinateur.
<br>

soit:

```bash
make MODEL=n0110 OMEGA_USERNAME="{Votre nom, max 15 caractères}" binpack -j4
```

pour compiler les binpacks que vous pouvez distribuer et flasher depuis le [Ti-planet's webDFU](https://ti-planet.github.io/webdfu_numworks/n0100/). Vous les trouverez dans `output/release/device/n0110/`.
</details>

</details>


<details>

<summary><b>Simulateur Natif</b></summary>

Lancez cette commande:
```bash
make clean
```
Vous pouvez soit choisir d'utiliser la commmande qui détectera automatiquement votre plateforme:
```bash
make PLATFORM=simulator
```
Ou choisir une commande qui correspond à votre plateforme:
```bash
make PLATFORM=simulator TARGET=android
make PLATFORM=simulator TARGET=ios
make PLATFORM=simulator TARGET=macos
make PLATFORM=simulator TARGET=web
make PLATFORM=simulator TARGET=windows
make PLATFORM=simulator TARGET=3ds
```

Vous trouverez les fichiers du simulateur dans `output/release/simulator/`.

</details>


<details>

<summary><b>Simulateur web</b></summary>

D'abord, installez emsdk :

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 1.40.1
./emsdk activate 1.40.1
source emsdk_env.sh
```

Puis, compilez Upsilon :

```bash
make clean
make PLATFORM=simulator TARGET=web OMEGA_USERNAME="{Votre nom, maximum 15 caractères}" -j4
```

Le simulateur se trouve dans `output/release/simulator/web/simulator.zip`

</details>

<details>

<summary><b>Simulateur pour 3DS</b></summary>

Il vous faut devkitPro et devkitARM installés et dans votre path (les instructions sont [ici](https://devkitpro.org/wiki/Getting_Started))

```bash
git clone --recursive https://github.com/UpsilonNumworks/Upsilon.git
cd Upsilon
git checkout --recursive upsilon-dev
make PLATFORM=simulator TARGET=3ds -j
```

Vous pouvez ensuite mettre epsilon.3dsx sur une carte SDpour le lancer depuis le HBC ou utilisez 3dslink pour le lancer via le réseau:

```bash
3dslink output/release/simulator/3ds/epsilon.3dsx -a <3DS' IP ADDRESS>
```

</details>

<br>

Important: n'oubliez pas l'argument `--recursive` Parce qu'Upsilon dépend de submodules.
Aussi, vous pouvez changer le nombre de processus de compilation en parallèles en changeant le nombre après l'argument `-j`.
N'oubliez pas de mettre votre nom à la place `{Votre nom, maximum 15 caractères}`.Si vous n'en voulez pas, enlevez l'argument `OMEGA_USERNAME`.

Si vous avez besoin d'aide, n'hésitez pas à rejoindre notre serveur discord : <https://discord.gg/Q9buEMduXG>

<a href="https://discord.gg/Q9buEMduXG"><p align="center"><img alt="Omega Banner Discord" src="https://user-images.githubusercontent.com/12123721/86287349-54ef5800-bbe8-11ea-80c1-34eb1f93eebd.png" /></p></a>
---

## Liens utiles

- [Upsilon external (pour installer des applications supplémentaires et des fonds d'écran)](https://upsilonnumworks.github.io/Upsilon-External/)
- [Documentation d'ulab](https://micropython-ulab.readthedocs.io/en/latest/)

## Contribution

Pour contribuer, merci de lire le [Wiki d'Omega](https://github.com/Omega-Numworks/Omega/wiki/Contributing), les mêmes règles s'appliquent ici.

## Les autres projets

Les anciens projets d'Omega, avant sa fermeture, qui ont été utilisés pour ce projet

- [Omega Themes](https://github.com/Omega-Numworks/Omega-Themes)
- [Omega Website](https://github.com/Omega-Numworks/Omega-Website)
- [Omega RPN `APP`](https://github.com/Omega-Numworks/Omega-RPN)
- [Omega Atomic `APP`](https://github.com/Omega-Numworks/Omega-Atomic)
- [Omega Design](https://github.com/Omega-Numworks/Omega-Design)
- [Omega Discord Bot](https://github.com/Omega-Numworks/Omega-Discord-Bot)
- [Omega App Template `BETA`](https://github.com/Omega-Numworks/Omega-App-Template)
- [External Apps](https://github.com/Omega-Numworks/External-Apps)

## À propos d'Epsilon

Upsilon est un fork d'Omega, visant a continuer le projet des OS utilisateurs pour Numworks

Omega est un fork d'Epsilon, un système d'exploitation performant pour calculatrices graphiques. Il inclut huit applications pour les mathématiques de lycée et d'études supérieurs

Vous pouvez essayer Epsilon depuis votre navigateur sur le [simulateur en ligne](https://www.numworks.com/simulator/).

## Licence

NumWorks est une marque déposée de NumWorks SAS, 24 Rue Godot de Mauroy, 75009 Paris, France.
Nintendo est Nintendo 3DS sont des marques déposées de Nintendo of America Inc, 4600 150th Ave NE, Redmond, WA 98052, Etats-Unis.
NumWorks SAS et Nintendo of America Inc ne sont en aucun cas associés avec ce projet.

- NumWorks Epsilon est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
- Omega est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
- Upsilon est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
