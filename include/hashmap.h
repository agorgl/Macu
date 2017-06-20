/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Hashmap key and value type long enough to hold any 64 bit value */
typedef int64_t hm_ptr;

/* Helper casts to avoid conversion warnings */
#define hm_cast(x) ((hm_ptr)(uintptr_t)(x))
#define hm_pcast(x) ((void*)(uintptr_t)(x))

struct hashmap_pair {
    hm_ptr key;
    hm_ptr value;
};

struct hashmap_node {
    struct hashmap_pair data;
    struct hashmap_node* next;
};

/* Hash func signature */
typedef size_t(*hashmap_hash_fn)(hm_ptr key);
/* Equality comparison func signature */
typedef int(*hashmap_eql_fn)(hm_ptr key1, hm_ptr key2);

struct hashmap {
    /* Array of lists */
    struct hashmap_node** buckets;
    /* Current capacity of the buckets array */
    size_t capacity;
    /* Size of the hashmap */
    size_t size;
    /* Hash function used */
    hashmap_hash_fn hashfn;
    /* Comparison function used */
    hashmap_eql_fn eqlfn;
};


/* Init/Destroy funcs */
void hashmap_init(struct hashmap* hm, hashmap_hash_fn hfn, hashmap_eql_fn efn);
void hashmap_destroy(struct hashmap* hm);

/* Iterate through hashmap */
typedef void(*hm_iter_fn)(hm_ptr key, hm_ptr value);
void hashmap_iter(struct hashmap* hm, hm_iter_fn iter_cb);

struct hashmap_iter {
    struct hashmap* map;
    struct hashmap_node* next;
    size_t bucket;
    struct hashmap_pair* p; /* Pointer to data pair */
};
void hashmap_iter_init(struct hashmap* hm, struct hashmap_iter* it);
struct hashmap_pair* hashmap_iter_next(struct hashmap_iter* it);
struct hashmap_pair* hashmap_iter_first(struct hashmap* hm, struct hashmap_iter* it);

#define hashmap_for(hm, it) \
    for (hashmap_iter_first(&(hm), &it); it.p; hashmap_iter_next(&it))

/*
 * Set the value for specified key in hashmap
 * Creates pair if given key it does not exist, overwrites value if it does
 */
void hashmap_put(struct hashmap* hm, hm_ptr key, hm_ptr value);

/* Remove the pair for the specified key in hashmap */
void hashmap_remove(struct hashmap* hm, hm_ptr key);

/* Retrieves ptr to the value assosiated with the specified key or 0 if it does not exist */
hm_ptr* hashmap_get(struct hashmap* hm, hm_ptr key);

/* Checks if pair with given key exists */
int hashmap_exists(struct hashmap* hm, hm_ptr key);

/* Premade hash/eql functions */
size_t hm_str_hash(hm_ptr key);
int hm_str_eql(hm_ptr k1, hm_ptr k2);
size_t hm_u64_hash(hm_ptr key);
int hm_u64_eql(hm_ptr k1, hm_ptr k2);

#ifdef __cplusplus
}
#endif

#endif /* ! _HASHMAP_H_ */
