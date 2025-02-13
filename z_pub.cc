#include <stdio.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>
#include <zenoh.hxx>
#include "getargs.hxx"

using namespace std::chrono_literals;
using namespace zenoh;

const char *defaultKeyexpr = "demo/example/zenoh-cpp-zenoh-c-pub";
const char *defaultValue = "Pub from C++ zenoh-c!";

//////////////////////////////////////////////////
int _main(int _argc, char **_argv) {
  auto &&[config, args] =
    ConfigCliArgParser(_argc, _argv)
      .named_value(
        {"k", "key"},
        "KEY_EXPRESSION",
        "Key expression to publish to (string)",
        defaultKeyexpr)
      .named_value(
        {"p", "payload"},
        "PAYLOAD",
        "Payload to publish (string)",
        defaultValue)
      .named_value(
        {"a", "attach"},
        "ATTACHMENT",
        "Attachment to add to each put (string)",
        "")
      .run();
  auto keyexpr = args.value("key");
  auto payload = args.value("payload");
  auto attachment = args.value("attach");

  std::cout << "Opening session..." << std::endl;
  auto session = Session::open(std::move(config));

  std::cout << "Declaring Publisher on '" << keyexpr << "'..." << std::endl;
  auto pub = session.declare_publisher(KeyExpr(keyexpr));
  std::cout << "Press CTRL-C to quit..." << std::endl;

  auto zToken = session.liveliness_declare_token(KeyExpr(keyexpr));

  for (int idx = 0; idx < std::numeric_limits<int>::max(); ++idx)
  {
    std::this_thread::sleep_for(1s);
    std::ostringstream ss;
    ss << "[" << idx << "] " << payload;
    auto s = ss.str();
    std::cout << "Putting Data ('" << keyexpr << "': '" << s << "')...\n";

    Publisher::PutOptions options;
    if (!attachment.empty())
      options.attachment = attachment;

    pub.put(s, std::move(options));
  }
  return 0;
}

//////////////////////////////////////////////////
int main(int argc, char **argv) {
  try
  {
    init_log_from_env_or("error");
   _main(argc, argv);
  } catch (ZException e) {
    std::cout << "Received an error :" << e.what() << "\n";
  }
}
