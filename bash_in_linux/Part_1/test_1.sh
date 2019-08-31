#!/bin/bash

# определяем кто нас запускает!

ROOT_UID=0     # Только пользователь с $UID 0 имеет привилегии root.
E_NOTROOT=67   # Признак отсутствия root-привилегий. (завели такой код)


if [ "$UID" -ne "$ROOT_UID" ]
then
  echo "Для работы сценария требуются права root."
  exit $E_NOTROOT
else
  echo "Привет root"
fi


