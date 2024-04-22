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

#ifndef oatpp_web_client_RetryPolicy_hpp
#define oatpp_web_client_RetryPolicy_hpp

#include "oatpp/Types.hpp"

#include <unordered_set>

namespace oatpp { namespace web { namespace client {

/**
 * Class to control retries in RequestExecutor.
 */
class RetryPolicy {
public:

  /**
   * This structure holds information about request attempts.
   */
  struct Context {

    /**
     * Attempt number.
     */
    v_int64 attempt = 0;
  };

public:

  /**
   * Virtual destructor.
   */
  virtual ~RetryPolicy() = default;

  /**
   * Check if the context is eligible to retry.
   * @param context - &l:RetryPolicy::Context ;.
   * @return - `true` - to retry. `false` - do NOT retry.
   */
  virtual bool canRetry(const Context& context) = 0;

  /**
   * Check whether the client should retry for a given response from the server.
   * @param responseStatusCode - HTTP status code of the response.
   * @param context - &l:RetryPolicy::Context ;.
   * @return - `true` - to retry. `false` - do NOT retry.
   */
  virtual bool retryOnResponse(v_int32 responseStatusCode, const Context& context) = 0;

  /**
   * How much client should wait before the next attempt?
   * @param context - &l:RetryPolicy::Context ;.
   * @return - delay in microseconds.
   */
  virtual v_int64 waitForMicroseconds(const Context& context) = 0;

};

class SimpleRetryPolicy : public RetryPolicy {
private:
  v_int64 m_maxAttempts;
  v_int64 m_delay;
  std::unordered_set<v_int32> m_httpCodes;
public:

  /**
   * Constructor.
   * @param maxAttempts - max number of attempts to retry. `-1` - retry infinitely.
   * @param delay - delay between attempts.
   * @param httpCodes - set of HTTP codes to retry for.
   */
  SimpleRetryPolicy(v_int64 maxAttempts,
                    const std::chrono::duration<v_int64, std::micro>& delay,
                    const std::unordered_set<v_int32>& httpCodes = {503});

  /**
   * Check if the context is eligible to retry.
   * @param context - &l:RetryPolicy::Context ;.
   * @return - `true` - to retry. `false` - do NOT retry.
   */
  bool canRetry(const Context& context) override;

  /**
   * Check whether the client should retry for a given response from the server. <br>
   * *This particular implementation returns `true` for codes from the set provided in the constructor*.
   * @param responseStatusCode - HTTP status code of the response.
   * @param context - &l:RetryPolicy::Context ;.
   * @return - `true` - to retry. `false` - do NOT retry.
   */
  bool retryOnResponse(v_int32 responseStatusCode, const Context& context) override;

  /**
   * How much client should wait before the next attempt? <br>
   * *This particular implementation returns the delay passed to the constructor*.
   * @param context - &l:RetryPolicy::Context ;.
   * @return - delay in microseconds.
   */
  v_int64 waitForMicroseconds(const Context& context) override;

};

}}}

#endif // oatpp_web_client_RetryPolicy_hpp
