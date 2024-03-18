# zenoh-cpp-example

* Build zenoh-c

```shell
# Build zenoh-c
mkdir -p zenohc-build && cd zenohc-build 
cmake ../zenoh-c
cmake --build . --config Release
cd ..
# Install zenoh-c
mkdir -p zenohc-install && cd zenohc-install
cmake ../zenoh-c -DCMAKE_INSTALL_PREFIX=../zenohc-install
cmake --build . --target install
cd ..
```

* Build examples

```shell
mkdir -p build && cd build
cmake ..
cmake --build .
cd ..
```
