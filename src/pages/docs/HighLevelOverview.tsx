import { Link } from "react-router-dom";
import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const highLevelCode = `ENDPOINT("GET", "/users/{userId}", getUserById,
         PATH(Int64, userId))
{
  return createDtoResponse(Status::CODE_200, m_userService.getUserById(userId));
}`;

const dtoCode = `class UserDto : public oatpp::DTO {
  DTO_INIT(UserDto, DTO)
  
  DTO_FIELD(Int64, id);
  DTO_FIELD(String, name);
  DTO_FIELD(String, email);
};`;

const HighLevelOverview = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">High Level Overview</h1>

        <p className="text-muted-foreground mb-6">
          This article provides an overview of oatpp's key components and features.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Api Controller</h2>
        <p className="text-muted-foreground mb-4">
          <Link to="/docs/components/api-controller" className="oat-link">Api Controller</Link> handles incoming requests
          and maps them to endpoint functions. It uses declarative endpoint annotations to define routes.
        </p>
        <CodeBlock code={highLevelCode} language="cpp" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Data Transfer Objects (DTOs)</h2>
        <p className="text-muted-foreground mb-4">
          <Link to="/docs/components/dto" className="oat-link">DTOs</Link> are used for data serialization
          and deserialization. They provide type-safe object mapping throughout the application.
        </p>
        <CodeBlock code={dtoCode} language="cpp" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Key Components</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <strong className="text-foreground">Api Controller</strong> - Handles HTTP requests and maps them to endpoint functions
          </li>
          <li>
            <strong className="text-foreground">Api Client</strong> - HTTP client with type-safe API calls
          </li>
          <li>
            <strong className="text-foreground">ORM</strong> - Database abstraction layer for easy data access
          </li>
          <li>
            <strong className="text-foreground">Swagger</strong> - Auto-generated API documentation
          </li>
          <li>
            <strong className="text-foreground">WebSocket</strong> - Full duplex communication support
          </li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Simple vs Async API</h2>
        <p className="text-muted-foreground mb-4">
          Oat++ provides two types of APIs:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <strong className="text-foreground">Simple API</strong> - Thread-per-connection model. Best for most use cases.
            Each request is handled in its own thread.
          </li>
          <li>
            <strong className="text-foreground">Async API</strong> - Coroutine-based model for high-concurrency scenarios.
            Can handle millions of connections on a single thread.
          </li>
        </ul>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <p className="text-sm text-muted-foreground">
            <strong className="text-foreground">Recommendation:</strong> Always choose Simple API wherever possible.
            It's more developed, cleaner, and easier to debug. Use Async API only for specific high-concurrency tasks
            like WebSocket servers or file streaming to thousands of clients.
          </p>
        </div>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default HighLevelOverview;
