# zenoh-cpp-example

The repo shows how to build C++ program with zenoh-c or zenoh-cpp.

## Build

* Get submodule

```shell
just prepare
```

* Build zenoh REST plugin

```shell
just zenoh
```

* Build zenoh-c

```shell
just zenoh-c
```

* Build zenoh-cpp

```shell
just zenoh-cpp
```

* Build examples

```shell
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
