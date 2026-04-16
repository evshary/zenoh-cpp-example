# zenoh-cpp-example

The repo shows how to build zenoh-cpp program with zenoh-c or zenoh-pico.

## Build

You have two options for providing the necessary Zenoh dependencies:

### Option 1: Install packages via APT (Ubuntu)

If you are on Ubuntu, you can install the pre-built libraries directly:

```bash
sudo apt update
sudo apt install libzenohcpp-dev libzenohc-dev libzenohpico-dev
```

Once installed, you can simply build the examples:

```bash
just examples
```

### Option 2: Build dependencies from source using justfile

* Get submodule

```bash
just prepare
```

* Build all the dependencies

```bash
just lib
```

Once built, you can simply build the examples:

```bash
just examples
```

## Usage

* Using zenoh-c

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
curl http://localhost:8000/**
```

* Using zenoh-cpp

```shell
# Run router
./build/zenoh_router
# Run put
## with zenoh-c backened
./build/zenoh_put_cpp_zc
## with zenoh-pico backend
./build/zenoh_put_cpp_zpico
```

## Note

To use plugins in router mode, note that:

* The plugins and `zenoh-c` should be consistent, including Zenoh version and build features.
* The feature `plugins` should be enabled while building `zenoh-c`.
* The feature `shared-memory` should be enabled while building plugins, since `zenoh-c` needs this.
