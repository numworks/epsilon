# SDL

## Current version
The sdl directory contains a snapshot of the SDL source code at version `release-2.0.14` (`4cd981609b50ed273d80c635c1ca4c1e5518fb21`).

## Additional fixes
> **_NOTE:_**  We should minimize these fixes as much as possible.

From future versions :
- `c877dd93c5154b26859beec7487d23c5411e774c` (release-2.0.18)
- `99af3281ee11ff947a82c561c2a275627ccea90c` (release-2.0.22)
- `7935cfacc52a33a164880e3e8d3df4021e737fc7` (release-2.0.22)

## Updating SDL

### Setup SDL sources

```git clone https://github.com/libsdl-org/SDL```
Checkout to the expected version.

### Update code
Copy and replace files into `Epsilon` repository
```cp -r sources/SDL/src epsilon/ion/src/simulator/external/sdl```
```cp -r sources/SDL/include epsilon/ion/src/simulator/external/sdl```
Parts of SDL source code must also be copied in `simulator/android` because Gradle expects Java code to live there.
```cp -r sources/SDL/android-project/app/src/main/java/org/libsdl/app epsilon/ion/src/simulator/android/src/java/org```

### Fix SDL static linking
> **_NOTE:_**  This process could be improved
In the SDL sources, run
```./configure --disable-shared```
A `Makefile.rules` file is generated, copy all `.c` and `.m` files into `ion/src/simulator/external/Makefile` and the other config files, depending on the platforms.
A lot of trimming and reordering across platforms will be necessary.

### Re-apply additional fixes
Apply the additional fixes mentioned above that are still in a future version.

## SDL version compatibilities
Our web simulator is tied to a specific emscripten version, and recent SDL releases no longer support it.
As of now, the SDL cannot be updated past a certain release (At least `release-2.0.22` isn't compatible) without updating emscripten.

# dlfcn

The dlfcn repository contains [dlfcn-win32](https://github.com/dlfcn-win32/dlfcn-win32) release 1.3.1
