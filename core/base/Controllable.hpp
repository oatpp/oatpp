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

#ifndef oatpp_base_Controllable
#define oatpp_base_Controllable

#include <memory>
#include "./Environment.hpp"

namespace oatpp { namespace base{
  
class Controllable : public std::enable_shared_from_this<Controllable> {
private:
  static const char* TAG;
public:
  template<class T>
  std::shared_ptr<T> getSharedPtr() {
    return std::static_pointer_cast<T>(shared_from_this());
  }
public:
  Controllable();
  virtual ~Controllable();
  
  static std::shared_ptr<Controllable> createShared(){
    return std::shared_ptr<Controllable>(new Controllable);
  }
  
};
  
}}

#endif /* oatpp_base_Controllable */
