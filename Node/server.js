var http = require('http');

var hostname = '127.0.0.1';
var port = '8080';

var server = Http.createServer(function(req, res) {
    res.end('Hello from my node server');
})

server.listen(port, hostname, function(){
    console.log('server is running');
})