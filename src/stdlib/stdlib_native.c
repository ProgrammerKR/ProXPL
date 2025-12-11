#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Forward declarations
static Value nativeAbs(int argCount, Value *args);
static Value nativeCeil(int argCount, Value *args);
static Value nativeFloor(int argCount, Value *args);
static Value nativeRound(int argCount, Value *args);
static Value nativeMax(int argCount, Value *args);
static Value nativeMin(int argCount, Value *args);
static Value nativePow(int argCount, Value *args);
static Value nativeSqrt(int argCount, Value *args);
static Value nativeSin(int argCount, Value *args);
static Value nativeCos(int argCount, Value *args);
static Value nativeTan(int argCount, Value *args);
static Value nativeLog(int argCount, Value *args);
static Value nativeExp(int argCount, Value *args);
static Value nativeRandom(int argCount, Value *args);
static Value nativeRandInt(int argCount, Value *args);

// String functions
static Value nativeLen(int argCount, Value *args);
static Value nativeUpper(int argCount, Value *args);
static Value nativeLower(int argCount, Value *args);
static Value nativeTrim(int argCount, Value *args);
static Value nativeStartsWith(int argCount, Value *args);
static Value nativeEndsWith(int argCount, Value *args);
static Value nativeContains(int argCount, Value *args);
static Value nativeIndexOf(int argCount, Value *args);
static Value nativeCharAt(int argCount, Value *args);

// IO functions
static Value nativePrint(int argCount, Value *args);
static Value nativeInput(int argCount, Value *args);
static Value nativeReadFile(int argCount, Value *args);
static Value nativeWriteFile(int argCount, Value *args);
static Value nativeAppendFile(int argCount, Value *args);

// Utility functions
static Value nativeType(int argCount, Value *args);
static Value nativeClock(int argCount, Value *args);
static Value nativeStr(int argCount, Value *args);
static Value nativeInt(int argCount, Value *args);
static Value nativeFloat(int argCount, Value *args);

// === Math Functions ===

static Value nativeAbs(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(fabs(AS_NUMBER(args[0])));
}

static Value nativeCeil(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(ceil(AS_NUMBER(args[0])));
}

static Value nativeFloor(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(floor(AS_NUMBER(args[0])));
}

static Value nativeRound(int argCount, Value *args) {
  if (argCount < 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;

  double value = AS_NUMBER(args[0]);
  int decimals =
      (argCount > 1 && IS_NUMBER(args[1])) ? (int)AS_NUMBER(args[1]) : 0;

  double multiplier = pow(10.0, decimals);
  return NUMBER_VAL(round(value * multiplier) / multiplier);
}

static Value nativeMax(int argCount, Value *args) {
  if (argCount == 0)
    return NULL_VAL;

  double max = -INFINITY;
  for (int i = 0; i < argCount; i++) {
    if (!IS_NUMBER(args[i]))
      continue;
    double val = AS_NUMBER(args[i]);
    if (val > max)
      max = val;
  }
  return NUMBER_VAL(max);
}

static Value nativeMin(int argCount, Value *args) {
  if (argCount == 0)
    return NULL_VAL;

  double min = INFINITY;
  for (int i = 0; i < argCount; i++) {
    if (!IS_NUMBER(args[i]))
      continue;
    double val = AS_NUMBER(args[i]);
    if (val < min)
      min = val;
  }
  return NUMBER_VAL(min);
}

static Value nativePow(int argCount, Value *args) {
  if (argCount != 2)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1]))
    return NULL_VAL;
  return NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
}

static Value nativeSqrt(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
}

static Value nativeSin(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(sin(AS_NUMBER(args[0])));
}

static Value nativeCos(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(cos(AS_NUMBER(args[0])));
}

static Value nativeTan(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(tan(AS_NUMBER(args[0])));
}

static Value nativeLog(int argCount, Value *args) {
  if (argCount < 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;

  double value = AS_NUMBER(args[0]);
  double base = (argCount > 1 && IS_NUMBER(args[1])) ? AS_NUMBER(args[1]) : M_E;

  return NUMBER_VAL(log(value) / log(base));
}

static Value nativeExp(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]))
    return NULL_VAL;
  return NUMBER_VAL(exp(AS_NUMBER(args[0])));
}

static Value nativeRandom(int argCount, Value *args) {
  return NUMBER_VAL((double)rand() / RAND_MAX);
}

static Value nativeRandInt(int argCount, Value *args) {
  if (argCount != 2)
    return NULL_VAL;
  if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1]))
    return NULL_VAL;

  int min = (int)AS_NUMBER(args[0]);
  int max = (int)AS_NUMBER(args[1]);

  return NUMBER_VAL(min + rand() % (max - min + 1));
}

// === String Functions ===

static Value nativeLen(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;

  if (IS_STRING(args[0])) {
    return NUMBER_VAL(AS_STRING(args[0])->length);
  } else if (IS_OBJ(args[0])) {
    // Could handle lists, dicts, etc.
    return NUMBER_VAL(0);
  }
  return NULL_VAL;
}

static Value nativeUpper(int argCount, Value *args) {
  if (argCount != 1 || !IS_STRING(args[0]))
    return NULL_VAL;

  ObjString *str = AS_STRING(args[0]);
  char *upper = ALLOCATE(char, str->length + 1);

  for (int i = 0; i < str->length; i++) {
    upper[i] = toupper(str->chars[i]);
  }
  upper[str->length] = '\0';

  ObjString *result = takeString(upper, str->length);
  return OBJ_VAL(result);
}

static Value nativeLower(int argCount, Value *args) {
  if (argCount != 1 || !IS_STRING(args[0]))
    return NULL_VAL;

  ObjString *str = AS_STRING(args[0]);
  char *lower = ALLOCATE(char, str->length + 1);

  for (int i = 0; i < str->length; i++) {
    lower[i] = tolower(str->chars[i]);
  }
  lower[str->length] = '\0';

  ObjString *result = takeString(lower, str->length);
  return OBJ_VAL(result);
}

static Value nativeTrim(int argCount, Value *args) {
  if (argCount != 1 || !IS_STRING(args[0]))
    return NULL_VAL;

  ObjString *str = AS_STRING(args[0]);
  const char *start = str->chars;
  const char *end = str->chars + str->length - 1;

  // Trim leading whitespace
  while (*start && isspace(*start))
    start++;

  // Trim trailing whitespace
  while (end > start && isspace(*end))
    end--;

  int length = end - start + 1;
  char *trimmed = ALLOCATE(char, length + 1);
  memcpy(trimmed, start, length);
  trimmed[length] = '\0';

  ObjString *result = takeString(trimmed, length);
  return OBJ_VAL(result);
}

static Value nativeStartsWith(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return BOOL_VAL(false);

  ObjString *str = AS_STRING(args[0]);
  ObjString *prefix = AS_STRING(args[1]);

  if (prefix->length > str->length)
    return BOOL_VAL(false);

  return BOOL_VAL(memcmp(str->chars, prefix->chars, prefix->length) == 0);
}

static Value nativeEndsWith(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return BOOL_VAL(false);

  ObjString *str = AS_STRING(args[0]);
  ObjString *suffix = AS_STRING(args[1]);

  if (suffix->length > str->length)
    return BOOL_VAL(false);

  int offset = str->length - suffix->length;
  return BOOL_VAL(memcmp(str->chars + offset, suffix->chars, suffix->length) ==
                  0);
}

static Value nativeContains(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return BOOL_VAL(false);

  ObjString *str = AS_STRING(args[0]);
  ObjString *substr = AS_STRING(args[1]);

  if (substr->length > str->length)
    return BOOL_VAL(false);

  for (int i = 0; i <= str->length - substr->length; i++) {
    if (memcmp(str->chars + i, substr->chars, substr->length) == 0) {
      return BOOL_VAL(true);
    }
  }

  return BOOL_VAL(false);
}

static Value nativeIndexOf(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return NUMBER_VAL(-1);

  ObjString *str = AS_STRING(args[0]);
  ObjString *substr = AS_STRING(args[1]);

  if (substr->length > str->length)
    return NUMBER_VAL(-1);

  for (int i = 0; i <= str->length - substr->length; i++) {
    if (memcmp(str->chars + i, substr->chars, substr->length) == 0) {
      return NUMBER_VAL(i);
    }
  }

  return NUMBER_VAL(-1);
}

static Value nativeCharAt(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_NUMBER(args[1]))
    return NULL_VAL;

  ObjString *str = AS_STRING(args[0]);
  int index = (int)AS_NUMBER(args[1]);

  if (index < 0 || index >= str->length)
    return NULL_VAL;

  char c[2] = {str->chars[index], '\0'};
  return OBJ_VAL(copyString(c, 1));
}

// === IO Functions ===

static Value nativePrint(int argCount, Value *args) {
  for (int i = 0; i < argCount; i++) {
    printValue(args[i]);
    if (i < argCount - 1)
      printf(" ");
  }
  printf("\n");
  return NULL_VAL;
}

static Value nativeInput(int argCount, Value *args) {
  if (argCount > 0 && IS_STRING(args[0])) {
    printf("%s", AS_CSTRING(args[0]));
  }

  char buffer[1024];
  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Remove newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
      len--;
    }
    return OBJ_VAL(copyString(buffer, len));
  }

  return NULL_VAL;
}

static Value nativeReadFile(int argCount, Value *args) {
  if (argCount != 1 || !IS_STRING(args[0]))
    return NULL_VAL;

  const char *path = AS_CSTRING(args[0]);
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return NULL_VAL;

  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = ALLOCATE(char, fileSize + 1);
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';

  fclose(file);

  ObjString *result = takeString(buffer, bytesRead);
  return OBJ_VAL(result);
}

static Value nativeWriteFile(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return BOOL_VAL(false);

  const char *path = AS_CSTRING(args[0]);
  const char *content = AS_CSTRING(args[1]);

  FILE *file = fopen(path, "w");
  if (file == NULL)
    return BOOL_VAL(false);

  fprintf(file, "%s", content);
  fclose(file);

  return BOOL_VAL(true);
}

static Value nativeAppendFile(int argCount, Value *args) {
  if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1]))
    return BOOL_VAL(false);

  const char *path = AS_CSTRING(args[0]);
  const char *content = AS_CSTRING(args[1]);

  FILE *file = fopen(path, "a");
  if (file == NULL)
    return BOOL_VAL(false);

  fprintf(file, "%s", content);
  fclose(file);

  return BOOL_VAL(true);
}

// === Utility Functions ===

static Value nativeType(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;

  if (IS_BOOL(args[0]))
    return OBJ_VAL(copyString("bool", 4));
  if (IS_NULL(args[0]))
    return OBJ_VAL(copyString("null", 4));
  if (IS_NUMBER(args[0]))
    return OBJ_VAL(copyString("number", 6));
  if (IS_STRING(args[0]))
    return OBJ_VAL(copyString("string", 6));
  if (IS_OBJ(args[0])) {
    switch (OBJ_TYPE(args[0])) {
    case OBJ_FUNCTION:
      return OBJ_VAL(copyString("function", 8));
    case OBJ_NATIVE:
      return OBJ_VAL(copyString("native", 6));
    default:
      return OBJ_VAL(copyString("object", 6));
    }
  }

  return OBJ_VAL(copyString("unknown", 7));
}

static Value nativeClock(int argCount, Value *args) {
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static Value nativeStr(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;

  // Convert value to string representation
  // This is simplified; a real implementation would use valueToString()
  if (IS_NUMBER(args[0])) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.6g", AS_NUMBER(args[0]));
    return OBJ_VAL(copyString(buffer, strlen(buffer)));
  }
  if (IS_BOOL(args[0])) {
    return OBJ_VAL(copyString(AS_BOOL(args[0]) ? "true" : "false",
                              AS_BOOL(args[0]) ? 4 : 5));
  }
  if (IS_NULL(args[0])) {
    return OBJ_VAL(copyString("null", 4));
  }
  if (IS_STRING(args[0])) {
    return args[0];
  }

  return OBJ_VAL(copyString("[object]", 8));
}

static Value nativeInt(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;

  if (IS_NUMBER(args[0])) {
    return NUMBER_VAL((double)(int)AS_NUMBER(args[0]));
  }
  if (IS_STRING(args[0])) {
    return NUMBER_VAL(atoi(AS_CSTRING(args[0])));
  }

  return NULL_VAL;
}

static Value nativeFloat(int argCount, Value *args) {
  if (argCount != 1)
    return NULL_VAL;

  if (IS_NUMBER(args[0])) {
    return args[0];
  }
  if (IS_STRING(args[0])) {
    return NUMBER_VAL(atof(AS_CSTRING(args[0])));
  }

  return NULL_VAL;
}

// === Registration Function ===

void registerStdLib(VM *vm) {
  // Initialize random seed
  srand(time(NULL));

  // Math functions
  defineNative(vm, "abs", nativeAbs);
  defineNative(vm, "ceil", nativeCeil);
  defineNative(vm, "floor", nativeFloor);
  defineNative(vm, "round", nativeRound);
  defineNative(vm, "max", nativeMax);
  defineNative(vm, "min", nativeMin);
  defineNative(vm, "pow", nativePow);
  defineNative(vm, "sqrt", nativeSqrt);
  defineNative(vm, "sin", nativeSin);
  defineNative(vm, "cos", nativeCos);
  defineNative(vm, "tan", nativeTan);
  defineNative(vm, "log", nativeLog);
  defineNative(vm, "exp", nativeExp);
  defineNative(vm, "random", nativeRandom);
  defineNative(vm, "randint", nativeRandInt);

  // String functions
  defineNative(vm, "len", nativeLen);
  defineNative(vm, "upper", nativeUpper);
  defineNative(vm, "lower", nativeLower);
  defineNative(vm, "trim", nativeTrim);
  defineNative(vm, "startswith", nativeStartsWith);
  defineNative(vm, "endswith", nativeEndsWith);
  defineNative(vm, "contains", nativeContains);
  defineNative(vm, "index_of", nativeIndexOf);
  defineNative(vm, "char_at", nativeCharAt);

  // IO functions
  defineNative(vm, "print", nativePrint);
  defineNative(vm, "input", nativeInput);
  defineNative(vm, "read_file", nativeReadFile);
  defineNative(vm, "write_file", nativeWriteFile);
  defineNative(vm, "append_file", nativeAppendFile);

  // Utility functions
  defineNative(vm, "type", nativeType);
  defineNative(vm, "clock", nativeClock);
  defineNative(vm, "str", nativeStr);
  defineNative(vm, "int", nativeInt);
  defineNative(vm, "float", nativeFloat);
}
