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

#include "InterpretationTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/data/mapping/TypeResolver.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

struct VPoint {
  v_int32 x;
  v_int32 y;
  v_int32 z;
};

struct VLine {
  VPoint p1;
  VPoint p2;
};

namespace __class {
  class PointClass;
  class LineClass;
}

typedef oatpp::data::mapping::type::Primitive<VPoint, __class::PointClass> Point;
typedef oatpp::data::mapping::type::Primitive<VLine, __class::LineClass> Line;

namespace __class {

  class PointClass {
  private:

    class PointDto : public oatpp::DTO {

      DTO_INIT(PointDto, DTO)

      DTO_FIELD(Int32, x);
      DTO_FIELD(Int32, y);
      DTO_FIELD(Int32, z);

    };

    class Inter : public oatpp::Type::Interpretation<Point, oatpp::Object<PointDto>>  {
    public:

      oatpp::Object<PointDto> interpret(const Point& value) const override {
        OATPP_LOGD("Point::Interpretation", "interpret");
        auto dto = PointDto::createShared();
        dto->x = value->x;
        dto->y = value->y;
        dto->z = value->z;
        return dto;
      }

      Point reproduce(const oatpp::Object<PointDto>& value) const override {
        OATPP_LOGD("Point::Interpretation", "reproduce");
        return Point({value->x, value->y, value->z});
      }

    };

  private:

    static oatpp::Type* createType() {
      oatpp::Type::Info info;
      info.interpretationMap = {
        {"test", new Inter()}
      };
      return new Type(CLASS_ID, info);
    }

  public:

    static const oatpp::ClassId CLASS_ID;

    static oatpp::Type* getType(){
      static Type* type = createType();
      return type;
    }

  };

  const oatpp::ClassId PointClass::CLASS_ID("test::Point");

  class LineClass {
  private:

    class LineDto : public oatpp::DTO {

      DTO_INIT(LineDto, DTO)

      DTO_FIELD(Point, p1);
      DTO_FIELD(Point, p2);

    };

    class Inter : public oatpp::Type::Interpretation<Line, oatpp::Object<LineDto>>  {
    public:

      oatpp::Object<LineDto> interpret(const Line& value) const override {
        OATPP_LOGD("Line::Interpretation", "interpret");
        auto dto = LineDto::createShared();
        dto->p1 = {value->p1.x, value->p1.y, value->p1.z};
        dto->p2 = {value->p2.x, value->p2.y, value->p2.z};
        return dto;
      }

      Line reproduce(const oatpp::Object<LineDto>& value) const override {
        OATPP_LOGD("Line::Interpretation", "reproduce");
        return Line({{value->p1->x, value->p1->y, value->p1->z},
                     {value->p2->x, value->p2->y, value->p2->z}});
      }

    };

  private:

    static oatpp::Type* createType() {
      oatpp::Type::Info info;
      info.interpretationMap = {
        {"test", new Inter()}
      };
      return new oatpp::Type(CLASS_ID, info);
    }

  public:

    static const oatpp::ClassId CLASS_ID;

    static oatpp::Type* getType(){
      static Type* type = createType();
      return type;
    }

  };

  const oatpp::ClassId LineClass::CLASS_ID("test::Line");

}

#include OATPP_CODEGEN_END(DTO)

}

void InterpretationTest::onRun() {

  oatpp::parser::json::mapping::ObjectMapper mapper;

  {
    auto config = mapper.getSerializer()->getConfig();
    config->enabledInterpretations = {"test"};
    config->useBeautifier = false;
  }

  {
    auto config = mapper.getDeserializer()->getConfig();
    config->enabledInterpretations = {"test"};
  }

  Point p1 ({1, 2, 3});
  Point p2 ({11, 12, 13});

  Line l ({p1, p2});

  auto json1 = mapper.writeToString(l);

  OATPP_LOGD(TAG, "json1='%s'", json1->c_str());

  auto rl = mapper.readFromString<Line>(json1);

  auto json2 = mapper.writeToString(rl);

  OATPP_LOGD(TAG, "json2='%s'", json2->c_str());

  OATPP_ASSERT(json1 == json2);

  oatpp::data::mapping::TypeResolver::Cache cache;

  {
    oatpp::data::mapping::TypeResolver tr;
    tr.setEnabledInterpretations({"test"});

    //oatpp::data::mapping::TypeResolver::Cache cache;

    auto v = tr.resolveObjectPropertyValue(l, {"p1", "x"}, cache);
    OATPP_ASSERT(v);
    OATPP_ASSERT(v.getValueType() == oatpp::Int32::Class::getType());
    OATPP_ASSERT(v.cast<oatpp::Int32>() == 1);
  }

  {
    oatpp::data::mapping::TypeResolver tr;
    tr.setEnabledInterpretations({"test"});

    //oatpp::data::mapping::TypeResolver::Cache cache;

    auto v = tr.resolveObjectPropertyValue(l, {"p1", "y"}, cache);
    OATPP_ASSERT(v);
    OATPP_ASSERT(v.getValueType() == oatpp::Int32::Class::getType());
    OATPP_ASSERT(v.cast<oatpp::Int32>() == 2);
  }

  {
    oatpp::data::mapping::TypeResolver tr;
    tr.setEnabledInterpretations({"test"});

    //oatpp::data::mapping::TypeResolver::Cache cache;

    auto v = tr.resolveObjectPropertyValue(l, {"p1", "z"}, cache);
    OATPP_ASSERT(v);
    OATPP_ASSERT(v.getValueType() == oatpp::Int32::Class::getType());
    OATPP_ASSERT(v.cast<oatpp::Int32>() == 3);
  }

}

}}}}}}