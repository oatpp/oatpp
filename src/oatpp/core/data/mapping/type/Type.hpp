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

#ifndef oatpp_data_type_Type_hpp
#define oatpp_data_type_Type_hpp

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <list>
#include <unordered_map>
#include <vector>
#include <string>

namespace oatpp { namespace data { namespace mapping { namespace type {
  
class Type; // FWD

/**
 * Structure representing `ID` of the type class.
 */
class ClassId {
private:
  static std::atomic_int ID_COUNTER;
public:
  /**
   * Get count of all type classes created.
   * @return
   */
  static int getClassCount();
public:

  /**
   * Constructor.
   * @param pName
   */
  ClassId(const char* pName);

  /**
   * Name of the type class.
   */
  const char* const name;

  /**
   * Integer ID of the type class. <br>
   * *Note: class type IDs are integer values incremented continuously from [0 to `getClassCount()`]*
   */
  const v_int32 id;
};


namespace __class {
  /**
   * Void Object Class.
   */
  class Void {
  public:
    /**
     * Class id.
     */
    static const ClassId CLASS_ID;

    /**
     * Get class type information.
     * @return - &l:Type;
     */
    static Type* getType();
  };

}

/**
 * ObjectWrapper holds std::shared_ptr to object, object static type, plus object dynamic type information.
 * @tparam T - Object Type.
 * @tparam Clazz - Static type info.
 */
template <class T, class Clazz = __class::Void>
class ObjectWrapper {
protected:
  std::shared_ptr<T> m_ptr;
public:

  /**
   * Static object type
   */
  typedef T ObjectType;

  /**
   * Static object class information.
   */
  typedef Clazz Class;
public:

  ObjectWrapper(const std::shared_ptr<T>& ptr)
    : m_ptr(ptr)
    , valueType(Class::getType())
  {}

  ObjectWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : m_ptr(ptr)
    , valueType(type)
  {}

  ObjectWrapper(std::shared_ptr<T>&& ptr, const Type* const type)
    : m_ptr(std::move(ptr))
    , valueType(type)
  {}
  
public:

  ObjectWrapper()
    : valueType(Class::getType())
  {}

  ObjectWrapper(std::nullptr_t)
    : valueType(Class::getType())
  {}

  ObjectWrapper(const Type* const type)
    : valueType(type)
  {}

  ObjectWrapper(const ObjectWrapper& other)
    : m_ptr(other.m_ptr)
    , valueType(other.valueType)
  {}

  ObjectWrapper(ObjectWrapper&& other)
    : m_ptr(std::move(other.m_ptr))
    , valueType(other.valueType)
  {}

  inline ObjectWrapper& operator=(const ObjectWrapper& other){
    m_ptr = other.m_ptr;
    return *this;
  }

  inline ObjectWrapper& operator=(ObjectWrapper&& other){
    m_ptr = std::move(other.m_ptr);
    return *this;
  }
  
  inline operator ObjectWrapper<void>() const {
    return ObjectWrapper<void>(this->m_ptr, valueType);
  }

  template<class Wrapper>
  Wrapper staticCast() const {
    return Wrapper(std::static_pointer_cast<typename Wrapper::ObjectType>(m_ptr), valueType);
  }

  inline T* operator->() const {
    return m_ptr.operator->();
  }
  
  T* get() const {
    return m_ptr.get();
  }
  
  void setPtr(const std::shared_ptr<T>& ptr) {
    m_ptr = ptr;
  }
  
  std::shared_ptr<T> getPtr() const {
    return m_ptr;
  }

  inline bool operator == (std::nullptr_t) const {
    return m_ptr.get() == nullptr;
  }

  inline bool operator != (std::nullptr_t) const {
    return m_ptr.get() != nullptr;
  }
  
  inline bool operator == (const ObjectWrapper& other) const {
    return m_ptr.get() == other.m_ptr.get();
  }
  
  inline bool operator != (const ObjectWrapper& other) const {
    return m_ptr.get() != other.m_ptr.get();
  }
  
  explicit inline operator bool() const {
    return m_ptr.operator bool();
  }

  /**
   * Value type information.
   * See &l:Type;.
   */
  const Type* const valueType;
  
};

typedef ObjectWrapper<void, __class::Void> Void;

template <typename T>
struct ObjectWrapperByUnderlyingType {};

/**
 * Object type data.
 */
class Type {
public:

  /**
   * Type Abstract Interpretation.
   */
  class AbstractInterpretation {
  public:

    virtual ~AbstractInterpretation() = default;

    /**
     * Convert the object to its interpretation.
     * @param originalValue
     * @return
     */
    virtual Void toInterpretation(const Void& originalValue) const = 0;

    /**
     * Convert interpretation back to the original object.
     * @param interValue
     * @return
     */
    virtual Void fromInterpretation(const Void& interValue) const = 0;

    /**
     * Type of the interpretation.
     * @return
     */
    virtual const Type* getInterpretationType() const = 0;
  };

  template<class OriginalWrapper, class InterWrapper>
  class Interpretation : public AbstractInterpretation {
  public:

    Void toInterpretation(const Void& originalValue) const override {
      return interpret(originalValue.staticCast<OriginalWrapper>());
    }

    Void fromInterpretation(const Void& interValue) const override {
      return reproduce(interValue.staticCast<InterWrapper>());
    }

    const Type* getInterpretationType() const override {
      return InterWrapper::Class::getType();
    }

  public:

    virtual InterWrapper interpret(const OriginalWrapper& value) const = 0;
    virtual OriginalWrapper reproduce(const InterWrapper& value) const = 0;

  };

  typedef std::unordered_map<std::string, const AbstractInterpretation*> InterpretationMap;

public:

  /**
   * Constructor.
   * @param pClassId - type class id.
   * @param pNameQualifier - type name qualifier.
   * @param pPolymorphicDispatcher - is an object to forward polymorphic calls to a correct object of type `Type`.
   * @param pInterpretationMap - Map of type Interpretations.
   */
  Type(const ClassId& pClassId,
       const char* pNameQualifier,
       void* pPolymorphicDispatcher = nullptr,
       InterpretationMap&& pInterpretationMap = InterpretationMap{});

  /**
   * type class id.
   */
  const ClassId classId;

  /**
   * Type name qualifier.
   */
  const char* const nameQualifier;

  /**
   * List of type parameters - for templated types.
   */
  std::list<const Type*> params;

  /**
   * PolymorphicDispatcher - is an object to forward polymorphic calls to a correct object of type `Type`.
   */
  const void* const polymorphicDispatcher;

  /**
   * Map of type Interpretations.
   */
  const InterpretationMap interpretationMap;

public:

  /**
   * Find type interpretation.
   * @param names - list of possible interpretation names.
   * @return - &l:Type::AbstractInterpretation;. Returns the first interpretation found from the list or NULL if no
   * interpretations found.
   */
  const AbstractInterpretation* findInterpretation(const std::vector<std::string>& names) const;
  
};

#define OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(WRAPPER_NAME, OBJECT_TYPE, OBJECT_CLASS) \
public: \
  WRAPPER_NAME(const std::shared_ptr<OBJECT_TYPE>& ptr, const type::Type* const valueType) \
    : type::ObjectWrapper<OBJECT_TYPE, OBJECT_CLASS>(ptr, valueType) \
  {} \
public: \
\
  WRAPPER_NAME() {} \
\
  WRAPPER_NAME(std::nullptr_t) {} \
\
  WRAPPER_NAME(const std::shared_ptr<OBJECT_TYPE>& ptr) \
    : type::ObjectWrapper<OBJECT_TYPE, OBJECT_CLASS>(ptr) \
  {} \
\
  WRAPPER_NAME(std::shared_ptr<OBJECT_TYPE>&& ptr) \
    : type::ObjectWrapper<OBJECT_TYPE, OBJECT_CLASS>(std::forward<std::shared_ptr<OBJECT_TYPE>>(ptr)) \
  {} \
\
  WRAPPER_NAME(const WRAPPER_NAME& other) \
    : type::ObjectWrapper<OBJECT_TYPE, OBJECT_CLASS>(other) \
  {} \
\
  WRAPPER_NAME(WRAPPER_NAME&& other) \
    : type::ObjectWrapper<OBJECT_TYPE, OBJECT_CLASS>(std::forward<WRAPPER_NAME>(other)) \
  {} \
\
  inline WRAPPER_NAME& operator = (std::nullptr_t) { \
    this->m_ptr.reset(); \
    return *this; \
  } \
\
  inline WRAPPER_NAME& operator = (const WRAPPER_NAME& other) { \
    this->m_ptr = other.m_ptr; \
    return *this; \
  } \
\
  inline WRAPPER_NAME& operator = (WRAPPER_NAME&& other) { \
    this->m_ptr = std::forward<std::shared_ptr<OBJECT_TYPE>>(other.m_ptr); \
    return *this; \
  } \


}}}}

namespace std {

template<>
struct hash<oatpp::data::mapping::type::Void> {

  typedef oatpp::data::mapping::type::Void argument_type;
  typedef v_uint64 result_type;

  result_type operator()(argument_type const& v) const noexcept {
    return (result_type) v.get();
  }

};

}
  
#endif /* oatpp_data_type_Type_hpp */
