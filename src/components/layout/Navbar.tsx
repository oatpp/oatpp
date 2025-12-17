import { useState } from "react";
import { Link, useLocation } from "react-router-dom";
import { Search, ExternalLink, Menu, X, Moon, Sun } from "lucide-react";
import { useTheme } from "@/hooks/useTheme";
import { SearchDialog } from "@/components/SearchDialog";

export const Navbar = () => {
  const location = useLocation();
  const [mobileMenuOpen, setMobileMenuOpen] = useState(false);
  const [searchOpen, setSearchOpen] = useState(false);
  const { theme, toggleTheme } = useTheme();

  const isActive = (path: string) => {
    if (path === "/docs/start") {
      return location.pathname.startsWith("/docs");
    }
    return location.pathname === path;
  };

  return (
    <>
      <header className="sticky top-0 z-50 border-b border-border bg-background/95 backdrop-blur supports-[backdrop-filter]:bg-background/60">
        <nav className="container flex h-14 items-center justify-between px-4 md:px-6">
          <Link to="/" className="flex items-center gap-2">
            <OatLogo />
            <span className="font-semibold text-lg text-foreground">Oat++</span>
          </Link>

          {/* Desktop Navigation */}
          <div className="hidden md:flex items-center gap-6">
            <div className="relative">
              <button
                onClick={() => setSearchOpen(true)}
                className="search-input w-48 lg:w-64 flex items-center gap-2 text-muted-foreground"
              >
                <Search className="w-4 h-4" />
                <span>Search...</span>
                <kbd className="ml-auto text-xs bg-muted px-1.5 py-0.5 rounded">⌘K</kbd>
              </button>
            </div>

            <Link
              to="/docs/start"
              className={`nav-link ${isActive("/docs/start") ? "nav-link-active" : ""}`}
            >
              Quick Start
            </Link>
            <Link
              to="/about"
              className={`nav-link ${isActive("/about") ? "nav-link-active" : ""}`}
            >
              About
            </Link>
            <Link
              to="/support"
              className={`nav-link ${isActive("/support") ? "nav-link-active" : ""}`}
            >
              Support
            </Link>
            <a
              href="https://github.com/oatpp/oatpp"
              target="_blank"
              rel="noopener noreferrer"
              className="nav-link flex items-center gap-1"
            >
              GitHub
              <ExternalLink className="w-3 h-3" />
            </a>
            <button
              onClick={toggleTheme}
              className="p-2 rounded-md hover:bg-muted transition-colors"
              aria-label="Toggle theme"
            >
              {theme === "dark" ? (
                <Sun className="w-4 h-4" />
              ) : (
                <Moon className="w-4 h-4" />
              )}
            </button>
          </div>

          {/* Mobile Menu Button */}
          <div className="flex items-center gap-2 md:hidden">
            <button
              onClick={toggleTheme}
              className="p-2 rounded-md hover:bg-muted transition-colors"
              aria-label="Toggle theme"
            >
              {theme === "dark" ? (
                <Sun className="w-4 h-4" />
              ) : (
                <Moon className="w-4 h-4" />
              )}
            </button>
            <button
              onClick={() => setMobileMenuOpen(!mobileMenuOpen)}
              className="p-2 rounded-md hover:bg-muted"
              aria-label="Toggle menu"
            >
              {mobileMenuOpen ? <X className="w-5 h-5" /> : <Menu className="w-5 h-5" />}
            </button>
          </div>
        </nav>

        {/* Mobile Menu */}
        {mobileMenuOpen && (
          <div className="md:hidden border-t border-border bg-background animate-slide-in">
            <div className="container px-4 py-4 space-y-4">
              <button
                onClick={() => {
                  setSearchOpen(true);
                  setMobileMenuOpen(false);
                }}
                className="search-input w-full flex items-center gap-2 text-muted-foreground"
              >
                <Search className="w-4 h-4" />
                <span>Search...</span>
              </button>
              <div className="flex flex-col gap-2">
                <Link
                  to="/docs/start"
                  className="nav-link py-2"
                  onClick={() => setMobileMenuOpen(false)}
                >
                  Quick Start
                </Link>
                <Link
                  to="/about"
                  className="nav-link py-2"
                  onClick={() => setMobileMenuOpen(false)}
                >
                  About
                </Link>
                <Link
                  to="/support"
                  className="nav-link py-2"
                  onClick={() => setMobileMenuOpen(false)}
                >
                  Support
                </Link>
                <a
                  href="https://github.com/oatpp/oatpp"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="nav-link py-2 flex items-center gap-1"
                >
                  GitHub
                  <ExternalLink className="w-3 h-3" />
                </a>
              </div>
            </div>
          </div>
        )}
      </header>

      <SearchDialog open={searchOpen} onOpenChange={setSearchOpen} />
    </>
  );
};

const OatLogo = () => (
  <svg width="28" height="28" viewBox="0 0 100 100" fill="none" xmlns="http://www.w3.org/2000/svg">
    <rect width="100" height="100" rx="8" fill="#2d3748"/>
    <g fill="none" stroke="#fff" strokeWidth="3">
      <ellipse cx="30" cy="35" rx="12" ry="18"/>
      <ellipse cx="70" cy="35" rx="12" ry="18"/>
      <ellipse cx="30" cy="70" rx="12" ry="18"/>
      <ellipse cx="70" cy="70" rx="12" ry="18"/>
      <line x1="50" y1="17" x2="50" y2="88" strokeWidth="3"/>
      <line x1="12" y1="52" x2="88" y2="52" strokeWidth="3"/>
    </g>
  </svg>
);

export default Navbar;
