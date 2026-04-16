all: lib examples

lib: zenoh zenoh-c zenoh-pico zenoh-cpp

prepare:
    git submodule init
    git submodule update

zenoh:
    cd zenoh && cargo build -p zenoh-plugin-rest --features zenoh/shared-memory --release

zenoh-c:
    mkdir -p zenohc-install
    mkdir -p zenohc-build
    cd zenohc-build && \
        cmake -DCMAKE_INSTALL_PREFIX=../zenohc-install -GNinja \
              -DZENOHC_BUILD_WITH_UNSTABLE_API=true -DZENOHC_BUILD_WITH_SHARED_MEMORY=true \
              ../zenoh-c \
              -DZENOHC_CARGO_FLAGS="--features=zenoh/default,zenoh/internal,zenoh/plugins"
    cd zenohc-build && \
        cmake --build . --config Release
    cd zenohc-build && \
        cmake --build . --target install

zenoh-pico:
    mkdir -p zenoh-pico-install
    mkdir -p zenoh-pico-build
    cd zenoh-pico-build && \
        cmake -DCMAKE_INSTALL_PREFIX=../zenoh-pico-install \
              -DCMAKE_BUILD_TYPE=Release \
              ../zenoh-pico
    cd zenoh-pico-build && make
    cd zenoh-pico-build && make install

zenoh-cpp: zenoh-c zenoh-pico
    mkdir -p zenohcpp-install
    mkdir -p zenohcpp-build
    cd zenohcpp-build && \
        cmake -DCMAKE_PREFIX_PATH="`pwd`/../zenoh-pico-install/lib/cmake/zenohpico/;`pwd`/../zenohc-install/lib/cmake/zenohc/" \
              -DZENOHCXX_ZENOHC=ON -DZENOHCXX_ZENOHPICO=ON \
              -DCMAKE_INSTALL_PREFIX=../zenohcpp-install \
              ../zenoh-cpp
    cd zenohcpp-build && \
        cmake --install .

examples build_zenohc="ON" build_zenohcpp_zenohc="ON" build_zenohcpp_zenohpico="ON":
    mkdir -p build
    cd build && \
        cmake -DCMAKE_PREFIX_PATH="`pwd`/../zenohc-install;`pwd`/../zenoh-pico-install;`pwd`/../zenohcpp-install" \
              -DBUILD_ZENOHC={{build_zenohc}} \
              -DBUILD_ZENOHCPP_ZENOHC={{build_zenohcpp_zenohc}} \
              -DBUILD_ZENOHCPP_ZENOHPICO={{build_zenohcpp_zenohpico}} .. && \
        cmake --build .

clean: clean-zenoh clean-zenoh-c clean-zenoh-pico clean-zenoh-cpp clean-examples

clean-zenoh:
    cd zenoh && cargo clean

clean-zenoh-c:
    rm -rf zenohc-build zenohc-install

clean-zenoh-pico:
    rm -rf zenoh-pico-build zenoh-pico-install

clean-zenoh-cpp:
    rm -rf zenohcpp-build zenohcpp-install

clean-examples:
    rm -rf build
