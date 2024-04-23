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

#include "CaretTest.hpp"

#include "oatpp/utils/parser/Caret.hpp"

namespace oatpp { namespace utils { namespace parser {

void CaretTest::onRun() {

  {
    Caret caret(" \t\n\r\f \t\n\r\f \t\n\r\fhello!\t\n\r\f");
    OATPP_ASSERT(caret.skipBlankChars())
    OATPP_ASSERT(caret.isAtChar('h'))
    OATPP_ASSERT(caret.isAtText("hello!"))
    OATPP_ASSERT(caret.isAtText("hello!", true))
    OATPP_ASSERT(caret.skipBlankChars() == false) // false because no other char found
    OATPP_ASSERT(caret.canContinue() == false)
    OATPP_ASSERT(caret.getPosition() == caret.getDataSize())
  }

  {
    Caret caret(" \t\n\r\f \t\n\r\f \t\n\r\fhello!\t\n\r\f");
    OATPP_ASSERT(caret.findText("hello!"))
    OATPP_ASSERT(caret.isAtText("hello!"))
    OATPP_ASSERT(caret.isAtTextNCS("HELLO!"))
    OATPP_ASSERT(caret.isAtTextNCS("HELLO!", true))
    OATPP_ASSERT(caret.skipBlankChars() == false) // false because no other char found
    OATPP_ASSERT(caret.canContinue() == false)
    OATPP_ASSERT(caret.getPosition() == caret.getDataSize())
  }

  {
    Caret caret(" \t\n\r\f \t\n\r\f \t\n\r\fhello!\t\n\r\f");
    OATPP_ASSERT(caret.findText("hello world!") == false)
    OATPP_ASSERT(caret.canContinue() == false)
    OATPP_ASSERT(caret.getPosition() == caret.getDataSize())
  }

  {
    Caret caret("\r\n'let\\'s'\r\n'play'");
    OATPP_ASSERT(caret.findRN())
    OATPP_ASSERT(caret.skipRN())

    auto label = caret.parseStringEnclosed('\'', '\'', '\\');
    OATPP_ASSERT(label)
    OATPP_ASSERT(label.toString() == "let\\'s")

    OATPP_ASSERT(caret.skipRN())

    label = caret.parseStringEnclosed('\'', '\'', '\\');
    OATPP_ASSERT(label)
    OATPP_ASSERT(label.toString() == "play")

    OATPP_ASSERT(caret.canContinue() == false)
    OATPP_ASSERT(caret.getPosition() == caret.getDataSize())
  }

}

}}}
