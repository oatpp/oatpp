import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const ormCode = `#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class UserDb : public oatpp::orm::DbClient {
public:
  
  UserDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {}
  
  QUERY(createUser,
        "INSERT INTO users (name, email) VALUES (:name, :email);",
        PARAM(oatpp::String, name),
        PARAM(oatpp::String, email))
  
  QUERY(getUserById,
        "SELECT * FROM users WHERE id = :id;",
        PARAM(oatpp::Int64, id))
  
  QUERY(getAllUsers,
        "SELECT * FROM users LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::Int32, limit),
        PARAM(oatpp::Int32, offset))
  
  QUERY(updateUser,
        "UPDATE users SET name = :name, email = :email WHERE id = :id;",
        PARAM(oatpp::Int64, id),
        PARAM(oatpp::String, name),
        PARAM(oatpp::String, email))
  
  QUERY(deleteUser,
        "DELETE FROM users WHERE id = :id;",
        PARAM(oatpp::Int64, id))
  
};

#include OATPP_CODEGEN_END(DbClient)`;

const usageCode = `// In your service or controller
auto result = m_database->getUserById(userId);

if(result->isSuccess()) {
  auto users = result->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
  if(users->size() > 0) {
    return users[0];
  }
}

throw std::runtime_error("User not found");`;

const ORM = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Object-Relational Mapping (ORM)</h1>

        <p className="text-muted-foreground mb-6">
          Oat++ ORM provides a simple and type-safe way to interact with databases.
          It supports multiple database backends through modules like oatpp-sqlite, oatpp-postgresql, and oatpp-mysql.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Supported Databases</h2>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-8">
          <li><a href="/docs/modules/oatpp-sqlite" className="oat-link">oatpp-sqlite</a> - SQLite support</li>
          <li><a href="https://github.com/oatpp/oatpp-postgresql" className="oat-link" target="_blank">oatpp-postgresql</a> - PostgreSQL support</li>
          <li><a href="https://github.com/oatpp/oatpp-mysql" className="oat-link" target="_blank">oatpp-mysql</a> - MySQL support</li>
        </ul>

        <h2 className="text-2xl font-semibold text-foreground mb-4">DbClient</h2>
        <p className="text-muted-foreground mb-4">
          Create a database client by extending <code className="oat-code">oatpp::orm::DbClient</code>:
        </p>
        <CodeBlock code={ormCode} language="cpp" showLineNumbers />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Using Queries</h2>
        <CodeBlock code={usageCode} language="cpp" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Query Macros</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Macro</th>
                <th>Description</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><code className="oat-code">QUERY</code></td>
                <td>Define a database query</td>
              </tr>
              <tr>
                <td><code className="oat-code">PARAM</code></td>
                <td>Define a query parameter</td>
              </tr>
              <tr>
                <td><code className="oat-code">PREPARE</code></td>
                <td>Prepare statement for reuse</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Query Result</h2>
        <p className="text-muted-foreground mb-4">
          Query results can be fetched as DTOs:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li><code className="oat-code">result-&gt;isSuccess()</code> - Check if query succeeded</li>
          <li><code className="oat-code">{"result->fetch<Vector<Object<T>>>()"}</code> - Fetch results as vector of DTOs</li>
          <li><code className="oat-code">result-&gt;getErrorMessage()</code> - Get error message if failed</li>
        </ul>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default ORM;
