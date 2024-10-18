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

#include "stdio.h"
#include "zenoh.hxx"
using namespace zenoh;

int _main(int argc, char **argv) {
    const char *keyexpr = "demo/router/zenoh-sub";
    const char *value = "Put to C++ Router!";
    Config config = zenoh::Config::create_default();

    std::cout << "Opening session...\n";
    auto session = Session::open(std::move(config));

    std::cout << "Putting Data (" << "'" << keyexpr << "': '" << value << "')...\n";

    Session::PutOptions put_options;
    put_options.encoding = Encoding("text/plain");

    std::unordered_map<std::string, std::string> attachment_map = {{"serial_number", "123"},
                                                                   {"coordinates", "48.7082,2.1498"}};
    put_options.attachment = ext::serialize(attachment_map);

    session.put(KeyExpr(keyexpr), value, std::move(put_options));
    return 0;
}

int main(int argc, char **argv) {
    try {
        _main(argc, argv);
    } catch (ZException e) {
        std::cout << "Received an error :" << e.what() << "\n";
    }
}