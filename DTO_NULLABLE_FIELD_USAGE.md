# DTO Nullable Field Support - Usage Guide

## Overview

Oat++ DTO fields support a `nullable` property that controls whether a field can accept `null` values and whether it should be included in the generated OpenAPI specification as a nullable field.

## Problem Statement

When using Oat++ DTOs with clients generated from OpenAPI specifications, fields that should be nullable were not properly marked, causing generated client code (e.g., C#) to use non-nullable types (e.g., `float` instead of `float?`).

## Solution

The `nullable` property in `DTO_FIELD_INFO` allows you to mark fields as nullable, which:
- Allows the field to accept `null` values during serialization/deserialization
- Affects validation behavior (when combined with `required`)
- Can be used by OpenAPI generators to properly mark fields as nullable

## Usage

### Basic Nullable Field

```cpp
#include OATPP_CODEGEN_BEGIN(DTO)

class MyDto : public oatpp::DTO {

  DTO_INIT(MyDto, DTO)

  DTO_FIELD_INFO(name) {
    info->description = "User name";
    info->required = true;
    info->nullable = false;
  }
  DTO_FIELD(String, name);

  DTO_FIELD_INFO(nickname) {
    info->description = "Optional nickname (can be null)";
    info->required = false;
    info->nullable = true;  // Field can be null
  }
  DTO_FIELD(String, nickname);

  DTO_FIELD_INFO(age) {
    info->description = "User age";
    info->required = false;
    info->nullable = false;  // Field can be omitted but not null
  }
  DTO_FIELD(Int32, age);

};

#include OATPP_CODEGEN_END(DTO)
```

### Field Property Combinations

| required | nullable | Behavior |
|----------|----------|----------|
| `true`   | `false`  | Field must be present and cannot be null (default for required fields) |
| `true`   | `true`   | Field must be present but can be null (rare, but valid) |
| `false`  | `false`  | Field can be omitted, but if present cannot be null (optional field) |
| `false`  | `true`   | Field can be omitted or null (flexible optional field) |

### Example: Complete DTO with Various Field Types

```cpp
class UserProfileDto : public oatpp::DTO {

  DTO_INIT(UserProfileDto, DTO)

  // Required non-nullable field
  DTO_FIELD_INFO(user_id) {
    info->description = "Unique user identifier";
    info->required = true;
    info->nullable = false;
  }
  DTO_FIELD(Int64, user_id);

  // Required but nullable (rare case)
  DTO_FIELD_INFO(bio) {
    info->description = "User biography (required but can be empty)";
    info->required = true;
    info->nullable = true;
  }
  DTO_FIELD(String, bio);

  // Optional non-nullable field
  DTO_FIELD_INFO(score) {
    info->description = "User score (optional, defaults to 0 if omitted)";
    info->required = false;
    info->nullable = false;
  }
  DTO_FIELD(Int32, score);

  // Optional nullable field
  DTO_FIELD_INFO(last_login) {
    info->description = "Last login timestamp (optional and can be null)";
    info->required = false;
    info->nullable = true;
  }
  DTO_FIELD(Int64, last_login);

};
```

### Serialization Behavior

#### Example 1: Nullable field set to null

```cpp
auto dto = MyDto::createShared();
dto->name = "John";
dto->nickname = nullptr;  // Explicitly set to null
dto->age = 30;

auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

// With default config (includeNullFields = false)
// Result: {"name":"John","age":30}
// nickname=null is NOT included in JSON

// With includeNullFields = true
// Result: {"name":"John","nickname":null,"age":30}
// nickname=null IS included in JSON
```

#### Example 2: Deserialization of nullable field

```cpp
// JSON: {"name":"John","nickname":null,"age":30}
auto dto = objectMapper->readFromString<oatpp::Object<MyDto>>(json);
// dto->nickname == nullptr ✓

// JSON: {"name":"John","age":30}  (nickname omitted)
auto dto = objectMapper->readFromString<oatpp::Object<MyDto>>(json);
// dto->nickname == nullptr ✓ (defaults to null when omitted)
```

### Validation Behavior

The combination of `required` and `nullable` affects validation:

```cpp
// required=true, nullable=false
// Field MUST be present and MUST NOT be null
DTO_FIELD_INFO(required_non_nullable) {
  info->required = true;
  info->nullable = false;
}
// Validation fails if field is missing OR null

// required=true, nullable=true
// Field MUST be present but CAN be null
DTO_FIELD_INFO(required_nullable) {
  info->required = true;
  info->nullable = true;
}
// Validation fails if field is missing, but succeeds if field is null

// required=false, nullable=false
// Field can be omitted, but if present must NOT be null
DTO_FIELD_INFO(optional_non_nullable) {
  info->required = false;
  info->nullable = false;
}
// Validation succeeds if field is omitted or has a value, fails if null

// required=false, nullable=true
// Field can be omitted or null
DTO_FIELD_INFO(optional_nullable) {
  info->required = false;
  info->nullable = true;
}
// Validation succeeds if field is omitted, null, or has a value
```

### Configuration: includeNullFields

Control whether null fields are included in JSON output:

```cpp
auto config = oatpp::parser::json::mapping::ObjectMapper::createShared();
config->config->includeNullFields = false;  // Default - don't include null fields
config->config->includeNullFields = true;   // Include null fields in JSON
```

### Impact on OpenAPI/Swagger Generation

When `nullable = true` is set on a field, it should be reflected in the generated OpenAPI specification:

```yaml
UserProfileDto:
  type: object
  properties:
    user_id:
      type: integer
      format: int64
      description: Unique user identifier
    nickname:
      type: string
      nullable: true
      description: Optional nickname (can be null)
    last_login:
      type: integer
      format: int64
      nullable: true
      description: Last login timestamp (optional and can be null)
```

This enables proper client code generation:
- **C#**: `string? nickname` instead of `string nickname`
- **Java**: `Long lastLogin` (boxed) instead of `long lastLogin` (primitive)
- **TypeScript**: `lastLogin?: number | null` instead of `lastLogin: number`

## Migration Guide

### Before (Problematic)

```cpp
DTO_FIELD_INFO(myField) {
  info->description = "My description";
  info->required = false;
}
DTO_FIELD(Float32, myField);
```

Generated C# client:
```csharp
public float MyField { get; set; }  // Cannot represent null!
```

### After (Fixed)

```cpp
DTO_FIELD_INFO(myField) {
  info->description = "My description";
  info->required = false;
  info->nullable = true;  // <-- Add this!
}
DTO_FIELD(Float32, myField);
```

Generated C# client:
```csharp
public float? MyField { get; set; }  // Properly nullable!
```

## Best Practices

1. **Use `nullable = true` for optional reference-type fields** that may not have a value
2. **Use `nullable = false` with `required = false`** for fields that have meaningful default values
3. **Avoid `required = true` with `nullable = true`** unless you have a specific use case
4. **Consistently apply nullable properties** to ensure predictable API behavior

## Testing

See `test/oatpp/data/mapping/NullableFieldTest.cpp` for comprehensive examples and test cases.

## Compatibility

- **Oat++ Version**: 1.3.x and later
- **Backward Compatible**: Yes, nullable defaults to `false`
- **OpenAPI Version**: 3.0.x (nullable property support)
