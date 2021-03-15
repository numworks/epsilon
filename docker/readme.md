# Docker Omega Builder

This docker build and upload the firmware of a NumWorks from a repo easily.

## How to use

Connect the calculator to the computer, then to build the latest official firmware, just run :

```
docker run -it --rm --privileged omeganumworks/omegabuild
```

To build a specific version, you can use the following syntax :

```
docker run -it --rm --privileged omeganumworks/omegabuild git-repository-URL git-branch model username make-options
```

For example, to build latest dev version :

```
docker run -it --rm --privileged omeganumworks/omegabuild https://github.com/Omega-Numworks/Omega.git omega-dev 