import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const installCommands = `$ git clone https://github.com/oatpp/oatpp.git
$ cd oatpp
$ mkdir build && cd build
$ cmake ..
$ make install`;

const InstallUnix = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Install on Unix/Linux</h1>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Requirements</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-6">
          <li>Git</li>
          <li>CMake version &gt;= 3.1</li>
          <li>C++ compiler with C++11 support (GCC, Clang)</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Install oatpp</h2>
        <CodeBlock code={installCommands} language="bash" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">CMake Options</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Option</th>
                <th>Default</th>
                <th>Description</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><code className="oat-code">OATPP_BUILD_TESTS</code></td>
                <td>ON</td>
                <td>Build tests</td>
              </tr>
              <tr>
                <td><code className="oat-code">OATPP_DISABLE_ENV_OBJECT_COUNTERS</code></td>
                <td>OFF</td>
                <td>Disable object counters</td>
              </tr>
              <tr>
                <td><code className="oat-code">OATPP_DISABLE_POOL_ALLOCATIONS</code></td>
                <td>OFF</td>
                <td>Disable pool allocations</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Verify Installation</h2>
        <p className="text-muted-foreground mb-4">
          After installation, you can verify by checking if oatpp can be found by CMake:
        </p>
        <CodeBlock
          code={`find_package(oatpp REQUIRED)`}
          language="cmake"
        />

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default InstallUnix;
