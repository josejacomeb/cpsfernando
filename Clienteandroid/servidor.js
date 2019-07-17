var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var numeroconectados = 0;
app.get('/', function(req, res) {
  res.sendFile(__dirname + '/public/index.html');
});

io.on('connection', function(socket) {
  numeroconectados += 1;
  console.log("<<Usuario conectado>> Usuarios en linea: " + numeroconectados);
  socket.on('disconnect', function() {
    numeroconectados -= 1;
    console.log('<<Usuario desconectado>> Usuarios en linea: ' + numeroconectados);
  });
  var booleano = 0;
  socket.on('motor', function(msg) {
    console.log('motor ' + msg);
    socket.broadcast.emit('motor', msg);
  });
  socket.on('led', function(msg) {
    console.log('led ' + msg);
    socket.broadcast.emit('led', msg);
  });
  socket.on('ultrasonico', function(msg) {
    console.log('ultrasonico ' + msg);
    socket.broadcast.emit('ultrasonico', msg);
  });
  socket.on('piston', function(msg) {
    console.log('piston ' + msg);
    socket.broadcast.emit('piston', msg);
  });
});

http.listen(3000, function() {
  console.log('listening on *:3000');
});
