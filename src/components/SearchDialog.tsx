import { useState, useEffect, useMemo } from "react";
import { useNavigate } from "react-router-dom";
import { Search, FileText, Code, BookOpen } from "lucide-react";
import {
  Dialog,
  DialogContent,
} from "@/components/ui/dialog";

interface SearchResult {
  title: string;
  path: string;
  type: "doc" | "api" | "example";
  description?: string;
}

const searchData: SearchResult[] = [
  { title: "Getting Started", path: "/docs/start", type: "doc", description: "Welcome to Oat++" },
  { title: "High Level Overview", path: "/docs/start/high-level-overview", type: "doc", description: "Learn about the main components" },
  { title: "Step By Step Guide", path: "/docs/start/step-by-step", type: "doc", description: "Create an Oat++ project from scratch" },
  { title: "Starter Project", path: "/docs/start/project", type: "doc", description: "Simple API starter" },
  { title: "Starter Project - Async API", path: "/docs/start/project-async", type: "doc", description: "Async API starter" },
  { title: "About Oat++", path: "/about", type: "doc", description: "About the framework" },
  { title: "5 Million WebSockets", path: "/benchmark/websocket-5m", type: "doc", description: "WebSocket benchmark" },
  { title: "2 Million WebSockets", path: "/benchmark/websocket-2m", type: "doc", description: "WebSocket benchmark" },
  { title: "REST Service Example", path: "/examples/crud", type: "example", description: "CRUD service with Swagger" },
  { title: "API Controller", path: "/docs/components/api-controller", type: "api", description: "REST API endpoints" },
  { title: "DTO - Data Transfer Object", path: "/docs/components/dto", type: "api", description: "Object mapping" },
  { title: "ORM", path: "/docs/components/orm", type: "api", description: "Database access" },
  { title: "oatpp-swagger", path: "/docs/modules/oatpp-swagger", type: "api", description: "Swagger UI module" },
  { title: "Installation - Unix/Linux", path: "/docs/installation/unix-linux", type: "doc", description: "Install on Unix/Linux" },
  { title: "Installation - Windows", path: "/docs/installation/windows", type: "doc", description: "Install on Windows" },
];

interface SearchDialogProps {
  open: boolean;
  onOpenChange: (open: boolean) => void;
}

export const SearchDialog = ({ open, onOpenChange }: SearchDialogProps) => {
  const [query, setQuery] = useState("");
  const navigate = useNavigate();

  const filteredResults = useMemo(() => {
    if (!query.trim()) return searchData.slice(0, 8);
    const lowerQuery = query.toLowerCase();
    return searchData.filter(
      (item) =>
        item.title.toLowerCase().includes(lowerQuery) ||
        item.description?.toLowerCase().includes(lowerQuery)
    );
  }, [query]);

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if ((e.metaKey || e.ctrlKey) && e.key === "k") {
        e.preventDefault();
        onOpenChange(true);
      }
    };

    document.addEventListener("keydown", handleKeyDown);
    return () => document.removeEventListener("keydown", handleKeyDown);
  }, [onOpenChange]);

  const handleSelect = (path: string) => {
    navigate(path);
    onOpenChange(false);
    setQuery("");
  };

  const getIcon = (type: string) => {
    switch (type) {
      case "api":
        return <Code className="w-4 h-4 text-oat-teal" />;
      case "example":
        return <BookOpen className="w-4 h-4 text-oat-teal" />;
      default:
        return <FileText className="w-4 h-4 text-muted-foreground" />;
    }
  };

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="p-0 gap-0 max-w-xl">
        <div className="flex items-center border-b border-border px-4">
          <Search className="w-4 h-4 text-muted-foreground" />
          <input
            type="text"
            placeholder="Search documentation..."
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            className="flex-1 px-3 py-4 bg-transparent outline-none text-sm"
            autoFocus
          />
          <kbd className="text-xs bg-muted px-1.5 py-0.5 rounded text-muted-foreground">ESC</kbd>
        </div>
        <div className="max-h-80 overflow-y-auto p-2">
          {filteredResults.length === 0 ? (
            <div className="px-4 py-8 text-center text-muted-foreground text-sm">
              No results found for "{query}"
            </div>
          ) : (
            <div className="space-y-1">
              {filteredResults.map((result) => (
                <button
                  key={result.path}
                  onClick={() => handleSelect(result.path)}
                  className="w-full flex items-start gap-3 px-3 py-2 rounded-md hover:bg-muted text-left transition-colors"
                >
                  {getIcon(result.type)}
                  <div className="flex-1 min-w-0">
                    <div className="text-sm font-medium text-foreground truncate">
                      {result.title}
                    </div>
                    {result.description && (
                      <div className="text-xs text-muted-foreground truncate">
                        {result.description}
                      </div>
                    )}
                  </div>
                </button>
              ))}
            </div>
          )}
        </div>
      </DialogContent>
    </Dialog>
  );
};
