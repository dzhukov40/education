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

enum slabStatus { FREE, HALF_BUSY, BUSY };

struct slabHeader {
    void *point; // хз, указатель куда-то
    size_t busy_objects_count; /* количество занятых объектов в одном SLAB-е */
};

struct slabListElement {
    slabListElement *next;  // указатель на следующий элемент
    slabListElement *prev;  // указатель на предыдущий элемент
    slabHeader *slabHeader; // указатель на заголовок SLAB-а

    slabStatus status;
};

/**
 * Эта структура представляет аллокатор, вы можете менять
 * ее как вам удобно. Приведенные в ней поля и комментарии
 * просто дают общую идею того, что вам может понадобится
 * сохранить в этой структуре.
 **/
struct cache {
    slabListElement* freeSlabs; /* список пустых SLAB-ов для поддержки cache_shrink */
    slabListElement* halfBusySlabs; /* список частично занятых SLAB-ов */
    slabListElement* fullBusySlabs; /* список заполненых SLAB-ов */

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
    //TODO: add realization
}

void addElementToSlabList(slabListElement* element, slabListElement* list) {
    if (element == NULL || list == NULL) return;

    // TODO: вставляем элемент
}

void moveElementInCorrectGroup(struct cache *cache, slabListElement* element) {
    slabStatus oldStatus = element->status;
    size_t limit = cache->slab_objects_count_limit;
    size_t busyCount = element->slabHeader->busy_objects_count;

    switch(oldStatus) {
        case BUSY:
            if (busyCount < limit) {
                // TODO: переместить в список HALF_BUSY
            }
            break;
        case HALF_BUSY:
            if (busyCount == limit) {
                // TODO: переместить в список BUSY
            }
            if (busyCount == 0) {
                // TODO: переместить в список FREE
            }
            break;
        case FREE:
            if (element->slabHeader->busy_objects_count != 0) {
                // TODO: переместить в список HALF_BUSY
            }
            break;
    }
}

slabListElement* createNewFreePlace(struct cache *cache) {
    // создаем новый слаб и указатель на него
}

slabListElement* getSlabWithFreePlace(struct cache *cache) {
    if (cache->halfBusySlabs != NULL) {
        return cache->halfBusySlabs;
    } else if (cache->freeSlabs != NULL) {
        return cache->freeSlabs;
    } else {
        //TODO: надо создать новый слаб, добавить в список пустых и вернуть его
    }
}

slabListElement* findSlab(struct cache *cache, void* ptr) {
    //TODO: поискать в занятых, есл там нет поискать в полузанятых

    return NULL;
}


void* busyPlaceInSlab(slabListElement *slab) {
    //TODO: add realization
}

void freePlaceInSlab(slabListElement *slab) {
    //TODO: add realization
}

void freeSlab(slabListElement *slab) {
    //TODO: add realization
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
    slabListElement *slabWithFreePlace = getSlabWithFreePlace(cache);
    void* result = busyPlaceInSlab(slabWithFreePlace);
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
    slabListElement *slabWithPlaceForFree = findSlab(cache, ptr);
    freePlaceInSlab(slabWithPlaceForFree);
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
    slabListElement *pointer = cache->freeSlabs->next;

    while (pointer != NULL) {
        freeSlab(pointer);
        pointer = pointer->next;
    }
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