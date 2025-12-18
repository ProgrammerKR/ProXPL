<<<<<<< HEAD
/* src/runtime/table.c */
#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"
#include "../include/object.h"
#include "../include/table.h"
#include "../include/value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
=======
#include <stdlib.h>
#include <string.h>

#include "../../include/memory.h"
#include "../../include/object.h"
#include "../../include/table.h"
#include "../../include/value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
>>>>>>> feature/opcode-tests
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

<<<<<<< HEAD
void freeTable(Table* table) {
=======
void freeTable(Table *table) {
>>>>>>> feature/opcode-tests
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

<<<<<<< HEAD
static Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
  uint32_t index = key->hash % capacity;
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        return tombstone != NULL ? tombstone : entry;
      } else {
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
=======
static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash % capacity;
  Entry *tombstone = NULL;

  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NULL(entry->value)) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      // We found the key.
>>>>>>> feature/opcode-tests
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

<<<<<<< HEAD
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
=======
static void adjustCapacity(Table *table, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NULL_VAL;
  }

  table->count = 0; // Don't count tombstones anymore
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL) continue;

    Entry *dest = findEntry(entries, capacity, entry->key);
>>>>>>> feature/opcode-tests
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

<<<<<<< HEAD
bool tableGet(Table* table, ObjString* key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

bool tableSet(Table* table, ObjString* key, Value value) {
=======
bool tableSet(Table *table, ObjString *key, Value value) {
>>>>>>> feature/opcode-tests
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

<<<<<<< HEAD
  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) table->count++;
=======
  Entry *entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NULL(entry->value)) table->count++;
>>>>>>> feature/opcode-tests

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

<<<<<<< HEAD
bool tableDelete(Table* table, ObjString* key) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
=======
bool tableGet(Table *table, ObjString *key, Value *value) {
  if (table->count == 0) return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  // Place a tombstone in the entry.
  entry->key = NULL;
  entry->value = BOOL_VAL(true); // Tombstone
  return true;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
>>>>>>> feature/opcode-tests
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

<<<<<<< HEAD
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
=======
ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash) {
>>>>>>> feature/opcode-tests
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;
  for (;;) {
<<<<<<< HEAD
    Entry* entry = &table->entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) return NULL;
    } else if (entry->key->length == length &&
               entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
=======
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      // Stop if we find an empty non-tombstone entry.
      if (IS_NULL(entry->value)) return NULL;
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      // We found it.
>>>>>>> feature/opcode-tests
      return entry->key;
    }

    index = (index + 1) % table->capacity;
  }
}
<<<<<<< HEAD

=======
>>>>>>> feature/opcode-tests
