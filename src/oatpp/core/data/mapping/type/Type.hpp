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
  static std::mutex& getClassMutex();
  static std::vector<const char*>& getClassNames();
  static v_int32 registerClassName(const char* name);
public:
  /**
   * Get count of all type classes created.
   * @return
   */
  static int getClassCount();

  /**
   * Get registered class names.
   * @return
   */
  static std::vector<const char*> getRegisteredClassNames();
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

public:

  inline bool operator == (const ClassId& other) const {
    return id == other.id;
  }

  inline bool operator != (const ClassId& other) const {
    return id != other.id;
  }

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

class Void; // FWD

/**
 * ObjectWrapper holds std::shared_ptr to object, object static type, plus object dynamic type information.
 * @tparam T - Object Type.
 * @tparam Clazz - Static type info.
 */
template <class T, class Clazz = __class::Void>
class ObjectWrapper {
  friend Void;
  template <class Q, class W>
  friend class ObjectWrapper;
protected:
  static void checkType(const Type* _this, const Type* other);
protected:
  std::shared_ptr<T> m_ptr;
  const Type* m_valueType;
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
    , m_valueType(Class::getType())
  {}

  ObjectWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : m_ptr(ptr)
    , m_valueType(type)
  {}

  ObjectWrapper(std::shared_ptr<T>&& ptr, const Type* const type)
    : m_ptr(std::move(ptr))
    , m_valueType(type)
  {}
  
public:

  ObjectWrapper()
    : m_valueType(Class::getType())
  {}

  ObjectWrapper(std::nullptr_t)
    : m_valueType(Class::getType())
  {}

  ObjectWrapper(const Type* const type)
    : m_valueType(type)
  {}

  ObjectWrapper(const ObjectWrapper& other)
    : m_ptr(other.m_ptr)
    , m_valueType(other.m_valueType)
  {}

  ObjectWrapper(ObjectWrapper&& other)
    : m_ptr(std::move(other.m_ptr))
    , m_valueType(other.m_valueType)
  {}

  template <class Q, class W>
  ObjectWrapper(const ObjectWrapper<Q, W>& other)
    : m_ptr(other.m_ptr)
    , m_valueType(other.m_valueType)
  {}

  template <class Q, class W>
  ObjectWrapper(ObjectWrapper<Q, W>&& other)
    : m_ptr(std::move(other.m_ptr))
    , m_valueType(other.m_valueType)
  {}

  inline ObjectWrapper& operator=(const ObjectWrapper& other){
    checkType(m_valueType, other.m_valueType);
    m_ptr = other.m_ptr;
    return *this;
  }

  inline ObjectWrapper& operator=(ObjectWrapper&& other){
    checkType(m_valueType, other.m_valueType);
    m_ptr = std::move(other.m_ptr);
    return *this;
  }

  template <class Q, class W>
  inline ObjectWrapper& operator=(const ObjectWrapper<Q, W>& other){
    checkType(m_valueType, other.m_valueType);
    m_ptr = other.m_ptr;
    return *this;
  }

  template <class Q, class W>
  inline ObjectWrapper& operator=(ObjectWrapper<Q, W>&& other){
    checkType(m_valueType, other.m_valueType);
    m_ptr = std::move(other.m_ptr);
    return *this;
  }

  template<class Wrapper>
  Wrapper cast() const;

  inline T* operator->() const {
    return m_ptr.operator->();
  }
  
  T* get() const {
    return m_ptr.get();
  }
  
  void resetPtr(const std::shared_ptr<T>& ptr = nullptr) {
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
   * Get value type
   * @return
   */
  const Type* getValueType() const {
    return m_valueType;
  }
  
};

class Void : public ObjectWrapper<void, __class::Void> {
public:
  Void(const std::shared_ptr<void>& ptr, const type::Type* const valueType)
    : ObjectWrapper<void, __class::Void>(ptr, valueType)
  {}
public:

  Void() {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  Void(T) {}

  Void(const Type* const type)
    : ObjectWrapper<void, __class::Void>(type)
  {}

  Void(const std::shared_ptr<void>& ptr)
    : type::ObjectWrapper<void, __class::Void>(ptr)
  {}

  Void(std::shared_ptr<void>&& ptr)
    : type::ObjectWrapper<void, __class::Void>(std::forward<std::shared_ptr<void>>(ptr))
  {}

  Void(const Void& other)
    : type::ObjectWrapper<void, __class::Void>(other.getPtr(), other.getValueType())
  {}

  Void(Void&& other)
    : type::ObjectWrapper<void, __class::Void>(std::move(other.getPtr()), other.getValueType())
  {}

  template<typename T, typename C>
  Void(const ObjectWrapper<T, C>& other)
    : type::ObjectWrapper<void, __class::Void>(other.getPtr(), other.getValueType())
  {}

  template<typename T, typename C>
  Void(ObjectWrapper<T, C>&& other)
    : type::ObjectWrapper<void, __class::Void>(std::move(other.getPtr()), other.getValueType())
  {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline Void& operator = (std::nullptr_t) {
    m_ptr.reset();
    return *this;
  }

  inline Void& operator = (const Void& other){
    m_ptr = other.m_ptr;
    m_valueType = other.getValueType();
    return *this;
  }

  inline Void& operator = (Void&& other){
    m_ptr = std::move(other.m_ptr);
    m_valueType = other.getValueType();
    return *this;
  }

  template<typename T, typename C>
  inline Void& operator = (const ObjectWrapper<T, C>& other){
    m_ptr = other.m_ptr;
    m_valueType = other.getValueType();
    return *this;
  }

  template<typename T, typename C>
  inline Void& operator = (ObjectWrapper<T, C>&& other){
    m_ptr = std::move(other.m_ptr);
    m_valueType = other.getValueType();
    return *this;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T) const {
    return m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T) const {
    return m_ptr.get() != nullptr;
  }

  template<typename T, typename C>
  inline bool operator == (const ObjectWrapper<T, C> &other) const {
    return m_ptr.get() == other.get();
  }

  template<typename T, typename C>
  inline bool operator != (const ObjectWrapper<T, C> &other) const {
    return m_ptr.get() != other.get();
  }
};

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
      return interpret(originalValue.template cast<OriginalWrapper>());
    }

    Void fromInterpretation(const Void& interValue) const override {
      return reproduce(interValue.template cast<InterWrapper>());
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
   * Type info.
   */
  struct Info {

    /**
     * Default constructor.
     */
    Info() {}

    /**
     * Type name qualifier.
     */
    const char* nameQualifier = nullptr;

    /**
     * List of type parameters - for templated types.
     */
    std::vector<const Type*> params;

    /**
     * PolymorphicDispatcher is responsible for forwarding polymorphic calls to a correct object of type `Type`.
     */
    void* polymorphicDispatcher = nullptr;

    /**
     * Map of type Interpretations.
     */
    InterpretationMap interpretationMap;

    /**
     * Parent type. <br>
     * Ex.: DTO super-class type. <br>
     * **Note:** setting `parent` type also means that child object can be
     * statically casted to parent type without any violations.
     */
    const Type* parent = nullptr;

    /**
     * polymorphicDispatcher extends &id:oatpp::data::mapping::type::__class::Collection::PolymorphicDispatcher;.
     */
     bool isCollection = false;

    /**
     * polymorphicDispatcher extends &id:oatpp::data::mapping::type::__class::Map::PolymorphicDispatcher;.
     */
     bool isMap = false;
  };

public:

  /**
   * Constructor.
   * @param pClassId - type class id.
   * @param typeInfo - type creation info. &l:Type::Info;.
   */
  Type(const ClassId& pClassId, const Info& typeInfo = Info());

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
  const std::vector<const Type*> params;

  /**
   * PolymorphicDispatcher - is an object to forward polymorphic calls to a correct object of type `Type`.
   */
  const void* const polymorphicDispatcher;

  /**
   * Map of type Interpretations.
   */
  const InterpretationMap interpretationMap;

  /**
   * Parent type. <br>
   * Ex.: DTO super-class type. <br>
   * **Note:** setting `parent` type also means that child object can be
   * statically casted to parent type without any violations.
   */
  const Type* const parent;

  /**
   * polymorphicDispatcher extends &id:oatpp::data::mapping::type::__class::Collection::PolymorphicDispatcher;.
   */
  const bool isCollection;

  /**
   * polymorphicDispatcher extends &id:oatpp::data::mapping::type::__class::Map::PolymorphicDispatcher;.
   */
  const bool isMap;

public:

  /**
   * Find type interpretation.
   * @param names - list of possible interpretation names.
   * @return - &l:Type::AbstractInterpretation;. Returns the first interpretation found from the list or NULL if no
   * interpretations found.
   */
  const AbstractInterpretation* findInterpretation(const std::vector<std::string>& names) const;

  /**
   * Check if type extends other type.
   * @param other
   * @return
   */
  bool extends(const Type* other) const;
  
};

template <class T, class Clazz>
template<class Wrapper>
Wrapper ObjectWrapper<T, Clazz>::cast() const {
  if(!Wrapper::Class::getType()->extends(m_valueType)) {
    if(Wrapper::Class::getType() != __class::Void::getType() && m_valueType != __class::Void::getType()) {
      throw std::runtime_error("[oatpp::data::mapping::type::ObjectWrapper::cast()]: Error. Invalid cast "
                               "from '" + std::string(m_valueType->classId.name) + "' to '" +
                               std::string(Wrapper::Class::getType()->classId.name) + "'.");
    }
  }
  return Wrapper(std::static_pointer_cast<typename Wrapper::ObjectType>(m_ptr), Wrapper::Class::getType());
}

template <class T, class Clazz>
void ObjectWrapper<T, Clazz>::checkType(const Type* _this, const Type* other) {
  if(!_this->extends(other)) {
    throw std::runtime_error("[oatpp::data::mapping::type::ObjectWrapper::checkType()]: Error. "
                             "Type mismatch: stored '" + std::string(_this->classId.name) + "' vs "
                             "assigned '" + std::string(other->classId.name) + "'.");
  }
}

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
    this->m_ptr = std::move(other.m_ptr); \
    return *this; \
  } \


}}}}

namespace std {

template<>
struct hash<oatpp::data::mapping::type::ClassId> {

  typedef oatpp::data::mapping::type::ClassId argument_type;
  typedef v_uint64 result_type;

  result_type operator()(argument_type const& v) const noexcept {
    return v.id;
  }

};

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
