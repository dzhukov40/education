# -*- coding: utf-8 -*-
import config
import ontime
# https://github.com/eternnoir/pyTelegramBotAPI/#getting-started
import telebot
import time

import threading


# это множество содержит id активных юзеров
users = set()


LOCK = threading.RLock()


bot = telebot.TeleBot(config.token)




@bot.message_handler(commands=['help'])
def repeat_all_messages(message):  
    bot.send_message(message.chat.id, config.helpMessage)


@bot.message_handler(commands=['check'])
def repeat_all_messages(message): 
    global users
    global LOCK

    id = int(message.chat.id)
    LOCK.acquire()
    bot.send_message(id, 'count active users = ' + str(len(users)))
    if id in users:
        bot.send_message(id, "вы добавлены, не волнуйся))")
    else:
        bot.send_message(id, "вы не подписаны!")
    LOCK.release() # отпираем замок




@bot.message_handler(commands=['start'])
def repeat_all_messages(message): 
    global users
    global LOCK

    id = int(message.chat.id)
    LOCK.acquire()
    if id in users:
        bot.send_message(id, "вы уже были добавлены, не нерничай))")
    else:   
        users.add(id)
        bot.send_message(id, "вы добавлены, теперь то не пропустишь))")
    LOCK.release() # отпираем замок


@bot.message_handler(commands=['stop'])
def repeat_all_messages(message): 
    global users
    global LOCK

    id = int(message.chat.id)
    LOCK.acquire() # накидываем блокировку
    if message.chat.id in users:
        users.remove(id)
        bot.send_message(id, "мониторинг для вас остановлен")
    else:   
    	bot.send_message(id, "мы и не запускали))")
    LOCK.release() # отпираем замок


# должен быть в конце, так как является 
# самым общим а проверка на совпадение идет по очереди
@bot.message_handler(content_types=["text"])
def repeat_all_messages(message): 
    bot.send_message(message.chat.id, config.commandList)




# python /home/dzhukov/My/education/telegramBot/bot/doneathome.py



workerFlag = 1

def worker():

    # loop
    while bool(workerFlag):
        doCheck()
        time.sleep(int(config.timePause))


def doCheck():
    global users
    global LOCK
    # get time
    timeNow = time.localtime()

    # chech time and send mesage
    if timeNow.tm_hour == 9 or timeNow.tm_hour == 10:
        if ontime.isActionOntime():
            #print("SEND!")
            LOCK.acquire() # накидываем блокировку
            for user in users:
                bot.send_message(user,"!!! ВСЕ ПО РУБЛЮ !!!")
            LOCK.release() # отпираем замок
        else:
            # for user in users:
            #     bot.send_message(user,'проверка (учти промежуток опроса)')
            pass



def main():
    global users
    global workerFlag

    thread_ = threading.Thread(target=worker)
    print('start worker')
    thread_.start()

    print('start bot')
    bot.polling(none_stop=True) # это запуск бота
    print('Завершение, необходимо подождать ' + str(config.timePause) + ' секунд')
    workerFlag = 0



if __name__ == '__main__':
	 main()
     # bot.polling(none_stop=True) # это запуск бота
     