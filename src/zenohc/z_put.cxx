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

    z_owned_config_t config;
    z_config_default(&config);
    if (argc > 1) {
        if (zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }

    printf("Opening session...\n");
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL) < 0) {
        printf("Unable to open session!\n");
        exit(-1);
    }

    printf("Putting Data ('%s': '%s')...\n", keyexpr, value);

    z_view_keyexpr_t ke;
    z_view_keyexpr_from_str(&ke, keyexpr);

    z_owned_bytes_t payload;
    z_bytes_from_static_str(&payload, value);
    z_put_options_t options;
    z_put_options_default(&options);

    z_owned_bytes_t attachment;
    ze_owned_serializer_t serializer;
    ze_serializer_empty(&serializer);
    ze_serializer_serialize_sequence_length(z_loan_mut(serializer), 1);  // 1 key-value pair
    ze_serializer_serialize_str(z_loan_mut(serializer), "hello");
    ze_serializer_serialize_str(z_loan_mut(serializer), "there");
    ze_serializer_finish(z_move(serializer), &attachment);

    options.attachment = z_move(attachment);  // attachement is consumed by z_put, so no need to drop it manually
    int res = z_put(z_loan(s), z_loan(ke), z_move(payload), &options);
    if (res < 0) {
        printf("Put failed...\n");
    }

    z_drop(z_move(s));

    return 0;
}