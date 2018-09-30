#include <iostream>
#include <vector>
#include <initializer_list>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>> 
class HashMap {

public:
    using pair_list = std::list<std::pair<const KeyType, ValueType>>;
    using iterator = typename pair_list::iterator;
    using const_iterator = typename pair_list::const_iterator;

private:
    size_t _count = 0;

    struct bucket {
        iterator begin;
        iterator end;
        bool idle = true;
    };

    pair_list all_elements;
    Hash hash_f;
    size_t cur_size = 16;
    std::vector<bucket> buckets = std::vector<bucket>(cur_size);
        
public: 
    Hash hash_function() const {
        return hash_f;
    }

    size_t pos(KeyType key) {
        return hash_f(key) % cur_size;
    }

    iterator begin() {
        return all_elements.begin();
    }

    const_iterator begin() const {
        return all_elements.begin();
    }

    iterator end() {
        return all_elements.end();
    }

    const_iterator end() const {
        return all_elements.end();
    }

    size_t size() const {
        return _count;
    }

    bool empty() const {
        return _count == 0;
    }

    void clear() {
        _count = 0;
        for (auto i : all_elements) {
            buckets[pos(i.first)].idle = true;
        }
        all_elements.clear();
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash _hash_f = Hash()) : hash_f(_hash_f) {
        for (auto i : list) {
            insert(i);
        }
    }

    HashMap(Hash _hash_f = Hash()) : hash_f(_hash_f) {
        buckets.resize(cur_size);
    }

    template<typename Iter>
    HashMap(Iter begin, Iter end, Hash _hash_f = Hash()) : hash_f(_hash_f) {
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    const HashMap & operator = (const HashMap & other) {
        _count = 0;
        for (auto i : all_elements) {
            buckets[pos(i.first)].idle = true;
        }
        all_elements.clear();
        for (auto i : other) {
            this->insert(i);
        }
        return other;
    }

    ValueType & operator[] (const KeyType key) {
        iterator it = this->find(key);
        if (it == this->end()) {
            this->insert(std::make_pair(key, ValueType()));
            it = this->find(key);
        }
        return it->second;
    }

    const ValueType & at(const KeyType key) const {
        const_iterator it = this->find(key);
        if (it == this->end()) {
            throw std::out_of_range("Invalid key\n");
        }
        return it->second;
    }

    iterator find(KeyType key) {
        size_t i = pos(key);
        if (buckets[i].idle) {
            return end();
        }
        iterator it = buckets[i].begin;
        while (it != buckets[i].end) {
            if (!(it->first == key)) {
                ++it;
            } else {
                return it;
            }
        }
        if (it->first == key) {
            return it;
        }
        return end();
    }

    const_iterator find(KeyType key) const {
        size_t i = hash_f(key) % cur_size;
        if (buckets[i].idle) {
            return end();
        }
        const_iterator it = buckets[i].begin;
        while (it != buckets[i].end) {
            if (!(it->first == key)) {
                ++it;
            } else {
                return it;
            }
        }
        if (it->first == key) {
            return it;
        }
        return end();
    }

    void rebuild() {
        if (static_cast<int>(size()) * 2 > static_cast<int>(cur_size)) {
             cur_size *= 2;
             buckets.clear();
             buckets.resize(cur_size);
             pair_list tmp;
             for (auto i : all_elements) {
                tmp.emplace_back(i);
             }
             all_elements.clear();
             _count = 0;
             for (auto i : tmp) {
                insert(i);
             }
        }
    }

    iterator insert(std::pair<const KeyType, ValueType> elem) {
        rebuild();
        size_t i = pos(elem.first);
        if (buckets[i].idle == true) {
            all_elements.push_back(elem);
            buckets[i].idle = false;
            buckets[i].begin = end();
            --buckets[i].begin;
            buckets[i].end = all_elements.end();
            --buckets[i].end;
        } else if (find(elem.first) == end()) {
            ++buckets[i].end;
            buckets[i].end = all_elements.insert(buckets[i].end, elem);
        }
        ++_count;
        return buckets[i].end;
    }

    void erase(KeyType key) {
        size_t i = pos(key);
        if (!buckets[i].idle) {
            if (buckets[i].begin == buckets[i].end) {
                if (buckets[i].begin->first == key) {
                    buckets[i].idle = true;
                    all_elements.erase(buckets[i].begin);
                    --_count;
                }
                return;
            } else if (buckets[i].begin->first == key || buckets[i].end->first == key) {
                if (buckets[i].begin->first == key) {
                    iterator tmp = buckets[i].begin;
                    ++buckets[i].begin;
                    all_elements.erase(tmp);
                    --_count;
                } else {
                    iterator tmp = buckets[i].end;
                    --buckets[i].end;
                    all_elements.erase(tmp);
                    --_count;
                }
            } else if (find(key) != end()) {
                all_elements.erase(find(key));
                --_count;
            }
        }
    }
};