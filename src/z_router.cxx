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
#include <unistd.h>

const char *queryable_expr = "demo/router/zenoh-queryable";
const char *queryable_value = "Queryable from C++ Router!";
z_keyexpr_t keyexpr;

// Queryable callback
void query_handler(const z_query_t *query, void *context) {
    z_owned_str_t keystr = z_keyexpr_to_string(z_query_keyexpr(query));
    z_bytes_t pred = z_query_parameters(query);
    z_value_t payload_value = z_query_value(query);
    char buf[256];
    if (payload_value.payload.len > 0) {
        sprintf(buf, ">> [Queryable ] Received Query '%s?%.*s' with value '%.*s'", z_loan(keystr), (int)pred.len,
                pred.start, (int)payload_value.payload.len, payload_value.payload.start);
    } else {
        sprintf(buf, ">> [Queryable ] Received Query '%s?%.*s'", z_loan(keystr), (int)pred.len, pred.start);
    }
    std::cout << buf << std::endl;
    z_query_reply_options_t options = z_query_reply_options_default();
    options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
    z_query_reply(query, z_keyexpr((const char *)context), (const unsigned char *)queryable_value, strlen(queryable_value), &options);
    z_drop(z_move(keystr));
}

int main(int argc, char **argv) {
    const char *pub_keyexpr = "demo/router/zenoh-pub";
    const char *pub_value = "Pub from C++ Router!";

    z_owned_config_t config = z_config_default();
    // Insert connect address
    if (argc > 1) {
        if (zc_config_insert_json(z_loan(config), Z_CONFIG_LISTEN_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }
    // Switch to router mode
    if (zc_config_insert_json(z_loan(config), Z_CONFIG_MODE_KEY, "\"router\"") < 0) {
        std::cout << "Unable to switch router mode" << std::endl;
        exit(-1);
    }
    // Load plugins
    // Not implemented yet

    std::cout << "Opening session..." << std::endl;
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        std::cout << "Unable to open session!" << std::endl;
        exit(-1);
    }

    // Create Queryable
    std::cout << "Declaring Queryable on '" << queryable_expr << "'..." << std::endl;
    z_owned_closure_query_t callback = z_closure(query_handler, NULL, queryable_expr);
    z_owned_queryable_t qable = z_declare_queryable(z_loan(s), z_keyexpr(queryable_expr), z_move(callback), NULL);
    if (!z_check(qable)) {
        std::cout << "Unable to create queryable." << std::endl;
        exit(-1);
    }

    // Create Publisher
    std::cout << "Declaring Publisher on '" << pub_keyexpr << "'..." << std::endl;
    z_owned_publisher_t pub = z_declare_publisher(z_loan(s), z_keyexpr(pub_keyexpr), NULL);
    if (!z_check(pub)) {
        std::cout << "Unable to declare Publisher for key expression!" << std::endl;
        exit(-1);
    }
    char buf[256];
    for (int idx = 0; 1; ++idx) {
        sleep(1);
        sprintf(buf, "Putting Data ('%s': '[%4d] %s')...", pub_keyexpr, idx, pub_value);
        std::cout << buf << std::endl;
        z_publisher_put_options_t options = z_publisher_put_options_default();
        options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
        z_publisher_put(z_loan(pub), (const uint8_t *)buf, strlen(buf), &options);
    }

    z_undeclare_publisher(z_move(pub));

    z_close(z_move(s));
    return 0;
}