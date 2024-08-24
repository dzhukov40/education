const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

const sessions = new Set();

wss.on('connection', function connection(ws) {
    console.log('WS new connection');
    sessions.add(ws);

    ws.on('message', function incoming(message) {
        // Broadcast to all clients except the sender
        console.log('WS broadcast message');
        sessions.forEach(function each(client) {
            if (client !== ws && client.readyState === WebSocket.OPEN) {
                client.send(message);
            }
        });
    });

    ws.on('close', function close() {
        console.log('WS closed');
        sessions.delete(ws);
    });
});

console.log('WebSocket server is running on ws://localhost:8080');
