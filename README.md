# zenoh-cpp-example

## Build

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

## Usage

```shell
# Run router
./build/zenoh_router "[\"tcp/localhost:8888\"]"
# Run subscriber
./build/zenoh_subscriber "[\"tcp/localhost:8888\"]"
# Run get
./build/zenoh_get "[\"tcp/localhost:8888\"]"
```
