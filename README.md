# oat++ [![oatpp build status](https://dev.azure.com/lganzzzo/lganzzzo/_apis/build/status/oatpp.oatpp)](https://dev.azure.com/lganzzzo/lganzzzo/_build?definitionId=1) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/oatpp/oatpp.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/oatpp/oatpp/context:cpp) [![Join the chat at https://gitter.im/oatpp-framework/Lobby](https://badges.gitter.im/oatpp-framework/Lobby.svg)](https://gitter.im/oatpp-framework/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Zero-Dependency. Performance oriented web-service-development framework.
Organic. Pure C++.

- [Website](https://oatpp.io/)
- [Docs](https://oatpp.io/docs/start/)
- [Api Reference](https://oatpp.io/api/latest/)
- Latest Benchmarks: [5 Million WebSockets](https://oatpp.io/benchmark/websocket/5-million/)

**Contributors wanted!**  
- See [Contributing to Oat++](CONTRIBUTING.md)

**Join the community**
- Join discussion on **Gitter**. [oat++ framework/Lobby](https://gitter.im/oatpp-framework/Lobby)
- Follow us on **Twitter** for latest news. [@oatpp_io](https://twitter.com/oatpp_io)
- Join community on **Reddit**. [r/oatpp](https://www.reddit.com/r/oatpp/)

## Features

- Blazingly fast
- Zero Dependency
- **Asynchronous server (High performance. Handle over 5 Million simultaneous WebSocket connections on a single server.)** [See benchmark](https://oatpp.io/benchmark/websocket/5-million/)
- Multithreaded server (Simple API)
- Connection agnostic. (Use whatever transport. Whatever SSL backend. Whatever sockets, pipes, files. etc. It cares about HTTP stream only)
- REST framework (with ability to autodocument endpoints see [oatpp-swagger](https://github.com/oatpp/oatpp-swagger))
- Retrofit-like client wrapper (Use whatever request executor for example cURL, or minimalistic one provided out of the box)
- Object mapping (Fast object serialization-deserialization. Currently JSON, more formats comes shortly)
- Simple dependency injection framework
- Simple Test framework
- HTTP_1.1 (2.0 comes shortly)

## Simple API overview
"Simple API" refers to as API used together with ```oatpp::web::server::HttpConnectionHandler``` utilizing multithreading plus blocking-IO approach. 

### Create Endpoint

```c++
ENDPOINT("GET", "demo/api/hello", hello) {
  return createResponse(Status::CODE_200, "Hello World!");
}
```

### Pass parameters to endpoint

```c++
ENDPOINT("GET", "demo/api/param/{param}", getWithParams,
         PATH(String, param)) {
  return createResponse(Status::CODE_200, "param=" + param);
}
```

### Return JSON

```c++
ENDPOINT("GET", "demo/api/json", getJson) {
  auto dto = MyDto::createShared();
  dto->statusCode = 200;
  dto->message = "Hello json";
  return createDtoResponse(Status::CODE_200, dto);
}
```
**Output:**
```
{"message": "Hello json", "statusCode": 200}
```

### Post JSON body

```c++
ENDPOINT("POST", "demo/api/json", postJson,
         BODY_DTO(MyDto::ObjectWrapper, dto)) {
  auto dtoMessage = dto->message;
  return createResponse(Status::CODE_200, "dtoMessage: " + dtoMessage);
}
```

**Terminal:**

```
$ curl -X POST "localhost:8001/demo/api/json" -d '{"message": "hello json post"}'
dtoMessage: hello json post
```

## Async API overview
"Async API" refers to as API used together with ```oatpp::web::server::AsyncHttpConnectionHandler``` utilizing oatpp-coroutines plus non-blocking-IO approach. 

### Create Endpoint Async
```c++
ENDPOINT_ASYNC("GET", "demo/api_async/hello", HelloAsync) {

  ENDPOINT_ASYNC_INIT(HelloAsync)

  Action act() override {
    return _return(controller->createResponse(Status::CODE_200, "Hello World Async API!"));
  }

};
```

### Pass parameters to endpoint Async
```c++
ENDPOINT_ASYNC("GET", "demo/api_async/param/{param}", GetWithParamsAsync) {

  ENDPOINT_ASYNC_INIT(GetWithParamsAsync)

  Action act() override {
    auto param = request->getPathVariable("param");
    return _return(controller->createResponse(Status::CODE_200, "param=" + param));
  }

};
```

### Return JSON Async
```c++
ENDPOINT_ASYNC("GET", "demo/api_async/json", GetJSONAsync) {

  ENDPOINT_ASYNC_INIT(GetJSONAsync)

  Action act() override {
    auto dto = MyDto::createShared();
    dto->statusCode = 200;
    dto->message = "Hello json";
    return _return(controller->createDtoResponse(Status::CODE_200, dto));
  }

};
```

**Output:**
```
{"message": "Hello json", "statusCode": 200}
```

### Post JSON body Async
```c++
ENDPOINT_ASYNC("POST", "demo/api_async/json", PostJSONAsync) {

  ENDPOINT_ASYNC_INIT(PostJSONAsync)

  Action act() override {
    return request->readBodyToDtoAsync<MyDto>(controller->getDefaultObjectMapper()).callbackTo(&PostJSONAsync::onBodyObtained);
  }

  Action onBodyObtained(const MyDto::ObjectWrapper& dto) {
    return _return(controller->createResponse(Status::CODE_200, "dtoMessage: " + dto->message));
  }

};
```

**Terminal:**
```
$ curl -X POST "localhost:8001/demo/api_async/json" -d '{"message": "hello json post"}'
dtoMessage: hello json post
```

### Swagger documentation

```c++
ENDPOINT_INFO(createUser) {
  info->summary = "Create new User";
  info->addConsumes<UserDto::ObjectWrapper>("application/json");
  info->addResponse<UserDto::ObjectWrapper>(Status::CODE_200, "application/json");
}
ENDPOINT("POST", "demo/api/users", createUser,
         BODY_DTO(UserDto::ObjectWrapper, userDto)) {
  return createDtoResponse(Status::CODE_200, m_database->createUser(userDto));
}
```

## How to start

Grab any project from [examples](https://github.com/oatpp/oatpp-examples), and follow README

### Examples:

- [Media-Stream (Http-Live-Streaming)](https://github.com/oatpp/example-hls-media-stream) - Example project of how-to build HLS-streaming server using oat++ Async-API.
- [CRUD](https://github.com/oatpp/example-crud) - Example project of how-to create basic CRUD endpoints.
- [AsyncApi](https://github.com/oatpp/example-async-api) - Example project of how-to use asynchronous API for handling large number of simultaneous connections.
- [ApiClient-Demo](https://github.com/oatpp/example-api-client) - Example project of how-to use Retrofit-like client wrapper (ApiClient) and how it works.
- [TLS-Libressl](https://github.com/oatpp/example-libressl) - Example project of how-to setup secure connection and serve via HTTPS.
- [Consul](https://github.com/oatpp/example-consul) - Example project of how-to use oatpp::consul::Client. Integration with Consul.
- [PostgreSQL](https://github.com/oatpp/example-postgresql) - Example of a production grade entity service storing information in PostgreSQL. With Swagger-UI and configuration profiles.
- [WebSocket](https://github.com/oatpp/example-websocket) - Collection of oatpp WebSocket examples.
