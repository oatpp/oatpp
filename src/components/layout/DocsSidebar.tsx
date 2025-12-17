import { useState } from "react";
import { Link, useLocation } from "react-router-dom";
import { ChevronRight } from "lucide-react";
import { cn } from "@/lib/utils";

interface NavItem {
  title: string;
  path?: string;
  children?: NavItem[];
}

const sidebarNav: NavItem[] = [
  {
    title: "About",
    children: [
      { title: "About", path: "/about" },
      { title: "Contact", path: "/about/contact" },
      { title: "Contributing", path: "/about/contributing" },
      { title: "Supported Platforms", path: "/about/supported-platforms" },
      { title: "Build Status", path: "/about/build-status" },
    ],
  },
  {
    title: "Benchmark",
    children: [
      { title: "5 Million WebSockets", path: "/benchmark/websocket-5m" },
      { title: "2 Million WebSockets", path: "/benchmark/websocket-2m" },
    ],
  },
  {
    title: "Examples",
    children: [
      { title: "REST Service (CRUD)", path: "/examples/crud" },
      { title: "REST Client", path: "/examples/rest-client" },
      { title: "WebSocket", path: "/examples/websocket" },
      { title: "Database", path: "/examples/database" },
    ],
  },
  {
    title: "Installation",
    children: [
      { title: "Unix/Linux", path: "/docs/installation/unix-linux" },
      { title: "Windows", path: "/docs/installation/windows" },
    ],
  },
  {
    title: "Start",
    children: [
      { title: "Getting Started", path: "/docs/start" },
      { title: "High Level Overview", path: "/docs/start/high-level-overview" },
      { title: "Step By Step Guide", path: "/docs/start/step-by-step" },
      { title: "Starter Project", path: "/docs/start/project" },
      { title: "Starter Project - Async API", path: "/docs/start/project-async" },
      { title: "Starter Project - Module", path: "/docs/start/project-module" },
    ],
  },
  {
    title: "Overview",
    children: [
      { title: "Build", path: "/docs/overview/build" },
      { title: "IO", path: "/docs/overview/io" },
      { title: "Threading", path: "/docs/overview/threading" },
    ],
  },
  {
    title: "Modules",
    children: [
      { title: "oatpp-swagger", path: "/docs/modules/oatpp-swagger" },
      { title: "oatpp-openssl", path: "/docs/modules/oatpp-openssl" },
      { title: "oatpp-websocket", path: "/docs/modules/oatpp-websocket" },
      { title: "oatpp-sqlite", path: "/docs/modules/oatpp-sqlite" },
    ],
  },
  {
    title: "API Reference",
    children: [
      { title: "Api Controller", path: "/docs/components/api-controller" },
      { title: "Api Client", path: "/docs/components/api-client" },
      { title: "DTO", path: "/docs/components/dto" },
      { title: "ORM", path: "/docs/components/orm" },
    ],
  },
];

export const DocsSidebar = () => {
  const location = useLocation();
  const [openGroups, setOpenGroups] = useState<string[]>(() => {
    const current = sidebarNav.find((group) =>
      group.children?.some((item) => item.path === location.pathname)
    );
    return current ? [current.title] : ["Start"];
  });

  const toggleGroup = (title: string) => {
    setOpenGroups((prev) =>
      prev.includes(title) ? prev.filter((t) => t !== title) : [...prev, title]
    );
  };

  return (
    <aside className="w-60 shrink-0 border-r border-border bg-background sticky top-14 h-[calc(100vh-3.5rem)] overflow-y-auto hidden md:block">
      <nav className="py-4 px-2">
        {sidebarNav.map((group) => (
          <div key={group.title} className="mb-1">
            <button
              onClick={() => toggleGroup(group.title)}
              className="sidebar-group-title w-full text-left"
            >
              <span>{group.title}</span>
              <ChevronRight
                className={cn(
                  "w-4 h-4 transition-transform",
                  openGroups.includes(group.title) && "rotate-90"
                )}
              />
            </button>
            {openGroups.includes(group.title) && group.children && (
              <div className="ml-2 mt-1 space-y-0.5 animate-fade-in">
                {group.children.map((item) => (
                  <Link
                    key={item.path}
                    to={item.path || "#"}
                    className={cn(
                      "sidebar-link",
                      location.pathname === item.path && "sidebar-link-active"
                    )}
                  >
                    {item.title}
                  </Link>
                ))}
              </div>
            )}
          </div>
        ))}
      </nav>
    </aside>
  );
};

export const MobileSidebar = ({ onNavigate }: { onNavigate?: () => void }) => {
  const location = useLocation();
  const [openGroups, setOpenGroups] = useState<string[]>(["Start"]);

  const toggleGroup = (title: string) => {
    setOpenGroups((prev) =>
      prev.includes(title) ? prev.filter((t) => t !== title) : [...prev, title]
    );
  };

  return (
    <nav className="py-4 px-2">
      {sidebarNav.map((group) => (
        <div key={group.title} className="mb-1">
          <button
            onClick={() => toggleGroup(group.title)}
            className="sidebar-group-title w-full text-left"
          >
            <span>{group.title}</span>
            <ChevronRight
              className={cn(
                "w-4 h-4 transition-transform",
                openGroups.includes(group.title) && "rotate-90"
              )}
            />
          </button>
          {openGroups.includes(group.title) && group.children && (
            <div className="ml-2 mt-1 space-y-0.5 animate-fade-in">
              {group.children.map((item) => (
                <Link
                  key={item.path}
                  to={item.path || "#"}
                  onClick={onNavigate}
                  className={cn(
                    "sidebar-link",
                    location.pathname === item.path && "sidebar-link-active"
                  )}
                >
                  {item.title}
                </Link>
              ))}
            </div>
          )}
        </div>
      ))}
    </nav>
  );
};

export default DocsSidebar;
