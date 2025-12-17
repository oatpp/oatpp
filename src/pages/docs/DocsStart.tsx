import { Link } from "react-router-dom";
import DocsLayout from "@/components/layout/DocsLayout";
import FeedbackWidget from "@/components/FeedbackWidget";
import { ExternalLink } from "lucide-react";

const DocsStart = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Getting Started</h1>

        <p className="text-muted-foreground mb-6">
          Welcome to 🌱Oat++
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">The recommended way to go</h2>

        <ol className="list-decimal list-inside text-muted-foreground space-y-4 mb-8">
          <li>
            <span>Install <strong className="text-foreground">oatpp</strong>:</span>
            <ul className="list-disc list-inside ml-6 mt-2 space-y-1">
              <li>
                <Link to="/docs/installation/unix-linux" className="oat-link">Unix/Linux</Link>
              </li>
              <li>
                <Link to="/docs/installation/windows" className="oat-link">Windows</Link>
              </li>
              <li>
                For exotic platforms look for stories on the{" "}
                <Link to="/about/supported-platforms" className="oat-link">Supported Platforms</Link> page.
              </li>
            </ul>
          </li>
          <li>
            <span>Take a look at:</span>
            <ul className="list-disc list-inside ml-6 mt-2 space-y-1">
              <li>
                <Link to="/docs/start/high-level-overview" className="oat-link">High Level Overview</Link>
                {" "}- Learn about the main components and features of Oat++.
              </li>
              <li>
                <Link to="/docs/start/step-by-step" className="oat-link">Step By Step Guide</Link>
                {" "}- How to create an Oat++ project from scratch. Get more low-level details.
              </li>
            </ul>
          </li>
          <li>
            <span>Check out examples and starter-projects:</span>
            <ul className="list-disc list-inside ml-6 mt-2 space-y-1">
              <li>
                <Link to="/docs/start/project" className="oat-link">Starter Project - Simple API</Link>
              </li>
              <li>
                <Link to="/docs/start/project-async" className="oat-link">Starter project - Async API</Link>
              </li>
            </ul>
          </li>
          <li>
            <span>👉Join dev community and don't hesitate to ask questions - </span>
            <a
              href="https://gitter.im/oatpp-framework/Lobby"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              Chat On Gitter
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}👈✌️
          </li>
        </ol>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Examples</h2>

        <h3 className="text-xl font-semibold text-foreground mb-3">REST-API</h3>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <Link to="/examples/crud" className="oat-link">REST Service</Link>
            {" "}- A complete example of a "CRUD" service (UserService) built with Oat++. REST + Swagger-UI + SQLite.
          </li>
          <li>
            <a
              href="https://github.com/oatpp/example-api-client"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              REST Client
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Example project of how-to use Retrofit-like client wrapper (ApiClient) and how it works.
          </li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">WebSocket</h3>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <a
              href="https://github.com/lganzzzo/canchat"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              Can Chat
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Feature-complete rooms-based chat for tens of thousands users. Client plus Server.
          </li>
          <li>
            <a
              href="https://github.com/oatpp/example-websocket"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              WebSocket
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Collection of oatpp WebSocket examples.
          </li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">Database</h3>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <a
              href="https://github.com/oatpp/example-postgresql"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              PostgreSQL
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Example project how-to work with PostgreSQL using oatpp-postgresql.
          </li>
          <li>
            <a
              href="https://github.com/oatpp/example-mongodb"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              MongoDB
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Example project how-to work with MongoDB using oatpp-mongo.
          </li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">IoT</h3>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <a
              href="https://github.com/nickmcdowall/lux"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              Lux
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Raspberry Pi LED Strip Controller. Built with oatpp web framework.
          </li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">Streaming</h3>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            <a
              href="https://github.com/nickmcdowall/lux"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link inline-flex items-center gap-1"
            >
              HTTP Live Streaming Server
              <ExternalLink className="w-3 h-3" />
            </a>
            {" "}- Stream video to browser client.
          </li>
        </ul>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default DocsStart;
