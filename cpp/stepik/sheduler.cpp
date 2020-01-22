#include <iostream>
#include <assert.h>
#include <queue>
#include <map>

using namespace std;

struct thread {
    int id;
    int timesliceCount;    // четчик времени выполнения потока
    bool blocked;          // true - заблокирон
    bool ended;            // true - поток закончен
};


struct sheduler {
    map <int, thread*> threadMap;
    queue<thread*> threadQueue;
    size_t timeslice;

    thread* currentThread;
};


static sheduler mySheduler;



thread* getNextThread() {
    thread* nextThread = NULL;
    if(!mySheduler.threadQueue.empty()) {
        nextThread = mySheduler.threadQueue.front();
        mySheduler.threadQueue.pop();
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
 * timeslice - квант времени, который нужно использовать.
 * Поток смещается с CPU, если пока он занимал CPU функция
 * timer_tick была вызвана timeslice раз.
 **/
void scheduler_setup(int timeslice)
{
    map<int, thread*>::iterator it;
    for ( it = mySheduler.threadMap.begin(); it != mySheduler.threadMap.end(); it++ ) {
        delete it->second;
    }
    mySheduler.threadMap.clear();

    int debugValueOfSize = mySheduler.threadQueue.size();
    bool isEmpty = mySheduler.threadQueue.empty();
    while(!mySheduler.threadQueue.empty()) {
        getNextThread();
    }

    mySheduler.timeslice = timeslice;

    if(mySheduler.currentThread != NULL) {
        delete mySheduler.currentThread;
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
    thread *newThread = new thread();

    //TODO: епонятный баг, если у нас одна аллокация, то возвращается обьект который есть в мапе уже, и меняем у него поля, ШОК
    thread *newThread1 = new thread();
    thread *newThread2= new thread();

    newThread->id = thread_id;
    newThread->timesliceCount = mySheduler.timeslice;
    newThread->blocked = false;
    newThread->ended = false;

    mySheduler.threadMap.insert(std::make_pair(thread_id, newThread));
    mySheduler.threadQueue.push(newThread);

    if(mySheduler.currentThread == NULL) {
        mySheduler.currentThread = getNextThread();
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
    mySheduler.currentThread->blocked = true;
    mySheduler.currentThread = getNextThread();
}

/**
 * Функция вызывается, когда один из заблокированных потоков
 * разблокируется. Гарантируется, что thread_id - идентификатор
 * ранее заблокированного потока.
 **/
void wake_thread(int thread_id)
{
    // возвращаем обьект в очередь
    thread* thread = mySheduler.threadMap[thread_id];
    thread->blocked = false;
    mySheduler.threadQueue.push(thread);

    if(mySheduler.currentThread == NULL)
        mySheduler.currentThread = getNextThread();
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
       mySheduler.currentThread->timesliceCount--;

       if(mySheduler.currentThread->timesliceCount == 0) {
           mySheduler.threadMap.erase(mySheduler.currentThread->id);
           delete mySheduler.currentThread;

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








void checkThreadId(int id) {
    int threadId = current_thread();
    assert(threadId == id);
}

void testVoidTimerTick() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    for(int i = 0; i < 3; i++)
        timer_tick();

    std::cout << "testVoidTimerTick: Ok" << std::endl;
}

void testTimerTick() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    checkThreadId(-1);
    new_thread(1);
    checkThreadId(1);

    for(int i = 0; i < 2; i++)
        timer_tick();

    checkThreadId(-1);

    std::cout << "testTimerTick: Ok" << std::endl;
}

void testTimerTickWithTwoThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    checkThreadId(1);
    new_thread(2);
    checkThreadId(1);

    for(int i = 0; i < 3; i++)
        timer_tick();

    checkThreadId(2);

    std::cout << "testTimerTickWithTwoThread: Ok" << std::endl;
}

void testTimerTickWithThreeThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    checkThreadId(1);
    new_thread(2);
    checkThreadId(1);

    for(int i = 0; i < 3; i++)
        timer_tick();

    checkThreadId(2);

    new_thread(3);
    timer_tick();
    checkThreadId(3);




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

    checkThreadId(-1);
    wake_thread(1);
    checkThreadId(1);

    timer_tick();

    checkThreadId(-1);

    std::cout << "testTimerTickWithBlockThread: Ok" << std::endl;
}

void testTimerTickWithExitThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    timer_tick();

    checkThreadId(1);
    exit_thread();
    checkThreadId(-1);

    std::cout << "testTimerTickWithExitThread: Ok" << std::endl;
}

void testBlockExitThread() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);
    timer_tick();

    block_thread();
    exit_thread();

    checkThreadId(-1);
    wake_thread(1);
    checkThreadId(1);

    std::cout << "testBlockExitThread: Ok" << std::endl;
}



int main() {

    testVoidTimerTick();
    testTimerTick();
    testTimerTickWithTwoThread();
    testTimerTickWithThreeThread();
    testTimerTickWithBlockThread();
    testTimerTickWithExitThread();
    testBlockExitThread();


    return 0;
}
