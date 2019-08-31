# -*- coding: utf-8 -*-
#
#  1) устанавливаеем pyton
#   - [ yum install python ]
#
#  2) устанавливаем pip
#   - качаем файл с [ https://bootstrap.pypa.io/get-pip.py ]
#   - запускаем [ sudo python get-pip.py ]
#
#  3) устанавливаем библиотеку для работы 
#   - [ sudo pip install pytelegrambotapi ]
#
#  4) импортим библиотеку
#   - [ python ]
#   - [ >>>  import telebot ] -> [enter] -> [ quit(); ]
#

#   зарегать бота надо:
#     идем @botfather и регаем)))  
#     [ doneathome ]
#     [ doneathome_bot ] 
#     получаем [token] -> 
#     -> [416385445:AAEZI0_oRTg7FC5d4M0fWQqdYBEBvs6UBoI]
#
# token - это то, что вернул Вам @BotFather при регистрации бота
token = '416385445:AAEZI0_oRTg7FC5d4M0fWQqdYBEBvs6UBoI'


ontimeUrl = 'https://ontime.online/api/link/ee9efd7a-51e8-4b71-9c21-a2844b065c17/'


fileInitName = 'initOntime.txt'


timePause = '60'

helpMessage = 'Этот бот поможет тебе купить еду за 1 р' + '\n(опрос с 9-00 по 10-00 каждые 60 сек)'

commandList = 'для работы с ботом есть команды: \n /help - справка \n /start - подписаться на уведоления о начале акции \n /stop - отписаться от уведомлений \n /check - проверить что вы подписаны и посмотреть сколько других участников'