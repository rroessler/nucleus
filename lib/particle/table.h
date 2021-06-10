#ifndef NUC_HASH_TABLE_H
#define NUC_HASH_TABLE_H

// Nucleus Headers
#include "../common.h"
#include "../utils/memory.h"
#include "objects/type.h"

/***********************
 *  TABLE DEFINITIONS  *
 ***********************/

// set a max load scale for tables growth
#define TABLE_MAX_LOAD 0.75

/** Hash Table Entries */
typedef struct {
    nuc_ObjString* key;
    nuc_Particle value;
} nuc_Entry;

/** Hash Table Structure */
typedef struct {
    int count;
    int capacity;
    nuc_Entry* entries;
} nuc_Table;

/*******************
 *  TABLE METHODS  *
 *******************/

/**
 * Initialises a Hash Table.
 * @param table             Hash table to initialise.
 */
void table_init(nuc_Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

/**
 * Frees a hash table from memory.
 * @param table             Hash table to free.
 */
void table_free(nuc_Table* table) {
    NUC_FREE_ARR(nuc_Entry, table->entries, table->capacity);
    table_init(table);
}

/**
 * Finds a valid table entry, whether complete or not.
 * @param entries           Entries to search for key.
 * @param capacity          Size of entries.
 * @param key               Key to search for.
 */
static nuc_Entry* table_findEntry(nuc_Entry* entries, int capacity, nuc_ObjString* key) {
    uint32_t index = key->hash & (capacity - 1);
    nuc_Entry* tombstone = NULL;  // tombstone entry

    for (;;) {
        nuc_Entry* entry = &entries[index];

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
static void table_adjustCapacity(nuc_Table* table, int capacity) {
    nuc_Entry* entries = NUC_ALLOC(nuc_Entry, capacity);  // allocate the size to memory
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NUC_NULL;  // default to null value
    }

    // set the table count to zero, so we can fix up tombstones
    table->count = 0;

    // and push the entries back onto the table
    for (int i = 0; i < table->capacity; i++) {
        nuc_Entry* entry = &table->entries[i];
        if (entry->key == NULL) continue;

        nuc_Entry* dest = table_findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;  // to fix up tombstones messing up count
    }

    NUC_FREE_ARR(nuc_Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

/**
 * Deletes an entry from a table if found.
 * @param table             Table to delete entry of.
 * @param key               Key of entry.
 */
bool table_delete(nuc_Table* table, nuc_ObjString* key) {
    if (table->count == 0) return false;  // no table to delete an item from

    // find an entry
    nuc_Entry* entry = table_findEntry(table->entries, table->capacity, key);
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
bool table_set(nuc_Table* table, nuc_ObjString* key, nuc_Particle value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = NUC_CAP_GROW_FAST(table->capacity);
        table_adjustCapacity(table, capacity);
    }

    nuc_Entry* entry = table_findEntry(table->entries, table->capacity, key);
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
bool table_get(nuc_Table* table, nuc_ObjString* key, nuc_Particle* value) {
    if (table->count == 0) return false;  // no values

    // search for the entry
    nuc_Entry* entry = table_findEntry(table->entries, table->capacity, key);
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
nuc_ObjString* table_findString(nuc_Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) return NULL;  // return early if empty

    // and iterate over the entries
    uint32_t index = hash & (table->capacity - 1);
    for (;;) {
        nuc_Entry* entry = &table->entries[index];

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
void table_addAll(nuc_Table* from, nuc_Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        nuc_Entry* entry = &from->entries[i];
        if (entry->key != NULL) table_set(to, entry->key, entry->value);
    }
}

#endif