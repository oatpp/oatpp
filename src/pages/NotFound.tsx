import { useLocation, Link } from "react-router-dom";
import { useEffect } from "react";
import Layout from "@/components/layout/Layout";

const NotFound = () => {
  const location = useLocation();

  useEffect(() => {
    console.error("404 Error: User attempted to access non-existent route:", location.pathname);
  }, [location.pathname]);

  return (
    <Layout>
      <div className="flex min-h-[60vh] items-center justify-center">
        <div className="text-center">
          <h1 className="mb-4 text-6xl font-bold text-foreground">404</h1>
          <p className="mb-6 text-xl text-muted-foreground">Page not found</p>
          <p className="mb-8 text-sm text-muted-foreground max-w-md">
            The page you're looking for doesn't exist or has been moved.
          </p>
          <div className="flex gap-4 justify-center">
            <Link to="/" className="btn-primary">
              Go Home
            </Link>
            <Link to="/docs/start" className="btn-outline">
              Documentation
            </Link>
          </div>
        </div>
      </div>
    </Layout>
  );
};

export default NotFound;
