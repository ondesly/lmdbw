# lmdbw
C++ simplifying wrapper for the LMDB embedded key-value data store.

## Example
Beside the example below, you may want to check test files in the tests directory.

Assume you have struct:

```cpp
struct item {
    uint16_t w;
    uint16_t h;
    std::vector<uint8_t> data;
};
```

And map with some values:

```cpp
std::unordered_map<uint32_t, item> map{
        {100, {8,  10, {1,  2,  3,  4,  5}}},
        {200, {10, 20, {11, 21, 31, 41, 51}}},
        {300, {12, 30, {12, 22, 32, 42, 52}}}
};
```

You could create database wrapper with a filename and environment flags (1), a database name and its flags (2) and struct fields (3):

```cpp
lm::dbw<uint32_t, item> db({"./db.db", lm::flag::env::no_subdir}, // 1
                           {"test", lm::flag::dbi::create | lm::flag::dbi::reverse_key}, // 2
                           {&item::w, &item::h, &item::data}); // 3
```

And with the wrapper you can put, get and delete:

```cpp
db.put(map);

const auto get_map = db.get(110, 200);

db.del(100, 250);
```

Should any operation in the above fail, an lm::exception will be thrown and terminate the program since we don't specify any exception handler. All resources will get automatically cleaned up due to RAII.

## Requirements

This library requires a C++17 compiler and standard library.
