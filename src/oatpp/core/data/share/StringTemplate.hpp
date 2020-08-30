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

#ifndef oatpp_data_share_StringTemplate_hpp
#define oatpp_data_share_StringTemplate_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <vector>

namespace oatpp { namespace data { namespace share {

/**
 * String template.
 */
class StringTemplate {
public:

  /**
   * Template variable.
   */
  struct Variable {

    /**
     * Position of the first char in the original template string.
     */
    v_buff_size posStart;

    /**
     * Position of the last char in the original template string.
     */
    v_buff_size posEnd;

    /**
     * Name of the template variable.
     */
    oatpp::String name;

    /**
     * Some auxiliary data.
     */
    std::shared_ptr<void> extra;
  };

public:

  /**
   * Abstract template value provider.
   */
  class ValueProvider {
  public:
    /**
     * Default virtual destructor.
     */
    virtual ~ValueProvider() = default;

    /**
     * Get value for variable.
     * @param variable - &l:StringTemplate::Variable;.
     * @param index - index of the variable in the template.
     * @return - value for the given variable.
     */
    virtual oatpp::String getValue(const Variable& variable, v_uint32 index) = 0;
  };

  /**
   * Provider of template variable-values based on the std::vector.
   */
  class VectorValueProvider : public ValueProvider {
  private:
    const std::vector<oatpp::String>* m_params;
  public:
    VectorValueProvider(const std::vector<oatpp::String>* params);
    oatpp::String getValue(const Variable& variable, v_uint32 index) override;
  };

  /**
   * Provider of template variable-values based on the std::unordered_map.
   */
  class MapValueProvider : public ValueProvider {
  private:
    const std::unordered_map<oatpp::String, oatpp::String>* m_params;
  public:
    MapValueProvider(const std::unordered_map<oatpp::String, oatpp::String>* params);
    oatpp::String getValue(const Variable& variable, v_uint32 index) override;
  };

  /**
   * Provider of template variable-values which returns the same value for all variables.
   */
  class SingleValueProvider : public ValueProvider {
  private:
    oatpp::String m_value;
  public:
    SingleValueProvider(const oatpp::String& value);
    oatpp::String getValue(const Variable& variable, v_uint32 index) override;
  };

private:
  oatpp::String m_text;
  std::vector<Variable> m_variables;
  std::shared_ptr<void> m_extra;
public:

  /**
   * Constructor.
   * @param text - original template text.
   * @param variables - template variables.
   */
  StringTemplate(const oatpp::String& text, std::vector<Variable>&& variables);

  /**
   * Format template.
   * @param stream - stream to write result to.
   * @param valueProvider - &l:StringTemplate::ValueProvider;.
   */
  void format(stream::ConsistentOutputStream* stream, ValueProvider* valueProvider) const;

  /**
   * Format template using &l:StringTemplate::VectorValueProvider;.
   * @param stream - stream to write result to.
   * @param params - `std::vector<oatpp::String>`.
   */
  void format(stream::ConsistentOutputStream* stream, const std::vector<oatpp::String>& params) const;

  /**
   * Format template using &l:StringTemplate::MapValueProvider;.
   * @param stream - stream to write result to.
   * @param params - `std::unordered_map<oatpp::String, oatpp::String>`.
   */
  void format(stream::ConsistentOutputStream* stream, const std::unordered_map<oatpp::String, oatpp::String>& params) const;

  /**
   * Format template using &l:StringTemplate::SingleValueProvider;.
   * @param stream - stream to write result to.
   * @param singleValue - value.
   */
  void format(stream::ConsistentOutputStream* stream, const oatpp::String& singleValue) const;

  /**
   * Format template.
   * @param valueProvider - &l:StringTemplate::ValueProvider;.
   * @return - &id:oatpp::String;.
   */
  oatpp::String format(ValueProvider* valueProvider) const;

  /**
   * Format template using &l:StringTemplate::VectorValueProvider;.
   * @param params - `std::vector<oatpp::String>`.
   * @return - resultant string.
   */
  oatpp::String format(const std::vector<oatpp::String>& params) const;

  /**
   * Format template using &l:StringTemplate::MapValueProvider;.
   * @param params - `std::unordered_map<oatpp::String, oatpp::String>`.
   * @return - resultant string.
   */
  oatpp::String format(const std::unordered_map<oatpp::String, oatpp::String>& params) const;

  /**
   * Format template using &l:StringTemplate::SingleValueProvider;.
   * @param singleValue - value.
   * @return - resultant string.
   */
  oatpp::String format(const oatpp::String& singleValue) const;

  /**
   * Get all template variables.
   * @return - `std::vector` of &l:StringTemplate::Variable;.
   */
  const std::vector<Variable>& getTemplateVariables() const;

  /**
   * Set some extra data associated with the template.
   * @param data
   */
  void setExtraData(const std::shared_ptr<void>& data);

  /**
   * Get extra data associated with the template.
   * @return
   */
  std::shared_ptr<void> getExtraData() const;

};

}}}

#endif // oatpp_data_share_StringTemplate_hpp
