import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const controllerCode = `#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UserController : public oatpp::web::server::api::ApiController {
public:
  UserController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
  
  ENDPOINT("GET", "/users", getUsers) {
    auto users = UserDto::createShared();
    return createDtoResponse(Status::CODE_200, users);
  }
  
  ENDPOINT("GET", "/users/{userId}", getUserById,
           PATH(Int64, userId)) {
    auto user = UserDto::createShared();
    user->id = userId;
    return createDtoResponse(Status::CODE_200, user);
  }
  
  ENDPOINT("POST", "/users", createUser,
           BODY_DTO(Object<UserDto>, userDto)) {
    return createDtoResponse(Status::CODE_201, userDto);
  }
  
  ENDPOINT("PUT", "/users/{userId}", updateUser,
           PATH(Int64, userId),
           BODY_DTO(Object<UserDto>, userDto)) {
    userDto->id = userId;
    return createDtoResponse(Status::CODE_200, userDto);
  }
  
  ENDPOINT("DELETE", "/users/{userId}", deleteUser,
           PATH(Int64, userId)) {
    return createResponse(Status::CODE_204, "");
  }
};

#include OATPP_CODEGEN_END(ApiController)`;

const annotationCode = `ENDPOINT_INFO(createUser) {
  info->summary = "Create new User";
  info->description = "Create a new user with the provided data";
  info->addConsumes<Object<UserDto>>("application/json");
  info->addResponse<Object<UserDto>>(Status::CODE_201, "application/json");
  info->addResponse<String>(Status::CODE_400, "text/plain");
}`;

const ApiController = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">API Controller</h1>

        <p className="text-muted-foreground mb-6">
          <code className="oat-code">ApiController</code> is a class that handles incoming HTTP requests and maps them to endpoint handler functions.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Basic Usage</h2>
        <p className="text-muted-foreground mb-4">
          Create a controller by extending <code className="oat-code">oatpp::web::server::api::ApiController</code>:
        </p>
        <CodeBlock code={controllerCode} language="cpp" showLineNumbers />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Endpoint Macros</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Macro</th>
                <th>Description</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><code className="oat-code">ENDPOINT</code></td>
                <td>Define an endpoint handler</td>
              </tr>
              <tr>
                <td><code className="oat-code">ENDPOINT_ASYNC</code></td>
                <td>Define an async endpoint handler</td>
              </tr>
              <tr>
                <td><code className="oat-code">ENDPOINT_INFO</code></td>
                <td>Add Swagger documentation to endpoint</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Parameter Mappings</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Macro</th>
                <th>Description</th>
                <th>Example</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><code className="oat-code">PATH</code></td>
                <td>Path parameter</td>
                <td><code className="oat-code">PATH(Int64, userId)</code></td>
              </tr>
              <tr>
                <td><code className="oat-code">QUERY</code></td>
                <td>Query parameter</td>
                <td><code className="oat-code">QUERY(String, name)</code></td>
              </tr>
              <tr>
                <td><code className="oat-code">HEADER</code></td>
                <td>Header parameter</td>
                <td><code className="oat-code">HEADER(String, auth, "Authorization")</code></td>
              </tr>
              <tr>
                <td><code className="oat-code">BODY_DTO</code></td>
                <td>Request body as DTO</td>
                <td><code className="oat-code">{"BODY_DTO(Object<UserDto>, user)"}</code></td>
              </tr>
              <tr>
                <td><code className="oat-code">BODY_STRING</code></td>
                <td>Request body as string</td>
                <td><code className="oat-code">BODY_STRING(String, body)</code></td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">API Documentation</h2>
        <p className="text-muted-foreground mb-4">
          Use <code className="oat-code">ENDPOINT_INFO</code> to add Swagger documentation:
        </p>
        <CodeBlock code={annotationCode} language="cpp" />

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default ApiController;
