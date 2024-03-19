//
// Copyright (c) 2024 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "zenoh.h"

int main(int argc, char **argv) {
    const char *keyexpr = "demo/router/zenoh-sub";
    const char *value = "Put to C++ Router!";

    z_owned_bytes_map_t attachment = z_bytes_map_new();
    z_bytes_map_insert_by_alias(&attachment, z_bytes_new("hello"), z_bytes_new("there"));

    z_owned_config_t config = z_config_default();
    if (argc > 1) {
        if (zc_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }

    std::cout << "Opening session..." << std::endl;
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        std::cout << "Unable to open session!" << std::endl;
        exit(-1);
    }

    std::cout << "Putting Data ('" << keyexpr << "': '" << value << "')..." << std::endl;
    z_put_options_t options = z_put_options_default();
    options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
    options.attachment = z_bytes_map_as_attachment(&attachment);
    int res = z_put(z_loan(s), z_keyexpr(keyexpr), (const uint8_t *)value, strlen(value), &options);
    if (res < 0) {
        std::cout << "Put failed..." << std::endl;
    }

    z_close(z_move(s));
    z_drop(z_move(attachment));
    return 0;
}