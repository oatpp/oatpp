import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";
import { ExternalLink } from "lucide-react";

const projectLayout = `|- CMakeLists.txt                        // projects CMakeLists.txt
|- sql/                                  // SQL migration scripts for SQLite database
|- src/
|   |
|   |- controller/                       // Folder containing REST Controllers (UserController)
|   |- db/                               // Folder containing the database client
|   |- dto/                              // DTOs are declared here
|   |- service/                          // Service business logic classes (UserService)
|   |- AppComponent.hpp                  // Service config
|   |- DatabaseComponent.hpp             // Database config
|   |- SwaggerComponent.hpp              // Swagger-UI config
|   |- App.cpp                           // main() is here
|
|- test/                                 // test folder
|- utility/install-oatpp-modules.sh      // utility script to install required oatpp-modules.`;

const endpointCode = `ENDPOINT_INFO(createUser) {
  info->summary = "Create new User";
  info->addConsumes<Object<UserDto>>("application/json");
  info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
}
ENDPOINT("POST", "users", createUser,
         BODY_DTO(Object<UserDto>, userDto))
{
  return createDtoResponse(Status::CODE_200, m_userService.createUser(userDto));
}`;

const ExampleCRUD = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Example-CRUD</h1>

        <p className="text-muted-foreground mb-4">
          <a
            href="https://github.com/oatpp/example-crud"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link inline-flex items-center gap-1"
          >
            Github Repository
            <ExternalLink className="w-3 h-3" />
          </a>
        </p>

        <p className="text-muted-foreground mb-6">
          A complete example of a "CRUD" service (UserService) built with Oat++.
        </p>

        <p className="text-muted-foreground mb-4">In this example:</p>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-8">
          <li>How to create CRUD endpoint.</li>
          <li>How to use <a href="/docs/components/orm" className="oat-link">oatpp ORM</a> - SQLite example.</li>
          <li>How to document API with Swagger-UI and OpenApi 3.0.0.</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Overview</h2>
        <p className="text-muted-foreground mb-4">This project is using the following oatpp modules:</p>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>
            <a href="https://github.com/oatpp/oatpp" target="_blank" rel="noopener noreferrer" className="oat-link">
              oatpp
            </a>
          </li>
          <li>
            <a href="https://github.com/oatpp/oatpp-swagger" target="_blank" rel="noopener noreferrer" className="oat-link">
              oatpp-swagger
            </a>
          </li>
          <li>
            <a href="https://github.com/oatpp/oatpp-sqlite" target="_blank" rel="noopener noreferrer" className="oat-link">
              oatpp-sqlite
            </a>
          </li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">Project layout</h3>
        <CodeBlock code={projectLayout} language="text" title="Project Structure" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Build and Run</h2>

        <h3 className="text-xl font-semibold text-foreground mb-3">Using CMake</h3>
        <CodeBlock
          code={`$ mkdir build && cd build
$ cmake ..
$ make
$ ./example-crud-exe`}
          language="bash"
        />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Endpoints</h2>

        <h3 className="text-xl font-semibold text-foreground mb-3">Create User</h3>
        <CodeBlock code={endpointCode} language="cpp" showLineNumbers />

        <h3 className="text-xl font-semibold text-foreground mb-3 mt-6">API Documentation</h3>
        <p className="text-muted-foreground mb-4">
          Swagger-UI is available at <code className="oat-code">http://localhost:8000/swagger/ui</code>
        </p>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <p className="text-sm text-muted-foreground">
            <strong className="text-foreground">Note:</strong> This example demonstrates a simple User service.
            For production applications, you would typically add authentication, input validation, and error handling.
          </p>
        </div>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default ExampleCRUD;
