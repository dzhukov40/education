#!/usr/bin/env bash




curl -XPUT 'https://sttslgels01.corp.dev.vtb:9200/test/user/1' \
  -H 'Content-Type: application/json' \
  -u system-administrator:password \
  --insecure \
  --key admin.pem \
  --cert admin.crt \
  -d '{
      "name":"John",
      "last_name" :"Smith",
      "age" :25,
      "about" :"I love to go rock climbing",
      "interests": [ "sports", "music" ]
  }'