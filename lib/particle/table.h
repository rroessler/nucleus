#ifndef NUC_TABLE_H
#define NUC_TABLE_H

// Nucleus Headers
#include "../common.h"
#include "../memory.h"
#include "object.h"
#include "value.h"

// set a max load for tables for growth
#define TABLE_MAX_LOAD 0.75

/** Table Entries */
typedef struct {
    ObjString* key;
    Particle value;
} Entry;

/** Hash Table */
typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

/**
 * Initialises a Hash Table.
 * @param table             Hash table to initialise.
 */
void table_init(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

/**
 * Frees a hash table from memory.
 * @param table             Hash table to free.
 */
void table_free(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    table_init(table);
}

/**
 * Finds a valid table entry, whether complete or not.
 * @param entries           Entries to search for key.
 * @param capacity          Size of entries.
 * @param key               Key to search for.
 */
static Entry* table_findEntry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash & (capacity - 1);
    Entry* tombstone = NULL;  // tombstone entry

    for (;;) {
        Entry* entry = &entries[index];

        // found a tombstone
        if (entry->key == NULL) {
            // found an empty entry
            if (IS_NULL(entry->value)) return tombstone != NULL ? tombstone : entry;
            if (tombstone == NULL) tombstone = entry;  // save the tombstone as valid entry to save to
        } else if (entry->key == key) {
            return entry;  // found entry
        }

        // continue iterating after incrementing the index
        index = (index + 1) & (capacity - 1);
    }
}

/**
 * Adjusts the capacity of a given table.
 * @param table                 Table to adjust capacity of.
 * @param capacity              Capacity to adjust to.
 */
static void table_adjustCapacity(Table* table, int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);  // allocate the size to memory
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NUC_NULL;  // default to null value
    }

    // set the table count to zero, so we can fix up tombstones
    table->count = 0;

    // and push the entries back onto the table
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) continue;

        Entry* dest = table_findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;  // to fix up tombstones messing up count
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

/**
 * Deletes an entry from a table if found.
 * @param table             Table to delete entry of.
 * @param key               Key of entry.
 */
bool table_delete(Table* table, ObjString* key) {
    if (table->count == 0) return false;  // no table to delete an item from

    // find an entry
    Entry* entry = table_findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    // place a tombstone on the entry
    entry->key = NULL;
    entry->value = NUC_BOOL(true);
    return true;
}

/**
 * Sets an entry to the table.
 * @param table             Table to set entry to.
 * @param key               Key of entry.
 * @param value             Value to set to entry.
 */
bool table_set(Table* table, ObjString* key, Particle value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        table_adjustCapacity(table, capacity);
    }

    Entry* entry = table_findEntry(table->entries, table->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey && IS_NULL(entry->value)) table->count++;

    // and set the entry values
    entry->key = key;
    entry->value = value;
    return isNewKey;
}

/**
 * Gets a property from a table.
 * @param table             Table to access value from.
 * @param key               Accessor key.
 * @param value             Pointer to store result to.
 */
bool table_get(Table* table, ObjString* key, Particle* value) {
    if (table->count == 0) return false;  // no values

    // search for the entry
    Entry* entry = table_findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;  // no match

    // and save the value
    *value = entry->value;
    return true;
}

/**
 * Finds a string from hash table strings.
 * @param table             Table to search keys of.
 * @param chars             Key to match.
 * @param length            Length of chars.
 * @param hash              Iteration hash.
 */
ObjString* table_findString(Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) return NULL;  // return early if empty

    // and iterate over the entries
    uint32_t index = hash & (table->capacity - 1);
    for (;;) {
        Entry* entry = &table->entries[index];

        // stop if we find an empty non-tombstone entry
        if (entry->key == NULL && IS_NULL(entry->value)) {
            return NULL;
        }

        // otherwise check if strings match
        if (entry->key->length == length &&
            entry->key->hash == hash &&
            memcmp(entry->key->chars, chars, length) == 0) return entry->key;

        // continue iterating
        index = (index + 1) & (table->capacity - 1);
    }
}

/**
 * Adds all values from one table to another.
 * @param from              Table from.
 * @param to                Table to.
 */
void table_addAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key != NULL) table_set(to, entry->key, entry->value);
    }
}

#endif