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

#define PUBLISH_EXPR    "demo/router/zenoh-pub"
#define PUBLISH_VALUE   "Pub from C++ Router!"
#define SUBSCRIBE_EXPR  "demo/router/zenoh-sub"
#define QUERYABLE_EXPR  "demo/router/zenoh-queryable"
#define QUERYABLE_VALUE "Queryable from C++ Router!"

// Queryable callback
void query_handler(const z_query_t *query, void *context) {
    const char *queryable_value = QUERYABLE_VALUE;
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

// Subscriber callback
void subscribe_handler(const z_sample_t *sample, void *arg) {
    z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);
    char buf[256];
    sprintf(buf, ">> [Subscriber] Received ('%s': '%.*s')",
                    z_loan(keystr), (int)sample->payload.len, sample->payload.start);
    std::cout << buf << std::endl;
    z_drop(z_move(keystr));
}

int main(int argc, char **argv) {
    const char *queryable_expr = QUERYABLE_EXPR;
    const char *sub_keyexpr = SUBSCRIBE_EXPR;
    const char *pub_keyexpr = PUBLISH_EXPR;
    const char *pub_value = PUBLISH_VALUE;

    z_owned_config_t config = z_config_default();
    // Insert listen address
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
    // Enable admin space
    if (zc_config_insert_json(z_loan(config), "adminspace/enabled", "true") < 0) {
        std::cout << "Unable to enable admin space" << std::endl;
        exit(-1);
    }
    // Load plugins
    if (zc_config_insert_json(z_loan(config), "plugins_loading/enabled", "true") < 0) {
        std::cout << "Unable to enable plugins" << std::endl;
        exit(-1);
    }
    // Load REST plugins
    if (
        zc_config_insert_json(z_loan(config), "plugins/rest/__required__", "true") < 0 ||
        zc_config_insert_json(z_loan(config), "plugins/rest/http_port", "8000") < 0
       ) {
        std::cout << "Unable to load REST plugins" << std::endl;
        exit(-1);
    }

    std::cout << "Opening session..." << std::endl;
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        std::cout << "Unable to open session!" << std::endl;
        exit(-1);
    }

    // Create Subscriber
    z_owned_closure_sample_t sub_callback = z_closure(subscribe_handler);
    std::cout << "Declaring Subscriber on '" << sub_keyexpr << "'..." << std::endl;
    z_owned_subscriber_t sub = z_declare_subscriber(z_loan(s), z_keyexpr(sub_keyexpr), z_move(sub_callback), NULL);
    if (!z_check(sub)) {
        std::cout << "Unable to declare subscriber." << std::endl;
        exit(-1);
    }

    // Create Queryable
    std::cout << "Declaring Queryable on '" << queryable_expr << "'..." << std::endl;
    z_owned_closure_query_t queryable_callback = z_closure(query_handler, NULL, queryable_expr);
    z_owned_queryable_t qable = z_declare_queryable(z_loan(s), z_keyexpr(queryable_expr), z_move(queryable_callback), NULL);
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