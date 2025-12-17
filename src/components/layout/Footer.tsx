import { Link } from "react-router-dom";

export const Footer = () => {
  return (
    <footer className="border-t border-border bg-muted/30 py-8 mt-auto">
      <div className="container px-4 md:px-6">
        <div className="grid grid-cols-1 md:grid-cols-4 gap-8">
          <div>
            <h3 className="font-semibold text-foreground mb-3">Oat++</h3>
            <p className="text-sm text-muted-foreground">
              An Open Source C++ Web Framework for highly scalable and resource-efficient web applications.
            </p>
          </div>
          <div>
            <h4 className="font-semibold text-foreground mb-3">Documentation</h4>
            <ul className="space-y-2 text-sm">
              <li>
                <Link to="/docs/start" className="oat-link">
                  Getting Started
                </Link>
              </li>
              <li>
                <Link to="/docs/start/high-level-overview" className="oat-link">
                  High Level Overview
                </Link>
              </li>
              <li>
                <Link to="/examples/crud" className="oat-link">
                  Examples
                </Link>
              </li>
            </ul>
          </div>
          <div>
            <h4 className="font-semibold text-foreground mb-3">Community</h4>
            <ul className="space-y-2 text-sm">
              <li>
                <a
                  href="https://github.com/oatpp/oatpp"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="oat-link"
                >
                  GitHub
                </a>
              </li>
              <li>
                <a
                  href="https://gitter.im/oatpp-framework/Lobby"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="oat-link"
                >
                  Gitter Chat
                </a>
              </li>
              <li>
                <a
                  href="https://twitter.com/oatpp_io"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="oat-link"
                >
                  Twitter
                </a>
              </li>
            </ul>
          </div>
          <div>
            <h4 className="font-semibold text-foreground mb-3">Resources</h4>
            <ul className="space-y-2 text-sm">
              <li>
                <Link to="/about" className="oat-link">
                  About
                </Link>
              </li>
              <li>
                <Link to="/about/contributing" className="oat-link">
                  Contributing
                </Link>
              </li>
              <li>
                <Link to="/support" className="oat-link">
                  Support
                </Link>
              </li>
            </ul>
          </div>
        </div>
        <div className="mt-8 pt-6 border-t border-border text-center text-sm text-muted-foreground">
          <p>© {new Date().getFullYear()} Oat++ Framework. Licensed under Apache-2.0.</p>
        </div>
      </div>
    </footer>
  );
};

export default Footer;
