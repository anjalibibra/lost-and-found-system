#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define DEFAULT_CAPACITY 11
#define MAX_DISPLACEMENTS 32

typedef struct {
    char *type;
    char *category;
    char *location;
    char *description;
    char *contact;
    long timestamp;
} Item;

typedef struct {
    char *key;
    Item *value;
} CuckooEntry;

typedef struct {
    CuckooEntry *table1;
    CuckooEntry *table2;
    size_t capacity;
    size_t size;
} CuckooHashTable;

static Item *create_item(const char *type,
                         const char *category,
                         const char *location,
                         const char *description,
                         const char *contact);
static void destroy_item(Item *item);
static CuckooHashTable *create_cuckoo_table(size_t capacity);
static void destroy_cuckoo_table(CuckooHashTable *ht);
static unsigned long hash_fn_1(const char *key, size_t capacity);
static unsigned long hash_fn_2(const char *key, size_t capacity);
static bool cuckoo_insert_internal(CuckooHashTable *ht,
                                   char *key,
                                   Item *value,
                                   bool allow_rehash);
static bool cuckoo_insert(CuckooHashTable *ht, const char *category, Item *value);
static Item *cuckoo_search(CuckooHashTable *ht, const char *category);
static void print_item(const Item *item);
static void display_search_result(CuckooHashTable *ht, const char *category);
static void print_cuckoo_tables(CuckooHashTable *ht);
static bool rehash(CuckooHashTable *ht);
static size_t next_prime(size_t n);
static bool is_prime(size_t n);
static void demo_cuckoo_hashing(void);

static Item *create_item(const char *type,
                         const char *category,
                         const char *location,
                         const char *description,
                         const char *contact) {
    Item *item = (Item *)malloc(sizeof(Item));
    if (!item) {
        fprintf(stderr, "Failed to allocate memory for item\n");
        return NULL;
    }

    item->type = strdup(type ? type : "");
    item->category = strdup(category ? category : "");
    item->location = strdup(location ? location : "");
    item->description = strdup(description ? description : "");
    item->contact = strdup(contact ? contact : "");
    item->timestamp = time(NULL);

    if (!item->type || !item->category || !item->location ||
        !item->description || !item->contact) {
        destroy_item(item);
        fprintf(stderr, "Failed to duplicate strings for item\n");
        return NULL;
    }

    return item;
}

static void destroy_item(Item *item) {
    if (!item) {
        return;
    }

    free(item->type);
    free(item->category);
    free(item->location);
    free(item->description);
    free(item->contact);
    free(item);
}

static CuckooHashTable *create_cuckoo_table(size_t capacity) {
    CuckooHashTable *ht = (CuckooHashTable *)malloc(sizeof(CuckooHashTable));
    if (!ht) {
        fprintf(stderr, "Failed to allocate memory for hash table\n");
        return NULL;
    }

    ht->capacity = capacity;
    ht->size = 0;
    ht->table1 = (CuckooEntry *)calloc(capacity, sizeof(CuckooEntry));
    ht->table2 = (CuckooEntry *)calloc(capacity, sizeof(CuckooEntry));

    if (!ht->table1 || !ht->table2) {
        free(ht->table1);
        free(ht->table2);
        free(ht);
        fprintf(stderr, "Failed to allocate memory for tables\n");
        return NULL;
    }

    return ht;
}

static void destroy_cuckoo_table(CuckooHashTable *ht) {
    if (!ht) {
        return;
    }

    for (size_t i = 0; i < ht->capacity; ++i) {
        if (ht->table1[i].key) {
            free(ht->table1[i].key);
            destroy_item(ht->table1[i].value);
        }
        if (ht->table2[i].key) {
            free(ht->table2[i].key);
            destroy_item(ht->table2[i].value);
        }
    }

    free(ht->table1);
    free(ht->table2);
    free(ht);
}

static unsigned long hash_fn_1(const char *key, size_t capacity) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + (unsigned long)c;
    }

    return hash % capacity;
}

static unsigned long hash_fn_2(const char *key, size_t capacity) {
    unsigned long hash = 0;
    int c;

    while ((c = *key++)) {
        hash = (hash << 7) ^ (hash >> 3) ^ (unsigned long)c;
    }

    return hash % capacity;
}

static bool cuckoo_insert_internal(CuckooHashTable *ht,
                                   char *key,
                                   Item *value,
                                   bool allow_rehash) {
    char *current_key = key;
    Item *current_item = value;

    for (size_t displacement = 0; displacement < MAX_DISPLACEMENTS; ++displacement) {
        CuckooEntry *table = (displacement % 2 == 0) ? ht->table1 : ht->table2;
        unsigned long index = (displacement % 2 == 0)
                                  ? hash_fn_1(current_key, ht->capacity)
                                  : hash_fn_2(current_key, ht->capacity);
        CuckooEntry *entry = &table[index];

        if (!entry->key) {
            entry->key = current_key;
            entry->value = current_item;
            ht->size++;
            return true;
        }

        if (strcmp(entry->key, current_key) == 0) {
            destroy_item(entry->value);
            entry->value = current_item;
            if (entry->key != current_key) {
                free(current_key);
            }
            return true;
        }

        char *evicted_key = entry->key;
        Item *evicted_item = entry->value;
        entry->key = current_key;
        entry->value = current_item;

        current_key = evicted_key;
        current_item = evicted_item;
    }

    if (!allow_rehash) {
        return false;
    }

    if (!rehash(ht)) {
        fprintf(stderr, "Failed to rehash cuckoo table\n");
        return false;
    }

    return cuckoo_insert_internal(ht, current_key, current_item, true);
}

static bool cuckoo_insert(CuckooHashTable *ht, const char *category, Item *value) {
    if (!ht || !category || !value) {
        return false;
    }

    char *key_copy = strdup(category);
    if (!key_copy) {
        fprintf(stderr, "Failed to duplicate key string\n");
        destroy_item(value);
        return false;
    }

    if (!cuckoo_insert_internal(ht, key_copy, value, true)) {
        destroy_item(value);
        return false;
    }

    return true;
}

static Item *cuckoo_search(CuckooHashTable *ht, const char *category) {
    if (!ht || !category) {
        return NULL;
    }

    unsigned long index1 = hash_fn_1(category, ht->capacity);
    if (ht->table1[index1].key && strcmp(ht->table1[index1].key, category) == 0) {
        return ht->table1[index1].value;
    }

    unsigned long index2 = hash_fn_2(category, ht->capacity);
    if (ht->table2[index2].key && strcmp(ht->table2[index2].key, category) == 0) {
        return ht->table2[index2].value;
    }

    return NULL;
}

static void print_item(const Item *item) {
    if (!item) {
        return;
    }

    printf("Type: %s\n", item->type);
    printf("Category: %s\n", item->category);
    printf("Location: %s\n", item->location);
    printf("Description: %s\n", item->description);
    printf("Contact: %s\n", item->contact);
    printf("Timestamp: %ld\n", item->timestamp);
}

static void display_search_result(CuckooHashTable *ht, const char *category) {
    printf("\nSearching for category: %s\n", category);
    Item *found = cuckoo_search(ht, category);

    if (found) {
        printf("Item found:\n");
        print_item(found);
    } else {
        printf("No item found for category: %s\n", category);
    }
}

static void print_cuckoo_tables(CuckooHashTable *ht) {
    if (!ht) {
        return;
    }

    printf("\n=== Cuckoo Hash Table State ===\n");
    printf("Capacity per table: %zu\n", ht->capacity);
    printf("Stored entries: %zu\n\n", ht->size);

    printf("Table 1:\n");
    for (size_t i = 0; i < ht->capacity; ++i) {
        if (ht->table1[i].key) {
            printf("  [%zu] %s -> %s\n", i, ht->table1[i].key, ht->table1[i].value->description);
        }
    }

    printf("\nTable 2:\n");
    for (size_t i = 0; i < ht->capacity; ++i) {
        if (ht->table2[i].key) {
            printf("  [%zu] %s -> %s\n", i, ht->table2[i].key, ht->table2[i].value->description);
        }
    }
    printf("\n");
}

static bool rehash(CuckooHashTable *ht) {
    size_t new_capacity = next_prime(ht->capacity * 2);
    CuckooEntry *old_table1 = ht->table1;
    CuckooEntry *old_table2 = ht->table2;
    size_t old_capacity = ht->capacity;

    CuckooEntry *new_table1 = (CuckooEntry *)calloc(new_capacity, sizeof(CuckooEntry));
    CuckooEntry *new_table2 = (CuckooEntry *)calloc(new_capacity, sizeof(CuckooEntry));

    if (!new_table1 || !new_table2) {
        free(new_table1);
        free(new_table2);
        return false;
    }

    ht->table1 = new_table1;
    ht->table2 = new_table2;
    ht->capacity = new_capacity;
    ht->size = 0;

    for (size_t i = 0; i < old_capacity; ++i) {
        if (old_table1[i].key) {
            if (!cuckoo_insert_internal(ht, old_table1[i].key, old_table1[i].value, false)) {
                free(old_table1);
                free(old_table2);
                return false;
            }
        }
        if (old_table2[i].key) {
            if (!cuckoo_insert_internal(ht, old_table2[i].key, old_table2[i].value, false)) {
                free(old_table1);
                free(old_table2);
                return false;
            }
        }
    }

    free(old_table1);
    free(old_table2);
    return true;
}

static size_t next_prime(size_t n) {
    if (n <= 2) {
        return 2;
    }
    if (n % 2 == 0) {
        n++;
    }
    while (!is_prime(n)) {
        n += 2;
    }
    return n;
}

static bool is_prime(size_t n) {
    if (n < 2) {
        return false;
    }
    if (n % 2 == 0) {
        return n == 2;
    }
    for (size_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

static void demo_cuckoo_hashing(void) {
    printf("=== Cuckoo Hashing Demo for Lost and Found Items ===\n");

    CuckooHashTable *ht = create_cuckoo_table(DEFAULT_CAPACITY);
    if (!ht) {
        return;
    }

    cuckoo_insert(ht, "phone", create_item("lost", "phone", "Library",
                                           "iPhone 13 with transparent case", "contact@john.com"));
    cuckoo_insert(ht, "wallet", create_item("found", "wallet", "Cafeteria",
                                            "Brown leather wallet with ID", "jane@email.com"));
    cuckoo_insert(ht, "keys", create_item("lost", "keys", "Gym",
                                          "Car keys with red keychain", "keys-owner@email.com"));
    cuckoo_insert(ht, "bag", create_item("found", "bag", "Parking Lot",
                                         "Blue backpack with laptop stickers", "security@campus.edu"));
    cuckoo_insert(ht, "watch", create_item("lost", "watch", "Auditorium",
                                           "Silver analog watch", "watch-owner@email.com"));
    cuckoo_insert(ht, "laptop", create_item("found", "laptop", "Computer Lab",
                                            "Black laptop with university logo", "lab@campus.edu"));

    print_cuckoo_tables(ht);

    const char *queries[] = {"phone", "wallet", "keys", "laptop", "camera"};
    size_t query_count = sizeof(queries) / sizeof(queries[0]);

    for (size_t i = 0; i < query_count; ++i) {
        display_search_result(ht, queries[i]);
    }

    destroy_cuckoo_table(ht);
    printf("\nDemo complete. All resources released.\n");
}

int main(void) {
    demo_cuckoo_hashing();
    return 0;
}


