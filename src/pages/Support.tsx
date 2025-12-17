import Layout from "@/components/layout/Layout";
import { ExternalLink, MessageSquare, BookOpen, Users, Mail } from "lucide-react";

const Support = () => {
  return (
    <Layout>
      <div className="container px-4 md:px-6 py-16 max-w-4xl">
        <h1 className="text-3xl font-bold text-foreground mb-6">Support</h1>
        
        <p className="text-muted-foreground mb-8">
          Need help with Oat++? Here are the best ways to get support and connect with the community.
        </p>

        <div className="grid md:grid-cols-2 gap-6 mb-12">
          <SupportCard
            icon={<MessageSquare className="w-8 h-8" />}
            title="Gitter Chat"
            description="Join our real-time chat for quick questions and community discussions."
            link="https://gitter.im/oatpp-framework/Lobby"
            linkText="Join Gitter Chat"
          />
          <SupportCard
            icon={<BookOpen className="w-8 h-8" />}
            title="Stack Overflow"
            description="Search existing answers or ask new questions with the oat++ tag."
            link="https://stackoverflow.com/questions/tagged/oat%2b%2b"
            linkText="Browse Questions"
          />
          <SupportCard
            icon={<Users className="w-8 h-8" />}
            title="GitHub Issues"
            description="Report bugs or request features on our GitHub repository."
            link="https://github.com/oatpp/oatpp/issues"
            linkText="Open an Issue"
          />
          <SupportCard
            icon={<Mail className="w-8 h-8" />}
            title="Email"
            description="For business inquiries or partnership opportunities."
            link="mailto:contact@oatpp.io"
            linkText="contact@oatpp.io"
          />
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Documentation</h2>
        <p className="text-muted-foreground mb-6">
          Before reaching out for support, please check our comprehensive documentation:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-8">
          <li>
            <a href="/docs/start" className="oat-link">Getting Started Guide</a> - Installation and first steps
          </li>
          <li>
            <a href="/docs/start/high-level-overview" className="oat-link">High Level Overview</a> - Understanding core concepts
          </li>
          <li>
            <a href="/examples/crud" className="oat-link">Examples</a> - Complete working examples
          </li>
          <li>
            <a href="/docs/components/api-controller" className="oat-link">API Reference</a> - Detailed API documentation
          </li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Professional Support</h2>
        <p className="text-muted-foreground">
          For professional support, consulting, or enterprise needs, please contact us at{" "}
          <a href="mailto:contact@oatpp.io" className="oat-link">contact@oatpp.io</a>.
        </p>
      </div>
    </Layout>
  );
};

const SupportCard = ({
  icon,
  title,
  description,
  link,
  linkText,
}: {
  icon: React.ReactNode;
  title: string;
  description: string;
  link: string;
  linkText: string;
}) => (
  <div className="p-6 border border-border rounded-lg hover:shadow-md transition-shadow">
    <div className="text-oat-teal mb-4">{icon}</div>
    <h3 className="text-lg font-semibold text-foreground mb-2">{title}</h3>
    <p className="text-sm text-muted-foreground mb-4">{description}</p>
    <a
      href={link}
      target={link.startsWith("mailto:") ? undefined : "_blank"}
      rel={link.startsWith("mailto:") ? undefined : "noopener noreferrer"}
      className="oat-link text-sm inline-flex items-center gap-1"
    >
      {linkText}
      {!link.startsWith("mailto:") && <ExternalLink className="w-3 h-3" />}
    </a>
  </div>
);

export default Support;
