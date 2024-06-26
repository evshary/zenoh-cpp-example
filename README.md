# zenoh-cpp-example

## Build

* Get submodule

```shell
git submodule init
git submodule update
```

* Build zenoh REST plugin

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
    -DZENOHC_CARGO_FLAGS="--features=zenoh/plugins"
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

To use plugins in router mode, note that:

* The plugins and `zenoh-c` should be consistent, including Zenoh version and build features.
* The feature `plugins` should be enabled while building `zenoh-c`.
* The feature `shared-memory` should be enabled while building plugins, since `zenoh-c` needs this.
