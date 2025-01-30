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
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <zenoh.hxx>
#include "getargs.hxx"

using namespace zenoh;

int _main(int argc, char **argv)
{
  auto &&[config, args] =
    ConfigCliArgParser(argc, argv)
      .named_value(
        {"s", "selector"},
        "SELECTOR",
        "Query selector (string)",
        "demo/example/**")
      .named_value(
        {"p", "payload"},
        "PAYLOAD",
        "Query payload (string)",
        "")
      .named_value(
        {"t", "target"},
        "TARGET",
        "Query target (BEST_MATCHING | ALL | ALL_COMPLETE)",
        "BEST_MATCHING")
      .named_value(
        {"o", "timeout"},
        "TIMEOUT",
        "Timeout in ms (number)",
        "10000")
      .run();

  uint64_t timeoutMs = std::atoi(args.value("timeout").data());
  QueryTarget queryTarget = parse_query_target(args.value("target"));
  Selector selector = parse_selector(args.value("selector"));
  auto payload = args.value("payload");

  std::cout << "Opening session...\n";
  auto session = Session::open(std::move(config));

  std::cout << "Sending Query '" << args.value("selector") << "'...\n";

  std::mutex m;
  std::condition_variable doneSignal;
  bool done = false;

  auto onReply = [](const Reply &reply)
  {
    if (reply.is_ok())
    {
      const auto &sample = reply.get_ok();
      std::cout << "Received ('" << sample.get_keyexpr().as_string_view()
                << "' : '" << sample.get_payload().as_string() << "')\n";
    }
    else
    {
      std::cout << "Received an error :"
                << reply.get_err().get_payload().as_string() << "\n";
    }
  };

  auto onDone = [&m, &done, &doneSignal]()
  {
    std::lock_guard lock(m);
    done = true;
    doneSignal.notify_all();
  };

  Session::GetOptions options;
  options.target = queryTarget;
  if (!payload.empty())
    options.payload = payload;
  
  options.timeout_ms = timeoutMs;
  session.get(selector.key_expr, selector.parameters,
              onReply, onDone, std::move(options));

  std::unique_lock lock(m);
  doneSignal.wait(lock, [&done] { return done; });

  return 0;
}

int main(int argc, char **argv)
{
  try
  {
    init_log_from_env_or("error");
    _main(argc, argv);
  } catch (ZException e)
  {
    std::cout << "Received an error :" << e.what() << "\n";
  }
}
