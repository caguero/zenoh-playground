/*
 * Copyright (C) 2025 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <stdio.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <zenoh.hxx>
#include "getargs.hxx"

using namespace std::chrono_literals;
using namespace zenoh;

const char *defaultKeyexpr = "demo/example/zenoh-cpp-zenoh-c-queryable";
const char *defaultPayload = "Queryable from C++ zenoh-c!";

int _main(int argc, char **argv) {
  auto &&[config, args] =
    ConfigCliArgParser(argc, argv)
      .named_value(
        {"k", "key"},
        "KEY_EXPRESSION",
        "Key expression matching queries to reply to (string)",
        defaultKeyexpr)
      .named_value(
        {"p", "payload"},
        "PAYLOAD",
        "Value to reply to queries with (string)",
        defaultPayload)
      .named_flag(
        {"complete"},
        "Flag to indicate whether queryable is complete or not")
      .run();

  auto keyexpr = args.value("key");
  auto payload = args.value("payload");

  std::cout << "Opening session...\n";
  auto session = Session::open(std::move(config));

  std::cout << "Declaring Queryable on '" << keyexpr << "'...\n";

  auto onQuery = [payload, keyexpr](const Query &_query)
  {
      auto params = _query.get_parameters();
      auto queryPayload = _query.get_payload();
      std::cout << ">> [Queryable ] Received Query '"
                << _query.get_keyexpr().as_string_view() << "?" << params;
      if (queryPayload.has_value())
        std::cout << "' with value = '" << queryPayload->get().as_string();

      std::cout << "'\n";
      std::cout << "[Queryable ] Responding ('" << keyexpr << "': '"
                << payload << "')\n";
      _query.reply(keyexpr, payload);
  };

  auto onDropQueryable = []() { std::cout << "Destroying queryable\n"; };

  Session::QueryableOptions opts;
  opts.complete = args.flag("complete");
  auto queryable = session.declare_queryable(
    keyexpr, onQuery, onDropQueryable, std::move(opts));

  printf("Press CTRL-C to quit...\n");
  while (true)
    std::this_thread::sleep_for(1s);

  return 0;
}

int main(int argc, char **argv)
{
  try {
    init_log_from_env_or("error");
    _main(argc, argv);
  } catch (ZException e) {
    std::cout << "Received an error :" << e.what() << "\n";
  }
}
