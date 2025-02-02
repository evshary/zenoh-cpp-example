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
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "zenoh.h"

int main(int argc, char **argv) {
    const char *expr = "demo/router/**";
    const char *value = "myvalue";
    z_view_keyexpr_t keyexpr;
    if (z_view_keyexpr_from_str(&keyexpr, expr) < 0) {
        std::cout << expr << " is not a valid key expression" << std::endl;
        exit(-1);
    }

    z_owned_config_t config;
    z_config_default(&config);
    if (argc > 1) {
        if (zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }

    std::cout << "Opening session..." << std::endl;
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL)) {
        printf("Unable to open session!\n");
        exit(-1);
    }

    printf("Sending Query '%s'...\n", expr);
    z_owned_fifo_handler_reply_t handler;
    z_owned_closure_reply_t closure;
    z_fifo_channel_reply_new(&closure, &handler, 16);

    z_get_options_t opts;
    z_get_options_default(&opts);

    z_owned_bytes_t payload;
    if (value != NULL) {
        z_bytes_from_static_str(&payload, value);
        opts.payload = z_move(payload);
    }
    z_get(z_loan(s), z_loan(keyexpr), "", z_move(closure),
          &opts);  // here, the send is moved and will be dropped by zenoh when adequate
    z_owned_reply_t reply;

    for (z_result_t res = z_recv(z_loan(handler), &reply); res == Z_OK; res = z_recv(z_loan(handler), &reply)) {
        if (z_reply_is_ok(z_loan(reply))) {
            const z_loaned_sample_t* sample = z_reply_ok(z_loan(reply));

            z_view_string_t key_str;
            z_keyexpr_as_view_string(z_sample_keyexpr(sample), &key_str);

            z_owned_string_t reply_str;
            z_bytes_to_string(z_sample_payload(sample), &reply_str);

            printf(">> Received ('%.*s': '%.*s')\n", (int)z_string_len(z_loan(key_str)), z_string_data(z_loan(key_str)),
                   (int)z_string_len(z_loan(reply_str)), z_string_data(z_loan(reply_str)));
            z_drop(z_move(reply_str));
        } else {
            printf("Received an error\n");
            std::cout << "Received an error" << std::endl;
        }
        z_drop(z_move(reply));
    }

    z_drop(z_move(handler));
    z_drop(z_move(s));

    return 0;
}