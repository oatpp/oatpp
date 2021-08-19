/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "FIFOStreamTest.hpp"

#include "oatpp/core/data/stream/FIFOStream.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/utils/Binary.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace stream {

void FIFOStreamTest::onRun() {

  typedef oatpp::data::stream::FIFOInputStream FIFOInputStream;
  typedef oatpp::data::stream::BufferOutputStream BufferOutputStream;

  {
    FIFOInputStream stream;

    stream.writeSimple("0123456789");

    OATPP_ASSERT(stream.availableToRead() == 10);

    oatpp::String string(3);
    stream.readExactSizeDataSimple((void*)string->data(), 3);

    OATPP_ASSERT(stream.availableToRead() == 7);
    OATPP_ASSERT(string == oatpp::String("012"));

    stream.writeSimple("ABCDEF");
    OATPP_ASSERT(stream.availableToRead() == 13);

    BufferOutputStream outStream;
    stream.flushBufferToStream(&outStream);

    OATPP_ASSERT(outStream.toString() == oatpp::String("3456789ABCDEF"));

  }

  {
    FIFOInputStream stream;

    stream.writeSimple("0123456789");

    BufferOutputStream outStream;
    stream.writeBufferToStream(&outStream, 4);

    OATPP_ASSERT(stream.availableToRead() == 6);
    OATPP_ASSERT(outStream.toString() == oatpp::String("0123"));

    stream.writeBufferToStream(&outStream, 1);
    OATPP_ASSERT(stream.availableToRead() == 5);
    OATPP_ASSERT(outStream.toString() == oatpp::String("01234"));

    stream.writeBufferToStream(&outStream, 5);
    OATPP_ASSERT(stream.availableToRead() == 0);
    OATPP_ASSERT(outStream.toString() == oatpp::String("0123456789"));

    stream.writeSimple("ABCDEF");
    OATPP_ASSERT(stream.availableToRead() == 6);
    stream.flushBufferToStream(&outStream);
    OATPP_ASSERT(stream.availableToRead() == 0);
    OATPP_ASSERT(outStream.toString() == oatpp::String("0123456789ABCDEF"));

  }

}

}}}}}