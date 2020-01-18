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


const size_t MAX_OBJECTS_IN_SLAB = 1000; // 1200 for stepik
enum slabStatus { FREE, HALF_BUSY, BUSY };
enum dataPlaceStatus { FREE_PLACE, BUSY_PLACE };

struct slabHeader {
    slabHeader *next;  // указатель на следующий слаб
    slabHeader *prev;  // указатель на предыдущий слаб

    slabStatus status;
    dataPlaceStatus *freePlace;
    size_t busy_objects_count; /* количество занятых объектов в одном SLAB-е */
};

/**
 * Эта структура представляет аллокатор, вы можете менять
 * ее как вам удобно. Приведенные в ней поля и комментарии
 * просто дают общую идею того, что вам может понадобится
 * сохранить в этой структуре.
 **/
struct cache {
   cache() : freeSlabs(nullptr),
   halfBusySlabs(nullptr),
   fullBusySlabs(nullptr),
   slab_object_size(0),
   slab_objects_count_limit(0),
   slab_order(0),
   slab_size(0) {}

    slabHeader* freeSlabs; /* список пустых SLAB-ов для поддержки cache_shrink */
    slabHeader* halfBusySlabs; /* список частично занятых SLAB-ов */
    slabHeader* fullBusySlabs; /* список заполненых SLAB-ов */

    size_t slab_object_size; /* размер аллоцируемого объекта в SLAB-е */
    size_t slab_objects_count_limit; /* максимальное количество объектов в одном SLAB-е */
    size_t slab_order; /* 4096 * 2^order байт. */
    size_t slab_size;   /* 4096 * 2^order байт. */
};

inline size_t getMaxCountObjectsInSlab(size_t object_size, size_t slabSize) {
    size_t allDataSize = slabSize - sizeof(slabHeader);
    size_t oneDataSize = sizeof(dataPlaceStatus) + object_size;
    return allDataSize / oneDataSize;
}

void fillCacheFields(size_t object_size, size_t *countLimit, size_t *order, size_t *slabSize) {
    *slabSize = 0;
    *order = -1;
    *countLimit = 0;

    while (*countLimit < MAX_OBJECTS_IN_SLAB) { //TODO: десятка выбрана случайно + что если не сможем уместить 10шт в самом большом слабе
        *order = *order + 1;
        *slabSize = 4096 * pow(2, *order);
        *countLimit = getMaxCountObjectsInSlab(object_size, *slabSize);
        if (*order == 10) break;
    }
}

inline slabHeader* createNewSlab(struct cache *cache) {
    void *slab = alloc_slab(cache->slab_order);

    slabHeader *header = (slabHeader*)slab;
    header->prev = NULL;
    header->next = NULL;
    header->busy_objects_count = 0;
    header->status = FREE;
    header->freePlace = (dataPlaceStatus*)((size_t)header + sizeof(slabHeader));

    void *point = (void*)((size_t)header + sizeof(slabHeader));
    for(int i = 0; i < cache->slab_objects_count_limit; i++) {
        *(dataPlaceStatus*)point = FREE_PLACE;
        point = (void*)((size_t)point + cache->slab_object_size);
    }

    return header;
}

inline void moveElementToSlabList(slabHeader* element, slabHeader** from, slabHeader** to) {
    if (element == NULL || to == NULL) return;

    if (from != NULL) {
        slabHeader *oldElementNext = element->next;
        slabHeader *oldElementPrev = element->prev;

        if (oldElementPrev == NULL)
            *from = oldElementNext;
        if (oldElementNext != NULL)
            oldElementNext->prev = oldElementPrev;
        if (oldElementPrev != NULL)
            oldElementPrev->next = oldElementNext;
    }

    if (*to != NULL) {
        slabHeader *oldFirstElement = *to;
        *to = element;

        element->prev = NULL;
        element->next = oldFirstElement;
        oldFirstElement->prev = *to;
    } else {
        *to = element;
        element->prev = NULL;
        element->next = NULL;
    }
}

void addEmptySlabs(struct cache *cache, size_t count) {
    slabHeader* newSlab;

    for(int i = 0; i < count; i++) {
        newSlab = createNewSlab(cache);
        moveElementToSlabList(newSlab, NULL, &cache->freeSlabs);
    }
}

inline void freeSlabList(slabHeader *pointer) {
    if(pointer == NULL) return;
    slabHeader *listPointer = pointer;

    while (listPointer->next != NULL)
        listPointer = listPointer->next;

    slabHeader *prevPointer;
    while (listPointer != NULL) {
        prevPointer = listPointer->prev;
        free_slab((void*)listPointer);
        listPointer = prevPointer;
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
    if (cache->slab_objects_count_limit != 0)
    addEmptySlabs(cache, 20);
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
    freeSlabList(cache->freeSlabs);
    cache->freeSlabs = NULL;
    freeSlabList(cache->halfBusySlabs);
    cache->halfBusySlabs = NULL;
    freeSlabList(cache->fullBusySlabs);
    cache->fullBusySlabs = NULL;
}

void moveElementInCorrectGroup(struct cache *cache, slabHeader* element) {
    slabStatus oldStatus = element->status;
    size_t limit = cache->slab_objects_count_limit;
    size_t busyCount = element->busy_objects_count;

    switch(oldStatus) {
        case BUSY:
            if (busyCount < limit) {
                moveElementToSlabList(element, &cache->fullBusySlabs, &cache->halfBusySlabs);
                element->status = HALF_BUSY;
            }
            break;
        case HALF_BUSY:
            if (busyCount == limit) {
                moveElementToSlabList(element, &cache->halfBusySlabs, &cache->fullBusySlabs);
                element->status = BUSY;
            }
            if (busyCount == 0) {
                moveElementToSlabList(element, &cache->halfBusySlabs, &cache->freeSlabs);
                element->status = FREE;
            }
            break;
        case FREE:
            if (busyCount != 0) {
                moveElementToSlabList(element, &cache->freeSlabs, &cache->halfBusySlabs);
                element->status = HALF_BUSY;
            }
            break;
    }
}



slabHeader* getSlabWithFreePlace(struct cache *cache) {
    if (cache->halfBusySlabs != NULL) {
        return cache->halfBusySlabs;
    } else if (cache->freeSlabs != NULL) {
        return cache->freeSlabs;
    } else {
        slabHeader* newSlab = createNewSlab(cache);
        moveElementToSlabList(newSlab, NULL, &cache->freeSlabs);
        return newSlab;
    }
}

inline bool isPointerInSlab(slabHeader* slab, void* ptr, size_t slabSize) {
    if(slab == NULL) return false;
    return ptr > slab && ptr < (void*)((size_t)slab + slabSize);
}

inline bool isPointerNotInSlab(slabHeader* slab, void* ptr, size_t slabSize) {
    return !isPointerInSlab(slab, ptr, slabSize);
}

inline slabHeader* findSlab(struct cache *cache, void* ptr) {
    slabHeader* pointer = cache->fullBusySlabs; // ищем в занятых

    while (isPointerNotInSlab(pointer, ptr, cache->slab_size)) {
        if (pointer == NULL) break;
        pointer = pointer->next;
    }

    if (isPointerInSlab(pointer, ptr, cache->slab_size))
        return pointer;

    pointer = cache->halfBusySlabs; // ищем в полузанятых

    while (isPointerNotInSlab(pointer, ptr, cache->slab_size)) {
        if (pointer == NULL) break;
        pointer = pointer->next;
    }

    if (isPointerInSlab(pointer, ptr, cache->slab_size))
        return pointer;

    std::cerr << "can not find Slab";
    return NULL;
}

void* busyPlaceInSlab(size_t slabObjectSize, slabHeader *slab) {
    dataPlaceStatus *point = slab->freePlace;

    slab->freePlace = (dataPlaceStatus*)((size_t)point + slabObjectSize);

    *point = BUSY_PLACE;
    slab->busy_objects_count++;
    return (void*)((size_t)point + sizeof(dataPlaceStatus));
}

void freePlaceInSlab(slabHeader *slab, void* ptr) {
    dataPlaceStatus *placeStatus = (dataPlaceStatus*)((size_t)ptr - sizeof(dataPlaceStatus));
    *placeStatus = FREE_PLACE;
    slab->freePlace = placeStatus;

    slab->busy_objects_count = slab->busy_objects_count - 1;
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
    if (cache->slab_objects_count_limit == 0)
        return NULL;
    slabHeader *slabWithFreePlace = getSlabWithFreePlace(cache);
    void* result = busyPlaceInSlab(cache->slab_object_size, slabWithFreePlace);
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
    if (slabWithPlaceForFree == NULL) return;
    freePlaceInSlab(slabWithPlaceForFree, ptr);
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
    freeSlabList(cache->freeSlabs);
    cache->freeSlabs = NULL;
}










void slabPrint(slabHeader* slab) {
    while (slab != NULL) {
        std::cout << "slab address: " << static_cast<void*>(slab) << std::endl;
        slab = slab->next;
    }
}

void cachePrint(struct cache *cache) {
    std::cout << std::endl << "CACHE STATUS:" << std::endl;

    std::cout << "print freeSlabs:" << std::endl;
    slabPrint(cache->freeSlabs);
    std::cout << "print halfBusySlabs:" << std::endl;
    slabPrint(cache->halfBusySlabs);
    std::cout << "print fullBusySlabs:" << std::endl;
    slabPrint(cache->fullBusySlabs);
}

/** */
void testOneAlloc() {
    const size_t OBJECT_SIZE = 100; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);

    cache_alloc(&cache);

    std::cout << "testOneAlloc: Ok" << std::endl;
}

/** */
void testOneAllocWithPointerCheck() {
    const size_t OBJECT_SIZE = 100; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test;

    test = cache_alloc(&cache);

    assert((void*)((size_t)cache.halfBusySlabs + sizeof(slabHeader) + sizeof(dataPlaceStatus)) == test);

    std::cout << "testOneAllocWithPointerCheck: Ok" << std::endl;
}

/** */
void testOneBigAlloc() {
    const size_t OBJECT_SIZE = 100000000; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test;

    test = cache_alloc(&cache);
    assert(test == NULL);

    std::cout << "testOneBigAlloc: Ok" << std::endl;
}

/** */
void testOneFree() {
    const size_t OBJECT_SIZE = 100; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test;

    test = cache_alloc(&cache);
    cache_free(&cache, test);

    std::cout << "testOneFree: Ok" << std::endl;
}

/** */
void testReAlloc() {
    const size_t OBJECT_SIZE = 100; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test, *test1;

    test = cache_alloc(&cache);
    cache_free(&cache, test);
    test1 = cache_alloc(&cache);

    assert(test == test1);

    std::cout << "testReAlloc: Ok" << std::endl;
}

/** */
void testManyAlloc() {
    const size_t OBJECT_SIZE = 1200; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[8];

    for (int i = 0; i<8; i++)
        test[i] = cache_alloc(&cache);

    for (int i = 0; i<8; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    std::cout << "testManyAlloc: Ok" << std::endl;
}

/** */
void testCacheShrink() {
    const size_t OBJECT_SIZE = 1200; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[8];

    for (int i = 0; i<8; i++)
        test[i] = cache_alloc(&cache);

    for (int i = 0; i<8; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);
    //assert(cache.freeSlabs->next != NULL);

    cache_shrink(&cache);

    assert(cache.freeSlabs == NULL);

    std::cout << "testCacheShrink: Ok" << std::endl;
}

/** */
void testCacheRelease() {
    const size_t OBJECT_SIZE = 1;
    const size_t SIZE = 5400;
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[SIZE];

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);

/*    for (int i = 0; i<SIZE; i++)
        cache_free(&cache, test[i]);*/

/*    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs != NULL);
    assert(cache.fullBusySlabs != NULL);*/

    cache_release(&cache);

    assert(cache.freeSlabs == NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    std::cout << "testCacheRelease: Ok" << std::endl;
}

/** */
void testAllocFreeAlloc() {
    const size_t OBJECT_SIZE = 1200;
    const size_t SIZE = 8;
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[SIZE], *test1[SIZE];

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);
    //cachePrint(&cache);

    for (int i = 0; i<SIZE; i++)
        cache_free(&cache, test[i]);
    //cachePrint(&cache);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    for (int i = 0; i<SIZE; i++)
        test1[i] = test[i];

   for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);
    //cachePrint(&cache);

   // assert(cache.freeSlabs == NULL);
    assert(cache.halfBusySlabs != NULL);
   // assert(cache.fullBusySlabs != NULL);

    for (int i = 0; i<SIZE; i++)
        cache_free(&cache, test[i]);
    //cachePrint(&cache);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    std::cout << "testAllocFreeAlloc: Ok" << std::endl;
}

/** */
void testRandomFree() {
    const size_t OBJECT_SIZE = 1200; // [0; 10] (4096 * 2^order)
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[8];

    for (int i = 0; i<8; i++)
        test[i] = cache_alloc(&cache);

    cache_free(&cache, test[3]);
    cache_free(&cache, test[7]);
    cache_free(&cache, test[0]);
    cache_free(&cache, test[1]);
    cache_free(&cache, test[6]);
    cache_free(&cache, test[4]);
    cache_free(&cache, test[2]);
    cache_free(&cache, test[5]);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    std::cout << "testRandomFree: Ok" << std::endl;
}

/** */
void testManySmallAllocFreeAlloc() {
    const size_t OBJECT_SIZE = 1;
    const size_t SIZE = 5400;
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[SIZE], *test1[SIZE];

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);

    for (int i = 0; i<SIZE; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    for (int i = 0; i<SIZE; i++)
        test1[i] = test[i];

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);

    //assert(cache.freeSlabs == NULL);
    assert(cache.halfBusySlabs != NULL);
    assert(cache.fullBusySlabs != NULL);

    for (int i = 0; i<SIZE; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    std::cout << "testManySmallAllocFreeAlloc: Ok" << std::endl;
}

/** */
void testDynamicAlloc() {
    const size_t OBJECT_SIZE = 1;
    const size_t SIZE = 5400;
    struct cache cache{};
    cache_setup(&cache, OBJECT_SIZE);
    void  *test[SIZE];

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);

    for (int i = 0; i<SIZE/4; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);

    cache_release(&cache);

    assert(cache.freeSlabs == NULL);
    assert(cache.halfBusySlabs == NULL);
    assert(cache.fullBusySlabs == NULL);

    for (int i = 0; i<SIZE; i++)
        test[i] = cache_alloc(&cache);

    assert(cache.freeSlabs == NULL);
    assert(cache.halfBusySlabs != NULL);
    assert(cache.fullBusySlabs != NULL);

    for (int i = 0; i<SIZE/2; i++)
        cache_free(&cache, test[i]);

    assert(cache.freeSlabs != NULL);

    cache_shrink(&cache);

    assert(cache.freeSlabs == NULL);

    std::cout << "testDynamicAlloc: Ok" << std::endl;
}


/**
 * Аллоцирует участок размером 4096 * 2^order байт,
 * выровненный на границу 4096 * 2^order байт. order
 * должен быть в интервале [0; 10] (обе границы
 * включительно), т. е. вы не можете аллоцировать больше
 * 4Mb за раз.
 **/
void *alloc_slab(int order) {
    int slabSize = 4096 * pow(2, order);
    return new int[slabSize];
}
void free_slab(void *slab) {
    delete[] (slabHeader*)slab;
  //  delete[] slab;
}

int main() {

    testOneAlloc();
    testOneAllocWithPointerCheck();
    testOneBigAlloc();
    testOneFree();
    testReAlloc();
    testManyAlloc();
    testCacheShrink();
    testCacheRelease();
    testAllocFreeAlloc();
    testRandomFree();
    testManySmallAllocFreeAlloc();
    testDynamicAlloc();



    return 0;
}