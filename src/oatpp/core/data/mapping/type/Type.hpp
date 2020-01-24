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

#include <list>
#include <unordered_map>
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
     * Name of the class - CLASS_NAME = "Void".
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
 * PolymorphicWrapper holds std::shared_ptr to object, object static type, plus object dynamic type information.
 * @tparam T - Object Type
 */
template <class T>
class PolymorphicWrapper {
protected:
  std::shared_ptr<T> m_ptr;
public:
  /**
   * Static object type
   */
  typedef T ObjectType;
public:
  /**
   * PolymorphicWrapper has no static object Class info.
   * It treats object as of class &id:oatpp::data::mapping::type::__class::Void;.
   */
  typedef __class::Void Class;
public:
  
  PolymorphicWrapper(const std::shared_ptr<T>& ptr)
    : m_ptr(ptr)
    , valueType(Class::getType())
  {}
  
  PolymorphicWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : m_ptr(ptr)
    , valueType(type)
  {}
  
  PolymorphicWrapper(std::shared_ptr<T>&& ptr, const Type* const type)
    : m_ptr(std::move(ptr))
    , valueType(type)
  {}
  
public:
  
  PolymorphicWrapper()
    : valueType(Class::getType())
  {}
  
  PolymorphicWrapper(const Type* const type)
    : valueType(type)
  {}
  
  PolymorphicWrapper(const PolymorphicWrapper& other)
    : m_ptr(other.m_ptr)
    , valueType(other.valueType)
  {}
  
  PolymorphicWrapper(PolymorphicWrapper&& other)
    : m_ptr(std::move(other.m_ptr))
    , valueType(other.valueType)
  {}
  
  static PolymorphicWrapper empty(){
    return PolymorphicWrapper();
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>& other){
    m_ptr = other.m_ptr;
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>&& other){
    m_ptr = std::move(other.m_ptr);
    return *this;
  }
  
  inline operator PolymorphicWrapper<oatpp::base::Countable>() const {
    return PolymorphicWrapper<oatpp::base::Countable>(this->m_ptr, valueType);
  }
  
  T* operator->() const {
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
  
  inline bool operator == (const PolymorphicWrapper& other){
    return m_ptr.get() == other.m_ptr.get();
  }
  
  inline bool operator != (const PolymorphicWrapper& other){
    return m_ptr.get() != other.m_ptr.get();
  }
  
  explicit operator bool() const {
    return m_ptr.operator bool();
  }

  /**
   * Value type information.
   * See &l:Type;.
   */
  const Type* const valueType;
  
};
  
template<class T, class F>
inline PolymorphicWrapper<T> static_wrapper_cast(const F& from){
  return PolymorphicWrapper<T>(std::static_pointer_cast<T>(from.getPtr()), from.valueType);
}

/**
 * ObjectWrapper holds std::shared_ptr to object, object static type, object static class information, plus object dynamic type information.
 * @tparam T - Object type.
 * @tparam Clazz - Static Object class information.
 */
template <class T, class Clazz>
class ObjectWrapper : public PolymorphicWrapper<T>{
public:
  /**
   * Object type.
   */
  typedef T ObjectType;
public:
  /**
   * Static object class information.
   */
  typedef Clazz Class;
public:
  ObjectWrapper(const std::shared_ptr<T>& ptr, const type::Type* const valueType)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {
    if(Class::getType() != valueType){
      throw std::runtime_error("Value type does not match");
    }
  }
public:
  
  ObjectWrapper()
    : PolymorphicWrapper<T>(Class::getType())
  {}
  
  ObjectWrapper(std::nullptr_t nullptrt)
    : PolymorphicWrapper<T>(nullptr, Class::getType())
  {(void)nullptrt;}
  
  ObjectWrapper(const std::shared_ptr<T>& ptr)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {}
  
  ObjectWrapper(const PolymorphicWrapper<T>& other)
    : PolymorphicWrapper<T>(other.getPtr(), Class::getType())
  {}
  
  ObjectWrapper(PolymorphicWrapper<T>&& other)
    : PolymorphicWrapper<T>(std::move(other.getPtr()), Class::getType())
  {}
  
  static ObjectWrapper empty(){
    return ObjectWrapper();
  }
  
  ObjectWrapper& operator=(const PolymorphicWrapper<T>& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("ObjectWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::ObjectWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (other);
    return *this;
  }
  
  ObjectWrapper& operator=(const PolymorphicWrapper<T>&& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("ObjectWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::ObjectWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (std::forward<PolymorphicWrapper<T>>(other));
    return *this;
  }
  
};

/**
 * PolymorphicWrapper over &id:oatpp::base::Countable; object.
 */
typedef PolymorphicWrapper<oatpp::base::Countable> AbstractObjectWrapper;

/**
 * Object type data.
 */
class Type {
public:
  typedef AbstractObjectWrapper (*Creator)();
public:
  class Property; // FWD
public:

  /**
   * Object type properties table.
   */
  class Properties {
  private:
    std::unordered_map<std::string, Property*> m_map;
    std::list<Property*> m_list;
  public:

    /**
     * Add property to the end of the list.
     * @param property
     */
    void pushBack(Property* property);

    /**
     * Add all properties to the beginning of the list.
     * @param properties
     */
    void pushFrontAll(Properties* properties);

    /**
     * Get properties as unordered map for random access.
     * @return reference to std::unordered_map of std::string to &id:oatpp::data::mapping::type::Type::Property;*.
     */
    const std::unordered_map<std::string, Property*>& getMap() const {
      return m_map;
    }

    /**
     * Get properties in ordered way.
     * @return std::list of &id:oatpp::data::mapping::type::Type::Property;*.
     */
    const std::list<Property*>& getList() const {
      return m_list;
    }
    
  };

public:

  /**
   * Class to map object properties.
   */
  class Property {
  private:
    const v_int64 offset;
  public:

    /**
     * Constructor.
     * @param properties - &l:Type::Properties;*. to push this property to.
     * @param pOffset - memory offset of object field from object start address.
     * @param pName - name of the property.
     * @param pType - &l:Type; of the property.
     */
    Property(Properties* properties, v_int64 pOffset, const char* pName, Type* pType);

    /**
     * Property name.
     */
    const char* const name;

    /**
     * Property type.
     */
    const Type* const type;

    /**
     * Set value of object field mapped by this property.
     * @param object - object address.
     * @param value - value to set.
     */
    void set(void* object, const AbstractObjectWrapper& value);

    /**
     * Get value of object field mapped by this property.
     * @param object - object address.
     * @return - value of the field.
     */
    AbstractObjectWrapper get(void* object);

    /**
     * Get reference to ObjectWrapper of the object field.
     * @param object - object address.
     * @return - reference to ObjectWrapper of the object field.
     */
    AbstractObjectWrapper& getAsRef(void* object);
    
  };
  
public:

  /**
   * Constructor.
   * @param pClassId - type class id.
   * @param pNameQualifier - type name qualifier.
   */
  Type(const ClassId& pClassId, const char* pNameQualifier);

  /**
   * Constructor.
   * @param pClassId - type class id.
   * @param pNameQualifier - type name qualifier.
   * @param pCreator - function pointer of Creator - function to create instance of this type.
   */
  Type(const ClassId& pClassId, const char* pNameQualifier, Creator pCreator);

  /**
   * Constructor.
   * @param pClassId - type class id.
   * @param pNameQualifier - type name qualifier.
   * @param pCreator - function pointer of Creator - function to create instance of this type.
   * @param pProperties - pointer to type properties.
   */
  Type(const ClassId& pClassId, const char* pNameQualifier, Creator pCreator, Properties* pProperties);

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
  std::list<Type*> params;

  /**
   * Creator - function to create instance of this type.
   */
  const Creator creator;

  /**
   * Pointer to type properties.
   */
  const Properties* const properties;
  
};
  
}}}}
  
#endif /* oatpp_data_type_Type_hpp */
