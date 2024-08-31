


## build
npm install

## run (front)
node server.js

## open for internet (front server)
ssh -R 80:127.0.0.1:3000 serveo.net


## use peer.html !!!
extreamly simple lib for make serverless comunication!!!


## instructions (run on different devices or websites)
1. open site on first and second device.
2. puch "Create Offer" on first device.
3. coppy generated JSON and put to second device to
"Paste Remote Offer/Answer and ICE Candidates Here:"
4. on second device push "Set Remote Description"
5. copy generated JSON from second device and put to first device
6. on first device push "Set Remote Description"
7. now we can send/received messegeg !!!
8. tested!! works!!
