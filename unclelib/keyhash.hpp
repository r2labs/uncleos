#ifndef __KEYHASH_H__
#define __KEYHASH_H__

static const int TABLE_SIZE = 128;

// Default hash function class
template <typename K>
struct KeyHash {
    unsigned long operator()(const K& key) const
    {
        return reinterpret_cast<unsigned long>(key) % TABLE_SIZE;
    }
};

#endif /* __KEYHASH_H__ */
