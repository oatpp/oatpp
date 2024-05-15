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

#include "StringTest.hpp"

#include "oatpp/Types.hpp"

#include <functional>

namespace oatpp { namespace data { namespace  type {

void StringTest::onRun() {

  {
    oatpp::String s = "hello"; // check hash function exists
    std::hash<oatpp::String>{}(s);
  }

  {
    OATPP_LOGi(TAG, "test default constructor")
    oatpp::String s;
    OATPP_ASSERT(!s)
    OATPP_ASSERT(s == nullptr)
    OATPP_ASSERT(s == static_cast<const char*>(nullptr))
    OATPP_ASSERT(s.getValueType() == oatpp::String::Class::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test nullptr constructor")
    oatpp::String s(nullptr);
    OATPP_ASSERT(!s)
    OATPP_ASSERT(s == nullptr)
    OATPP_ASSERT(s.getValueType() == oatpp::String::Class::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test const char* constructor")
    oatpp::String s("abc\0xyz");
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 3)
    OATPP_ASSERT(s == "abc")
    OATPP_ASSERT(s == "abc\0xyz")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test std::string constructor")
    std::string a("abc\0xyz", 7);
    oatpp::String s(a);
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 7)
    OATPP_ASSERT(s != "abc")
    OATPP_ASSERT(s != "abc\0xyz")
    OATPP_ASSERT(s == std::string("abc\0xyz", 7))
    OATPP_ASSERT(s == a)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test std::string move constructor")
    std::string a("abc\0xyz", 7);
    oatpp::String s(std::move(a));
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 7)
    OATPP_ASSERT(s != "abc")
    OATPP_ASSERT(s != "abc\0xyz")
    OATPP_ASSERT(s == std::string("abc\0xyz", 7))
    OATPP_ASSERT(a == "")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test const char* assign operator")
    oatpp::String s;
    s = "abc\0xyz";
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 3)
    OATPP_ASSERT(s == "abc")
    OATPP_ASSERT(s == "abc\0xyz")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test std::string assign operator")
    oatpp::String s;
    std::string a = std::string("abc\0xyz", 7);
    s = a;
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 7)
    OATPP_ASSERT(s != "abc")
    OATPP_ASSERT(s != "abc\0xyz")
    OATPP_ASSERT(s == std::string("abc\0xyz", 7))
    OATPP_ASSERT(s == a)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test std::string move assign operator")
    oatpp::String s;
    std::string a = std::string("abc\0xyz", 7);
    s = std::move(a);
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 7)
    OATPP_ASSERT(s != "abc")
    OATPP_ASSERT(s != "abc\0xyz")
    OATPP_ASSERT(s == std::string("abc\0xyz", 7))
    OATPP_ASSERT(a == "")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test const char* implicit constructor")
    oatpp::String s = "";
    OATPP_ASSERT(s)
    OATPP_ASSERT(s != nullptr)
    OATPP_ASSERT(s->size() == 0)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test '==', '!=' operators")
    oatpp::String s1 = "a";
    oatpp::String s2;

    OATPP_ASSERT(s1 != s2)
    OATPP_ASSERT(s2 != s1)

    OATPP_ASSERT(s1 == "a")
    OATPP_ASSERT(s1 != "aa")
    OATPP_ASSERT(s1 != "")

    s2 = "aa";

    OATPP_ASSERT(s1 != s2)
    OATPP_ASSERT(s2 != s1)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test copy-asssign operator")
    oatpp::String s1 = "s1";
    oatpp::String s2;

    s2 = s1;

    OATPP_ASSERT(s1 == s2)
    OATPP_ASSERT(s1.get() == s2.get())

    s1 = "s2";

    OATPP_ASSERT(s1 != s2)
    OATPP_ASSERT(s2 != s1)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test const char* assign operator")
    oatpp::String s1 = "s1";
    oatpp::String s2(s1);

    OATPP_ASSERT(s1 == s2)
    OATPP_ASSERT(s1.get() == s2.get())

    s1 = "s2";

    OATPP_ASSERT(s1 != s2)
    OATPP_ASSERT(s2 != s1)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test move assign operator")
    oatpp::String s1 = "s1";
    oatpp::String s2;

    s2 = std::move(s1);

    OATPP_ASSERT(s1 == nullptr)
    OATPP_ASSERT(s2 != nullptr)
    OATPP_ASSERT(s2 == "s1")

    OATPP_ASSERT(s1 != s2)
    OATPP_ASSERT(s1.get() != s2.get())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test compareCI_ASCII methods 1")

    oatpp::String s1 = "hello";

    {
      oatpp::String s2;
      OATPP_ASSERT(!s1.equalsCI_ASCII(s2))
    }

    {
      const char* s2 = nullptr;
      OATPP_ASSERT(!s1.equalsCI_ASCII(s2))
    }

  }

  {
    OATPP_LOGi(TAG, "test compareCI_ASCII methods 2")

    oatpp::String s1;

    {
      oatpp::String s2 = "hello";
      OATPP_ASSERT(!s1.equalsCI_ASCII(s2))
    }

    {
      std::string s2 = "hello";
      OATPP_ASSERT(!s1.equalsCI_ASCII(s2))
    }

    {
      const char* s2 = "hello";
      OATPP_ASSERT(!s1.equalsCI_ASCII(s2))
    }

    {
      oatpp::String s2;
      OATPP_ASSERT(s1.equalsCI_ASCII(s2))
    }

    {
      const char* s2 = nullptr;
      OATPP_ASSERT(s1.equalsCI_ASCII(s2))
    }

    {
      OATPP_ASSERT(s1.equalsCI_ASCII(nullptr))
    }

    {

      bool exceptionThrown = false;

      try {
        std::string s2 = s1;
      } catch (const std::runtime_error&) {
        exceptionThrown = true;
      }

      OATPP_ASSERT(exceptionThrown)

    }

  }

  {
    OATPP_LOGi(TAG, "test compareCI_ASCII methods 3")

    oatpp::String s1 = "hello";

    {
      oatpp::String s2 = "HELLO";
      OATPP_ASSERT(s1.equalsCI_ASCII(s2))
    }

    {
      std::string s2 = "HELLO";
      OATPP_ASSERT(s1.equalsCI_ASCII(s2))
    }

    {
      const char* s2 = "HELLO";
      OATPP_ASSERT(s1.equalsCI_ASCII(s2))
    }

    {
      OATPP_ASSERT(s1.equalsCI_ASCII("HELLO"))
    }

  }

  {
    OATPP_LOGi(TAG, "check default value")
    oatpp::String s0;
    oatpp::String s1 = "hello";
    OATPP_ASSERT(s0.getValue("def") == "def")
    OATPP_ASSERT(s1.getValue("def") == "hello")
  }

}

}}}
