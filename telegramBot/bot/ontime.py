# -*- coding: utf-8 -*-
import config
import requests
import json
import re



# return [True]  -> you need send mesage !!!
# return [False] -> Action not detected (((
def isActionOntime():
    # get json response
    response = requests.get(config.ontimeUrl)
    data = response.json()

    # print(str(data))
    # print("  ")

    for soffer in data['soffer']:
        if str(soffer['user']['groupclient']['tga']) != 'None':
            print(soffer['user']['groupclient']['tga'])
            print(' ')
            return True
    return False


   


if __name__ == '__main__':
     print(str(isActionOntime()))