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
#include <chrono>
#include <iostream>
#include <thread>
#include <zenoh.hxx>
#include "getargs.hxx"

using namespace std::chrono_literals;
using namespace zenoh;

//////////////////////////////////////////////////
const char *kindToStr(SampleKind _kind)
{
  switch (_kind)
  {
    case SampleKind::Z_SAMPLE_KIND_PUT:
      return "PUT";
    case SampleKind::Z_SAMPLE_KIND_DELETE:
        return "DELETE";
    default:
      return "UNKNOWN";
  }
}

//////////////////////////////////////////////////
int _main(int _argc, char **_argv)
{
  auto &&[config, args] =
    ConfigCliArgParser(_argc, _argv)
      .named_value(
        {"k", "key"},
        "KEY_EXPRESSION",
        "Key expression to subscribe to (string)",
        "demo/example/**")
      .run();

  KeyExpr keyexpr(args.value("key"));

  std::cout << "Opening session..." << std::endl;
  // auto session = Session::open(std::move(config));
  auto session = Session::open(Config::create_default());

  auto dataHandler = [](const Sample &_sample)
  {
    std::cout << ">> [Subscriber] Received " << kindToStr(_sample.get_kind())
              << " ('" << _sample.get_keyexpr().as_string_view() << "' : '"
              << _sample.get_payload().as_string() << "')";

    auto attachment = _sample.get_attachment();
    if (attachment.has_value())
      std::cout << "  (" << attachment->get().as_string() << ")";
    std::cout << std::endl;
  };

  std::cout << "Declaring Subscriber on '" << keyexpr.as_string_view()
            << "'..." << std::endl;
  auto subscriber = session.declare_subscriber(
    keyexpr, dataHandler, closures::none);

  std::cout << "Press CTRL-C to quit...\n";
  while (true)
    std::this_thread::sleep_for(1s);

  return 0;
}

//////////////////////////////////////////////////
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
