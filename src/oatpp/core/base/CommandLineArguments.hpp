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

#ifndef oatpp_base_CommandLineArguments_hpp
#define oatpp_base_CommandLineArguments_hpp

#include "./Environment.hpp"

namespace oatpp { namespace base {

/**
 * Class for storing and managing Command Line arguments.
 */
class CommandLineArguments {
public:

  /**
   * Command Line arguments parser.
   */
  class Parser {
  public:

    /**
     * Check the specified argument is present among command line arguments.
     * @param argc - count of arguments in argv array.
     * @param argv - array of arguments.
     * @param argName - name of the target argument.
     * @return - `true` if `getArgumentIndex(argName) >= 0`
     */
    static bool hasArgument(int argc, const char * argv[], const char* argName);
    
    /**
     * get index of the argument with the name == argName
     */

    /**
     * Get index of the argument specified by name in the argv[] array.
     * @param argc - count of arguments in argv array.
     * @param argv - array of arguments.
     * @param argName - name of the target argument.
     * @return - index of the argument in argv[] array. -1 if there is no such argument.
     */
    static v_int32 getArgumentIndex(int argc, const char * argv[], const char* argName);

    /**
     * Get argument which starts with the prefix. <br>
     * Example: <br>
     * For command line: `-k -c 1000 -n 100 http://127.0.0.1:8000/` <br>
     * `getArgumentWhichStartsWith("http") == http://127.0.0.1:8000/`
     * @param argc - count of arguments in argv array.
     * @param argv - array of arguments.
     * @param argNamePrefix - prefix to search.
     * @param defaultValue - default value to return in case not found.
     * @return - argument which starts with the specified prefix.
     */
    static const char* getArgumentStartingWith(int argc, const char * argv[], const char* argNamePrefix, const char* defaultValue = nullptr);

    /**
     * Get value preceded by the argument. <br>
     * Example: <br>
     * For command line: `-k -c 1000 -n 100` <br>
     * `getNamedArgumentValue("-c") == "1000"`, `getNamedArgumentValue("-n") == "100"`
     * @param argc - count of arguments in argv array.
     * @param argv - array of arguments.
     * @param argName - name of the preceded argument.
     * @param defaultValue - default value to return in case not found.
     * @return - value preceded by the argument.
     */
    static const char* getNamedArgumentValue(int argc, const char * argv[], const char* argName, const char* defaultValue = nullptr);
    
  };
  
private:
  int m_argc;
  const char ** m_argv;
public:

  /**
   * Default constructor.
   */
  CommandLineArguments();

  /**
   * Constructor.
   * @param argc - count of arguments in argv[] array.
   * @param argv - array of arguments.
   */
  CommandLineArguments(int argc, const char * argv[]);

  /**
   * Check the specified argument is present.
   * @param argName - name of the target argument.
   * @return - `true` if present.
   */
  bool hasArgument(const char* argName) const {
    return Parser::hasArgument(m_argc, m_argv, argName);
  }

  /**
   * Get index of the argument specified by name.
   * @param argName - name of the target argument.
   * @return - index of the argument in argv[] array. -1 if there is no such argument.
   */
  v_int32 getArgumentIndex(const char* argName) const {
    return Parser::getArgumentIndex(m_argc, m_argv, argName);
  }

  /**
   * Get argument which starts with the prefix. <br>
   * Example: <br>
   * For command line: `-k -c 1000 -n 100 'http://127.0.0.1:8000/'` <br>
   * `getArgumentWhichStartsWith("http") == http://127.0.0.1:8000/`
   * @param argNamePrefix - prefix to search.
   * @param defaultValue - default value to return in case not found.
   * @return - argument which starts with the specified prefix. defaultValue if not found.
   */
  const char* getArgumentStartingWith(const char* argNamePrefix, const char* defaultValue = nullptr) const {
    return Parser::getArgumentStartingWith(m_argc, m_argv, argNamePrefix, defaultValue);
  }

  /**
   * Get value preceded by the argument. <br>
   * Example: <br>
   * For command line: `-k -c 1000 -n 100` <br>
   * `getNamedArgumentValue("-c") == "1000"`, `getNamedArgumentValue("-n") == "100"`
   * @param argName - name of the preceded argument.
   * @param defaultValue - default value to return in case not found.
   * @return - value preceded by the argument. defaultValue if not found.
   */
  const char* getNamedArgumentValue(const char* argName, const char* defaultValue = nullptr) const {
    return Parser::getNamedArgumentValue(m_argc, m_argv, argName, defaultValue);
  }
  
};
  
}}

#endif /* oatpp_base_CommandLineArguments_hpp */
