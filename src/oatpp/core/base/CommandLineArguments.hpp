/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef oatpp_base_CommandLineArguments_hpp
#define oatpp_base_CommandLineArguments_hpp

#include "./Environment.hpp"

namespace oatpp { namespace base {

class CommandLineArguments {
public:
  
  class Parser {
  public:
    
    /**
     * returns true if getArgumentIndex(argName) >= 0
     */
    static bool hasArgument(int argc, const char * argv[], const char* argName);
    
    /**
     * get index of the argument with the name == argName
     */
    static v_int32 getArgumentIndex(int argc, const char * argv[], const char* argName);
    
    /**
     * return argument wich starts with the prefix
     * ex:
     * for cmd = "-k -c 1000 -n 100 'http://127.0.0.1:8000/'"
     * getArgumentWhichStartsWith("http") == http://127.0.0.1:8000/
     * if no argument found defaultValue returned
     */
    static const char* getArgumentStartingWith(int argc, const char * argv[], const char* argNamePrefix, const char* defaultValue = nullptr);
    
    /**
     * return value preceded by the argument
     * ex:
     * for cmd = "-k -c 1000 -n 100"
     * getNamedArgumentValue("-c") == "1000"
     * getNamedArgumentValue("-n") == "100"
     */
    static const char* getNamedArgumentValue(int argc, const char * argv[], const char* argName, const char* defaultValue = nullptr);
    
  };
  
private:
  int m_argc;
  const char ** m_argv;
public:
  
  CommandLineArguments()
    : m_argc(0)
    , m_argv(nullptr)
  {}
  
  CommandLineArguments(int argc, const char * argv[])
    : m_argc(argc)
    , m_argv(argv)
  {}
  
  bool hasArgument(const char* argName) {
    return Parser::hasArgument(m_argc, m_argv, argName);
  }
  
  v_int32 getArgumentIndex(const char* argName) {
    return Parser::getArgumentIndex(m_argc, m_argv, argName);
  }
  
  const char* getArgumentStartingWith(const char* argNamePrefix, const char* defaultValue = nullptr) {
    return Parser::getArgumentStartingWith(m_argc, m_argv, argNamePrefix, defaultValue);
  }
  
  const char* getNamedArgumentValue(const char* argName, const char* defaultValue = nullptr) {
    return Parser::getNamedArgumentValue(m_argc, m_argv, argName, defaultValue);
  }
  
};
  
}}

#endif /* oatpp_base_CommandLineArguments_hpp */
