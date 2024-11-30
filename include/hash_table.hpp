#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>

#define INITIAL_TABLE_SIZE 64

#include "hash_slot.hpp"
#include "hash_funcs.hpp"

template <typename K, typename V>
class HashTable {
public:
    HashTable(HashFunc *hash_func);
    ~HashTable();
    int get(const K &key, V &value);
    int put(const K &key, const V &value);
    int remove(const K &key);
    size_t get_table_size();
    size_t get_size();
    double get_load_factor();

protected:
    size_t table_size;
    
private:
    HashFunc *hash_func;
    size_t size;
    HashSlot<K, V> *table;

    // Should be overriden by the derived class
    virtual unsigned long get_next_pos(unsigned long pos,
                                       unsigned long step) = 0;
    unsigned long get_pos(const K key);
    void enlarge_table();
};

template <typename K, typename V>
HashTable<K, V>::HashTable(HashFunc *hash_func): table(), hash_func(hash_func),
                                                 size(0), table_size(INITIAL_TABLE_SIZE) {
    table = new HashSlot<K, V>[table_size];
}

template <typename K, typename V>
HashTable<K, V>::~HashTable() {
    delete[] table;
}

template <typename K, typename V>
void HashTable<K, V>::enlarge_table() {
    // TODO
    HashSlot<K, V> *old_table = this->table;
    size_t old_table_size = this->table_size;

    this->table_size = 2 * this->table_size;
    this->table = new HashSlot<K, V>[this->table_size];
    this->size = 0;

    for(int i = 0; i < old_table_size; i++) {
        if(old_table[i].is_empty() == false && old_table[i].is_removed() == false) {
            this->put(old_table[i].get_key(), old_table[i].get_value());
        }
    }
}

template <typename K, typename V>
unsigned long HashTable<K, V>::get_pos(const K key) {
    // TODO
    return this->hash_func->hash(key) % this->table_size;
}

template <typename K, typename V>
int HashTable<K, V>::get(const K &key, V &value) {
    // TODO
    unsigned int pos = this->get_pos(key);
    unsigned int next_pos = pos;
    int i = 0;
    while(this->table[next_pos].is_empty() == false) {
        if(this->table[next_pos].is_removed() == false) {
            if(this->table[next_pos].get_key() == key) {
                value = this->table[next_pos].get_value();
                return i + 1;
            }
        }
        i++;
        next_pos = this->get_next_pos(pos, i);
    }
    return -1;
}

template <typename K, typename V>
int HashTable<K, V>::put(const K &key, const V &value) {
    // TODO
    unsigned int pos = this->get_pos(key);
    unsigned int next_pos = pos;
    int i = 0;
    while(this->table[next_pos].is_empty() == false) {
        if(this->table[next_pos].is_removed() == false) {
            if(this->table[next_pos].get_key() == key) {
                return -1;
            }
        } else {
            break;
        }
        i++;
        next_pos = this->get_next_pos(pos, i);
    }
    this->table[next_pos].set_key_value(key, value);
    this->size++;
    if(this->get_load_factor() > 0.5) {
        this->enlarge_table();
    }
    return i + 1;
}

template <typename K, typename V>
int HashTable<K, V>::remove(const K &key) {
    // TODO        
    unsigned int pos = this->get_pos(key);
    unsigned int next_pos = pos;
    int i = 0;
    while(this->table[next_pos].is_empty() == false) {
        if(this->table[next_pos].is_removed() == false) {
            if(this->table[next_pos].get_key() == key) {
                this->table[next_pos].set_removed();
                this->size--;
                return i + 1;
            }
        }
        i++;
        next_pos = this->get_next_pos(pos, i);
    }
    return -1;
}

template <typename K, typename V>
size_t HashTable<K, V>::get_table_size() {
    return table_size;
}

template <typename K, typename V>
size_t HashTable<K, V>::get_size() {
    return size;
}

template <typename K, typename V>
double HashTable<K, V>::get_load_factor() {
    return (double)size/table_size;
}


template <typename K, typename V>
class LinearProbeHashTable: public HashTable<K, V> {
public:
    LinearProbeHashTable(HashFunc *hash_func): HashTable<K, V>(hash_func) {
    }
    
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return (pos + step) % this->table_size;
    }
};

template <typename K, typename V>
class QuadProbeHashTable: public HashTable<K, V> {
public:
    QuadProbeHashTable(HashFunc *hash_func): HashTable<K, V>(hash_func) {
    }
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return ((unsigned long) (pos + 0.5*step + 0.5*step*step)) % this->table_size;
    }
};

