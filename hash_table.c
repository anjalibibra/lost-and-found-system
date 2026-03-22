/*
 * Hash Table Implementation for Lost and Found Item Lookup by Category
 * This demonstrates the DSA (Data Structures and Algorithms) part of the project
 * 
 * Features:
 * - Dynamic hash table with chaining for collision resolution
 * - String-based keys (categories)
 * - Generic value storage (item pointers)
 * - Automatic resizing when load factor exceeds threshold
 * - Memory management with proper cleanup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Constants
#define INITIAL_CAPACITY 16
#define MAX_LOAD_FACTOR 0.75
#define GROWTH_FACTOR 2

// Item structure representing a lost/found item
typedef struct {
    char* type;        // "lost" or "found"
    char* category;    // Item category
    char* location;    // Where item was lost/found
    char* description; // Item description
    char* contact;     // Contact information
    long timestamp;    // Unix timestamp
} Item;

// Hash table node
typedef struct HashNode {
    char* key;           // Category name
    Item* value;         // Item data
    struct HashNode* next; // Next node in chain
} HashNode;

// Hash table structure
typedef struct {
    HashNode** buckets;  // Array of bucket pointers
    size_t capacity;     // Current capacity
    size_t size;         // Number of items stored
} HashTable;

// Function prototypes
HashTable* create_hash_table();
void destroy_hash_table(HashTable* ht);
unsigned long hash_function(const char* key, size_t capacity);
bool insert_item(HashTable* ht, const char* category, Item* item);
Item* find_item(HashTable* ht, const char* category);
bool delete_item(HashTable* ht, const char* category);
void resize_table(HashTable* ht);
void print_table(HashTable* ht);
void print_items_by_category(HashTable* ht, const char* category);
Item* create_item(const char* type, const char* category, const char* location, 
                  const char* description, const char* contact);
void destroy_item(Item* item);

// Create a new hash table
HashTable* create_hash_table() {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    ht->capacity = INITIAL_CAPACITY;
    ht->size = 0;
    ht->buckets = (HashNode**)calloc(ht->capacity, sizeof(HashNode*));
    
    if (!ht->buckets) {
        free(ht);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    return ht;
}

// Destroy hash table and free all memory
void destroy_hash_table(HashTable* ht) {
    if (!ht) return;
    
    for (size_t i = 0; i < ht->capacity; i++) {
        HashNode* current = ht->buckets[i];
        while (current) {
            HashNode* next = current->next;
            free(current->key);
            destroy_item(current->value);
            free(current);
            current = next;
        }
    }
    
    free(ht->buckets);
    free(ht);
}

// Hash function using djb2 algorithm
unsigned long hash_function(const char* key, size_t capacity) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % capacity;
}

// Insert an item into the hash table
bool insert_item(HashTable* ht, const char* category, Item* item) {
    if (!ht || !category || !item) return false;
    
    // Check if we need to resize
    if ((double)ht->size / ht->capacity >= MAX_LOAD_FACTOR) {
        resize_table(ht);
    }
    
    unsigned long index = hash_function(category, ht->capacity);
    HashNode* current = ht->buckets[index];
    
    // Check if key already exists
    while (current) {
        if (strcmp(current->key, category) == 0) {
            // Update existing item
            destroy_item(current->value);
            current->value = item;
            return true;
        }
        current = current->next;
    }
    
    // Create new node
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    
    new_node->key = strdup(category);
    new_node->value = item;
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->size++;
    
    return true;
}

// Find an item by category
Item* find_item(HashTable* ht, const char* category) {
    if (!ht || !category) return NULL;
    
    unsigned long index = hash_function(category, ht->capacity);
    HashNode* current = ht->buckets[index];
    
    while (current) {
        if (strcmp(current->key, category) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;
}

// Delete an item from the hash table
bool delete_item(HashTable* ht, const char* category) {
    if (!ht || !category) return false;
    
    unsigned long index = hash_function(category, ht->capacity);
    HashNode* current = ht->buckets[index];
    HashNode* prev = NULL;
    
    while (current) {
        if (strcmp(current->key, category) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                ht->buckets[index] = current->next;
            }
            
            free(current->key);
            destroy_item(current->value);
            free(current);
            ht->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

// Resize the hash table when load factor is exceeded
void resize_table(HashTable* ht) {
    size_t old_capacity = ht->capacity;
    HashNode** old_buckets = ht->buckets;
    
    ht->capacity *= GROWTH_FACTOR;
    ht->buckets = (HashNode**)calloc(ht->capacity, sizeof(HashNode*));
    
    if (!ht->buckets) {
        fprintf(stderr, "Memory allocation failed during resize\n");
        return;
    }
    
    // Rehash all existing items
    for (size_t i = 0; i < old_capacity; i++) {
        HashNode* current = old_buckets[i];
        while (current) {
            HashNode* next = current->next;
            unsigned long new_index = hash_function(current->key, ht->capacity);
            
            current->next = ht->buckets[new_index];
            ht->buckets[new_index] = current;
            
            current = next;
        }
    }
    
    free(old_buckets);
}

// Print all items in the hash table
void print_table(HashTable* ht) {
    if (!ht) return;
    
    printf("\n=== Hash Table Contents ===\n");
    printf("Capacity: %zu, Size: %zu\n\n", ht->capacity, ht->size);
    
    for (size_t i = 0; i < ht->capacity; i++) {
        HashNode* current = ht->buckets[i];
        if (current) {
            printf("Bucket %zu: ", i);
            while (current) {
                printf("[%s] -> ", current->key);
                current = current->next;
            }
            printf("NULL\n");
        }
    }
    printf("\n");
}

// Print all items of a specific category
void print_items_by_category(HashTable* ht, const char* category) {
    if (!ht || !category) return;
    
    printf("\n=== Items in category: %s ===\n", category);
    
    unsigned long index = hash_function(category, ht->capacity);
    HashNode* current = ht->buckets[index];
    bool found = false;
    
    while (current) {
        if (strcmp(current->key, category) == 0) {
            Item* item = current->value;
            printf("Type: %s\n", item->type);
            printf("Category: %s\n", item->category);
            printf("Location: %s\n", item->location);
            printf("Description: %s\n", item->description);
            printf("Contact: %s\n", item->contact);
            printf("Timestamp: %ld\n", item->timestamp);
            printf("---\n");
            found = true;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("No items found in category: %s\n", category);
    }
}

// Create a new item
Item* create_item(const char* type, const char* category, const char* location, 
                  const char* description, const char* contact) {
    Item* item = (Item*)malloc(sizeof(Item));
    if (!item) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    item->type = strdup(type);
    item->category = strdup(category);
    item->location = strdup(location);
    item->description = strdup(description ? description : "");
    item->contact = strdup(contact ? contact : "");
    item->timestamp = time(NULL);
    
    return item;
}

// Destroy an item and free its memory
void destroy_item(Item* item) {
    if (!item) return;
    
    free(item->type);
    free(item->category);
    free(item->location);
    free(item->description);
    free(item->contact);
    free(item);
}

// Demo function to test the hash table
void demo_hash_table() {
    printf("=== Lost and Found Hash Table Demo ===\n\n");
    
    // Create hash table
    HashTable* ht = create_hash_table();
    if (!ht) {
        printf("Failed to create hash table\n");
        return;
    }
    
    // Add some sample items
    Item* item1 = create_item("lost", "phone", "Library", "iPhone 12, black case", "john@email.com");
    Item* item2 = create_item("found", "phone", "Cafeteria", "Samsung Galaxy, blue case", "jane@email.com");
    Item* item3 = create_item("lost", "wallet", "Parking Lot", "Brown leather wallet", "bob@email.com");
    Item* item4 = create_item("found", "keys", "Gym", "Set of 3 keys on keychain", "alice@email.com");
    Item* item5 = create_item("lost", "bag", "Library", "Red backpack with laptop", "charlie@email.com");
    
    // Insert items
    insert_item(ht, "phone", item1);
    insert_item(ht, "phone", item2);  // This will update the existing phone entry
    insert_item(ht, "wallet", item3);
    insert_item(ht, "keys", item4);
    insert_item(ht, "bag", item5);
    
    // Print table structure
    print_table(ht);
    
    // Search for items by category
    print_items_by_category(ht, "phone");
    print_items_by_category(ht, "wallet");
    print_items_by_category(ht, "keys");
    print_items_by_category(ht, "nonexistent");
    
    // Test finding items
    Item* found = find_item(ht, "wallet");
    if (found) {
        printf("\nFound wallet item: %s at %s\n", found->description, found->location);
    }
    
    // Test deletion
    printf("\nDeleting wallet item...\n");
    delete_item(ht, "wallet");
    print_table(ht);
    
    // Clean up
    destroy_hash_table(ht);
    printf("Hash table destroyed. Demo completed.\n");
}

// Main function
int main() {
    demo_hash_table();
    return 0;
}
