<<<<<<< HEAD
/* include/table.h */
#ifndef PROXPL_TABLE_H
#define PROXPL_TABLE_H
=======
#ifndef PROX_TABLE_H
#define PROX_TABLE_H
>>>>>>> feature/opcode-tests

#include "common.h"
#include "value.h"

typedef struct {
<<<<<<< HEAD
  ObjString* key;
=======
  ObjString *key;
>>>>>>> feature/opcode-tests
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
<<<<<<< HEAD
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, ObjString* key, Value* value);
bool tableSet(Table* table, ObjString* key, Value value);
bool tableDelete(Table* table, ObjString* key);
void tableAddAll(Table* from, Table* to);
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);

#endif

=======
  Entry *entries;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableSet(Table *table, ObjString *key, Value value);
bool tableDelete(Table *table, ObjString *key);
void tableAddAll(Table *from, Table *to);
ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash);

#endif
>>>>>>> feature/opcode-tests
