all: zenoh zenoh-c zenoh-cpp examples

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
              ../zenoh-c -DZENOHC_CARGO_CHANNEL="+1.75.0" \
              -DZENOHC_CARGO_FLAGS="--features=zenoh/default,zenoh/internal,zenoh/plugins"
    cd zenohc-build && \
        cmake --build . --config Release
    cd zenohc-build && \
        cmake --build . --target install

zenoh-cpp:
    mkdir -p zenohcpp-install
    mkdir -p zenohcpp-build
    cd zenohcpp-build && \
        cmake -DCMAKE_PREFIX_PATH=../zenohc-install/lib/cmake/zenohc/ \
              -DCMAKE_INSTALL_PREFIX=../zenohcpp-install \
              ../zenoh-cpp
    cd zenohcpp-build && \
        cmake --install .

examples:
    mkdir -p build
    cd build && \
        cmake .. && \
        cmake --build .

clean: clean-zenoh clean-zenoh-c clean-zenoh-cpp clean-examples

clean-zenoh:
    cd zenoh && cargo clean

clean-zenoh-c:
    rm -rf zenohc-build zenohc-install

clean-zenoh-cpp:
    rm -rf zenohcpp-build zenohcpp-install

clean-examples:
    rm -rf build
