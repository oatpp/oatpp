/***************************************************************************
 *
 * Fuzz harness: chunked transfer-encoding codec.
 *
 * Threat model: a chunked request/response body is fully attacker-controlled
 * wire data. DecoderChunked must parse attacker-chosen hex chunk sizes,
 * chunk extensions and trailer sections; EncoderChunked is the counterpart
 * every oatpp server/client uses to emit chunked bodies.
 *
 * Part 1 feeds arbitrary bytes through the decoder driven by
 * data::stream::transfer (the production drive loop, small 256-byte buffer
 * to force FLUSH_DATA_OUT paths). Mutation-resistant gates in the wire
 * format (hex chunk-size digits, "\r\n" framing, ';' chunk extensions) are
 * covered by the shipped seed corpus and dictionary.
 *
 * Part 2 is a property: for any payload, encode -> decode must reproduce the
 * payload exactly. A violation is a real codec logic bug, so the harness
 * aborts on mismatch.
 *
 * Expected bug classes: memory safety in the chunk-header state machine,
 * hangs/pathological slowness on adversarial framing, codec logic bugs.
 ***************************************************************************/

#include "oatpp/web/protocol/http/encoding/Chunked.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

#include <cstdlib>
#include <cstring>

namespace {

void fuzzDecode(const uint8_t* data, std::size_t size) {
  oatpp::String text(reinterpret_cast<const char*>(data), size);
  oatpp::data::stream::BufferInputStream inStream(text);
  oatpp::data::stream::BufferOutputStream outStream;
  oatpp::web::protocol::http::encoding::DecoderChunked decoder;

  v_char8 buffer[256];
  oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, sizeof(buffer), &decoder);
}

void fuzzEncodeDecodeProperty(const uint8_t* data, std::size_t size) {

  /* payload = all bytes after the first (first byte drives nothing but
   * input-diversity; payload sizes vary with mutation) */
  const uint8_t* payload = data + 1;
  std::size_t payloadSize = size - 1;
  oatpp::String payloadText(reinterpret_cast<const char*>(payload), payloadSize);

  oatpp::String encoded;
  {
    oatpp::data::stream::BufferInputStream inStream(payloadText);
    oatpp::data::stream::BufferOutputStream outStream;
    oatpp::web::protocol::http::encoding::EncoderChunked encoder;
    v_char8 buffer[256];
    oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, sizeof(buffer), &encoder);
    encoded = outStream.toString();
  }

  oatpp::String decoded;
  {
    oatpp::data::stream::BufferInputStream inStream(encoded);
    oatpp::data::stream::BufferOutputStream outStream;
    oatpp::web::protocol::http::encoding::DecoderChunked decoder;
    v_char8 buffer[256];
    oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, sizeof(buffer), &decoder);
    decoded = outStream.toString();
  }

  if (decoded != payloadText) {
    /* encode->decode round-trip violated: genuine codec bug */
    abort();
  }

}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {

  if (size == 0) {
    return 0;
  }

  try {
    fuzzDecode(data, size);
    fuzzEncodeDecodeProperty(data, size);
  } catch (...) {
    /* decoder reports malformed framing via processor error codes or
     * exceptions; memory-safety faults still surface via ASan */
  }

  return 0;

}
