// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdlib.h>
#include <string.h>

#include "../../include/memory.h"
#include "../../include/object.h"
#include "../../include/table.h"
#include "../../include/value.h"
#include "../../include/gc.h"

#define TABLE_MAX_LOAD 0.75
#define CTRL_EMPTY 0
#define CTRL_TOMBSTONE 1
#define H2(hash) ((uint8_t)(((hash) >> 25) | 0x80))

void initTable(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
  table->ctrl = NULL;
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  FREE_ARRAY(uint8_t, table->ctrl, table->capacity);
  initTable(table);
}

static Entry *findEntry(Entry *entries, uint8_t *ctrl, int capacity, ObjString *key) {
  uint32_t index = key->hash & (capacity - 1);
  Entry *tombstone = NULL;
  uint8_t hashCtrl = H2(key->hash);

  for (;;) {
    if (ctrl[index] == CTRL_EMPTY) {
      return tombstone != NULL ? tombstone : &entries[index];
    } else if (ctrl[index] == CTRL_TOMBSTONE) {
      if (tombstone == NULL) tombstone = &entries[index];
    } else if (ctrl[index] == hashCtrl) {
      if (entries[index].key == key) {
        return &entries[index];
      }
    }

    index = (index + 1) & (capacity - 1);
  }
}

static void adjustCapacity(Table *table, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  uint8_t *ctrl = ALLOCATE(uint8_t, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NULL_VAL;
    ctrl[i] = CTRL_EMPTY;
  }

  table->count = 0; // Don't count tombstones anymore
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL) continue;

    Entry *dest = findEntry(entries, ctrl, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    ctrl[dest - entries] = H2(entry->key->hash);
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  FREE_ARRAY(uint8_t, table->ctrl, table->capacity);
  table->entries = entries;
  table->ctrl = ctrl;
  table->capacity = capacity;
}

bool tableSet(Table *table, ObjString *key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry *entry = findEntry(table->entries, table->ctrl, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NULL(entry->value)) table->count++;

  entry->key = key;
  entry->value = value;
  table->ctrl[entry - table->entries] = H2(key->hash);
  return isNewKey;
}

bool tableGet(Table *table, ObjString *key, Value *value) {
  if (table->count == 0) return false;

  Entry *entry = findEntry(table->entries, table->ctrl, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

Entry* tableGetEntry(Table* table, ObjString* key) {
    if (table->capacity == 0) return NULL;
    Entry* entry = findEntry(table->entries, table->ctrl, table->capacity, key);
    return entry->key == NULL ? NULL : entry;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry *entry = findEntry(table->entries, table->ctrl, table->capacity, key);
  if (entry->key == NULL) return false;

  // Place a tombstone in the entry.
  entry->key = NULL;
  entry->value = BOOL_VAL(true); // Tombstone
  table->ctrl[entry - table->entries] = CTRL_TOMBSTONE;
  return true;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

ObjString *tableFindString(Table *table, const char *chars, int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash & (table->capacity - 1);
  uint8_t hashCtrl = H2(hash);

  for (;;) {
    if (table->ctrl[index] == CTRL_EMPTY) {
      return NULL;
    } else if (table->ctrl[index] == hashCtrl) {
      Entry *entry = &table->entries[index];
      if (entry->key->length == length && entry->key->hash == hash &&
          memcmp(entry->key->chars, chars, length) == 0) {
        return entry->key;
      }
    }
    index = (index + 1) & (table->capacity - 1);
  }
}

void markTable(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    markObject((Obj *)entry->key);
    markValue(entry->value);
  }
}

void tableRemoveWhite(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key != NULL && !entry->key->obj.isMarked) {
      tableDelete(table, entry->key);
    }
  }
}
