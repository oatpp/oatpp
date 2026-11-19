/***************************************************************************
 *
 * Fuzz harness: URL parsing + URL-pattern routing.
 *
 * Threat model: request URLs are fully attacker-controlled. network::Url
 * parses scheme/authority/port/path/query-params from the raw request
 * target; web::url::mapping::Pattern parses route patterns and matches URLs
 * against them; web::server::HttpRouter drives pattern matching for every
 * incoming request. All three were previously unreachable by any fuzz
 * target (the web/ and network/ modules were not linked into the binary).
 *
 * Input shape: first line = route pattern, remainder = URL. Both are pure
 * string data an attacker controls; splitting them mirrors production where
 * the pattern is developer-owned and the URL attacker-owned, but both sides
 * of the match are fuzzed to also catch pattern-side blowups.
 *
 * Expected bug classes: memory safety in Caret-based string slicing, int
 * parsing of the port field, pathological blowups in pattern matching and
 * router traversal.
 ***************************************************************************/

#include "oatpp/network/Url.hpp"
#include "oatpp/web/url/mapping/Pattern.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include <string>
#include <unordered_map>
#include <cstddef>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {

  if (size < 3) {
    return 0;
  }

  try {

    /* split: first line = pattern, remainder = URL */
    std::size_t nl = 0;
    while (nl < size && data[nl] != '\n') {
      ++nl;
    }
    if (nl == 0 || nl >= size) {
      return 0;
    }
    std::size_t patternLen = nl;
    if (data[patternLen - 1] == '\r') {
      patternLen -= 1;
    }
    if (patternLen == 0 || patternLen > 256) {
      return 0;
    }

    oatpp::String patternText(reinterpret_cast<const char*>(data), patternLen);
    oatpp::String urlText(reinterpret_cast<const char*>(data + nl + 1), size - nl - 1);

    /* URL parsing */
    oatpp::network::Url url = oatpp::network::Url::Parser::parseUrl(urlText);
    oatpp::network::Url::Parameters params;
    oatpp::network::Url::Parser::parseQueryParams(params, urlText);
    (void) url.scheme;
    (void) url.path;

    /* pattern parse + match + reconstruct + toString */
    auto pattern = oatpp::web::url::mapping::Pattern::parse(patternText);
    if (pattern) {
      oatpp::web::url::mapping::Pattern::MatchMap matchMap;
      bool matched = pattern->match(urlText, matchMap);
      if (matched) {
        std::unordered_map<oatpp::String, oatpp::String> vars;
        for (const auto& v : matchMap.getVariables()) {
          vars[v.first.toString()] = v.second.toString();
        }
        auto rebuilt = pattern->reconstruct(vars, matchMap.getTail());
        (void) rebuilt;
      }
      auto asText = pattern->toString();
      (void) asText;
    }

    /* router: register fuzzed pattern, resolve fuzzed URL against it */
    oatpp::web::server::HttpRouterTemplate<v_int32> router;
    router.route("GET", patternText, 1);
    router.route("POST", "/", 2);
    router.route("GET", "*", 3);
    auto route = router.getRoute("GET", urlText);
    if (route.isValid() && route) {
      (void) route.getEndpoint();
    }

  } catch (...) {
    /* routers and pattern matching report failures by non-match; faults
     * surfaced by ASan before an exception are findings */
  }

  return 0;

}
