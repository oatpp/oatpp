import { useState } from "react";
import DocsLayout from "@/components/layout/DocsLayout";
import { Play, RefreshCw } from "lucide-react";

const defaultCode = `ENDPOINT("GET", "/hello", hello) {
  return createResponse(Status::CODE_200, "Hello, World!");
}

ENDPOINT("POST", "/users", createUser,
         BODY_DTO(Object<UserDto>, userDto)) {
  return createDtoResponse(Status::CODE_201, userDto);
}`;

const Playground = () => {
  const [code, setCode] = useState(defaultCode);
  const [output, setOutput] = useState("");
  const [isRunning, setIsRunning] = useState(false);

  const handleRun = () => {
    setIsRunning(true);
    // Simulate compilation and output
    setTimeout(() => {
      setOutput(`Compiling...

✓ Code compiled successfully!

Server started on http://localhost:8000

Available endpoints:
  GET  /hello     → Returns "Hello, World!"
  POST /users     → Creates a new user

Try making requests to these endpoints.`);
      setIsRunning(false);
    }, 1500);
  };

  const handleReset = () => {
    setCode(defaultCode);
    setOutput("");
  };

  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Code Playground</h1>

        <p className="text-muted-foreground mb-6">
          Experiment with Oat++ code snippets in this interactive playground.
          Note: This is a simulated environment for learning purposes.
        </p>

        <div className="not-prose">
          <div className="border border-border rounded-lg overflow-hidden">
            {/* Toolbar */}
            <div className="flex items-center justify-between px-4 py-2 bg-muted border-b border-border">
              <span className="text-sm font-medium">main.cpp</span>
              <div className="flex gap-2">
                <button
                  onClick={handleReset}
                  className="btn-outline text-xs py-1 px-2 flex items-center gap-1"
                >
                  <RefreshCw className="w-3 h-3" />
                  Reset
                </button>
                <button
                  onClick={handleRun}
                  disabled={isRunning}
                  className="btn-primary text-xs py-1 px-2 flex items-center gap-1"
                >
                  <Play className="w-3 h-3" />
                  {isRunning ? "Running..." : "Run"}
                </button>
              </div>
            </div>

            {/* Code Editor */}
            <div className="grid md:grid-cols-2 divide-y md:divide-y-0 md:divide-x divide-border">
              <div className="relative">
                <textarea
                  value={code}
                  onChange={(e) => setCode(e.target.value)}
                  className="w-full h-64 md:h-80 p-4 bg-oat-code text-oat-code-text font-mono text-sm resize-none focus:outline-none"
                  spellCheck={false}
                />
              </div>
              <div className="bg-background">
                <div className="px-4 py-2 bg-muted border-b border-border">
                  <span className="text-xs font-medium text-muted-foreground">Output</span>
                </div>
                <pre className="p-4 h-56 md:h-72 overflow-auto text-sm font-mono text-muted-foreground whitespace-pre-wrap">
                  {output || "Click 'Run' to compile and execute the code..."}
                </pre>
              </div>
            </div>
          </div>

          <div className="mt-6 p-4 bg-muted rounded-lg">
            <h3 className="font-semibold text-foreground mb-2">Try These Examples:</h3>
            <ul className="space-y-2 text-sm text-muted-foreground">
              <li>
                <button
                  onClick={() => setCode(`ENDPOINT("GET", "/api/status", status) {
  auto response = StatusDto::createShared();
  response->status = "OK";
  response->timestamp = std::time(nullptr);
  return createDtoResponse(Status::CODE_200, response);
}`)}
                  className="oat-link"
                >
                  → Status endpoint with DTO response
                </button>
              </li>
              <li>
                <button
                  onClick={() => setCode(`ENDPOINT("GET", "/users/{userId}", getUserById,
         PATH(Int64, userId),
         QUERY(String, fields, "fields")) {
  auto user = m_userService->getById(userId);
  return createDtoResponse(Status::CODE_200, user);
}`)}
                  className="oat-link"
                >
                  → Endpoint with path and query parameters
                </button>
              </li>
              <li>
                <button
                  onClick={() => setCode(`ENDPOINT_ASYNC("GET", "/stream", streamData) {
  
  class StreamCoroutine : public oatpp::async::Coroutine<StreamCoroutine> {
  public:
    Action act() override {
      return _return(createResponse(Status::CODE_200, "Streaming..."));
    }
  };
  
  return StreamCoroutine::start();
}`)}
                  className="oat-link"
                >
                  → Async endpoint example
                </button>
              </li>
            </ul>
          </div>
        </div>
      </article>
    </DocsLayout>
  );
};

export default Playground;
