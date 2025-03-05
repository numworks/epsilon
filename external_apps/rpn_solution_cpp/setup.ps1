winget install --accept-package-agreements python
winget install ezwinports.make
git clone https://github.com/emscripten-core/emsdk.git
./emsdk/emsdk install 3.1.61
./emsdk/emsdk activate 3.1.61
npm install -g ./nwlink-0.0.18.tgz
