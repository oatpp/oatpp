import { Link } from "react-router-dom";
import Layout from "@/components/layout/Layout";
import CodeBlock from "@/components/CodeBlock";

const codeExample = `ENDPOINT("POST", "/users", createUser,
         BODY_DTO(Object<UserDto>, userDto))
{
  return createDtoResponse(Status::CODE_200, m_database->createUser(userDto));
}`;

const users = [
  { name: "OpenEye Scientific", url: "https://www.eyesopen.com/", logo: "https://www.eyesopen.com/hubfs/2016/07/openeye-logo.svg" },
  { name: "Milvus", url: "https://milvus.io/", logo: "https://milvus.io/images/milvus_logo.svg" },
  { name: "DeepDetect", url: "https://www.deepdetect.com/", logo: "https://www.deepdetect.com/img/icons/menu/sidebar/deepdetect.svg" },
];

const features = [
  {
    title: "Run It Anywhere",
    description: "Oat++ has",
    highlight: "zero-dependencies",
    descriptionEnd: "and it's",
    highlight2: "easy-portable",
    descriptionEnd2: "with a wide range of",
    link: { text: "supported platforms.", url: "/about/supported-platforms" },
  },
  {
    title: "Build Robust APIs",
    description: "With Oat++",
    highlight: "Simple-API",
    descriptionEnd: "it's easy and fun to build flexible and robust APIs.",
    link: { text: "See high-level overview.", url: "/docs/start/high-level-overview" },
  },
  {
    title: "Handle 5M Connections",
    description: "With Oat++",
    highlight: "Async-API",
    descriptionEnd: "you can handle over",
    highlight2: "5 Million",
    descriptionEnd2: "concurrent connections on a single server.",
    link: { text: "See benchmark.", url: "/benchmark/websocket-5m" },
  },
  {
    title: "Access Databases",
    description: "Oat++",
    highlight: "ORM",
    descriptionEnd: "provides an easy and unified way to access databases.",
    link: { text: "Read more.", url: "/docs/components/orm" },
  },
  {
    title: "Keep Code Consistent",
    description: "Oat++ relies on",
    highlight: "Object-Mapping",
    descriptionEnd: "throughout its codebase to ensure consistency of APIs and data models.",
    link: { text: "Read more.", url: "/docs/components/dto" },
  },
  {
    title: "Document Your APIs",
    description: "Automatically document endpoints with",
    highlight: "Swagger-UI",
    descriptionEnd: "and",
    highlight2: "OpenAPI 3.0.0",
    descriptionEnd2: ".",
    link: { text: "Read more.", url: "/docs/components/api-controller" },
  },
];

const Index = () => {
  return (
    <Layout>
      {/* Hero Section */}
      <section className="py-16 md:py-24">
        <div className="container px-4 md:px-6 text-center">
          <div className="flex justify-center mb-6">
            <OatLogoLarge />
          </div>
          <h1 className="text-4xl md:text-5xl font-bold text-foreground mb-4">
            Oat++
          </h1>
          <p className="text-xl md:text-2xl text-muted-foreground mb-8">
            An Open Source C++ Web Framework
          </p>
          <Link to="/docs/start" className="btn-primary text-base">
            Get Started
          </Link>
          
          <div className="mt-8 flex flex-col sm:flex-row items-center justify-center gap-4 text-sm">
            <div className="flex items-center gap-2">
              <span className="text-muted-foreground">Version:</span>
              <code className="oat-code">1.3.0</code>
            </div>
            <div className="flex items-center gap-2">
              <span className="text-muted-foreground">License:</span>
              <code className="oat-code">Apache-2.0 License</code>
            </div>
          </div>
        </div>
      </section>

      {/* Features Grid */}
      <section className="border-t border-border">
        <div className="container px-4 md:px-6">
          <div className="grid md:grid-cols-3 divide-y md:divide-y-0 md:divide-x divide-border">
            {features.slice(0, 3).map((feature) => (
              <FeatureCard key={feature.title} feature={feature} />
            ))}
          </div>
          <div className="grid md:grid-cols-3 divide-y md:divide-y-0 md:divide-x divide-border border-t border-border">
            {features.slice(3).map((feature) => (
              <FeatureCard key={feature.title} feature={feature} />
            ))}
          </div>
        </div>
      </section>

      {/* Code Example */}
      <section className="border-t border-border py-16">
        <div className="container px-4 md:px-6 max-w-3xl">
          <h2 className="text-2xl font-semibold text-foreground mb-6 text-center">
            How It Looks
          </h2>
          <CodeBlock code={codeExample} language="cpp" showLineNumbers />
        </div>
      </section>

      {/* Users Section */}
      <section className="border-t border-border py-16">
        <div className="container px-4 md:px-6">
          <h2 className="text-2xl font-semibold text-foreground mb-8 text-center">
            Oat++ Users
          </h2>
          <div className="flex flex-wrap justify-center items-center gap-8 md:gap-12">
            {users.map((user) => (
              <a
                key={user.name}
                href={user.url}
                target="_blank"
                rel="noopener noreferrer"
                className="grayscale hover:grayscale-0 transition-all opacity-70 hover:opacity-100"
              >
                <img
                  src={user.logo}
                  alt={user.name}
                  className="h-10 md:h-12 max-w-32 object-contain"
                />
              </a>
            ))}
          </div>
        </div>
      </section>

      {/* Community Section */}
      <section className="border-t border-border py-16 bg-muted/30">
        <div className="container px-4 md:px-6">
          <h2 className="text-2xl font-semibold text-foreground mb-8 text-center">
            Join Our Community
          </h2>
          <div className="grid md:grid-cols-3 gap-8 max-w-4xl mx-auto">
            <div className="text-center">
              <h3 className="font-semibold text-foreground mb-2">Use Oat++</h3>
              <p className="text-sm text-muted-foreground mb-4">
                Start using Oat++ and share your experience.
              </p>
              <Link to="/docs/start" className="oat-link text-sm">
                Get Started!
              </Link>
            </div>
            <div className="text-center">
              <h3 className="font-semibold text-foreground mb-2">Contribute</h3>
              <p className="text-sm text-muted-foreground mb-4">
                Oat++ is an open-source project and your contributions matter.
              </p>
              <Link to="/about/contributing" className="oat-link text-sm">
                Contribute!
              </Link>
            </div>
            <div className="text-center">
              <h3 className="font-semibold text-foreground mb-2">Connect</h3>
              <div className="flex justify-center gap-4 text-sm">
                <a href="https://twitter.com/oatpp_io" target="_blank" rel="noopener noreferrer" className="oat-link">
                  Twitter
                </a>
                <a href="https://gitter.im/oatpp-framework/Lobby" target="_blank" rel="noopener noreferrer" className="oat-link">
                  Gitter
                </a>
              </div>
            </div>
          </div>
        </div>
      </section>

      {/* FAQ Section */}
      <section className="border-t border-border py-16">
        <div className="container px-4 md:px-6 max-w-3xl">
          <h2 className="text-2xl font-semibold text-foreground mb-8 text-center">
            Frequently Asked Questions
          </h2>
          <div className="space-y-6">
            <FaqItem
              question='Q: "Oat++" name?'
              answer={
                <>
                  <li>"Oat" is something light, organic, and green. It can be easily cooked and consumed with no effort.</li>
                  <li>"++" gives a hint that it is "something" for C++.</li>
                </>
              }
            />
            <FaqItem
              question="Q: What is the main area of Oat++ application?"
              answer="Oat++ is used for many different purposes, from building REST APIs that run on embedded devices to building microservices and highly-loaded cloud applications. But the majority of use cases appears to be in IoT and Robotics."
            />
            <FaqItem
              question="Q: How portable is Oat++?"
              answer={
                <>
                  Theoretically, Oat++ can be <strong>easily</strong> ported everywhere where you have <strong>threads</strong> and <strong>network stack</strong>.
                </>
              }
            />
            <FaqItem
              question="Q: What is the size of a minimal Oat++ application?"
              answer="About 1Mb, depending on C/C++ std-lib and oatpp version."
            />
            <FaqItem
              question="Q: Which Oat++ API to choose, Simple or Async?"
              answer="Always choose Simple API wherever possible. Simple API is more developed and makes the code cleaner. Async API is designed for small, specific tasks that run at high concurrency levels."
            />
          </div>
        </div>
      </section>
    </Layout>
  );
};

const FeatureCard = ({ feature }: { feature: typeof features[0] }) => (
  <div className="feature-card">
    <h3 className="text-lg font-semibold text-foreground mb-3">{feature.title}</h3>
    <p className="text-sm text-muted-foreground leading-relaxed">
      {feature.description}{" "}
      <strong className="text-foreground">{feature.highlight}</strong>{" "}
      {feature.descriptionEnd}
      {feature.highlight2 && (
        <>
          {" "}
          <strong className="text-foreground">{feature.highlight2}</strong>{" "}
          {feature.descriptionEnd2}
        </>
      )}
      {feature.link && (
        <>
          {" "}
          <Link to={feature.link.url} className="oat-link">
            {feature.link.text}
          </Link>
        </>
      )}
    </p>
  </div>
);

const FaqItem = ({ question, answer }: { question: string; answer: React.ReactNode }) => (
  <div>
    <h3 className="font-semibold text-foreground mb-2">{question}</h3>
    {typeof answer === "string" ? (
      <p className="text-sm text-muted-foreground">{answer}</p>
    ) : (
      <ul className="text-sm text-muted-foreground list-disc list-inside space-y-1">
        {answer}
      </ul>
    )}
  </div>
);

const OatLogoLarge = () => (
  <svg width="140" height="140" viewBox="0 0 100 100" fill="none" xmlns="http://www.w3.org/2000/svg">
    <rect width="100" height="100" rx="8" fill="#2d3748"/>
    <g fill="none" stroke="#fff" strokeWidth="2.5">
      <ellipse cx="30" cy="35" rx="12" ry="18"/>
      <ellipse cx="70" cy="35" rx="12" ry="18"/>
      <ellipse cx="30" cy="70" rx="12" ry="18"/>
      <ellipse cx="70" cy="70" rx="12" ry="18"/>
      <line x1="50" y1="17" x2="50" y2="88" strokeWidth="2.5"/>
      <line x1="12" y1="52" x2="88" y2="52" strokeWidth="2.5"/>
    </g>
  </svg>
);

export default Index;
