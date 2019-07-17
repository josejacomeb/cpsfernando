var express = require('express');
var path = require('path');
var favicon = require('static-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var routes = require('./routes/index');
var users = require('./routes/users');
var fs = require('fs'); //require filesystem module
var mongo = require('mongodb');     ///Añadido base de datos MongoDB
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";
var numeroconectados = 0;
var numeroimagenes = 0;
var app = express();
var server = require('http').Server(app);
var io = require('socket.io')(server);
var imagenes = {//Donde guardar las imagenes
    nombre: "",
    nombreprocesada: "",
    fecha: new Date(),
    errores: "",
    numerodientes: 0,
    modulo: 0.0,
    paso: 0.0,
    diametroexterno: 0.0
};
var totalimagenes = [];

var valormotorservidor = 0;
var valorpresenciaservidor = false;
var valorclasificadorservidor = false;
var valorestadoservidor = 0;
var rutaimagen = "/images/Logo_UPC.png";
var rutaimagenprocesada = "";
//INICIO PARTE MANUAL
var modofuncservidor = false;
var sentidomotorservidor = 0;
var direcclasificacionservidor = 0;
var capturarengraneservidor = 0;
var umbralservidor = 0;
//FIN PARTE MANUAL

var dispositivo = {
    dispositivo: 1,
    motor: 255,
    presencia: false,
    estado: -1,
    imagenprocesada: false,
    clasificador: false,
    modofunc: false,
    ////
    //
//INICIO PARTE MANUAL
    //añado la parte manual

    sentidomotor: 1,
    direcclasificacion: 1,
    capturarengrane: false,
    umbral: 100
            //FIN PARTE MANUAL
};


var ledservidor = false;
var equipo2 = {
    equipo2: 1,
    led: false
};
var arreglodispositivo = []; // inicia en forma de vector
var fechaimagen;
var errores;
var obj;  //Variable para leer JSON

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

app.use(favicon());
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded());
app.use(cookieParser());
app.use(require('less-middleware')({src: path.join(__dirname, 'public')}));
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);

/// catch 404 and forwarding to error handler
app.use(function (req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

/// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use(function (err, req, res, next) {
        res.status(err.status || 500);
        res.render('error', {
            message: err.message,
            error: err
        });
    });
}

// production error handler
// no stacktraces leaked to user
app.use(function (err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
        message: err.message,
        error: {}
    });
});
//Cargo el JSON si este existe
fs.readFile(__dirname + '/public/images/imagenes.json', 'utf8', function (err, data) {
    if (!err) {
        console.log("Leyendo");
        obj = JSON.parse(data);
        numeroimagenes = obj.length - 1;
        for (var x = 0; x < obj.length; x++) {
            totalimagenes.push(obj[x]);
        }
        rutaimagenprocesada = totalimagenes[totalimagenes.length - 1].nombreprocesada;
        console.log("Finalizo de leer");
    } else {
        numeroimagenes = 0;
        totalimagenes = [];
        rutaimagenprocesada = rutaimagen;
    }
    console.log("Acabo");
});
io.on('connection', function (socket) {
    numeroconectados += 1;
    console.log("<<Usuario conectado>> Usuarios en linea: " + numeroconectados);
    if (dispositivo.motor != null) {
        dispositivo.motor = valormotorservidor;
    }
    if (dispositivo.clasificador != null) {
        dispositivo.clasificador = valorclasificadorservidor;
    }
    if (dispositivo.estado != null) {
        dispositivo.estado = valorestadoservidor;
    }
    if (dispositivo.presencia != null) {
        dispositivo.presencia = valorpresenciaservidor;
    }
    if (dispositivo.modofunc != null) {
        dispositivo.modofunc = modofuncservidor;
    }
//INICIO PARTE MANUAL
    //CONEXION A LA LIBRERIA SOCKETIO PARTE MANUAL

    if (dispositivo.sentidomotor != null) {
        dispositivo.sentidomotor = sentidomotorservidor;
    }
    if (dispositivo.direcclasificacion != null) {
        dispositivo.direcclasificacion = direcclasificacionservidor;
    }
    if (dispositivo.capturarengrane != null) {
        dispositivo.capturarengrane = capturarengraneservidor;
    }
    if (dispositivo.umbral != null) {
        dispositivo.umbral = umbralservidor;
    }
//FIN PARTE MANUAL

    //segundo sistema
    if (equipo2.led != null) {
        equipo2.led = ledservidor;
    }


    arreglodispositivo.push(dispositivo);
    dispositivo = {};
    arreglodispositivo.push(equipo2);
    equipo2 = {};
    socket.emit("nuevousuario", arreglodispositivo);
    arreglodispositivo = [];
    socket.emit("lastimagen", rutaimagenprocesada);
    socket.on('disconnect', function () {
        numeroconectados -= 1;
        console.log('<<Usuario desconectado>> Usuarios en linea: ' + numeroconectados);
    });

    socket.on("datos", function (data) {
        console.log("*****Datos********");
        console.log(data);
        var sizedatos = data.length;
        console.log("Tamaño de datos: " + sizedatos);
        for (i = 0; i < sizedatos; i++) {
            if (valormotorservidor != null) {
                valormotorservidor = data[i].motor;
            }
            if (valorpresenciaservidor != null) {
                valorpresenciaservidor = data[i].presencia;
            }
            if (valorclasificadorservidor != null) {
                valorclasificadorservidor = data[i].clasificador;
            }
            if (valorestadoservidor != null) {
                valorestadoservidor = data[i].estado;
            }
            if (modofuncservidor != null) {
                modofuncservidor = data[i].modofunc;
            }
            if (typeof data[i].fecha !== 'undefined') {
                console.log("data i fecha");
                console.log(data[i].fecha);
                var fechaservidor = new Date();
                var fechacliente = new Date(data[i].fecha);
                var stringlatencia = Math.abs(fechacliente.getTime() - fechaservidor.getTime());
                var jsonlatencia = {};
                console.log("Fecha cliente: " + JSON.stringify(fechacliente));
                console.log("Fecha servidor: " + JSON.stringify(fechaservidor));
                jsonlatencia.fechacliente = fechacliente.toISOString();
                jsonlatencia.fechaservidor = fechaservidor.toISOString();
                jsonlatencia.latencia = stringlatencia;
                console.log(jsonlatencia);
                socket.broadcast.emit('latenciaentrada', JSON.stringify(jsonlatencia));
                socket.emit('latenciaentrada', JSON.stringify(jsonlatencia));
            }
            //INICIO PARTE MANUAL
            //añado conexion ṕarte manual 

            if (sentidomotorservidor != null) {
                sentidomotorservidor = data[i].sentidomotor;
            }

            if (direcclasificacionservidor != null) {
                direcclasificacionservidor = data[i].direcclasificacion;
            }

            if (capturarengraneservidor != null) {
                capturarengraneservidor = data[i].capturarengrane;
            }

            if (umbralservidor != null) {
                umbralservidor = data[i].umbral;
            }
//FIN PARTE MANUAL

            // AÑADO DE UN SEGUNDO SISTEMA
            if (ledservidor != null) {
                ledservidor = data[i].led;
            }
        }
        var fechaserver = new Date();
        data.fecha = fechaserver.toISOString();
        socket.broadcast.emit('datos', data); //Envia a todos los clientes
        console.log("Enviando datos");

    });

    socket.on('imagen', function (data) {
        console.log("Llego una imagen");
        fechaimagen = new Date(data.fecha);
        //errores = data.errores;
        console.log("Llego una imagen creada en: " + fechaimagen);
        rutaimagenprocesada = "/images/extradata/imagenp" + numeroimagenes + "." + data.extension;
        imagenes.nombreprocesada = rutaimagenprocesada;
        imagenes.nombre = "/images/extradata/imagen" + numeroimagenes + "." + data.extension;
        imagenes.fecha = fechaimagen;
        imagenes.errores = data.errores;
        imagenes.numerodientes = data.numerodientes;
        imagenes.modulo = data.modulo;
        imagenes.paso = data.paso;
        imagenes.diametroexterno = data.diametroexterno;
        imagenes.procesador = data.procesador;
        imagenes.tiempoprocesamiento = data.tiempoprocesamiento;
        var fechaservidor = new Date();
        var fechacliente = new Date(data.fecha);
        var stringlatencia = Math.abs(fechacliente.getTime() - fechaservidor.getTime());
        var jsonlatencia = {};
        console.log("Fecha cliente imagen: " + JSON.stringify(fechacliente));
        console.log("Fecha servidor imagen: " + JSON.stringify(fechaservidor));
        jsonlatencia.fechacliente = fechacliente.toISOString();
        jsonlatencia.fechaservidor = fechaservidor.toISOString();
        jsonlatencia.latencia = stringlatencia;
        console.log(jsonlatencia);
        socket.broadcast.emit('latenciaimagen', JSON.stringify(jsonlatencia));
        socket.emit('latenciaimagen', JSON.stringify(jsonlatencia));
        imagenes.latencia = jsonlatencia.latencia;
        console.log(imagenes);
        totalimagenes.push(imagenes);
        var variableimagenes = JSON.stringify(totalimagenes);
        fs.writeFile(__dirname + "/public/images/extradata/imagen" + numeroimagenes + "." + data.extension, data.cadena64, 'base64', function (err) {
            console.log("Imagen guardada en:" + __dirname + "/public/images/extradata/imagen" + numeroimagenes + "." + data.extension);
        });
        fs.writeFile(__dirname + "/public/images/extradata/imagenp" + numeroimagenes + "." + data.extension, data.cadena64procesada, 'base64', function (err) {
            console.log("Imagen guardada en:" + __dirname + "/public/images/extradata/imagenp" + numeroimagenes + "." + data.extension);
        });
        fs.writeFile(__dirname + "/public/images/imagenes.json", variableimagenes, 'utf8', function (err) {
            console.log("JSON generado correctamente");
        });
        socket.broadcast.emit('nueva_imagen', imagenes); //Envia a todos los clientes
        imagenes = {};
        //Guardar en base de datos
        numeroimagenes += 1;
    });
    socket.on('datofecha', function (data) {
        console.log(data);
        var fechaservidor = new Date();
        var fechacliente = new Date(data);
        var stringlatencia = Math.abs(fechacliente.getTime() - fechaservidor.getTime());
        var jsonlatencia = {};
        console.log("Fecha cliente: " + JSON.stringify(fechacliente));
        console.log("Fecha servidor: " + JSON.stringify(fechaservidor));
        jsonlatencia.fechacliente = fechacliente.toISOString();
        jsonlatencia.fechaservidor = fechaservidor.toISOString();
        jsonlatencia.latencia = stringlatencia;
        console.log(jsonlatencia);
        socket.broadcast.emit('latenciasalida', JSON.stringify(jsonlatencia));
        socket.emit('latenciasalida', JSON.stringify(jsonlatencia));
    });
});
module.exports = {app: app, server: server};

