/**
 * Setup the http server 
 * The variable clientCode is the code above and
 * can be set in this file or you could read it in from 
 * a file. See the node.js documentation for 
 * filesystem interaction
 */
var http = require('http');
httpServer = http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end(clientCode);
});
httpServer.listen(1337, "127.0.0.1");

/**
 * Add Now.js
 */
var nowjs = require('now');
var everyone = nowjs.initialize(httpServer);

/**
 * Receive incoming messages and distribute them
 */
everyone.now.sendMessage = function(message) {
  everyone.now.receiveMessage(message);
}
