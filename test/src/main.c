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
    ensure(hashmap_exists(&hm, (void*)85));
    hashmap_remove(&hm, (void*)85);
    hashmap_remove(&hm, (void*)67);
    hashmap_remove(&hm, (void*)76);
    hashmap_remove(&hm, (void*)96);
    ensure(!hashmap_exists(&hm, (void*)76));
    printf("hm.size = %d\n", hm.size);
    hashmap_iter(&hm, hm_iter);
    hashmap_destroy(&hm);
}

void vector_test()
{
    struct sample_pod { int x, y, z; };
    struct vector v;
    vector_init(&v, sizeof(struct sample_pod));

    /* Apend test data to vector */
    struct sample_pod sd = { 0, 0, 0 };
    vector_append(&v, &sd);
    sd.x = 5; sd.z = 7;
    vector_append(&v, &sd);
    sd.x = 1; sd.y = 2; sd.z = 3;
    vector_append(&v, &sd);
    sd.x = 4; sd.y = 5; sd.z = 7;
    vector_append(&v, &sd);
    /* Insert test data to vector */
    sd.x = 2; sd.y = 3; sd.z = 9;
    vector_insert(&v, 2, &sd);

    /* Show them */
    for (size_t i = 0; i < v.size; ++i) {
        struct sample_pod* spp = vector_at(&v, i);
        printf("zv[%d] = (%d, %d, %d)\n", i, spp->x, spp->y, spp->z);
    }

    vector_destroy(&v);
}

void leak_detect_test()
{
    ld_init();
    int* x = malloc(5);
    *x = 3;
    free(x);
    x = malloc(7);
    ld_print_leaks();
    ld_shutdown();
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    vector_test();
    hashmap_test();
    leak_detect_test();

    return 0;
}
