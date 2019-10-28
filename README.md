# Oat++ [![oatpp build status](https://dev.azure.com/lganzzzo/lganzzzo/_apis/build/status/oatpp.oatpp)](https://dev.azure.com/lganzzzo/lganzzzo/_build?definitionId=1) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/oatpp/oatpp.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/oatpp/oatpp/context:cpp) [![Join the chat at https://gitter.im/oatpp-framework/Lobby](https://badges.gitter.im/oatpp-framework/Lobby.svg)](https://gitter.im/oatpp-framework/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Oat++ is the modern Web Framework for C++. It's fully loaded and contains all necessary components for
effective production level development. It's also light and has small memory footprint.

- [Website](https://oatpp.io/)
- [Docs](https://oatpp.io/docs/start/)
- [Api Reference](https://oatpp.io/api/latest/)
- [Supported Platforms](https://oatpp.io/supported-platforms/)
- Latest Benchmarks: [5 Million WebSockets](https://oatpp.io/benchmark/websocket/5-million/)

**Contributors wanted!**  
- See [Contributing to Oat++](CONTRIBUTING.md)

**Join the community**
- Join discussion on **Gitter**. [oat++ framework/Lobby](https://gitter.im/oatpp-framework/Lobby)
- Follow us on **Twitter** for latest news. [@oatpp_io](https://twitter.com/oatpp_io)
- Join community on **Reddit**. [r/oatpp](https://www.reddit.com/r/oatpp/)

## High Level Overview

### API Controller And Request Mapping

For more info see [Api Controller](https://oatpp.io/docs/components/api-controller/)

#### Declare Endpoint

```cpp
ENDPOINT("PUT", "/users/{userId}", putUser,
         PATH(Int64, userId),
         BODY_DTO(dto::UserDto::ObjectWrapper, userDto)) 
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
         BODY_DTO(dto::UserDto::ObjectWrapper, userDto)) 
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
         BODY_DTO(dto::UserDto::ObjectWrapper, userDto)) 
{
  OATPP_ASSERT_HTTP(authObject->userId == "Ivan" && authObject->password == "admin", Status::CODE_401, "Unauthorized");
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
auto user = response->readBodyToDto<dto::UserDto>(objectMapper);
```

### Swagger-UI Annotations

For more info see [Endpoint Annotation And API Documentation](https://oatpp.io/docs/components/api-controller/#endpoint-annotation-and-api-documentation)

#### Additional Endpoint Info

```cpp
ENDPOINT_INFO(putUser) {
  // general
  info->summary = "Update User by userId";
  info->addConsumes<dto::UserDto::ObjectWrapper>("application/json");
  info->addResponse<dto::UserDto::ObjectWrapper>(Status::CODE_200, "application/json");
  info->addResponse<String>(Status::CODE_404, "text/plain");
  // params specific
  info->pathParams["userId"].description = "User Identifier";
}
ENDPOINT("PUT", "/users/{userId}", putUser,
         PATH(Int64, userId),
         BODY_DTO(dto::UserDto::ObjectWrapper, userDto)) 
{
  userDto->id = userId;
  return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
}
```

### Object Mapping

For more info see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/).

#### Declare DTO

```cpp
class UserDto : public oatpp::data::mapping::type::Object {

  DTO_INIT(UserDto, Object)

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

### Read Next

- [Well Structured Project](https://oatpp.io/docs/start/step-by-step/#well-structured-project)
- [Build For Unix/Linux](https://oatpp.io/docs/installation/unix-linux/)
- [Build For Windows](https://oatpp.io/docs/installation/windows/)

### Examples:

- [Media-Stream (Http-Live-Streaming)](https://github.com/oatpp/example-hls-media-stream) - Example project of how-to build HLS-streaming server using oat++ Async-API.
- [CRUD](https://github.com/oatpp/example-crud) - Example project of how-to create basic CRUD endpoints.
- [AsyncApi](https://github.com/oatpp/example-async-api) - Example project of how-to use asynchronous API for handling large number of simultaneous connections.
- [ApiClient-Demo](https://github.com/oatpp/example-api-client) - Example project of how-to use Retrofit-like client wrapper (ApiClient) and how it works.
- [TLS-Libressl](https://github.com/oatpp/example-libressl) - Example project of how-to setup secure connection and serve via HTTPS.
- [Consul](https://github.com/oatpp/example-consul) - Example project of how-to use oatpp::consul::Client. Integration with Consul.
- [PostgreSQL](https://github.com/oatpp/example-postgresql) - Example of a production grade entity service storing information in PostgreSQL. With Swagger-UI and configuration profiles.
- [WebSocket](https://github.com/oatpp/example-websocket) - Collection of oatpp WebSocket examples.
