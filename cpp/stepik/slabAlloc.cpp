#include <iostream>
#include <assert.h>
#include <cmath>


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



enum slabStatus { FREE, HALF_BUSY, BUSY };

struct slabHeader {
    void* data;        // указатель на начало данных
    int64_t busyMask;  // маска занятых/свободных кусочков памяти
    slabHeader *next;  // указатель на следующий элемент
    slabHeader *prev;  // указатель на предыдущий элемент

    slabStatus status;
    size_t busy_objects_count; /* количество занятых объектов в одном SLAB-е */
};

/**
 * Эта структура представляет аллокатор, вы можете менять
 * ее как вам удобно. Приведенные в ней поля и комментарии
 * просто дают общую идею того, что вам может понадобится
 * сохранить в этой структуре.
 **/
struct cache {
    slabHeader* freeSlabs; /* список пустых SLAB-ов для поддержки cache_shrink */
    slabHeader* halfBusySlabs; /* список частично занятых SLAB-ов */
    slabHeader* fullBusySlabs; /* список заполненых SLAB-ов */

    size_t slab_object_size; /* размер аллоцируемого объекта в SLAB-е */
    size_t slab_objects_count_limit; /* максимальное количество объектов в одном SLAB-е */
    size_t slab_order; /* 4096 * 2^order байт. */
    size_t slab_size;   /* 4096 * 2^order байт. */
};

bool getMaxCountObjectsInSlab(size_t object_size, size_t slabSize) {
    return slabSize - sizeof(slabHeader) / object_size;
}

//TODO: как оптимально подобрать размер SLAB ?
void fillCacheFields(size_t object_size, size_t *countLimit, size_t *order, size_t *slabSize) {
    *slabSize = 0;
    *order = 0;
    *countLimit = 0;

    while (*countLimit < 16) {
        //(*order)++;
        *order = *order + 1;
        *slabSize = 4096 * pow(2, *order);
        *countLimit = getMaxCountObjectsInSlab(object_size, *slabSize);
    }
}

void freeSlabList(slabHeader *pointer) {
    while (pointer != NULL) {
        free_slab(pointer->data);
        pointer = pointer->next;
    }
}

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
    cache->slab_object_size = object_size;
    fillCacheFields(object_size, &cache->slab_objects_count_limit, &cache->slab_order, &cache->slab_size);
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
    if(cache == NULL) return;
    freeSlabList(cache->freeSlabs->next);
    freeSlabList(cache->halfBusySlabs->next);
    freeSlabList(cache->fullBusySlabs->next);
}

void movElementToSlabList(slabHeader* element, slabHeader** list) {
    if (element == NULL || list == NULL) return;

    if (*list != NULL)
        (*list)->prev = element;
    *list = element;
}

void moveElementInCorrectGroup(struct cache *cache, slabHeader* element) {
    slabStatus oldStatus = element->status;
    size_t limit = cache->slab_objects_count_limit;
    size_t busyCount = element->busy_objects_count;

    switch(oldStatus) {
        case BUSY:
            if (busyCount < limit)
                movElementToSlabList(element, &cache->halfBusySlabs);
            break;
        case HALF_BUSY:
            if (busyCount == limit)
                movElementToSlabList(element, &cache->fullBusySlabs);
            if (busyCount == 0)
                movElementToSlabList(element, &cache->freeSlabs);
            break;
        case FREE:
            if (busyCount != 0)
                movElementToSlabList(element, &cache->halfBusySlabs);
            break;
    }
}

slabHeader* createNewSlab(struct cache *cache) {
    void *slab = alloc_slab(cache->slab_order);

    slabHeader *header = (slabHeader*)(((uint64_t)slab + cache->slab_size) - sizeof(slabHeader));
    header->prev = NULL;
    header->next = NULL;
    header->busy_objects_count = 0;
    header->status = FREE;
    header->data = slab;
}

slabHeader* getSlabWithFreePlace(struct cache *cache) {
    if (cache->halfBusySlabs != NULL) {
        return cache->halfBusySlabs;
    } else if (cache->freeSlabs != NULL) {
        return cache->freeSlabs;
    } else {
        slabHeader* newSlab = createNewSlab(cache);
        movElementToSlabList(newSlab, &cache->freeSlabs);
    }
}

bool isPointerInSlab(slabHeader* slab, void* ptr, size_t slabSize) {
    if(slab == NULL) return false;
    return ptr > slab + slabSize && ptr < slab;
}

bool isPointerNotInSlab(slabHeader* slab, void* ptr, size_t slabSize) {
    return !isPointerInSlab(slab, ptr, slabSize);
}

slabHeader* findSlab(struct cache *cache, void* ptr) {
    slabHeader* pointer = cache->fullBusySlabs; // ищем в занятых

    while (isPointerNotInSlab(pointer, ptr, cache->slab_size)) {
        slabHeader* pointer = pointer->next;
    }

    if (isPointerInSlab(pointer, ptr, cache->slab_size))
        return pointer;

    pointer = cache->halfBusySlabs; // ищем в полузанятых

    while (isPointerNotInSlab(pointer, ptr, cache->slab_size)) {
        slabHeader* pointer = pointer->next;
    }

    if (isPointerInSlab(pointer, ptr, cache->slab_size))
        return pointer;

    return NULL;
}

void* busyPlaceInSlab(size_t objectsCountLimit, slabHeader *slab) {
    int64_t busyMask = ~slab->busyMask;

    int64_t bit;
    int i = 0;
    for(; i < objectsCountLimit; i++) {
        bit = 1 << i;
        if(busyMask & bit) {
            slab->busyMask & bit;
            slab->busy_objects_count++;
            break;
        }
    }

    return (void*)((uint64_t)slab->data + (i * objectsCountLimit));
}

void freePlaceInSlab(size_t objectsCountLimit, slabHeader *slab, void* ptr) {
    uint64_t bitNumber = (uint64_t)((uint64_t)slab->data - (uint64_t)ptr) / objectsCountLimit;
    slab->busyMask & ~(uint64_t)(1 << bitNumber);
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
    slabHeader *slabWithFreePlace = getSlabWithFreePlace(cache);
    void* result = busyPlaceInSlab(cache->slab_objects_count_limit, slabWithFreePlace);
    moveElementInCorrectGroup(cache, slabWithFreePlace);

    return result;
}

/**
 * Функция освобождения памяти назад в кеширующий аллокатор.
 * Гарантируется, что ptr - указатель ранее возвращенный из
 * cache_alloc.
 **/
void cache_free(struct cache *cache, void *ptr)
{
    slabHeader *slabWithPlaceForFree = findSlab(cache, ptr);
    if(slabWithPlaceForFree == NULL) return;
    freePlaceInSlab(cache->slab_objects_count_limit, slabWithPlaceForFree, ptr);
    moveElementInCorrectGroup(cache, slabWithPlaceForFree);
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
    if(cache == NULL) return;
    freeSlabList(cache->freeSlabs->next);
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