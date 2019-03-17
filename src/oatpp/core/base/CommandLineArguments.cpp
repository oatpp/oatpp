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

#include "CommandLineArguments.hpp"

#include <cstring>

namespace oatpp { namespace base {

CommandLineArguments::CommandLineArguments()
  : m_argc(0)
  , m_argv(nullptr)
{}

CommandLineArguments::CommandLineArguments(int argc, const char * argv[])
  : m_argc(argc)
  , m_argv(argv)
{}
  
bool CommandLineArguments::Parser::hasArgument(int argc, const char * argv[], const char* argName) {
  return getArgumentIndex(argc, argv, argName) >= 0;
}

v_int32 CommandLineArguments::Parser::getArgumentIndex(int argc, const char * argv[], const char* argName) {
  for(v_int32 i = 0; i < argc; i ++) {
    if(std::strcmp(argName, argv[i]) == 0){
      return i;
    }
  }
  return -1;
}

const char* CommandLineArguments::Parser::getArgumentStartingWith(int argc, const char * argv[], const char* argNamePrefix, const char* defaultValue) {
  for(v_int32 i = 0; i < argc; i ++) {
    if(std::strncmp(argNamePrefix, argv[i], std::strlen(argNamePrefix)) == 0){
      return argv[i];
    }
  }
  return defaultValue;
}

const char* CommandLineArguments::Parser::getNamedArgumentValue(int argc, const char * argv[], const char* argName, const char* defaultValue) {
  for(v_int32 i = 0; i < argc; i ++) {
    if(std::strcmp(argName, argv[i]) == 0){
      if(i + 1 < argc) {
        return argv[i + 1];
      }
    }
  }
  return defaultValue;
}
  
}}
