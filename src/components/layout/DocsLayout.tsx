import { ReactNode, useState } from "react";
import { Menu, X } from "lucide-react";
import Navbar from "./Navbar";
import { DocsSidebar, MobileSidebar } from "./DocsSidebar";
import { Sheet, SheetContent, SheetTrigger } from "@/components/ui/sheet";

interface DocsLayoutProps {
  children: ReactNode;
}

export const DocsLayout = ({ children }: DocsLayoutProps) => {
  const [sidebarOpen, setSidebarOpen] = useState(false);

  return (
    <div className="min-h-screen flex flex-col">
      <Navbar />
      <div className="flex flex-1">
        <DocsSidebar />
        
        {/* Mobile sidebar trigger */}
        <div className="md:hidden fixed bottom-4 left-4 z-40">
          <Sheet open={sidebarOpen} onOpenChange={setSidebarOpen}>
            <SheetTrigger asChild>
              <button className="p-3 bg-oat-teal text-primary-foreground rounded-full shadow-lg">
                <Menu className="w-5 h-5" />
              </button>
            </SheetTrigger>
            <SheetContent side="left" className="w-72 p-0">
              <div className="pt-4 px-4">
                <h2 className="font-semibold text-lg">Documentation</h2>
              </div>
              <MobileSidebar onNavigate={() => setSidebarOpen(false)} />
            </SheetContent>
          </Sheet>
        </div>

        <div className="flex-1 overflow-hidden">
          <div className="container py-8 px-4 md:px-8 max-w-4xl">
            {children}
          </div>
        </div>
      </div>
    </div>
  );
};

export default DocsLayout;
