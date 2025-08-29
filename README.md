# ht.h
this is a header-only library for a hash table, simply define 
KEY_TYPE, VAL_TYPE, HASH(key), CMP(key1, key2) and include the library and you're ready to go

if you need the declarations elsewhere, but not the implementation, you can define HT_NO_IMPLEMENTATION

for further usage, there are comments describing the behaviour of the functions in [the header](ht.h)
and you can look into [the example](example.c)
