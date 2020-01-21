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
    mySheduler.timeslice = timeslice;
    mySheduler.currentThread = NULL;
}

/**
 * Функция вызывается, когда создается новый поток управления.
 * thread_id - идентификатор этого потока и гарантируется, что
 * никакие два потока не могут иметь одинаковый идентификатор.
 **/
void new_thread(int thread_id)
{
    thread *newThread = new thread;

    newThread->id = thread_id;
    newThread->timesliceCount = mySheduler.timeslice;
    newThread->blocked = false;
    newThread->ended = false;

    mySheduler.threadMap.insert(std::make_pair(thread_id, newThread));
    mySheduler.threadQueue.push(newThread);

    if(mySheduler.currentThread == NULL) {
        mySheduler.currentThread = mySheduler.threadQueue.front();
        mySheduler.threadQueue.pop();
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
    mySheduler.threadMap.erase(mySheduler.currentThread->id);
    mySheduler.currentThread = mySheduler.threadQueue.front();
    mySheduler.threadQueue.pop();
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
    mySheduler.currentThread->blocked = true;
    mySheduler.threadQueue.push(mySheduler.currentThread);

    mySheduler.currentThread = mySheduler.threadQueue.front();
    mySheduler.threadQueue.pop();
}

/**
 * Функция вызывается, когда один из заблокированных потоков
 * разблокируется. Гарантируется, что thread_id - идентификатор
 * ранее заблокированного потока.
 **/
void wake_thread(int thread_id)
{
    mySheduler.threadMap[thread_id]->blocked = false;
}

/**
 * Ваш таймер. Вызывается каждый раз, когда проходит единица
 * времени.
 **/
void timer_tick()
{
   if(mySheduler.currentThread != NULL) {
       mySheduler.currentThread->timesliceCount--;

       if(mySheduler.currentThread->timesliceCount == 0) {
           mySheduler.threadMap.erase(mySheduler.currentThread->id);
           mySheduler.currentThread = mySheduler.threadQueue.front();
           mySheduler.threadQueue.pop();
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

    for(int i = 0; i < 3; i++)
        timer_tick();

    std::cout << "testVoidTimerTick: Ok" << std::endl;
}

void testTimerTick() {
    const int TIME_SLICE = 2;
    scheduler_setup(TIME_SLICE);

    new_thread(1);

    for(int i = 0; i < 3; i++)
        timer_tick();

    std::cout << "testTimerTick: Ok" << std::endl;
}






int main() {

    // testTimerTick();
    testTimerTick();




    return 0;
}
