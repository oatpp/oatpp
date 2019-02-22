/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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
 ***************************************************************************/

#include "CommandLineArgumentsTest.hpp"

#include "oatpp/core/base/CommandLineArguments.hpp"

#include <cstring>

namespace oatpp { namespace test { namespace base {
  
void CommandLineArgumentsTest::onRun() {
  
  /* -k -c 100 -n 500000 "http://127.0.0.1:8000/" */
  int argc = 6;
  const char * argv[] = {"-k", "-c", "100", "-n", "500000", "http://127.0.0.1:8000/"};
  
  oatpp::base::CommandLineArguments args(argc, argv);
  
  OATPP_ASSERT(args.getArgumentIndex("-k") == 0);
  OATPP_ASSERT(args.getArgumentIndex("-c") == 1);
  OATPP_ASSERT(args.getArgumentIndex("100") == 2);
  OATPP_ASSERT(args.getArgumentIndex("-n") == 3);
  OATPP_ASSERT(args.getArgumentIndex("500000") == 4);
  OATPP_ASSERT(args.getArgumentIndex("http://127.0.0.1:8000/") == 5);
  OATPP_ASSERT(args.getArgumentIndex("not-existing-arg") == -1);
  
  OATPP_ASSERT(args.hasArgument("-k"));
  OATPP_ASSERT(args.hasArgument("not-existing-arg") == false);
  
  OATPP_ASSERT(std::strcmp(args.getArgumentStartingWith("http"), "http://127.0.0.1:8000/") == 0);
  OATPP_ASSERT(std::strcmp(args.getArgumentStartingWith("tcp", "tcp://default/"), "tcp://default/") == 0);
  
  OATPP_ASSERT(std::strcmp(args.getNamedArgumentValue("-c"), "100") == 0);
  OATPP_ASSERT(std::strcmp(args.getNamedArgumentValue("-c", nullptr), "100") == 0);
  
  OATPP_ASSERT(std::strcmp(args.getNamedArgumentValue("-n"), "500000") == 0);
  OATPP_ASSERT(std::strcmp(args.getNamedArgumentValue("--non-existing", "default"), "default") == 0);

}
  
}}}
