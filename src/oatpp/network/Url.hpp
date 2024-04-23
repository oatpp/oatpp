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

#ifndef oatpp_network_Url_hpp
#define oatpp_network_Url_hpp

#include "oatpp/data/share/LazyStringMap.hpp"
#include "oatpp/utils/parser/Caret.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace network {


/**
 * Class holding URL information.
 */
class Url {
public:
  /**
   * Convenience typedef for &id:oatpp::data::share::StringKeyLabel;.
   */
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:
  /**
   * Parameters - map string to string. &id:oatpp::data::share::LazyStringMultimap;.
   */
  typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabel> Parameters;

public:

  /**
   * Structure representing URL Authority information.
   */
  struct Authority {
    /**
     * User info.
     */
    oatpp::String userInfo;

    /**
     * Host.
     */
    oatpp::String host;

    /**
     * Port. Treat -1 as undefined or as default.
     */
    v_int32 port = -1;
  };
  
public:

  /**
   * Url parser.
   */
  class Parser {
  public:
    
    /**
     * parse `<scheme>`:
     * example "http", "https", "ftp"
     * returns lowercase string before ':' char
     * caret should be at the first char of the scheme
     */
    static oatpp::String parseScheme(oatpp::utils::parser::Caret& caret);
    
    /**
     * parse url authority components.
     * userinfo is not parsed into login and password separately as
     * inclusion of password in userinfo is deprecated and ignored here
     * caret should be at the first char of the authority (not at "//")
     */
    static Url::Authority parseAuthority(oatpp::utils::parser::Caret& caret);
    
    /**
     * parse path of the url
     * caret should be at the first char of the path
     */
    static oatpp::String parsePath(oatpp::utils::parser::Caret& caret);
    
    /**
     * parse query params in form of `"?<paramName>=<paramValue>&<paramName>=<paramValue>..."` referred by ParsingCaret
     * and put that params to Parameters map
     */
    static void parseQueryParams(Url::Parameters& params, oatpp::utils::parser::Caret& caret);
    
    /**
     * parse query params in form of `"?<paramName>=<paramValue>&<paramName>=<paramValue>..."` referred by str
     * and put that params to Parameters map
     */
    static void parseQueryParams(Url::Parameters& params, const oatpp::String& str);
    
    /**
     * parse query params in form of `"?<paramName>=<paramValue>&<paramName>=<paramValue>..."` referred by ParsingCaret
     */
    static Url::Parameters parseQueryParams(oatpp::utils::parser::Caret& caret);
    
    /**
     * parse query params in form of `"?<paramName>=<paramValue>&<paramName>=<paramValue>..."` referred by str
     */
    static Url::Parameters parseQueryParams(const oatpp::String& str);

    /**
     * Parse Url
     * @param caret
     * @return parsed URL structure
     */
    static Url parseUrl(oatpp::utils::parser::Caret& caret);

    /**
     * Parse Url
     * @param str
     * @return parsed URL structure
     */
    static Url parseUrl(const oatpp::String& str);
    
  };
  
public:

  /**
   * Url scheme. Ex.: [http, https, ftp, etc.]
   */
  oatpp::String scheme;

  /**
   * Utl authority.
   */
  Authority authority;

  /**
   * Path to resource.
   */
  oatpp::String path;

  /**
   * Query params.
   */
  Parameters queryParams;
  
};
  
}}

#endif /* oatpp_network_url_Url_hpp */
