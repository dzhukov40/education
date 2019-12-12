#include <iostream>
#include <assert.h>


struct rootMetaData {
    size_t size;
    void *buf;
    void *endBuf;
    void *next;
};

struct dataHeader {
    size_t size;
    void *next;
    void *previous;
};

struct bestChoice {
    uint64_t freeSpace;
    void *next;
    void *previous;
    void *newPlace;
};

inline void* createMetaData(void* point, std::size_t size, void* prevPoint, void* nextPoint)
{
    ((dataHeader*)point)->size = size;
    ((dataHeader*)point)->previous = prevPoint;
    ((dataHeader*)point)->next = nextPoint;

    //std::cout << "dataPointer: " << (void*)((uint64_t)point + sizeof(dataHeader)) << std::endl;
    return point;
}

inline void* getDataPoint(void *point) {
    return (void*)((uint64_t)point + sizeof(dataHeader));
}

inline  void* newPointBySize(void *point, uint64_t size) {
    return (void*)((uint64_t)point + size);
}

inline uint64_t getFreeSpace(rootMetaData* root, void *point, void *prevPoint) {
    if (point == NULL)  // дошлли до конца списка
        return (uint64_t)root->endBuf - ((uint64_t)prevPoint + ((dataHeader*)prevPoint)->size + sizeof(dataHeader));
    else
        return (uint64_t)prevPoint - (uint64_t)point;
}

inline  dataHeader* castPoint(void *point) {
    return (dataHeader*)point;
}

inline dataHeader* findAllocatedPlace(rootMetaData* localRoot, void *point) {
    dataHeader* ptr = (dataHeader*)localRoot->next;
    if (ptr == NULL) return ptr;

    while(ptr != NULL) {
        if (getDataPoint(ptr) == point) break;
        ptr = (dataHeader*)ptr->next;
    }

    return ptr;
}

inline void freePlace(rootMetaData* localRoot, dataHeader* ptr) {
    if(ptr == NULL) return;
    dataHeader* nextPtr = castPoint(ptr->next);

    if(ptr->previous == localRoot) {
        ((rootMetaData*)ptr->previous)->next = nextPtr;
    } else {
        dataHeader* previousPtr = castPoint(ptr->previous);
        previousPtr->next = nextPtr;
    }
}

void printAllDataHeader(dataHeader* header, int i) {
    std::cout << "-- " << i << " --" << std::endl;
    std::cout << "dataHeader->size: " << header->size << std::endl;
    std::cout << "dataHeader->next: " << static_cast<void*>(header->next) << std::endl;
    std::cout << "dataHeader->previous: " << static_cast<void*>(header->previous) << std::endl;

    if (header->next != NULL)
        printAllDataHeader((dataHeader*)header->next, ++i);
}

void printMemory(rootMetaData* localRoot) {
    std::cout << "-- START MEMORY --" << std::endl;
    std::cout << "sizeof(dataHeader): " << sizeof(dataHeader) << std::endl;
    std::cout << "sizeof(rootMetaData): " << sizeof(rootMetaData) << std::endl << std::endl;
    std::cout << "localRoot->size: " << localRoot->size << std::endl;
    std::cout << "localRoot->buf: " << static_cast<void*>(localRoot->buf) << std::endl;
    std::cout << "localRoot->endBuf: " << static_cast<void*>(localRoot->endBuf) << std::endl;
    std::cout << "localRoot->next: " << static_cast<void*>(localRoot->next) << std::endl;

    int i = 0;
    printAllDataHeader((dataHeader*)localRoot->next, i);

    std::cout << "-- END MEMORY --" << std::endl;
}


static rootMetaData root;

void mysetup(void *buf, std::size_t size)
{
    root.size = size;
    root.buf = buf;
    root.endBuf = (void*)((uint64_t)buf + size); // избыточное поле
    root.next = NULL;
}

inline void findBestChoice(rootMetaData* localRoot, int64_t size, bestChoice* bestPlace) {
    void* point = localRoot->next;
    void* prevPoint = localRoot->buf;
    bestPlace->freeSpace = UINT64_MAX;

    int i = 1;
    while (i++) { // пробегаемся по списку и ищем лучшее место
        int64_t freeSpace = getFreeSpace(localRoot, point, prevPoint);

        if (freeSpace >= size && freeSpace < bestPlace->freeSpace) { // обновляем лучший вариант
            bestPlace->freeSpace = freeSpace;
            bestPlace->next = point;
            bestPlace->previous = prevPoint;
            bestPlace->newPlace = newPointBySize(prevPoint, sizeof(dataHeader));
        }

        if (point == NULL) { //дошли до конца
            if (i == 2) localRoot->next = bestPlace->newPlace;
            break;
        }

        prevPoint = point; // подготавливаемся к переходу на след шаг
        point = castPoint(point)->next;
    }
}


// Функция аллокации
void *myalloc(std::size_t size)
{
    struct bestChoice bestPlace{};
    findBestChoice(&root, size, &bestPlace); // получить подходящее место для хранения

    if (bestPlace.newPlace == NULL)
        return NULL;

    // создаем мета информацию на занимаемый кусочек
    void* dataPoint = createMetaData(bestPlace.newPlace, size, bestPlace.previous, bestPlace.next);

    dataHeader* previousPtr = castPoint(bestPlace.previous); // оставим так для отладки
    dataHeader* nextPtr = castPoint(bestPlace.next);
    previousPtr->next = bestPlace.newPlace;  // у крайних элементов поменять указатели
    if (nextPtr != NULL)
        nextPtr->previous = bestPlace.newPlace;

    // debug
    // printMemory(&root);


    return dataPoint;
}

// Функция освобождения
void myfree(void *p)
{
    dataHeader* placeForFree = findAllocatedPlace(&root, p);
    freePlace(&root, placeForFree);

    // debug
   // printMemory(&root);
}









/**
buf ->  0x600063ce0  ->  25770212576
test -> 0x600063d10  ->  25770212624  почему разница > 24
 */
void OneAllocTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 400;
    void *buf = malloc(BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void* test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));

    std::cout << "OneAllocTest: Ok" << std::endl;
}

/**
test -> 25770278768
        25770278774   разница в "ALLOC_SIZE" ?????
 */
void ManyAllocTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 6;
    void *buf = malloc(BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void* test1;
    for(int i = 0; i<50; i++) {
        test1 = myalloc(ALLOC_SIZE);
        assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader) + (ALLOC_SIZE + sizeof(dataHeader)) * i));
    }

    std::cout << "ManyAllocTest: Ok" << std::endl;
}

/**

 */
void BigAllocTest() {
    const int BUF_SIZE = 5242880;
    const int ALLOC_SIZE = 524280;
    void *buf = malloc(BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void* test1 = myalloc(ALLOC_SIZE);
    assert((uint64_t)test1 == ((uint64_t)buf + ALLOC_SIZE + sizeof(dataHeader)));

    std::cout << "BigAllocTest: Ok" << std::endl;
}

/**
 */
void OneFreeTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 100;
    void *buf = malloc(BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void* test1;
    test1 = myalloc(ALLOC_SIZE);
    free(test1);
    test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + ALLOC_SIZE + sizeof(dataHeader)));

    std::cout << "OneFreeTest: Ok" << std::endl;
}





int main() {

    OneAllocTest();
    ManyAllocTest();
    BigAllocTest();
    OneFreeTest();


/*    const int BUF_SIZE = 500;
    void *buf = malloc(BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void* test1 = myalloc(430);
    myfree(test1);
    void* test2 = myalloc(1);
    // myfree(test2);
     void* test3 = myalloc(1);
   //  myfree(test3);

    std::cout << "buf: " << static_cast<void*>(buf) << std::endl;
    std::cout << "last pointer: " << static_cast<void*>(test3) << std::endl;*/

    return 0;
}