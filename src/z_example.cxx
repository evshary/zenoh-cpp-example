//
// Copyright (c) 2022 ZettaScale Technology
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
#include <unistd.h>

int main(int argc, char **argv) {
    const char *keyexpr = "demo/example/zenoh-c-pub";
    const char *value = "Pub from C!";

    if (argc > 1) keyexpr = argv[1];
    if (argc > 2) value = argv[2];

    z_owned_config_t config = z_config_default();
    if (argc > 3) {
        if (zc_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, argv[3]) < 0) {
            std::cout << "Couldn't insert value " << argv[3] << " in configuration." << std::endl;
            exit(-1);
        }
    }

    std::cout << "Opening session..." << std::endl;
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        std::cout << "Unable to open session!" << std::endl;
        exit(-1);
    }

    std::cout << "Declaring Publisher on " << keyexpr << "..." << std::endl;
    z_owned_publisher_t pub = z_declare_publisher(z_loan(s), z_keyexpr(keyexpr), NULL);
    if (!z_check(pub)) {
        std::cout << "Unable to declare Publisher for key expression!" << std::endl;
        exit(-1);
    }

    char buf[256];
    for (int idx = 0; 1; ++idx) {
        sleep(1);
        sprintf(buf, "Putting Data ('%s': '[%4d] %s')...", keyexpr, idx, value);
        std::cout << buf << std::endl;
        z_publisher_put_options_t options = z_publisher_put_options_default();
        options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
        z_publisher_put(z_loan(pub), (const uint8_t *)buf, strlen(buf), &options);
    }

    z_undeclare_publisher(z_move(pub));

    z_close(z_move(s));
    return 0;
}