#include <iostream>
#include <assert.h>
#include <set>


/**
 * Эти две функции вы должны использовать для аллокации
 * и освобождения памяти в этом задании. Считайте, что
 * внутри они используют buddy аллокатор с размером
 * страницы равным 4096 байтам.
 **/

/**
 * Аллоцирует участок размером 4096 * 2^order байт,
 * выровненный на границу 4096 * 2^order байт. order
 * должен быть в интервале [0; 10] (обе границы
 * включительно), т. е. вы не можете аллоцировать больше
 * 4Mb за раз.
 **/
void *alloc_slab(int order);
/**
 * Освобождает участок ранее аллоцированный с помощью
 * функции alloc_slab.
 **/
void free_slab(void *slab);

struct slabList {
    void *point; // хз, указатель куда-то
    size_t slabs_count; /* количество SLAB-ов */
};

struct slabHeader {
    void *point; // хз, указатель куда-то
    size_t busy_objects_count; /* количество занятых объектов в одном SLAB-е */
};

/**
 * Эта структура представляет аллокатор, вы можете менять
 * ее как вам удобно. Приведенные в ней поля и комментарии
 * просто дают общую идею того, что вам может понадобится
 * сохранить в этой структуре.
 **/
struct cache {
    slabList freeBusySlabs; /* список пустых SLAB-ов для поддержки cache_shrink */
    slabList halfBusySlabs; /* список частично занятых SLAB-ов */
    slabList fullBusySlabs; /* список заполненых SLAB-ов */

    size_t slab_object_size; /* размер аллоцируемого объекта в SLAB-е */
    size_t slab_objects_count_limit; /* максимальное количество объектов в одном SLAB-е */
    size_t slab_size; /* используемый размер SLAB-а */
};

/**
 * Функция инициализации будет вызвана перед тем, как
 * использовать это кеширующий аллокатор для аллокации.
 * Параметры:
 *  - cache - структура, которую вы должны инициализировать
 *  - object_size - размер объектов, которые должен
 *    аллоцировать этот кеширующий аллокатор
 **/
void cache_setup(struct cache *cache, size_t object_size)
{
    cache->freeBusySlabs = slabList{};
    cache->freeBusySlabs.slabs_count = 0;
    cache->halfBusySlabs = slabList{};
    cache->halfBusySlabs.slabs_count = 0;
    cache->fullBusySlabs = slabList{};
    cache->fullBusySlabs.slabs_count = 0;

    cache->slab_object_size = object_size;
    cache->slab_objects_count_limit = 10;
    cache->slab_size = object_size * cache->slab_objects_count_limit + sizeof(slabHeader);
}


/**
 * Функция освобождения будет вызвана когда работа с
 * аллокатором будет закончена. Она должна освободить
 * всю память занятую аллокатором. Проверяющая система
 * будет считать ошибкой, если не вся память будет
 * освбождена.
 **/
void cache_release(struct cache *cache)
{
    /* Реализуйте эту функцию. */
}


/**
 * Функция аллокации памяти из кеширующего аллокатора.
 * Должна возвращать указатель на участок памяти размера
 * как минимум object_size байт (см cache_setup).
 * Гарантируется, что cache указывает на корректный
 * инициализированный аллокатор.
 **/
void *cache_alloc(struct cache *cache)
{
    /* Реализуйте эту функцию. */
    return NULL;
}


/**
 * Функция освобождения памяти назад в кеширующий аллокатор.
 * Гарантируется, что ptr - указатель ранее возвращенный из
 * cache_alloc.
 **/
void cache_free(struct cache *cache, void *ptr)
{
    /* Реализуйте эту функцию. */
}


/**
 * Функция должна освободить все SLAB, которые не содержат
 * занятых объектов. Если SLAB не использовался для аллокации
 * объектов (например, если вы выделяли с помощью alloc_slab
 * память для внутренних нужд вашего алгоритма), то освбождать
 * его не обязательно.
 **/
void cache_shrink(struct cache *cache)
{
    /* Реализуйте эту функцию. */
}







/** */
void testOneAlloc() {
    const size_t OBJECT_SIZE = 10; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test;

    test = cache_alloc(&cache);

    std::cout << "testOneAlloc: Ok" << std::endl;
}


// реализация этих функций от меня не нужна
void *alloc_slab(int order){}
void free_slab(void *slab){}

int main() {

    testOneAlloc();




    return 0;
}