# zenoh-cpp-example

The repo shows how to build zenoh-cpp program with zenoh-c or zenoh-pico.

## Build

* Get submodule

```bash
just prepare
```

* Build zenoh REST plugin

```bash
just zenoh
```

* Build zenoh-cpp example with zenoh-c

```bash
just all-c
```

* Build zenoh-cpp example with zenoh-pico

```shell
just all-pico
```

* (Optional) You can build them separately.

```bash
just zenoh-c
just zenoh-pico
just zenoh-cpp
# Or just zenoh-cpp-pico
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
./build/zenoh_put_cpp
```

## Note

To use plugins in router mode, note that:

* The plugins and `zenoh-c` should be consistent, including Zenoh version and build features.
* The feature `plugins` should be enabled while building `zenoh-c`.
* The feature `shared-memory` should be enabled while building plugins, since `zenoh-c` needs this.
