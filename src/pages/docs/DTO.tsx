import DocsLayout from "@/components/layout/DocsLayout";
import CodeBlock from "@/components/CodeBlock";
import FeedbackWidget from "@/components/FeedbackWidget";

const dtoCode = `#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class UserDto : public oatpp::DTO {
  
  DTO_INIT(UserDto, DTO)
  
  DTO_FIELD(Int64, id);
  DTO_FIELD(String, name);
  DTO_FIELD(String, email);
  DTO_FIELD(Int32, age);
  DTO_FIELD(Boolean, active, "is_active");
  
  DTO_FIELD_INFO(id) {
    info->description = "User's unique identifier";
  }
  
  DTO_FIELD_INFO(name) {
    info->description = "User's full name";
    info->required = true;
  }
  
};

#include OATPP_CODEGEN_END(DTO)`;

const nestedDtoCode = `class AddressDto : public oatpp::DTO {
  DTO_INIT(AddressDto, DTO)
  
  DTO_FIELD(String, street);
  DTO_FIELD(String, city);
  DTO_FIELD(String, country);
};

class UserWithAddressDto : public oatpp::DTO {
  DTO_INIT(UserWithAddressDto, DTO)
  
  DTO_FIELD(Int64, id);
  DTO_FIELD(String, name);
  DTO_FIELD(Object<AddressDto>, address);
  DTO_FIELD(Vector<Object<AddressDto>>, addresses);
};`;

const DTO = () => {
  return (
    <DocsLayout>
      <article className="prose prose-sm max-w-none">
        <h1 className="text-3xl font-bold text-foreground mb-6">Data Transfer Object (DTO)</h1>

        <p className="text-muted-foreground mb-6">
          DTOs (Data Transfer Objects) are used throughout oatpp for type-safe object mapping.
          They provide automatic serialization/deserialization for JSON, and are used in API Controllers,
          API Clients, and ORM.
        </p>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Basic Usage</h2>
        <CodeBlock code={dtoCode} language="cpp" showLineNumbers />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Field Types</h2>
        <div className="table-container mb-6 not-prose">
          <table className="oat-table">
            <thead>
              <tr>
                <th>Type</th>
                <th>Description</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><code className="oat-code">Int8, Int16, Int32, Int64</code></td>
                <td>Signed integers</td>
              </tr>
              <tr>
                <td><code className="oat-code">UInt8, UInt16, UInt32, UInt64</code></td>
                <td>Unsigned integers</td>
              </tr>
              <tr>
                <td><code className="oat-code">Float32, Float64</code></td>
                <td>Floating point numbers</td>
              </tr>
              <tr>
                <td><code className="oat-code">Boolean</code></td>
                <td>Boolean value</td>
              </tr>
              <tr>
                <td><code className="oat-code">String</code></td>
                <td>String value</td>
              </tr>
              <tr>
                <td><code className="oat-code">{"Object<T>"}</code></td>
                <td>Nested DTO object</td>
              </tr>
              <tr>
                <td><code className="oat-code">{"Vector<T>"}</code></td>
                <td>Array/List of values</td>
              </tr>
              <tr>
                <td><code className="oat-code">{"Fields<T>"}</code></td>
                <td>Map/Dictionary</td>
              </tr>
            </tbody>
          </table>
        </div>

        <h2 className="text-2xl font-semibold text-foreground mb-4">Nested DTOs</h2>
        <CodeBlock code={nestedDtoCode} language="cpp" />

        <h2 className="text-2xl font-semibold text-foreground mb-4 mt-8">Field Annotations</h2>
        <p className="text-muted-foreground mb-4">
          Use <code className="oat-code">DTO_FIELD_INFO</code> to add metadata for Swagger documentation:
        </p>
        <ul className="list-disc list-inside text-muted-foreground space-y-2 mb-6">
          <li><code className="oat-code">info-&gt;description</code> - Field description</li>
          <li><code className="oat-code">info-&gt;required</code> - Mark as required field</li>
          <li><code className="oat-code">info-&gt;pattern</code> - Regex pattern for validation</li>
        </ul>

        <FeedbackWidget />
      </article>
    </DocsLayout>
  );
};

export default DTO;
