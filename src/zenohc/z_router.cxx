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
void query_handler(z_loaned_query_t *query, void *context) {
    const char *queryable_value = QUERYABLE_VALUE;
    z_view_string_t key_string;
    z_keyexpr_as_view_string(z_query_keyexpr(query), &key_string);

    z_view_string_t params;
    z_query_parameters(query, &params);

    const z_loaned_bytes_t *payload = z_query_payload(query);
    if (payload != NULL && z_bytes_len(payload) > 0) {
        z_owned_string_t payload_string;
        z_bytes_to_string(payload, &payload_string);

        printf(">> [Queryable ] Received Query '%.*s?%.*s' with value '%.*s'\n", (int)z_string_len(z_loan(key_string)),
               z_string_data(z_loan(key_string)), (int)z_string_len(z_loan(params)), z_string_data(z_loan(params)),
               (int)z_string_len(z_loan(payload_string)), z_string_data(z_loan(payload_string)));
        z_drop(z_move(payload_string));
    } else {
        printf(">> [Queryable ] Received Query '%.*s?%.*s'\n", (int)z_string_len(z_loan(key_string)),
               z_string_data(z_loan(key_string)), (int)z_string_len(z_loan(params)), z_string_data(z_loan(params)));
    }
    z_query_reply_options_t options;
    z_query_reply_options_default(&options);

    z_owned_bytes_t reply_payload;
    z_bytes_from_static_str(&reply_payload, (char *)queryable_value);

    z_view_keyexpr_t reply_keyexpr;
    z_view_keyexpr_from_str(&reply_keyexpr, (const char *)context);

    z_query_reply(query, z_loan(reply_keyexpr), z_move(reply_payload), &options);
}

// Subscriber callback
void subscribe_handler(z_loaned_sample_t *sample, void *arg) {
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
    const char *queryable_expr = QUERYABLE_EXPR;
    const char *sub_keyexpr = SUBSCRIBE_EXPR;
    const char *pub_keyexpr = PUBLISH_EXPR;
    const char *pub_value = PUBLISH_VALUE;

    z_view_keyexpr_t sub_ke;
    z_view_keyexpr_from_str(&sub_ke, sub_keyexpr);
    z_view_keyexpr_t queryable_ke;
    z_view_keyexpr_from_str(&queryable_ke, queryable_expr);

    z_owned_config_t config;
    z_config_default(&config);
    // Insert listen address
    if (argc > 1) {
        if (zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, argv[1]) < 0) {
            std::cout << "Couldn't insert value " << argv[1] << " into the configuration." << std::endl;
            std::cout << "Format should be [\"tcp/localhost:7447\"]" << std::endl;
            exit(-1);
        }
    }
    // Switch to router mode
    if (zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_MODE_KEY, "\"router\"") < 0) {
        std::cout << "Unable to switch router mode" << std::endl;
        exit(-1);
    }
    // Enable admin space
    if (zc_config_insert_json5(z_loan_mut(config), "adminspace/enabled", "true") < 0) {
        std::cout << "Unable to enable admin space" << std::endl;
        exit(-1);
    }
    // Load plugins
    if (zc_config_insert_json5(z_loan_mut(config), "plugins_loading/enabled", "true") < 0) {
        std::cout << "Unable to enable plugins" << std::endl;
        exit(-1);
    }
    // Load REST plugins
    if (
        zc_config_insert_json5(z_loan_mut(config), "plugins/rest/__path__", "\"./zenoh/target/release/libzenoh_plugin_rest.so\"") < 0 ||
        zc_config_insert_json5(z_loan_mut(config), "plugins/rest/__required__", "true") < 0 ||
        zc_config_insert_json5(z_loan_mut(config), "plugins/rest/http_port", "8000") < 0
       ) {
        std::cout << "Unable to load REST plugins" << std::endl;
        exit(-1);
    }

    printf("Opening session...\n");
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL) < 0) {
        printf("Unable to open session!\n");
        exit(-1);
    }

    // Create Subscriber
    z_owned_closure_sample_t sub_callback;
    z_closure(&sub_callback, subscribe_handler, NULL, NULL);
    std::cout << "Declaring Subscriber on '" << sub_keyexpr << "'..." << std::endl;
    z_owned_subscriber_t sub;
    if (z_declare_subscriber(z_loan(s), &sub, z_loan(sub_ke), z_move(sub_callback), NULL) < 0) {
        printf("Unable to declare subscriber.\n");
        exit(-1);
    }

    // Create Queryable
    printf("Declaring Queryable on '%s'...\n", queryable_expr);
    z_owned_closure_query_t queryable_callback;
    z_closure(&queryable_callback, query_handler, NULL, (void *)queryable_expr);
    z_owned_queryable_t qable;

    if (z_declare_queryable(z_loan(s), &qable, z_loan(queryable_ke), z_move(queryable_callback), NULL) < 0) {
        printf("Unable to create queryable.\n");
        exit(-1);
    }

    // Create Publisher
    printf("Declaring Publisher on '%s'...\n", pub_keyexpr);
    z_owned_publisher_t pub;
    z_view_keyexpr_t ke;
    z_view_keyexpr_from_str(&ke, pub_keyexpr);
    if (z_declare_publisher(z_loan(s), &pub, z_loan(ke), NULL) < 0) {
        printf("Unable to declare Publisher for key expression!\n");
        exit(-1);
    }
    char buf[256] = {};
    for (int idx = 0; 1; ++idx) {
        sleep(1);
        sprintf(buf, "[%4d] %s", idx, pub_value);
        printf("Putting Data ('%s': '%s')...\n", pub_keyexpr, buf);
        z_publisher_put_options_t options;
        z_publisher_put_options_default(&options);

        z_owned_bytes_t payload;
        z_bytes_copy_from_str(&payload, buf);

        z_publisher_put(z_loan(pub), z_move(payload), &options);
    }

    z_drop(z_move(s));
    return 0;
}