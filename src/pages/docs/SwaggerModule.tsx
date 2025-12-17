import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const swaggerComponentCode = `#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/core/macro/component.hpp"

class SwaggerComponent {
public:
  
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swaggerDocumentInfo)([] {
    
    oatpp::swagger::DocumentInfo::Builder builder;
    
    builder
      .setTitle("My API")
      .setDescription("API built with Oat++ Web Framework")
      .setVersion("1.0")
      .setContactName("API Support")
      .setContactUrl("https://example.com/support")
      .setContactEmail("support@example.com")
      .setLicenseName("Apache License, Version 2.0")
      .setLicenseUrl("http://www.apache.org/licenses/LICENSE-2.0")
      .addServer("http://localhost:8000", "Local server");
    
    return builder.build();
    
  }());
  
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swaggerResources)([] {
    return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
  }());
  
};`;

const routerCode = `#include "oatpp-swagger/Controller.hpp"

// Add swagger endpoints to router
auto swaggerController = oatpp::swagger::Controller::createShared(docEndpoints);
swaggerController->addEndpointsToRouter(router);`;

const SwaggerModule = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">oatpp-swagger</h1>

        <p className="text-muted-foreground mb-6">
          oatpp-swagger module provides automatic API documentation using Swagger-UI and OpenAPI 3.0.0 specification.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Installation</h2>
        <CodeBlock
          code={`$ git clone https://github.com/oatpp/oatpp-swagger.git
$ cd oatpp-swagger
$ mkdir build && cd build
$ cmake ..
$ make install`}
          language="bash"
        />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Configuration</h2>
        <p className="text-muted-foreground mb-4">
          Create a SwaggerComponent to configure the documentation:
        </p>
        <CodeBlock code={swaggerComponentCode} language="cpp" showLineNumbers />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Adding to Router</h2>
        <CodeBlock code={routerCode} language="cpp" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Accessing Swagger UI</h2>
        <p className="text-muted-foreground mb-4">
          Once configured, Swagger UI is available at:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li><code className="oat-code">http://localhost:8000/swagger/ui</code> - Swagger UI interface</li>
          <li><code className="oat-code">http://localhost:8000/api-docs/oas-3.0.0.json</code> - OpenAPI JSON spec</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Documenting Endpoints</h2>
        <p className="text-muted-foreground mb-4">
          Use <code className="oat-code">ENDPOINT_INFO</code> macro to add documentation to your endpoints:
        </p>
        <CodeBlock
          code={`ENDPOINT_INFO(getUserById) {
  info->summary = "Get user by ID";
  info->description = "Retrieve a user by their unique identifier";
  info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
  info->addResponse<String>(Status::CODE_404, "text/plain", "User not found");
}
ENDPOINT("GET", "/users/{userId}", getUserById,
         PATH(Int64, userId)) {
  // endpoint implementation
}`}
          language="cpp"
        />

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default SwaggerModule;
