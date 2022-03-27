<p align="center"><img src="https://user-images.githubusercontent.com/12123721/160276449-891ac0fe-0034-45b0-b43f-0fcc81826dc0.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Omega-Numworks/Omega/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Omega-Numworks/Omega.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
  <br/>
  <a href="https://discord.gg/X2TWhh9"><img alt="Discord" src="https://img.shields.io/discord/663420259851567114?color=blue&labelColor=292929&label=chat%20-%20discord&logo=discord&style=for-the-badge" /></a>
</p>

> Don't understand french ? speak english ? here's the [english README](./README.md) !

## À propos

Omega est un fork d'Epsilon, l'OS de Numworks tournant sur les calculatrices du même nom, qui apporte beaucoup de fonctionnalités en plus. Omega est fait pour ceux qui aimeraient ajouter certaines fonctionnalités ayant été rejetées par Numworks à leurs calculatrices (pour des raisons 100% compréhensibles !). [Essayez en ligne](https://getomega.web.app/simulator).

### Quelques fonctionnalités supplémentaires
- Retour du calcul littéral
- Une application RPN
- Application Externes
- Des thèmes
- Python amélioré (module os, méthode open...)
- Un tableau périodique et toutes les masses molaires des éléments dans la toolbox
- *Ainsi que d'autres à découvrir...* [Changelogs complets](https://github.com/Omega-Numworks/Omega/wiki/Changelog) | [Fonctionnalités princpales & captures d'écran](https://github.com/Omega-Numworks/Omega/wiki/Main-features).

## Installation

### Automatique

Vous pouvez installer Omega automatiquement depuis [notre site](https://getomega.web.app/) sur la page "installer".

<a href="https://getomega.web.app"><p align="center"><img alt="Omega Banner Discord" src="https://user-images.githubusercontent.com/12123721/86352956-e9000480-bc66-11ea-82b7-79fd7e56fa27.png" /></p></a>

### Manuelle

Tout d'abord, suivez **la première étape** [ici](https://www.numworks.com/resources/engineering/software/build/), puis :

<details>
  <summary><b>Modèle n0100</b></summary>

(note : vous pouvez changer `EPSILON_I18N=fr` en `en`, `nl`, `pt`, `it`, `de`, `es` ou `hu`).

```bash
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make MODEL=n0100 clean
make MODEL=n0100 EPSILON_I18N=fr OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
make MODEL=n0100 epsilon_flash
```

Important : N'oubliez pas l'argument `--recursive`, Omega a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Modèle n0110</b></summary>

```bash
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
make epsilon_flash
```

Important : N'oubliez pas l'argument `--recursive`, Omega a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Fichiers binaires</b></summary>
  
Ces fichiers peuvent être utilisés pour distribuer Omega (pour que tout le monde puisse le flasher via [Webdfu_Numworks](https://ti-planet.github.io/webdfu_numworks/)).

```bash
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make MODEL=n0100 OMEGA_USERNAME="" -j8
make MODEL=n0100 OMEGA_USERNAME="" binpack -j8
make OMEGA_USERNAME="" -j8
make OMEGA_USERNAME="" binpack -j8
```

Important : N'oubliez pas l'argument `--recursive`, Omega a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.
  
</details>

<details>
  <summary><b>Simulateur web</b></summary>
  
D'abord, installez emsdk :

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest-fastcomp
./emsdk activate latest-fastcomp
source emsdk_env.sh
```

Puis, compilez Omega :

```bash
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout omega-master
make clean
make PLATFORM=simulator TARGET=web OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
```

Le simulateur se trouve dans `output/release/simulator/web/simulator.zip`

Important : N'oubliez pas l'argument `--recursive`, Omega a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Simulateur 3DS</b></summary>
  
Vous aurez besoin de devkitPro et de devkitARM disponible dans votre `$PATH` (instructions [ici](https://devkitpro.org/wiki/Getting_Started) (en anglais))

```bash
git clone --recursive https://github.com/Omega-Numworks/Omega.git
cd Omega
git checkout --recursive omega-dev
make PLATFORM=simulator TARGET=3ds -j
```

Vous pouvez ensuite copier epsilon.3dsx sur une carte SD pour l'exécuter depuis le HBC ou utiliser 3dslink pour le lancer via le réseau :

```bash
3dslink output/release/simulator/3ds/epsilon.3dsx -a <ADRESSE IP 3DS>
```

</details>

Si vous avez besoin d'aide, n'hésitez pas à rejoindre notre serveur discord : https://discord.gg/X2TWhh9

<a href="https://discord.gg/X2TWhh9"><p align="center"><img alt="Omega Banner Discord" src="https://user-images.githubusercontent.com/12123721/86287349-54ef5800-bbe8-11ea-80c1-34eb1f93eebd.png" /></p></a>
---

## Contribution

Pour contribuer, merci de lire le [Wiki](https://github.com/Omega-Numworks/Omega/wiki/Contributing)

## Nos autres projets

* [Omega Themes](https://github.com/Omega-Numworks/Omega-Themes)
* [Omega Website](https://github.com/Omega-Numworks/Omega-Website)
* [Omega RPN `APP`](https://github.com/Omega-Numworks/Omega-RPN)
* [Omega Atomic `APP`](https://github.com/Omega-Numworks/Omega-Atomic)
* [Omega Design](https://github.com/Omega-Numworks/Omega-Design)
* [Omega Discord Bot](https://github.com/Omega-Numworks/Omega-Discord-Bot)
* [Omega App Template `BETA`](https://github.com/Omega-Numworks/Omega-App-Template)
* [External Apps](https://github.com/Omega-Numworks/External-Apps)

## À propos d'Epsilon

Omega est un fork d'Epsilon, un système d'exploitation performant pour calculatrices graphiques. Il inclut huit applications pour les mathématiques de lycée et d'études supérieurs

Vous pouvez essayer Epsilon depuis votre navigateur sur le [simulateur en ligne](https://www.numworks.com/simulator/).

## Licence

NumWorks est une marque déposée de NumWorks SAS, 24 Rue Godot de Mauroy, 75009 Paris, France.
Nintendo est Nintendo 3DS sont des marques déposées de Nintendo of America Inc, 4600 150th Ave NE, Redmond, WA 98052, Etats-Unis.
NumWorks SAS et Nintendo of America Inc ne sont en aucun cas associés avec ce projet.

* NumWorks Epsilon est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Omega est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
