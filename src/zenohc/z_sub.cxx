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
#include <unistd.h>
#include "zenoh.h"

void data_handler(z_loaned_sample_t *sample, void *arg) {
    z_view_string_t key_string;
    z_keyexpr_as_view_string(z_sample_keyexpr(sample), &key_string);

    z_owned_string_t payload_string;
    z_bytes_to_string(z_sample_payload(sample), &payload_string);

    printf(">> [Subscriber] Received ('%.*s': '%.*s')\n",
           (int)z_string_len(z_loan(key_string)), z_string_data(z_loan(key_string)),
           (int)z_string_len(z_loan(payload_string)), z_string_data(z_loan(payload_string)));
    z_drop(z_move(payload_string));
}

int main(int argc, char **argv) {
    const char *expr = "demo/router/**";

    z_owned_config_t config;
    z_config_default(&config);
    if (argc > 1) {
        if (zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }

    z_view_keyexpr_t ke;
    z_view_keyexpr_from_str(&ke, expr);

    printf("Opening session...\n");
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL) < 0) {
        printf("Unable to open session!\n");
        exit(-1);
    }

    z_owned_closure_sample_t callback;
    z_closure(&callback, data_handler, NULL, NULL);
    printf("Declaring Subscriber on '%s'...\n", expr);
    z_owned_subscriber_t sub;
    if (z_declare_subscriber(z_loan(s), &sub, z_loan(ke), z_move(callback), NULL) < 0) {
        printf("Unable to declare subscriber.\n");
        exit(-1);
    }

    std::cout << "Enter 'q' to quit..." << std::endl;
    char c = 0;
    while (c != 'q') {
        c = getchar();
        if (c == -1) {
            sleep(1);
        }
    }

    z_drop(z_move(sub));
    z_drop(z_move(s));
    return 0;
}
