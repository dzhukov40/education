#!/bin/bash 

# run example:  ./stressTest.sh 10


for ((i=1; i<=$1; i++)); 
do
  curl -XGET "http://localhost:9090/myapp.access"  -d 'json={"event":"data"}'
  echo "$i"
done
