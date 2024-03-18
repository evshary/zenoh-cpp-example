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

const char *kind_to_str(z_sample_kind_t kind);

void data_handler(const z_sample_t *sample, void *arg) {
    z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);
    char buf[256];
    sprintf(buf, ">> [Subscriber] Received ('%s': '%.*s')",
                    z_loan(keystr), (int)sample->payload.len, sample->payload.start);
    std::cout << buf << std::endl;
    z_drop(z_move(keystr));
}

int main(int argc, char **argv) {
    const char *expr = "demo/router/**";

    z_owned_config_t config = z_config_default();
    if (argc > 1) {
        if (zc_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }

    std::cout << "Opening session..." << std::endl;;
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        std::cout << "Unable to open session!" << std::endl;;
        exit(-1);
    }

    z_owned_closure_sample_t callback = z_closure(data_handler);
    std::cout << "Declaring Subscriber on '" << expr << "'..." << std::endl;
    z_owned_subscriber_t sub = z_declare_subscriber(z_loan(s), z_keyexpr(expr), z_move(callback), NULL);
    if (!z_check(sub)) {
        std::cout << "Unable to declare subscriber." << std::endl;
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

    z_close(z_move(s));
    return 0;
}
