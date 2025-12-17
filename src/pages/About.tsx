import { Link } from "react-router-dom";
import DocsLayout from "@/components/layout/DocsLayout";
import FeedbackWidget from "@/components/FeedbackWidget";
import { ExternalLink, Star, GitFork } from "lucide-react";

const About = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">About Oat++</h1>

        {/* GitHub Stats */}
        <div className="flex flex-wrap gap-3 mb-6 not-prose">
          <a
            href="https://github.com/oatpp/oatpp"
            target="_blank"
            rel="noopener noreferrer"
            className="btn-outline flex items-center gap-2"
          >
            <Star className="w-4 h-4" />
            Star
            <span className="px-2 py-0.5 bg-muted rounded text-xs font-semibold">8,499</span>
          </a>
          <a
            href="https://github.com/oatpp/oatpp/fork"
            target="_blank"
            rel="noopener noreferrer"
            className="btn-outline flex items-center gap-2"
          >
            <GitFork className="w-4 h-4" />
            Fork
            <span className="px-2 py-0.5 bg-muted rounded text-xs font-semibold">1,354</span>
          </a>
        </div>

        <p className="text-muted-foreground mb-4">
          Latest version <code className="oat-code">1.3.0</code> 🎉
        </p>

        <p className="text-muted-foreground mb-6">
          Oat++ is an open-source C++ web framework for highly scalable and resource-efficient web applications.
        </p>

        <p className="text-muted-foreground mb-4">
          It provides all the necessary components for production-grade development, including:
        </p>

        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li>
            Advanced REST framework with request parameters mapping and Swagger-UI annotations.
            See <Link to="/docs/components/api-controller" className="oat-link">Api Controller</Link> and{" "}
            <Link to="/docs/components/api-client" className="oat-link">Api Client</Link>.
          </li>
          <li>
            ORM Framework. See <Link to="/docs/components/orm" className="oat-link">Object-Relational Mapping (ORM)</Link>
          </li>
          <li>
            WebSocket framework. See <Link to="/benchmark/websocket-5m" className="oat-link">5-million connections benchmark</Link>.
          </li>
          <li>
            Object Mapping. See <Link to="/docs/components/dto" className="oat-link">Data Transfer Object (DTO)</Link>.
          </li>
          <li>Dependency Injection</li>
          <li>
            Swagger-UI. See <Link to="/docs/modules/oatpp-swagger" className="oat-link">oatpp-swagger</Link> module.
          </li>
        </ul>

        <p className="text-muted-foreground mb-8">
          It's zero-dependency, easy-portable, and high-performance.
        </p>

        {/* Maintainers */}
        <h2 className="text-2xl font-semibold text-foreground mb-6">Maintainers</h2>
        <div className="grid md:grid-cols-2 gap-6 mb-8 not-prose">
          <MaintainerCard
            name="Leonid Stryzhevskyi"
            avatar="https://avatars.githubusercontent.com/u/5940640?v=4"
            description="Creator of @oatpp framework."
            github="lganzzzo"
          />
          <MaintainerCard
            name="Benedikt-Alexander Mokroß"
            avatar="https://avatars.githubusercontent.com/u/4124784?v=4"
            description="Vascular Biometrics | Embedded (Linux) Systems | Computer Vision | C/C++ | @oatpp"
            github="bamkrs"
          />
        </div>

        {/* Contributors */}
        <h2 className="text-2xl font-semibold text-foreground mb-6">Contributors</h2>
        <p className="text-muted-foreground mb-4">
          Contribute to Oat++ with us!
        </p>
        <p className="text-muted-foreground mb-8">
          <a
            href="https://github.com/oatpp/oatpp"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link inline-flex items-center gap-1"
          >
            Clone Oat++ on Github
            <ExternalLink className="w-3 h-3" />
          </a>{" "}
          and join our{" "}
          <a
            href="https://gitter.im/oatpp-framework/Lobby"
            target="_blank"
            rel="noopener noreferrer"
            className="oat-link inline-flex items-center gap-1"
          >
            Dev-Chat on Gitter
            <ExternalLink className="w-3 h-3" />
          </a>
        </p>

        {/* User Community */}
        <h2 className="text-2xl font-semibold text-foreground mb-6">Oat++ User Community</h2>
        <p className="text-muted-foreground mb-4">
          Do you use Oat++ for your projects? Share your story with us:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-8">
          <li>
            <a href="mailto:contact@oatpp.io" className="oat-link">contact@oatpp.io</a>
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
          <li>
            <a
              href="https://gitter.im/oatpp-framework/Lobby"
              target="_blank"
              rel="noopener noreferrer"
              className="oat-link"
            >
              Dev-Chat on Gitter
            </a>
          </li>
        </ul>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

const MaintainerCard = ({
  name,
  avatar,
  description,
  github,
}: {
  name: string;
  avatar: string;
  description: string;
  github: string;
}) => (
  <div className="flex gap-4 p-4 border border-border rounded-lg">
    <img
      src={avatar}
      alt={name}
      className="w-16 h-16 rounded-full"
    />
    <div>
      <h3 className="font-semibold text-foreground">{name}</h3>
      <p className="text-sm text-muted-foreground mb-2">{description}</p>
      <a
        href={`https://github.com/${github}`}
        target="_blank"
        rel="noopener noreferrer"
        className="btn-outline text-xs py-1 px-2"
      >
        Follow @{github}
      </a>
    </div>
  </div>
);

export default About;
