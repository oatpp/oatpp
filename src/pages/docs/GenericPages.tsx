import DocsLayout from "@/components/layout/DocsLayout";
import FeedbackWidget from "@/components/FeedbackWidget";

const GenericDocsPage = ({
  title,
  description,
  content,
}: {
  title: string;
  description?: string;
  content?: React.ReactNode;
}) => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">{title}</h1>
        {description && (
          <p className="text-muted-foreground mb-6">{description}</p>
        )}
        {content}
        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export const StepByStep = () => (
  <GenericDocsPage
    title="Step By Step Guide"
    description="Learn how to create an Oat++ project from scratch with this detailed guide."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">1. Project Structure</h2>
        <p className="text-muted-foreground mb-6">
          A typical Oat++ project consists of controllers, DTOs, services, and configuration components.
          The project uses CMake for building.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">2. Create CMakeLists.txt</h2>
        <p className="text-muted-foreground mb-6">
          Start by creating a CMakeLists.txt file that finds and links the oatpp library.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">3. Define DTOs</h2>
        <p className="text-muted-foreground mb-6">
          Create Data Transfer Objects for your API data models.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">4. Create Controllers</h2>
        <p className="text-muted-foreground mb-6">
          Define API controllers with endpoints that handle HTTP requests.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">5. Configure App Components</h2>
        <p className="text-muted-foreground mb-6">
          Set up application components including the server, object mapper, and connection handler.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">6. Build and Run</h2>
        <p className="text-muted-foreground mb-6">
          Use CMake to build your project and run the executable.
        </p>
      </>
    }
  />
);

export const StarterProject = () => (
  <GenericDocsPage
    title="Starter Project - Simple API"
    description="A minimal starter project demonstrating a simple REST API with Oat++."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Overview</h2>
        <p className="text-muted-foreground mb-6">
          This starter project provides a minimal setup to get started with Oat++ Simple API.
          It includes a basic controller, DTO, and app configuration.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Quick Start</h2>
        <ol className="list-decimal list-inside text-muted-foreground space-y-2 mb-6">
          <li>Clone the repository</li>
          <li>Install oatpp (see Installation guide)</li>
          <li>Build with CMake</li>
          <li>Run the executable</li>
        </ol>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <a
            href="https://github.com/oatpp/oatpp-starter"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link"
          >
            View on GitHub →
          </a>
        </div>
      </>
    }
  />
);

export const StarterProjectAsync = () => (
  <GenericDocsPage
    title="Starter Project - Async API"
    description="A starter project for high-concurrency applications using Oat++ Async API."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Overview</h2>
        <p className="text-muted-foreground mb-6">
          This starter project demonstrates how to use Oat++ Async API for high-concurrency applications.
          Async API uses coroutines to handle thousands of concurrent connections efficiently.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">When to Use Async API</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>WebSocket servers with many concurrent connections</li>
          <li>File streaming to many clients</li>
          <li>Long-polling applications</li>
          <li>Applications requiring 1K+ concurrent connections</li>
        </ul>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <a
            href="https://github.com/oatpp/oatpp-starter-async"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link"
          >
            View on GitHub →
          </a>
        </div>
      </>
    }
  />
);

export const StarterProjectModule = () => (
  <GenericDocsPage
    title="Starter Project - Module"
    description="Template for creating reusable Oat++ modules."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Overview</h2>
        <p className="text-muted-foreground mb-6">
          This template helps you create reusable Oat++ modules that can be shared across projects.
        </p>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <a
            href="https://github.com/oatpp/oatpp-module-starter"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link"
          >
            View on GitHub →
          </a>
        </div>
      </>
    }
  />
);

export const InstallWindows = () => (
  <GenericDocsPage
    title="Install on Windows"
    description="Guide to install Oat++ on Windows using Visual Studio or MinGW."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Requirements</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>Git</li>
          <li>CMake version &gt;= 3.1</li>
          <li>Visual Studio 2017+ or MinGW with C++11 support</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Using Visual Studio</h2>
        <ol className="list-decimal list-inside text-muted-foreground space-y-2 mb-6">
          <li>Clone the oatpp repository</li>
          <li>Open CMake GUI and configure the project</li>
          <li>Generate Visual Studio solution</li>
          <li>Build and install from Visual Studio</li>
        </ol>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Using MinGW</h2>
        <ol className="list-decimal list-inside text-muted-foreground space-y-2 mb-6">
          <li>Open MinGW terminal</li>
          <li>Clone and build similar to Unix/Linux</li>
          <li>Use <code className="oat-code">-G "MinGW Makefiles"</code> with CMake</li>
        </ol>
      </>
    }
  />
);

export const Benchmark2M = () => (
  <GenericDocsPage
    title="2 Million WebSockets"
    description="Benchmark results for 2 million concurrent WebSocket connections."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Setup</h2>
        <p className="text-muted-foreground mb-6">
          Server: Google-Cloud n1-highmem-8 (8 vCPUs, 52 GB memory)
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Results</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Metric</th>
                <th>Value</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td>Connections</td>
                <td>2,000,000</td>
              </tr>
              <tr>
                <td>Messages per second</td>
                <td>~400,000</td>
              </tr>
              <tr>
                <td>Memory usage</td>
                <td>~18 GB</td>
              </tr>
            </tbody>
          </table>
        </div>
      </>
    }
  />
);

export const SupportedPlatforms = () => (
  <GenericDocsPage
    title="Supported Platforms"
    description="Oat++ runs on a wide variety of platforms thanks to its zero-dependency design."
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">Officially Tested</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>Linux (Ubuntu, Debian, CentOS, Arch)</li>
          <li>macOS</li>
          <li>Windows (Visual Studio, MinGW)</li>
          <li>FreeBSD</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Embedded & IoT</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>Raspberry Pi</li>
          <li>BeagleBone</li>
          <li>Various ARM boards</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Community Ports</h2>
        <p className="text-muted-foreground mb-6">
          The community has successfully ported Oat++ to many other platforms.
          If you have threads and a network stack, Oat++ can likely run there.
        </p>
      </>
    }
  />
);

export const Contributing = () => (
  <GenericDocsPage
    title="Contributing"
    description="We welcome contributions from the community!"
    content={
      <>
        <h2 className="text-2xl font-semibold text-foreground mb-4">How to Contribute</h2>
        <ol className="list-decimal list-inside text-muted-foreground space-y-2 mb-6">
          <li>Fork the repository on GitHub</li>
          <li>Create a feature branch</li>
          <li>Make your changes</li>
          <li>Write tests if applicable</li>
          <li>Submit a pull request</li>
        </ol>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Guidelines</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>Follow the existing code style</li>
          <li>Write clear commit messages</li>
          <li>Add documentation for new features</li>
          <li>Ensure all tests pass</li>
        </ul>

        <div className="p-4 bg-muted rounded-lg mb-6 not-prose">
          <a
            href="https://github.com/oatpp/oatpp/blob/master/CONTRIBUTING.md"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link"
          >
            Read full contribution guidelines →
          </a>
        </div>
      </>
    }
  />
);

export default GenericDocsPage;
