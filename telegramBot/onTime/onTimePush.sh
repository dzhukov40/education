#!/bin/bash




getCommand="curl -s https://ontime.online/api/link/ee9efd7a-51e8-4b71-9c21-a2844b065c17/  | grep -ioe .tga.:null}"
getRezult=""
testResult=""






if [[ $1 = init ]]
then
  eval "$getCommand" 1> ./tort.txt
  exit 0 
fi



sendPush()
{

   TITLE="onTime"
   MESSAGE=$1

   APP_TOKEN="aaoqjupbb51he5q6mn83jev5dm3zc4"
   USER_TOKEN="u12dvub4nhs5p6f6n95wb8hmhuximt"

wget https://api.pushover.net/1/messages.json --post-data="token=$APP_TOKEN&user=$USER_TOKEN&message=$MESSAGE&title=$TITLE" -qO- > /dev/null 2>&1 &
}


getRez()
{
    eval "$getCommand" 1> ./tort2.txt
    
 
    if diff ./tort.txt ./tort2.txt 1> /dev/null 2> /dev/null
      then
        echo "equal"
        #sendPush "проверка"
      else
        echo "send push"
        sendPush "еда за 1"
    fi

}



timeChasCommand="date | grep -ioe [0-9][0-9]:..:.. | grep -ioe ^.." 
timeMinCommand="date | grep -ioe [0-9][0-9]:..:.. | grep -ioe :..: | grep -ioe [0-9][0-9]"



timePoint()
{

   eval "$timeChasCommand" 1> timeResultCommand.txt
   T=`cat ./timeResultCommand.txt`

   echo "$T"

   if [[ $T -eq "9" || $T -eq "10" ]]
   then

     eval "$timeMinCommand" 1> timeResultCommand.txt
     T=`cat ./timeResultCommand.txt`

     echo "$T"

     return 0   

   fi

   return 1
}


#-- RUN --

timeStenp="55"

echo "start monitoring ->"
sendPush "start monitoring ->"


while true; do

   sleep $timeStenp
   
   echo " do chech"

   if timePoint
   then 
     getRez
   fi
	

done

