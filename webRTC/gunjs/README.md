


## build
npm install

## run (front)
node server.js

## open for internet (front server)
ssh -R 80:127.0.0.1:3000 serveo.net





## !!!! signal server work by web-socket.
### free tunnels not work with web-socket

## run (signal server)
node signal-server.js

## open for internet (signal server)
ngrok http 8765

## put from internet avaleble server link to front
-> public/index.html "... peers: ['http:localhost:8765/gun'] ..."







## (*) instal util
curl -s https://ngrok-agent.s3.amazonaws.com/ngrok.asc | \
  sudo gpg --dearmor -o /etc/apt/keyrings/ngrok.gpg && \
  echo "deb [signed-by=/etc/apt/keyrings/ngrok.gpg] https://ngrok-agent.s3.amazonaws.com buster main" | \
  sudo tee /etc/apt/sources.list.d/ngrok.list && \
  sudo apt update && sudo apt install ngrok