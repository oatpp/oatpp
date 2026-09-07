# Request Timeout Feature - Usage Guide

## Overview

This feature implements per-request timeout for Oat++ servers. When a request handler takes longer than the configured timeout to process, the server returns a `504 Gateway Timeout` response.

## Configuration

### 1. Configure Request Timeout (Synchronous Server)

```cpp
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

// Create router
auto router = oatpp::web::server::HttpRouter::createShared();

// Add your endpoints
router->route("GET", "/fast", std::make_shared<FastHandler>());
router->route("GET", "/slow", std::make_shared<SlowHandler>());

// Configure timeout (in milliseconds)
auto config = std::make_shared<oatpp::web::server::HttpProcessor::Config>();
config->requestTimeout = 5000; // 5 seconds

// Create connection handler with config
auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router, config);

// Create server
auto provider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8080});
auto server = std::make_shared<oatpp::network::Server>(provider, connectionHandler);
```

### 2. Configure Request Timeout (Asynchronous Server)

```cpp
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

// Create router
auto router = oatpp::web::server::HttpRouter::createShared();

// Add your async endpoints
router->route("GET", "/fast", std::make_shared<AsyncFastHandler>());
router->route("GET", "/slow", std::make_shared<AsyncSlowHandler>());

// Configure timeout (in milliseconds)
auto config = std::make_shared<oatpp::web::server::HttpProcessor::Config>();
config->requestTimeout = 5000; // 5 seconds

// Create async executor
auto executor = std::make_shared<oatpp::async::Executor>();

// Create async connection handler with config
auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor, config);

// Create server
auto provider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8080});
auto server = std::make_shared<oatpp::network::Server>(provider, connectionHandler);
```

### 3. Using with AppComponent (OATPP_COMPONENT)

```cpp
class AppComponent {
public:

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    auto router = oatpp::web::server::HttpRouter::createShared();
    // Add routes...
    return router;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpProcessor::Config>, httpProcessorConfig)([this] {
    auto config = std::make_shared<oatpp::web::server::HttpProcessor::Config>();
    // Set request timeout to 10 seconds
    config->requestTimeout = 10000;
    return config;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared(
      {m_config->host, m_config->port}
    );
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpConnectionHandler>, serverConnectionHandler)([this] {
    return oatpp::web::server::HttpConnectionHandler::createShared(
      httpRouter.getObject(),
      httpProcessorConfig.getObject()
    );
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::Server>, server)([this] {
    return std::make_shared<oatpp::network::Server>(
      serverConnectionProvider.getObject(),
      serverConnectionHandler.getObject()
    );
  }());
};
```

## Behavior

- **No Timeout (requestTimeout = 0)**: Default behavior, no timeout is enforced
- **Timeout Configured**: If handler execution exceeds the timeout:
  - **Synchronous Server**: Handler is interrupted and `504 Gateway Timeout` is returned
  - **Asynchronous Server**: After handler completes, if execution time exceeded timeout, `504 Gateway Timeout` is returned

## Notes

1. **Timeout Value**: Set in milliseconds. Example: `5000` = 5 seconds
2. **Granularity**: Timeout is checked after handler execution starts
3. **Resource Cleanup**: Ensure your handlers properly clean up resources when interrupted
4. **Async Handlers**: For async handlers, timeout is checked when handler completes

## Example Handlers

### Fast Handler (Completes within timeout)

```cpp
class FastHandler : public oatpp::web::server::HttpRequestHandler {
public:
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    // Process quickly
    return ResponseFactory::createResponse(Status::CODE_200, "Fast response");
  }
};
```

### Slow Handler (Will timeout)

```cpp
class SlowHandler : public oatpp::web::server::HttpRequestHandler {
public:
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    // This will take longer than configured timeout
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return ResponseFactory::createResponse(Status::CODE_200, "This won't be reached");
  }
};
```

## Error Handling

When a timeout occurs, the server automatically returns a `504 Gateway Timeout` response to the client. The exception is logged and handled by the error handler.

## Testing

Run the test suite to verify timeout functionality:

```bash
# Run request timeout tests
./oatpp-test --test=RequestTimeoutTest
```
