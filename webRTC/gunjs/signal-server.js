const Gun = require('gun');
const express = require('express');
const app = express();

// Serve static files if needed
app.use(express.static('public'));

// Create a GUN instance and bind to the Express server
const server = app.listen(8765);
Gun({ web: server });

console.log('GUN server running on port 8765');
