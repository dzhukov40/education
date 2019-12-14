#include <iostream>
#include <assert.h>
#include <set>

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

    return (void*)((uint64_t)point + sizeof(dataHeader));
}

inline void* getDataPoint(void *point) {
    return (void*)((uint64_t)point + sizeof(dataHeader));
}

inline  void* newPointBySize(void *point, uint64_t size) {
    return (void*)((uint64_t)point + size);
}

inline uint64_t getFreeSpace(rootMetaData* root, void *point, void *prevPoint) {
    void* localPrevPoint = (prevPoint == NULL) ? root->buf : prevPoint;

    if (prevPoint == NULL && point == NULL) // самый первый кусочек
        return root->size;
    else if (point == NULL)  // дошлли до конца списка
        return (uint64_t)root->endBuf - ((uint64_t)localPrevPoint + ((dataHeader*)localPrevPoint)->size + sizeof(dataHeader));
    if (prevPoint == NULL) // первый элемент в цепочке
        return (uint64_t)point - (uint64_t)root->buf;
    else // где - то между элементами
        return (uint64_t)point - ((uint64_t)localPrevPoint + ((dataHeader*)localPrevPoint)->size + sizeof(dataHeader));
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
    ptr->size = 0; // костыль

    if(ptr->previous == NULL) {
        localRoot->next = nextPtr;
        if (nextPtr != NULL)
            nextPtr->previous = NULL;
    } else {
        dataHeader* previousPtr = castPoint(ptr->previous);
        previousPtr->next = nextPtr;
        if (nextPtr != NULL)
            nextPtr->previous = ptr->previous;
    }
}

void printAllDataHeader(dataHeader* header, int i) {
    if(header == NULL) return;

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

    std::cout << "-- END MEMORY --" << std::endl << std::endl;
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
    void *point = localRoot->next;
    void *prevPoint = NULL;
    bestPlace->freeSpace = UINT64_MAX;

    int i = 1;
    while (i++) { // пробегаемся по списку и ищем лучшее место
        int64_t freeSpace = getFreeSpace(localRoot, point, prevPoint);

        if (freeSpace >= (size + sizeof(dataHeader)) && freeSpace < bestPlace->freeSpace) { // обновляем лучший вариант
            bestPlace->freeSpace = freeSpace;
            bestPlace->next = point;
            bestPlace->previous = prevPoint;
            bestPlace->newPlace = (prevPoint == NULL)
                                  ? localRoot->buf
                                  : newPointBySize(prevPoint, castPoint(prevPoint)->size + sizeof(dataHeader));
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
    if (size < 1) return NULL;

    struct bestChoice bestPlace{};
    findBestChoice(&root, size, &bestPlace); // получить подходящее место для хранения

    if (bestPlace.newPlace == NULL)
        return NULL;

    // создаем мета информацию на занимаемый кусочек
    void* dataPoint = createMetaData(bestPlace.newPlace, size, bestPlace.previous, bestPlace.next);

    //TODO: можно вынести в отдельную ф-ию
    dataHeader* previousPtr = castPoint(bestPlace.previous); // оставим так для отладки
    dataHeader* nextPtr = castPoint(bestPlace.next);
    if (previousPtr != NULL)
        previousPtr->next = bestPlace.newPlace;  // у крайних элементов поменять указатели
    else
        root.next = bestPlace.newPlace;
    if (nextPtr != NULL)
        nextPtr->previous = bestPlace.newPlace;

    // debug
    // std::cout << "## MY MALLOC " << static_cast<void*>(bestPlace.newPlace) << " ##" << std::endl;
    // printMemory(&root);

    return dataPoint;
}

// Функция освобождения
void myfree(void *p)
{
    dataHeader* placeForFree = findAllocatedPlace(&root, p);
    freePlace(&root, placeForFree);

    // debug
    // std::cout << "## MY FREE " << static_cast<void*>((void*)((uint64_t)p - sizeof(dataHeader))) << " ##" << std::endl;
    // printMemory(&root);
}









void checkRangeOfPointer(void *endBuf, void *point) {
    if(point == NULL)
        return;
    assert(endBuf > point);
}

/**
 */
void OneAllocTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 400;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void* test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test1);

    std::cout << "OneAllocTest: Ok" << std::endl;
}

/**
 */
void ManyAllocTest() {
    const int BUF_SIZE = 5000;
    const int ALLOC_SIZE = 5;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void* test1;
    for(int i = 0; i<5; i++) {
        test1 = myalloc(ALLOC_SIZE);
        assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader) + (ALLOC_SIZE + sizeof(dataHeader)) * i));
        checkRangeOfPointer(endBuf, test1);
    }

    std::cout << "ManyAllocTest: Ok" << std::endl;
}

/**
 */
void BigAllocTest() {
    const int BUF_SIZE = 5242880;
    const int ALLOC_SIZE = 5242700;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void* test1 = myalloc(ALLOC_SIZE);
    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test1);

    std::cout << "BigAllocTest: Ok" << std::endl;
}

/**
 */
void outOfRangeTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 500;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);

    void *test1, *test2, *test3, *test4, *test5;

    test1 = myalloc(ALLOC_SIZE);
    assert(test1 == NULL);

    test2 = myalloc((ALLOC_SIZE + 1) - sizeof(dataHeader));
    assert(test2 == NULL);

    test3 = myalloc(ALLOC_SIZE - sizeof(dataHeader));
    assert(test3 != NULL);
    checkRangeOfPointer(endBuf, test3);
    myfree(test3);

    test4 = myalloc((ALLOC_SIZE - 1) - sizeof(dataHeader));
    assert(test4 != NULL);
    checkRangeOfPointer(endBuf, test4);
    myfree(test4);

    test5 = myalloc(0);
    assert(test5 == NULL);
    checkRangeOfPointer(endBuf, test5);

    std::cout << "outOfRangeTest: Ok" << std::endl;
}

/**
 */
void oneFreeTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 100;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void* test1;
    void* test2;
    test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test1);

    myfree(test1);
    test2 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test2 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test2);

    std::cout << "oneFreeTest: Ok" << std::endl;
}

/**
 */
void twoReversFreeTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 10;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void *test1, *test2, *test3;

    test1 = myalloc(ALLOC_SIZE);
    test2 = myalloc(ALLOC_SIZE);
    test3 = myalloc(ALLOC_SIZE);

    checkRangeOfPointer(endBuf, test1);
    checkRangeOfPointer(endBuf, test2);
    checkRangeOfPointer(endBuf, test3);

    myfree(test1);
    myfree(test2);
    myfree(test3);

    test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test1);

    std::cout << "twoReversFreeTest: Ok" << std::endl;
}

/**
 */
void hzTest() {
    const int BUF_SIZE = 500;
    const int ALLOC_SIZE = 10;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void *test1, *test2, *test3;

    test1 = myalloc(ALLOC_SIZE);
    test2 = myalloc(ALLOC_SIZE);
    test3 = myalloc(ALLOC_SIZE);

    checkRangeOfPointer(endBuf, test1);
    checkRangeOfPointer(endBuf, test2);
    checkRangeOfPointer(endBuf, test3);

    myfree(test1);

    test1 = myalloc(ALLOC_SIZE);

    assert((uint64_t)test1 == ((uint64_t)buf + sizeof(dataHeader)));
    checkRangeOfPointer(endBuf, test1);

    std::cout << "hzTest: Ok" << std::endl;
}


/**
 */
void nearEndTest() {
    const int BUF_SIZE = 90;
    const int ALLOC_SIZE = 6;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void *test1, *test2, *test3;

    test1 = myalloc(ALLOC_SIZE);
    test2 = myalloc(ALLOC_SIZE);
    test3 = myalloc(ALLOC_SIZE );

    assert(test3 != NULL);

    checkRangeOfPointer(endBuf, test1);
    checkRangeOfPointer(endBuf, test2);
    checkRangeOfPointer(endBuf, test3);

    std::cout << "nearEndTest: Ok" << std::endl;
}

/**
 */
void fragmentationTest() {
    const int BUF_SIZE = 170;
    const int ALLOC_SIZE = 10;
    void *buf = malloc(BUF_SIZE);
    void *endBuf = (void*)((uint64_t)buf + BUF_SIZE);

    mysetup(buf, BUF_SIZE);
    void *test1, *test2, *test3, *test4, *test5, *test6;

    test1 = myalloc(ALLOC_SIZE);
    test2 = myalloc(ALLOC_SIZE);
    test3 = myalloc(ALLOC_SIZE);
    test4 = myalloc(ALLOC_SIZE);
    test5 = myalloc(ALLOC_SIZE);

    checkRangeOfPointer(endBuf, test1);
    checkRangeOfPointer(endBuf, test2);
    checkRangeOfPointer(endBuf, test3);
    checkRangeOfPointer(endBuf, test4);
    checkRangeOfPointer(endBuf, test5);

    assert(test5 != NULL);

    myfree(test4);
    myfree(test2);
    myfree(test1);
    myfree(test5);
    myfree(test3);

    test1 = myalloc(ALLOC_SIZE);
    test2 = myalloc(ALLOC_SIZE);
    test3 = myalloc(ALLOC_SIZE);
    test4 = myalloc(ALLOC_SIZE);
    test6 = myalloc(ALLOC_SIZE);

    checkRangeOfPointer(endBuf, test1);
    checkRangeOfPointer(endBuf, test2);
    checkRangeOfPointer(endBuf, test3);
    checkRangeOfPointer(endBuf, test4);
    checkRangeOfPointer(endBuf, test6);

    assert(test5 != NULL);
    assert(test5 == test6);

    std::cout << "fragmentationTest: Ok" << std::endl;
}













int main() {

    OneAllocTest();
    ManyAllocTest();
    BigAllocTest();
    outOfRangeTest();
    oneFreeTest();
    twoReversFreeTest();
    hzTest();
    nearEndTest();
    fragmentationTest();



    return 0;
}