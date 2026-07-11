# `validation.prox` — Schema & Data Validation

Provides schema-based data validation for ProXPL. Declare the shape your
data should have, validate any value against that shape, and get back
structured results with clear per-field error messages.

## Usage

```javascript
use std.lib.validation;
```

---

## Classes

- [`Validator`](#validator) — Entry point, creates typed validators
- [`StringValidator`](#stringvalidator) — Rules for string fields
- [`NumberValidator`](#numbervalidator) — Rules for numeric fields
- [`BoolValidator`](#boolvalidator) — Rules for boolean fields
- [`ListValidator`](#listvalidator) — Rules for list fields
- [`AnyValidator`](#anyvalidator) — Accepts any non-null value
- [`Schema`](#schema) — Validates a full dictionary
- [`ValidationResult`](#validationresult) — Returned by every `.validate()` call
- [`Validate`](#validate) — Standalone one-off helper functions

---

## Validator

Entry point. All validator chains start here.

```javascript
Validator.string()   // → StringValidator
Validator.number()   // → NumberValidator
Validator.bool()     // → BoolValidator
Validator.list()     // → ListValidator
Validator.any()      // → AnyValidator
```

---

## StringValidator

Returned by `Validator.string()`. All methods return `this` for chaining.

| Method | Description |
|---|---|
| `.required()` | Field must be present and non-null |
| `.optional()` | Field may be absent or null (default) |
| `.minLength(n)` | String length must be ≥ n |
| `.maxLength(n)` | String length must be ≤ n |
| `.length(n)` | String length must equal n exactly |
| `.pattern(str)` | String must contain this substring |
| `.email()` | Must be a valid email address |
| `.url()` | Must start with `http://` or `https://` |
| `.alphanumeric()` | Only letters and digits allowed |
| `.noWhitespace()` | No space characters allowed |
| `.oneOf(list)` | Value must be one of the provided strings |
| `.withMessage(msg)` | Override the default error message |

```javascript
let v = Validator.string()
    .required()
    .minLength(2)
    .maxLength(50)
    .email();
```

---

## NumberValidator

Returned by `Validator.number()`. All methods return `this` for chaining.

| Method | Description |
|---|---|
| `.required()` | Field must be present and non-null |
| `.optional()` | Field may be absent or null (default) |
| `.min(n)` | Value must be ≥ n |
| `.max(n)` | Value must be ≤ n |
| `.between(min, max)` | Value must be ≥ min and ≤ max |
| `.integer()` | Must be a whole number (no fractional part) |
| `.positive()` | Must be > 0 |
| `.negative()` | Must be < 0 |
| `.oneOf(list)` | Value must equal one of the provided numbers |
| `.withMessage(msg)` | Override the default error message |

```javascript
let v = Validator.number()
    .required()
    .min(0)
    .max(120)
    .integer();
```

---

## BoolValidator

Returned by `Validator.bool()`. All methods return `this` for chaining.

| Method | Description |
|---|---|
| `.required()` | Field must be present and non-null |
| `.optional()` | Field may be absent or null (default) |
| `.isTrue()` | Value must be exactly `true` |
| `.isFalse()` | Value must be exactly `false` |
| `.withMessage(msg)` | Override the default error message |

```javascript
let v = Validator.bool().required().isTrue();
```

---

## ListValidator

Returned by `Validator.list()`. All methods return `this` for chaining.

| Method | Description |
|---|---|
| `.required()` | Field must be present and non-null |
| `.optional()` | Field may be absent or null (default) |
| `.minItems(n)` | List must have at least n items |
| `.maxItems(n)` | List must have at most n items |
| `.items(validator)` | Every item must pass this validator |
| `.withMessage(msg)` | Override the default error message |

When `.items()` is used, per-item errors are reported as
`fieldName[index]` (e.g. `scores[2]: must be at most 100`).

```javascript
let v = Validator.list()
    .required()
    .minItems(1)
    .maxItems(10)
    .items(Validator.number().min(0).max(100));
```

---

## AnyValidator

Returned by `Validator.any()`. Accepts a value of any type.

| Method | Description |
|---|---|
| `.required()` | Field must be present and non-null |
| `.optional()` | Field may be absent or null (default) |
| `.withMessage(msg)` | Override the default error message |

```javascript
let v = Validator.any().required();
```

---

## Schema

Validates a full dictionary against a set of named field validators.

### `Schema.define(fields)`

Defines a schema. `fields` is a dictionary of `{ fieldName: validator }`.

```javascript
let schema = Schema.define({
    "name":  Validator.string().required(),
    "age":   Validator.number().min(0).required(),
    "email": Validator.string().email().required()
});
```

### `schema.validate(data)`

Validates `data` against all declared rules. Returns a
[`ValidationResult`](#validationresult). Collects all errors — does not
stop at the first failure.

```javascript
let result = schema.validate({
    "name":  "Alice",
    "age":   25,
    "email": "alice@example.com"
});

if (!result.valid) {
    print(result.summary());
}
```

---

## ValidationResult

Returned by `schema.validate()`.

| Property / Method | Type | Description |
|---|---|---|
| `.valid` | bool | `true` if all rules passed |
| `.errors` | list | List of `{ field, message }` dictionaries |
| `.data` | any | The original input passed to `.validate()` |
| `.errorCount()` | number | Number of errors collected |
| `.hasError(field)` | bool | Does this field have at least one error? |
| `.getError(field)` | string\|null | First error message for a field, or null |
| `.summary()` | string | Human-readable summary of all errors |

```javascript
let result = schema.validate(data);

print(result.valid);          // true / false
print(result.errorCount());   // 0

if (result.hasError("email")) {
    print(result.getError("email"));
}

// "Validation failed (2 error(s)):\n  - age: ...\n  - email: ..."
print(result.summary());
```

---

## Validate

Standalone helper functions for quick one-off checks without defining a schema.

| Function | Returns | Description |
|---|---|---|
| `Validate.isEmail(value)` | bool | Valid email address? |
| `Validate.isURL(value)` | bool | Starts with `http://` or `https://`? |
| `Validate.isNotEmpty(value)` | bool | Non-null and non-empty string? |
| `Validate.isInteger(value)` | bool | Whole number (no decimal part)? |
| `Validate.isPositive(value)` | bool | Strictly greater than zero? |
| `Validate.isNegative(value)` | bool | Strictly less than zero? |
| `Validate.isInRange(value, min, max)` | bool | min ≤ value ≤ max? |
| `Validate.isOneOf(value, list)` | bool | Value present in list? |
| `Validate.matchesLength(value, min, max)` | bool | String length between min and max? |
| `Validate.isAlphanumeric(value)` | bool | Only letters and digits? |

```javascript
if (!Validate.isEmail(email)) {
    print("Invalid email.");
}

if (!Validate.isInRange(age, 0, 120)) {
    print("Age out of range.");
}
```

---

## Examples

### User registration form

```javascript
use std.lib.validation;

let registerSchema = Schema.define({
    "username": Validator.string().minLength(3).maxLength(20)
                    .alphanumeric().required(),
    "password": Validator.string().minLength(8).required(),
    "email":    Validator.string().email().required(),
    "age":      Validator.number().min(13).integer().required(),
    "newsletter": Validator.bool().optional()
});

func handleRegister(body) {
    let result = registerSchema.validate(body);

    if (!result.valid) {
        print("Errors:");
        for (let i = 0; i < len(result.errors); i = i + 1) {
            print("  " + result.errors[i]["field"] +
                  ": " + result.errors[i]["message"]);
        }
        return false;
    }

    // data is guaranteed clean here
    createUser(body);
    return true;
}
```

### Config file validation

```javascript
use std.lib.validation;

let configSchema = Schema.define({
    "host":      Validator.string().required(),
    "port":      Validator.number().min(1).max(65535).integer().required(),
    "debug":     Validator.bool().optional(),
    "logLevel":  Validator.string().oneOf(["info", "warn", "error"]).optional(),
    "maxRetries": Validator.number().integer().positive().optional()
});

func loadConfig(data) {
    let result = configSchema.validate(data);
    if (!result.valid) {
        print("Invalid config:");
        print(result.summary());
        return null;
    }
    return data;
}
```

### List validation with per-item errors

```javascript
use std.lib.validation;

let gameSchema = Schema.define({
    "playerName": Validator.string().minLength(1).required(),
    "scores":     Validator.list()
                      .minItems(1)
                      .maxItems(10)
                      .items(Validator.number().min(0).max(100))
                      .required()
});

let result = gameSchema.validate({
    "playerName": "Alice",
    "scores": [80, 110, 75]   // 110 is invalid
});

// scores[1]: must be at most 100
print(result.getError("scores[1]"));
```

### Quick inline checks with `Validate`

```javascript
use std.lib.validation;

let email = input("Enter email: ");
if (!Validate.isEmail(email)) {
    print("That doesn't look right.");
}

let age = to_number(input("Enter age: "));
if (!Validate.isInRange(age, 0, 120)) {
    print("Age must be between 0 and 120.");
}
```

---

## Notes

- `Schema.validate()` collects **all errors** before returning — it does
  not stop at the first failure, so users see every problem at once.
- Optional fields that are `null` or absent are silently skipped — no
  rules are checked for them.
- `ListValidator.items()` reports per-item errors as `fieldName[index]`
  so the caller knows exactly which item failed.
- This library uses only built-in ProXPL functions (`len`, `contains`,
  `startswith`, `substr`, `to_string`, `to_number`, `floor`, `keys`,
  `list_push`) — no native C extensions required.
