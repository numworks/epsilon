<p align="center"><img src="https://github.com/Laporte12974/UpsilonDesign/blob/89a15953ae128aef8aa7d066dcaaf8d5c70f02a5/UPSILogo.png" /></p>

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="cc by-nc-sa 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/Lauryy06/Upsilon/issues"><img alt="Issues" src="https://img.shields.io/github/issues/Lauryy06/Upsilon.svg?labelColor=292929&logo=git&style=for-the-badge" /></a>
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
Rendez-vous sur le [site d'Upsilon](https://lolocomotive.github.io/Upsilon-website) à la section "Installer". 
Si votre calculatrice est reconnue, qu'elle contient une version d'Epsilon inférieure à 16 et que votre navigateur accepte WebUSB, la page vous proposera d'installer Upsilon.  
Ne débranchez votre calculatrice qu'une fois l'installation terminée.

### Manuelle

Tout d'abord, suivez **la première étape** [ici](https://www.numworks.com/resources/engineering/software/build/), puis :

<details>
  <summary><b>Modèle n0100</b></summary>

(note : vous pouvez changer `EPSILON_I18N=fr` en `en`, `nl`, `pt`, `it`, `de`, `es` ou `hu`).

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make MODEL=n0100 clean
make MODEL=n0100 EPSILON_I18N=fr OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
make MODEL=n0100 epsilon_flash
```

Important : N'oubliez pas l'argument `--recursive`, Upsilon a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Modèle n0110</b></summary>

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make clean
make OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
make epsilon_flash
```

Important : N'oubliez pas l'argument `--recursive`, Upsilon a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Fichiers binaires</b></summary>
  
Ces fichiers peuvent être utilisés pour distribuer Upsilon (pour que tout le monde puisse le flasher via [Webdfu_Numworks](https://ti-planet.github.io/webdfu_numworks/)).

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make clean
make MODEL=n0100 OMEGA_USERNAME="" -j8
make MODEL=n0100 OMEGA_USERNAME="" binpack -j8
make OMEGA_USERNAME="" -j8
make OMEGA_USERNAME="" binpack -j8
```

Important : N'oubliez pas l'argument `--recursive`, Upsilon a besoin de sous-modules.
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

Puis, compilez Upsilon :

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout omega-master
make clean
make PLATFORM=simulator TARGET=web OMEGA_USERNAME="{Votre nom ici, 15 caractères max}" -j4
```

Le simulateur se trouve dans `output/release/simulator/web/simulator.zip`

Important : N'oubliez pas l'argument `--recursive`, Upsilon a besoin de sous-modules.
Vous pouvez aussi changer le nombre de processus parallèles pendant la compilation en changeant la valeur suivant `-j`.

</details>

<details>
  <summary><b>Simulateur 3DS</b></summary>
  
Vous aurez besoin de devkitPro et de devkitARM disponible dans votre `$PATH` (instructions [ici](https://devkitpro.org/wiki/Getting_Started) (en anglais))

```bash
git clone --recursive https://github.com/Lauryy06/Upsilon.git
cd Upsilon
git checkout --recursive upsilon-dev
make PLATFORM=simulator TARGET=3ds -j
```

Vous pouvez ensuite copier epsilon.3dsx sur une carte SD pour l'exécuter depuis le HBC ou utiliser 3dslink pour le lancer via le réseau :

```bash
3dslink output/release/simulator/3ds/epsilon.3dsx -a <ADRESSE IP 3DS>
```

</details>

Si vous avez besoin d'aide, n'hésitez pas à rejoindre notre serveur discord : https://discord.gg/Q9buEMduXG

<a href="https://discord.gg/Q9buEMduXG"><p align="center"><img alt="Omega Banner Discord" src="https://user-images.githubusercontent.com/12123721/86287349-54ef5800-bbe8-11ea-80c1-34eb1f93eebd.png" /></p></a>
---

## Liens utiles
* [Upsilon external (pour installer des applications supplémentaires et des fonds d'écran)](https://lauryy06.github.io/Upsilon-External/)
* [Documentation d'ulab](https://micropython-ulab.readthedocs.io/en/latest/)

## Contribution

Pour contribuer, merci de lire le [Wiki d'Omega](https://github.com/Omega-Numworks/Omega/wiki/Contributing), les mêmes règles s'appliquent ici.

## Les autres projets

Les anciens projets d'Omega, avant sa fermeture, qui ont été utilisés pour ce projet

* [Omega Themes](https://github.com/Omega-Numworks/Omega-Themes)
* [Omega Website](https://github.com/Omega-Numworks/Omega-Website)
* [Omega RPN `APP`](https://github.com/Omega-Numworks/Omega-RPN)
* [Omega Atomic `APP`](https://github.com/Omega-Numworks/Omega-Atomic)
* [Omega Design](https://github.com/Omega-Numworks/Omega-Design)
* [Omega Discord Bot](https://github.com/Omega-Numworks/Omega-Discord-Bot)
* [Omega App Template `BETA`](https://github.com/Omega-Numworks/Omega-App-Template)
* [External Apps](https://github.com/Omega-Numworks/External-Apps)

## À propos d'Epsilon

Upsilon est un fork d'Omega, visant a continuer le projet des OS utilisateurs pour Numworks

Omega est un fork d'Epsilon, un système d'exploitation performant pour calculatrices graphiques. Il inclut huit applications pour les mathématiques de lycée et d'études supérieurs

Vous pouvez essayer Epsilon depuis votre navigateur sur le [simulateur en ligne](https://www.numworks.com/simulator/).

## Licence

NumWorks est une marque déposée de NumWorks SAS, 24 Rue Godot de Mauroy, 75009 Paris, France.
Nintendo est Nintendo 3DS sont des marques déposées de Nintendo of America Inc, 4600 150th Ave NE, Redmond, WA 98052, Etats-Unis.
NumWorks SAS et Nintendo of America Inc ne sont en aucun cas associés avec ce projet.

* NumWorks Epsilon est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Omega est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
* Upsilon est disponible sous [Lisense CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).
