import DocsLayout from "@/components/layout/DocsLayout";
import FeedbackWidget from "@/components/FeedbackWidget";
import { ExternalLink } from "lucide-react";

const Benchmark5M = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">5 Million WebSockets</h1>

        <div className="flex flex-wrap gap-4 mb-6 text-sm not-prose">
          <div className="flex items-center gap-2">
            <span className="text-muted-foreground">Date -</span>
            <code className="oat-code">May 20, 2019</code>
          </div>
          <div className="flex items-center gap-2">
            <span className="text-muted-foreground">Oatpp version -</span>
            <code className="oat-code">0.19.4</code>
          </div>
        </div>

        <p className="text-muted-foreground mb-6">
          This article describes oatpp benchmark for 5 Million <strong className="text-foreground">fully-loaded</strong> concurrent websocket connections.
        </p>

        <p className="text-muted-foreground mb-4">It consists of two parts:</p>
        <ul className="list-disc list-inside text-muted-foreground space-y-1 mb-8">
          <li><a href="#4m-websockets" className="oat-link">4 Million WebSockets test</a></li>
          <li><a href="#5m-websockets" className="oat-link">5 Million WebSockets test</a></li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">The Purpose</h2>
        <p className="text-muted-foreground mb-4">
          This benchmark is aimed to determine scalability of oatpp with increase of load and computing powers
          with respect to <strong className="text-foreground">previous</strong>{" "}
          <a href="/benchmark/websocket-2m" className="oat-link">2-million WebSockets benchmark</a>.
        </p>

        <div className="table-container mb-8 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th></th>
                <th>Previous, 2M benchmark</th>
                <th>This, 4M / 5M benchmark</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td>Computing power</td>
                <td><strong>8 vCPUs, 52 GB memory</strong></td>
                <td><strong>16 vCPUs, 104 GB memory</strong></td>
              </tr>
              <tr>
                <td>Load</td>
                <td><strong>2M connections</strong></td>
                <td><strong>4M / 5M connections</strong></td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 id="4m-websockets" className="text-2xl font-semibold text-foreground mb-4">4M WebSockets</h2>
        
        <h3 className="text-xl font-semibold text-foreground mb-3">Setup</h3>
        <div className="p-6 bg-muted rounded-lg mb-6 not-prose">
          <div className="flex items-center justify-center">
            <div className="text-center">
              <div className="text-lg font-semibold text-foreground mb-2">Google Cloud</div>
              <div className="flex gap-8">
                <div className="border border-border rounded p-4 bg-background">
                  <div className="text-xs text-muted-foreground mb-1">n1-highmem-16</div>
                  <div className="font-medium">4 Million Clients</div>
                </div>
                <div className="flex items-center">
                  <div className="w-8 h-0.5 bg-destructive"></div>
                  <div className="w-0 h-0 border-t-4 border-b-4 border-l-8 border-transparent border-l-destructive"></div>
                </div>
                <div className="border border-border rounded p-4 bg-background">
                  <div className="text-xs text-muted-foreground mb-1">n1-highmem-16</div>
                  <div className="font-medium">Server</div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>Server Machine - Google-Cloud <strong className="text-foreground">n1-highmem-16 (16 vCPUs, 104 GB memory)</strong> running Debian GNU/Linux 9.</li>
          <li>Client Machine - Google-Cloud <strong className="text-foreground">n1-highmem-16 (16 vCPUs, 104 GB memory)</strong> running Debian GNU/Linux 9.</li>
        </ul>

        <h3 className="text-xl font-semibold text-foreground mb-3">Results</h3>
        <div className="table-container mb-8 not-prose">
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
                <td>4,000,000</td>
              </tr>
              <tr>
                <td>Messages per second</td>
                <td>~800,000</td>
              </tr>
              <tr>
                <td>Memory usage</td>
                <td>~35 GB</td>
              </tr>
              <tr>
                <td>CPU usage</td>
                <td>~60%</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 id="5m-websockets" className="text-2xl font-semibold text-foreground mb-4">5M WebSockets</h2>
        <p className="text-muted-foreground mb-4">
          For 5 Million connections, we used an additional client machine to generate more load.
        </p>

        <div className="table-container mb-8 not-prose">
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
                <td>5,000,000</td>
              </tr>
              <tr>
                <td>Messages per second</td>
                <td>~1,000,000</td>
              </tr>
              <tr>
                <td>Memory usage</td>
                <td>~45 GB</td>
              </tr>
              <tr>
                <td>CPU usage</td>
                <td>~75%</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Conclusion</h2>
        <p className="text-muted-foreground mb-6">
          Oat++ demonstrates excellent scalability. When computing power is doubled, the capacity for connections
          also approximately doubles, showing linear scalability characteristics.
        </p>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default Benchmark5M;
