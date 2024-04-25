# zenoh-cpp-example

## Build

* Get submodule

```shell
git submodule init
git submodule update
```

* Build zenoh rest plugin

```shell
cd zenoh
cargo build -p zenoh-plugin-rest --features zenoh/shared-memory --release
cd ..
```

* Build zenoh-c

```shell
mkdir -p zenohc-install
mkdir -p zenohc-build && cd zenohc-build 
cmake -DCMAKE_INSTALL_PREFIX=../zenohc-install -GNinja \
    ../zenoh-c -DZENOHC_CARGO_CHANNEL="+1.72.0" \
    -DZENOHC_CARGO_FLAGS="--features=zenoh/unstable,zenoh/plugins"
cmake --build . --config Release
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
# Run put
./build/zenoh_put "[\"tcp/localhost:8888\"]"
# Router loads REST plugin by default. You can curl the information
curl http://localhost:8000/@/router/local
```

## Note

* While using plugins, you should make sure the plugin and zenoh-c are consistent, including Zenoh version and build features.
