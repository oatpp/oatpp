# Oat++ [![oatpp build status](https://dev.azure.com/lganzzzo/lganzzzo/_apis/build/status/oatpp.oatpp)](https://dev.azure.com/lganzzzo/lganzzzo/_build?definitionId=1) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/oatpp/oatpp.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/oatpp/oatpp/context:cpp) [![Join the chat at https://gitter.im/oatpp-framework/Lobby](https://badges.gitter.im/oatpp-framework/Lobby.svg)](https://gitter.im/oatpp-framework/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

**Attention**  

The new oatpp `1.2.0` is coming soon. Follow the [changelog](https://github.com/oatpp/oatpp/blob/v_1.2.0/changelog/1.2.0.md) for details.

---

Oat++ is a modern Web Framework for C++. 
It's fully loaded and contains all necessary components for effective production level development. 
It's also light and has a small memory footprint.

**Start**
- [Get Started](https://oatpp.io/docs/start/)
- [Build For Unix/Linux](https://oatpp.io/docs/installation/unix-linux/)
- [Build For Windows](https://oatpp.io/docs/installation/windows/)
- [Examples](#examples)

**About**
- [Website](https://oatpp.io/)
- [Supported Platforms](https://oatpp.io/supported-platforms/)
- Latest Benchmarks: [5 Million WebSockets](https://oatpp.io/benchmark/websocket/5-million/)
- [Contributing to Oat++](CONTRIBUTING.md)

**Join Our Community**
- Chat on **Gitter**. [Oat++ framework/Lobby](https://gitter.im/oatpp-framework/Lobby)
- Follow us on **Twitter** for latest news. [@oatpp_io](https://twitter.com/oatpp_io)
- Join community on **Reddit**. [r/oatpp](https://www.reddit.com/r/oatpp/)

## High Level Overview

- [API Controller And Request Mapping](#api-controller-and-request-mapping)
    * [Declare Endpoint](#declare-endpoint)
    * [Add CORS for Endpoint](#add-cors-for-endpoint)
    * [Endpoint with Authorization](#endpoint-with-authorization)
- [Swagger-UI Annotations](#swagger-ui-annotations)
    * [Additional Endpoint Info](#additional-endpoint-info)
- [API Client - Retrofit / Feign Like Client](#api-client---retrofit--feign-like-client)
    * [Declare Client](#declare-client)
    * [Using API Client](#using-api-client)
- [Object Mapping](#object-mapping)
    * [Declare DTO](#declare-dto)
    * [Serialize DTO Using ObjectMapper](#serialize-dto-using-objectmapper)


### API Controller And Request Mapping

For more info see [Api Controller](https://oatpp.io/docs/components/api-controller/)

#### Declare Endpoint

```cpp
ENDPOINT("PUT", "/users/{userId}", putUser,
         PATH(Int64, userId),
         BODY_DTO(Object<UserDto>, userDto)) 
{
  userDto->id = userId;
  return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
}
```

#### Add CORS for Endpoint

For more info see [Api Controller / CORS](https://oatpp.io/docs/components/api-controller/#cors)

```cpp
ADD_CORS(putUser)
ENDPOINT("PUT", "/users/{userId}", putUser,
         PATH(Int64, userId),
         BODY_DTO(Object<UserDto>, userDto)) 
{
  userDto->id = userId;
  return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
}
```

#### Endpoint with Authorization

For more info see [Api Controller / Authorization](https://oatpp.io/docs/components/api-controller/#authorization-basic)

```cpp
using namespace oatpp::web::server::handler;
  
ENDPOINT("PUT", "/users/{userId}", putUser,
         AUTHORIZATION(std::shared_ptr<DefaultBasicAuthorizationObject>, authObject),
         PATH(Int64, userId),
         BODY_DTO(Object<UserDto>, userDto)) 
{
  OATPP_ASSERT_HTTP(authObject->userId == "Ivan" && authObject->password == "admin", Status::CODE_401, "Unauthorized");
  userDto->id = userId;
  return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
}
```

### Swagger-UI Annotations

For more info see [Endpoint Annotation And API Documentation](https://oatpp.io/docs/components/api-controller/#endpoint-annotation-and-api-documentation)

#### Additional Endpoint Info

```cpp
ENDPOINT_INFO(putUser) {
  // general
  info->summary = "Update User by userId";
  info->addConsumes<Object<UserDto>>("application/json");
  info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
  info->addResponse<String>(Status::CODE_404, "text/plain");
  // params specific
  info->pathParams["userId"].description = "User Identifier";
}
ENDPOINT("PUT", "/users/{userId}", putUser,
         PATH(Int64, userId),
         BODY_DTO(Object<UserDto>, userDto)) 
{
  userDto->id = userId;
  return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
}
```

### API Client - Retrofit / Feign Like Client

For more info see [Api Client](https://oatpp.io/docs/components/api-client/)

#### Declare Client

```cpp
class UserService : public oatpp::web::client::ApiClient {
public:

  API_CLIENT_INIT(UserService)

  API_CALL("GET", "/users", getUsers)
  API_CALL("GET", "/users/{userId}", getUserById, PATH(Int64, userId))

};
```

#### Using API Client

```cpp
auto response = userService->getUserById(id);
auto user = response->readBodyToDto<oatpp::Object<UserDto>>(objectMapper);
```

### Object Mapping

For more info see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/).

#### Declare DTO

```cpp
class UserDto : public oatpp::DTO {

  DTO_INIT(UserDto, DTO)

  DTO_FIELD(Int64, id);
  DTO_FIELD(String, name);

};
```

#### Serialize DTO Using ObjectMapper

```cpp
using namespace oatpp::parser::json::mapping;

auto user = UserDto::createShared();
user->id = 1;
user->name = "Ivan";

auto objectMapper = ObjectMapper::createShared();
auto json = objectMapper->writeToString(user);
```

Output:

```json
{
  "id": 1,
  "name": "Ivan"
}
```

#### Serialize/Deserialize Data In Free Form

While DTO objects apply strict rules on data ser/de, you can also
serialize/deserialize data in free form using type `oatpp::Any`.

```cpp
oatpp::Fields<oatpp::Any> map = {
  {"title", oatpp::String("Hello Any!")},
  {"listOfAny",
   oatpp::List<oatpp::Any>({
     oatpp::Int32(32),
     oatpp::Float32(0.32),
     oatpp::Boolean(true)
   })
  }
};

auto json = mapper->writeToString(map); 
```

Output:

```json
{
  "title": "Hello Any!",
  "listOfAny": [
    32,
    0.3199999928474426,
    true
  ]
}
```

### Examples:

#### REST-API

- [ApiClient-Demo](https://github.com/oatpp/example-api-client) - Example project of how-to use Retrofit-like client wrapper (ApiClient) and how it works.
- [AsyncApi](https://github.com/oatpp/example-async-api) - Example project of how-to use asynchronous API for handling large number of simultaneous connections.
- [CRUD](https://github.com/oatpp/example-crud) - Example project of how-to create basic CRUD endpoints.

#### WebSocket

- [Can-Chat](https://github.com/lganzzzo/canchat) - Feature-complete rooms-based chat for tens of thousands users. Client plus Server.
- [WebSocket](https://github.com/oatpp/example-websocket) - Collection of oatpp WebSocket examples.
- [YUV-Websocket-Stream](https://github.com/oatpp/example-yuv-websocket-stream) - Example project how-to create a YUV image stream from a V4L device (i.E. Webcam) using websockets.

### IoT

- [Example-IoT-Hue](https://github.com/oatpp/example-iot-hue-ssdp) - Example project how-to create an Philips Hue compatible REST-API that is discovered and controllable by Hue compatible Smart-Home devices like Amazon Alexa or Google Echo.

#### Streaming

- [Media-Stream (Http-Live-Streaming)](https://github.com/oatpp/example-hls-media-stream) - Example project of how-to build HLS-streaming server using oat++ Async-API.
- [YUV-Websocket-Stream](https://github.com/oatpp/example-yuv-websocket-stream) - Example project how-to create a YUV image stream from a V4L device (i.E. Webcam) using websockets.

#### TLS

- [TLS-Libressl](https://github.com/oatpp/example-libressl) - Example project how-to setup secure connection and serve via HTTPS.

#### Microservices

- [Consul](https://github.com/oatpp/example-consul) - Example project of how-to use oatpp::consul::Client. Integration with Consul.
- [Microservices](https://github.com/oatpp/example-microservices) - Example project on how to build microservices with Oat++, 
and example on how to consolidate those microservices using [monolithization](https://oatpp.io/docs/monolithization/) technique.

#### Databases

- [MongoDB](https://github.com/oatpp/example-mongodb) - Example project how to work with MongoDB using [oatpp-mongo](https://github.com/oatpp/oatpp-mongo) mondule.
Project is a web-service with basic CRUD and Swagger-UI.
- [PostgreSQL](https://github.com/oatpp/example-postgresql) - Example of a production grade entity service storing information in PostgreSQL. With Swagger-UI and configuration profiles.

