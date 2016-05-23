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
#include <stdio.h>
#include "macu.h"
#include "vector.h"
#include "hashmap.h"
#include "dbc.h"

size_t hash_fn(void* key)
{
    long h = (long) key;
    return (13 * h) ^ (h >> 15);
}

void hm_iter(void* key, void* value)
{
    printf("hm[%ld] = %ld\n", (long)key, (long) value);
}

int hm_eql(void* k1, void* k2)
{
    return k1 == k2;
}

void hashmap_test()
{
    struct hashmap hm;
    const int hashmap_test_data[] = {
        77, 69,
        63, 39,
        99, 21,
        28, 60,
        9, 53,
        85, 12,
        43, 64,
        67, 30,
        76, 26,
        55, 86,
        96, 58,
        73, 16,
        94, 27,
    };
    hashmap_init(&hm, hash_fn, hm_eql);
    for (size_t i = 0; i < sizeof(hashmap_test_data) / sizeof(int); i+=2) {
        int key = hashmap_test_data[i];
        int value = hashmap_test_data[i + 1];
        hashmap_put(&hm, (void*)key, (void*)value);
    }
    hashmap_put(&hm, (void*)43, (void*)43);
    hashmap_iter(&hm, hm_iter);
    ensure(hashmap_exists(&hm, (void*)85));
    hashmap_destroy(&hm);
}

void vector_test()
{
    struct vector v;
    vector_init(&v);

    /* Apend test data to vector */
    vector_append(&v, 5);
    vector_append(&v, 1);
    vector_append(&v, 8);
    vector_append(&v, 7);

    /* Show them */
    for (size_t i = 0; i < v.size; ++i)
        printf("zv[%d] = %ld\n", i, v.data[i]);

    vector_destroy(&v);
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    dummy();
    vector_test();
    hashmap_test();

    return 0;
}