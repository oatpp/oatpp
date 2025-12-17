import { Toaster } from "@/components/ui/toaster";
import { Toaster as Sonner } from "@/components/ui/sonner";
import { TooltipProvider } from "@/components/ui/tooltip";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { BrowserRouter, Routes, Route } from "react-router-dom";

// Pages
import Index from "./pages/Index";
import About from "./pages/About";
import Support from "./pages/Support";
import Playground from "./pages/Playground";
import NotFound from "./pages/NotFound";

// Docs
import DocsStart from "./pages/docs/DocsStart";
import HighLevelOverview from "./pages/docs/HighLevelOverview";
import InstallUnix from "./pages/docs/InstallUnix";
import ApiController from "./pages/docs/ApiController";
import DTO from "./pages/docs/DTO";
import ORM from "./pages/docs/ORM";
import SwaggerModule from "./pages/docs/SwaggerModule";
import {
  StepByStep,
  StarterProject,
  StarterProjectAsync,
  StarterProjectModule,
  InstallWindows,
  Benchmark2M,
  SupportedPlatforms,
  Contributing,
} from "./pages/docs/GenericPages";

// Benchmark
import Benchmark5M from "./pages/benchmark/Benchmark5M";

// Examples
import ExampleCRUD from "./pages/examples/ExampleCRUD";

const queryClient = new QueryClient();

const App = () => (
  <QueryClientProvider client={queryClient}>
    <TooltipProvider>
      <Toaster />
      <Sonner />
      <BrowserRouter>
        <Routes>
          {/* Main Pages */}
          <Route path="/" element={<Index />} />
          <Route path="/about" element={<About />} />
          <Route path="/support" element={<Support />} />
          <Route path="/playground" element={<Playground />} />

          {/* About sub-pages */}
          <Route path="/about/contact" element={<About />} />
          <Route path="/about/contributing" element={<Contributing />} />
          <Route path="/about/supported-platforms" element={<SupportedPlatforms />} />
          <Route path="/about/build-status" element={<About />} />

          {/* Docs - Start */}
          <Route path="/docs/start" element={<DocsStart />} />
          <Route path="/docs/start/high-level-overview" element={<HighLevelOverview />} />
          <Route path="/docs/start/step-by-step" element={<StepByStep />} />
          <Route path="/docs/start/project" element={<StarterProject />} />
          <Route path="/docs/start/project-async" element={<StarterProjectAsync />} />
          <Route path="/docs/start/project-module" element={<StarterProjectModule />} />

          {/* Docs - Installation */}
          <Route path="/docs/installation/unix-linux" element={<InstallUnix />} />
          <Route path="/docs/installation/windows" element={<InstallWindows />} />

          {/* Docs - Components/API Reference */}
          <Route path="/docs/components/api-controller" element={<ApiController />} />
          <Route path="/docs/components/api-client" element={<ApiController />} />
          <Route path="/docs/components/dto" element={<DTO />} />
          <Route path="/docs/components/orm" element={<ORM />} />

          {/* Docs - Modules */}
          <Route path="/docs/modules/oatpp-swagger" element={<SwaggerModule />} />
          <Route path="/docs/modules/oatpp-openssl" element={<SwaggerModule />} />
          <Route path="/docs/modules/oatpp-websocket" element={<SwaggerModule />} />
          <Route path="/docs/modules/oatpp-sqlite" element={<ORM />} />

          {/* Docs - Overview */}
          <Route path="/docs/overview/build" element={<HighLevelOverview />} />
          <Route path="/docs/overview/io" element={<HighLevelOverview />} />
          <Route path="/docs/overview/threading" element={<HighLevelOverview />} />

          {/* Benchmark */}
          <Route path="/benchmark/websocket-5m" element={<Benchmark5M />} />
          <Route path="/benchmark/websocket-2m" element={<Benchmark2M />} />

          {/* Examples */}
          <Route path="/examples/crud" element={<ExampleCRUD />} />
          <Route path="/examples/rest-client" element={<ExampleCRUD />} />
          <Route path="/examples/websocket" element={<ExampleCRUD />} />
          <Route path="/examples/database" element={<ExampleCRUD />} />

          {/* 404 */}
          <Route path="*" element={<NotFound />} />
        </Routes>
      </BrowserRouter>
    </TooltipProvider>
  </QueryClientProvider>
);

export default App;
