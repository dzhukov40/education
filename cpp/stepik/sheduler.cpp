#include <iostream>
#include <assert.h>
#include <queue>
#include <map>

using namespace std;

struct thread {
    int id;
    int timeSliceCount;    // четчик времени выполнения потока
    bool blocked;          // true - заблокирон
};

struct sheduler {
    map <int, thread*> threadMap;
    queue<thread*> threadQueue;
    size_t timeSlice;

    thread* currentThread;
};
static sheduler mySheduler;


thread* getNextThread() {
    thread* nextThread = NULL;
    if(!mySheduler.threadQueue.empty()) {
        nextThread = mySheduler.threadQueue.front();
        mySheduler.threadQueue.pop();
        nextThread->timeSliceCount = mySheduler.timeSlice;
    }

    return nextThread;
}


/**
 * Функция будет вызвана перед каждым тестом, если вы
 * используете глобальные и/или статические переменные
 * не полагайтесь на то, что они заполнены 0 - в них
 * могут храниться значения оставшиеся от предыдущих
 * тестов.
 *
 * timeSlice - квант времени, который нужно использовать.
 * Поток смещается с CPU, если пока он занимал CPU функция
 * timer_tick была вызвана timeSlice раз.
 **/
void scheduler_setup(int timeslice)
{
    while(!mySheduler.threadQueue.empty()) {
        getNextThread();
    }

    map<int, thread*>::iterator it;
    for ( it = mySheduler.threadMap.begin(); it != mySheduler.threadMap.end(); it++ ) {
        delete it->second;
        it->second = NULL;
    }
    mySheduler.threadMap.clear();


    mySheduler.timeSlice = timeslice;

    if(mySheduler.currentThread != NULL) {
        mySheduler.currentThread = NULL;
    }
}

/**
 * Функция вызывается, когда создается новый поток управления.
 * thread_id - идентификатор этого потока и гарантируется, что
 * никакие два потока не могут иметь одинаковый идентификатор.
 **/
void new_thread(int thread_id)
{
    if (mySheduler.threadMap.find(thread_id) == mySheduler.threadMap.end()) {
        thread *newThread = new thread();

        newThread->id = thread_id;
        newThread->timeSliceCount = mySheduler.timeSlice;
        newThread->blocked = false;

        mySheduler.threadMap.insert(std::make_pair(thread_id, newThread));
        mySheduler.threadQueue.push(newThread);

        if(mySheduler.currentThread == NULL) {
            mySheduler.currentThread = getNextThread();
        }
    } else {
        std::cerr << "this thread_id already exists" << std::endl;
    }
}

/**
 * Функция вызывается, когда поток, исполняющийся на CPU,
 * завершается. Завершится может только поток, который сейчас
 * исполняется, поэтому thread_id не передается. CPU должен
 * быть отдан другому потоку, если есть активный
 * (незаблокированный и незавершившийся) поток.
 **/
void exit_thread()
{
    if(mySheduler.currentThread != NULL) {
        mySheduler.threadMap.erase(mySheduler.currentThread->id);
        delete mySheduler.currentThread;

        mySheduler.currentThread = getNextThread();
    }
}

/**
 * Функция вызывается, когда поток, исполняющийся на CPU,
 * блокируется. Заблокироваться может только поток, который
 * сейчас исполняется, поэтому thread_id не передается. CPU
 * должен быть отдан другому активному потоку, если таковой
 * имеется.
 **/
void block_thread()
{
    // при блокировке потока больше нет в очереди но он есть в мапе
    if(mySheduler.currentThread != NULL) {
        mySheduler.currentThread->blocked = true;
        mySheduler.currentThread = getNextThread();
    }
}

/**
 * Функция вызывается, когда один из заблокированных потоков
 * разблокируется. Гарантируется, что thread_id - идентификатор
 * ранее заблокированного потока.
 **/
void wake_thread(int thread_id)
{
    // возвращаем обьект в очередь
    if (mySheduler.threadMap.find(thread_id) != mySheduler.threadMap.end()) {
        thread* thread = mySheduler.threadMap[thread_id];
        thread->blocked = false;
        mySheduler.threadQueue.push(thread);

        if(mySheduler.currentThread == NULL)
            mySheduler.currentThread = getNextThread();
    } else {
        std::cerr << "this thread_id not exists. Can not wake thread" << std::endl;
    }
}

/**
 * Ваш таймер. Вызывается каждый раз, когда проходит единица
 * времени.
 **/
void timer_tick()
{
    if(mySheduler.currentThread == NULL)
        mySheduler.currentThread = getNextThread();

    if(mySheduler.currentThread != NULL) {
        mySheduler.currentThread->timeSliceCount--;

        if(mySheduler.currentThread->timeSliceCount < 1) {
            mySheduler.threadQueue.push(mySheduler.currentThread);
            mySheduler.currentThread = getNextThread();
        }
    }

}

/**
 * Функция должна возвращать идентификатор потока, который в
 * данный момент занимает CPU, или -1 если такого потока нет.
 * Единственная ситуация, когда функция может вернуть -1, это
 * когда нет ни одного активного потока (все созданные потоки
 * либо уже завершены, либо заблокированы).
 **/
int current_thread()
{
    return mySheduler.currentThread == NULL ? -1 : mySheduler.currentThread->id;
}








void testVoidTimerTick() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    assert(current_thread() == -1);
    timer_tick();
    assert(current_thread() == -1);

    std::cout << "testVoidTimerTick: Ok" << std::endl;
}

void testTimerTick() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    assert(current_thread() == -1);
    new_thread(1);
    assert(current_thread() ==  1);

    timer_tick();
    assert(current_thread() ==  1);
    timer_tick();

    assert(current_thread() ==  1);

    std::cout << "testTimerTick: Ok" << std::endl;
}

void testTimerTickWithTwoThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    assert(current_thread() ==  1);
    new_thread(2);
    assert(current_thread() ==  1);

    for(int i = 0; i < 3; i++)
        timer_tick();

    assert(current_thread() ==  2);

    std::cout << "testTimerTickWithTwoThread: Ok" << std::endl;
}

void testTimerTickWithThreeThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    assert(current_thread() ==  1);
    new_thread(2);
    assert(current_thread() ==  1);

    for(int i = 0; i < 3; i++)
        timer_tick();

    assert(current_thread() ==  2);

    new_thread(3);
    timer_tick();
    assert(current_thread() ==  1);

    std::cout << "testTimerTickWithThreeThread: Ok" << std::endl;
}

void testTimerTickWithBlockThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    timer_tick();
    block_thread();
    timer_tick();
    timer_tick();
    timer_tick();

    assert(current_thread() == -1);
    wake_thread(1);
    assert(current_thread() ==  1);

    timer_tick();
    assert(current_thread() == 1);

    std::cout << "testTimerTickWithBlockThread: Ok" << std::endl;
}

void testTimerTickWithExitThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    timer_tick();

    assert(current_thread() ==  1);
    exit_thread();
    assert(current_thread() == -1);

    std::cout << "testTimerTickWithExitThread: Ok" << std::endl;
}

void testBlockExitThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    timer_tick();

    block_thread();
    exit_thread();

    assert(current_thread() == -1);
    wake_thread(1);
    assert(current_thread() ==  1);

    std::cout << "testBlockExitThread: Ok" << std::endl;
}

void testBigTimeSlice() {
    const int TIME_SLICE = 50;
    scheduler_setup(TIME_SLICE);

    new_thread(0);
    new_thread(1);
    new_thread(2);
    new_thread(3);
    new_thread(4);
    new_thread(5);
    new_thread(6);
    new_thread(7);
    new_thread(8);
    new_thread(9);

    assert(current_thread() ==  0);

    for (int i = 0; i < 150; i++)
        timer_tick();

    assert(current_thread() ==  3);

    std::cout << "testBigTimeSlice: Ok" << std::endl;
}

void testWithManyThread() {
    const int TIME_SLICE = 1;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    new_thread(2);
    new_thread(3);
    new_thread(4);

    block_thread();
    block_thread();

    assert(current_thread() ==  3);

    timer_tick();
    assert(current_thread() ==  4);

    new_thread(5);
    new_thread(6);

    assert(current_thread() ==  4);
    exit_thread();
    assert(current_thread() ==  3);

    wake_thread(2);
    timer_tick();
    assert(current_thread() ==  5);
    timer_tick();
    assert(current_thread() ==  6);
    timer_tick();
    assert(current_thread() == 2);

    wake_thread(1);
    assert(current_thread() ==  2);
    exit_thread();
    assert(current_thread() == 3);

    std::cout << "testWithManyThread: Ok" << std::endl;
}

void testFromStepik() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(0);
    block_thread();
    timer_tick();
    new_thread(1);
    block_thread();
    timer_tick();
    wake_thread(0);
    new_thread(2);
    block_thread();
    timer_tick();
    wake_thread(1);
    new_thread(3);
    block_thread();
    timer_tick();

    assert(current_thread() ==  1);

    std::cout << "testFromStepik: Ok" << std::endl;
}


void testExit() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    assert(current_thread() == -1);

    exit_thread();
    exit_thread();

    assert(current_thread() == -1);

    block_thread();
    block_thread();

    assert(current_thread() == -1);

    new_thread(0);
    assert(current_thread() ==  0);

    exit_thread();
    assert(current_thread() == -1);
    new_thread(1);
    timer_tick();
    block_thread();

    assert(current_thread() == -1);
    exit_thread();
    assert(current_thread() == -1);
    wake_thread(1);
    assert(current_thread() ==  1);

    std::cout << "testExit: Ok" << std::endl;
}







int main() {

    testVoidTimerTick();
    testTimerTick();
    testTimerTickWithTwoThread();
    testTimerTickWithThreeThread();
    testTimerTickWithBlockThread();
    testTimerTickWithExitThread();
    testBlockExitThread();
    testBigTimeSlice();
    testWithManyThread();
    testFromStepik();
    testExit();


    return 0;
}
