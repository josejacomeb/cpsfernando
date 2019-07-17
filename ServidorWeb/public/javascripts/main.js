


//PARTE NUEVA
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

var socket = io(); // CREA UNA VARIABLE PARA ENVIAR AL CLIENTE DEPEDNIEDO DE LA VARIABLE EN HTML
var valormotor = document.getElementById("motor");
var labelmotor = document.getElementById("motorvalor");
var valoremergencia = document.getElementById("emergencia");
var valorstart = document.getElementById("start");
var observacionstart = document.getElementById("obserstart");
var valorstop = document.getElementById("stop");
var estadoclasificador = document.getElementById("estadoclasifi");
var estadopresencia = document.getElementById("valorpresencia");
var imagen1 = document.getElementById("imagen_actual");
var reporteerrores = document.getElementById("reporteerrores");
var fecha = document.getElementById('fecha');
var colorstart = document.getElementById('startcolor');
var colorstop = document.getElementById('stopcolor');
var coloremergencia = document.getElementById('emergenciacolor');
var botemergencia = document.getElementById("botonemergencia");
var textemerg = document.getElementById("textemergencia");
var texobjbueno = document.getElementById("textobjetobueno");
var texobjmalo = document.getElementById("textobjetomalo");
var textparo = document.getElementById("textstop");
var textrun = document.getElementById("textstart");
var textcongear = document.getElementById("textconobjeto");
var textsingear = document.getElementById("textsinobjeto");
var infoengranaje = document.getElementById("detalleengrane");
var sistemaautomatico = document.getElementById("automatico");
var latenciaentrada = document.getElementById("latenciaentrada");
var latenciasalida = document.getElementById("latenciasalida");
var latenciasimagen = document.getElementById("latenciaimagen");

//INICIO PARTE MANUAL
// AÑADIR PARTE MANUAL
var estadopresenciamanual = document.getElementById("valorpresenciamanual");
var sistemamanual = document.getElementById("manual");
var motoradelante = document.getElementById("motoradelante");
var motoradelantelabel = document.getElementById("motoradelantelabel");
var motoratras = document.getElementById("motoratras");
var motoratraslabel = document.getElementById("motoratraslabel");
var capturarengrane = document.getElementById("capturarengrane");
var textcapturarengrane = document.getElementById("textcapturarengrane");
var clasificacionbueno = document.getElementById("clasificacionbueno");
var engranebueno = document.getElementById("engranebueno");
var clasificacionmalo = document.getElementById("clasificacionmalo");
var engranemalo = document.getElementById("engranemalo");
var umbral = document.getElementById("umbral");
var umbralvalor = document.getElementById("umbralvalor");
var datocalibrar = document.getElementById("datocalibrar");
var body = document.getElementById("divtabla");
var bodysalidas = document.getElementById("divtablasalidas");
//FIN PARTE MANUAL

var mensaje;

var arreglodispositivo = []; // inicia en forma de vector

var fechasentrada = [];
var fechassalida = [];
var fechasimagen = [];

var jsonfechaentrada = {};
var jsonfechasalida = {};
var jsonfechasimagen = {};


// A $( document ).ready() block.
$(document).ready(function () {
    $("#verdatos").click(function () {
        body.innerHTML = "";
        var parrafo = document.createElement("p");
        parrafo.innerHTML = "Datos salida";
        body.appendChild(parrafo);
        var promedio = 0;
        if (fechasentrada.length === 0) {
            body.innerHTML = "Sin Datos";
        } else {
            var tabla = document.createElement("table");
            tabla.setAttribute("class", "w3-table w3-striped w3-bordered w3-white");
            tabla.setAttribute("id", "tablahistorial");
            var tblBody = document.createElement("tbody");
            var tblHeader = document.createElement("thead");
            var hilera = document.createElement("tr");
            var textoCelda = document.createTextNode("Nº");
            var celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Fecha cliente");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Fecha servidor");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Latencia");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            tblHeader.appendChild(hilera);
            tabla.appendChild(tblHeader);
            var celda = document.createElement("td");
            var hilera = document.createElement("tr");
            for (var x = 0; x < fechasentrada.length; x++) {
                celda = document.createElement("td");
                hilera = document.createElement("tr");
                var textoCelda = document.createTextNode(x);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var d = new Date(fechasentrada[x].fechacliente);
                var options = {weekday: 'long', year: 'numeric', month: 'long',
                    day: 'numeric', hour: "2-digit", minute: "numeric"};
                var textoCelda = document.createTextNode(d.toLocaleDateString("es-ec", options));
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var d = new Date(fechasentrada[x].fechaservidor);
                var textoCelda = document.createTextNode(d.toLocaleDateString("es-ec", options));
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var textoCelda = document.createTextNode(fechasentrada[x].latencia);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                hilera.appendChild(celda);
                tblBody.appendChild(hilera);
                promedio += fechasentrada[x].latencia;
            }
            promedio = promedio / fechasentrada.length;
            tabla.appendChild(tblBody);
            body.appendChild(tabla);
        }
        var parrafo = document.createElement("p");
        parrafo.innerHTML = "Promedio latencia entrada: " + String(promedio);
        body.appendChild(parrafo);
        promedio = 0;
        if (fechassalida.length === 0) {
            bodysalidas.innerHTML = "Sin Datos";
        } else {
            bodysalidas.innerHTML = "";
            var tabla = document.createElement("table");
            tabla.setAttribute("class", "w3-table w3-striped w3-bordered w3-white");
            tabla.setAttribute("id", "tablahistorial");
            var tblBody = document.createElement("tbody");
            var tblHeader = document.createElement("thead");
            var hilera = document.createElement("tr");
            var textoCelda = document.createTextNode("Nº");
            var celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Fecha cliente");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Fecha servidor");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            var textoCelda = document.createTextNode("Latencia");
            celda = document.createElement("td");
            celda.appendChild(textoCelda);
            hilera.appendChild(celda);
            tblHeader.appendChild(hilera);
            tabla.appendChild(tblHeader);
            var celda = document.createElement("td");
            var hilera = document.createElement("tr");
            for (var x = 0; x < fechassalida.length; x++) {
                celda = document.createElement("td");
                hilera = document.createElement("tr");
                var textoCelda = document.createTextNode(x);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var d = new Date(fechassalida[x].fechacliente);
                var options = {weekday: 'long', year: 'numeric', month: 'long',
                    day: 'numeric', hour: "2-digit", minute: "numeric"};
                var textoCelda = document.createTextNode(d.toLocaleDateString("es-ec", options));
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var d = new Date(fechassalida[x].fechaservidor);
                var textoCelda = document.createTextNode(d.toLocaleDateString("es-ec", options));
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var textoCelda = document.createTextNode(fechassalida[x].latencia);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                hilera.appendChild(celda);
                tblBody.appendChild(hilera);
                promedio += fechassalida[x].latencia;
            }
            console.log("Promedio: " + String(promedio));
            promedio = promedio / fechassalida.length;
            tabla.appendChild(tblBody);
            bodysalidas.appendChild(tabla);
        }
        var parrafo = document.createElement("p");
        parrafo.innerHTML = "Promedio latencia salida: " + String(promedio);
        body.appendChild(parrafo);


    });
});
sistemaautomatico.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = true;
    dispositivo.estado = 0;

    valoremergencia.disabled = false;
    valorstart.disabled = false;
    valorstop.disabled = false;
    texobjbueno.style.display = "none";
    texobjmalo.style.display = "none";
    textcongear.style.display = "none";
    textemerg.style.display = "none";
    textrun.style.display = "none";
    textsingear.style.display = "none";
    textparo.style.display = "none";

    //INICIO PARTE manual
    motoradelante.disabled = true;
    motoratras.disabled = true;
    clasificacionbueno.disabled = true;
    capturarengrane.disabled = true;
    clasificacionmalo.disabled = true;
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "none";
    textcapturarengrane.style.display = "none";
    engranebueno.style.display = "none";
    engranemalo.style.display = "none";
    // fin parte manual

    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    console.log(arreglodispositivo);
    arreglodispositivo = [];
});

//INICIO PARTE MANUAL
sistemamanual.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.direcclasificacion = 0;
    dispositivo.sentidomotor = 0;
    motoradelante.disabled = false;
    motoratras.disabled = false;
    capturarengrane.disabled = false;
    clasificacionbueno.disabled = false;
    clasificacionmalo.disabled = false;
    valoremergencia.disabled = true;
    valorstart.disabled = true;
    valorstop.disabled = true;
    engranebueno.style.display = "none";
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "none";
    texobjbueno.style.display = "none";
    texobjmalo.style.display = "none";
    textcongear.style.display = "none";
    textemerg.style.display = "none";
    textrun.style.display = "none";
    textsingear.style.display = "none";
    textparo.style.display = "none";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
//INICIO PARTE MANUAL


valormotor.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};

    dispositivo.motor = Number(valormotor.value);
    labelmotor.innerHTML = valormotor.value;
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
valoremergencia.addEventListener("click", function () {
    var dispositivo = {};
    if (valoremergencia.checked) {
        dispositivo.dispositivo = 1;
        dispositivo.modofunc = true;
        dispositivo.estado = -1;
        valorstart.disabled = true;
        valorstop.disabled = true;
        textemerg.style.display = "block";
        textrun.style.display = "none";
        textparo.style.display = "none";
        coloremergencia.style.backgroundColor = 'Red';
        colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
        colorstop.style.backgroundColor = "#35424a";
    } else {
        dispositivo.dispositivo = 1;
        dispositivo.modofunc = true;
        dispositivo.estado = 0;
        console.log("StopTomate");
        colorstop.style.backgroundColor = 'Orange';
        colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
        coloremergencia.style.backgroundColor = "#f4f4f4";//gris oscuro
        valorstart.disabled = false;
        valorstop.disabled = true;
        textemerg.style.display = "none";
        textparo.style.display = "block";

    }
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});


botemergencia.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = true;
    dispositivo.estado = -1;
    valoremergencia.checked = true;
    valorstart.disabled = true;
    valorstop.disabled = true;
    coloremergencia.style.backgroundColor = 'Red';
    colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
    colorstop.style.backgroundColor = "#35424a";
    textemerg.style.display = "block";
    textrun.style.display = "none";
    textparo.style.display = "none";

    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
valorstart.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = true;
    dispositivo.estado = 1;
    valorstart.disabled = true;
    valorstop.disabled = false;
    colorstart.style.backgroundColor = 'Green';
    colorstop.style.backgroundColor = "#35424a";
    coloremergencia.style.backgroundColor = "#f4f4f4";//gris oscuro;
    textrun.style.display = "block";
    textparo.style.display = "none";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});

valorstop.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = true;
    dispositivo.estado = 0;
    valorstop.disabled = true;
    valorstart.disabled = false;
    console.log("StopTomate");
    colorstop.style.backgroundColor = 'Orange';
    colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
    coloremergencia.style.backgroundColor = "#f4f4f4";//gris oscuro
    textparo.style.display = "block";
    textrun.style.display = "none";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
//INICIO PARTE MANUAL

//AÑADIR LOS COMANDOS PARA LOS DEMAS
umbral.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.capturarengrane = false;
    dispositivo.umbral = Number(umbral.value);
    umbralvalor.innerHTML = umbral.value;
    if (umbral.value >= 200 & umbral.value <= 300) {

        datocalibrar.style.display = "block";
    } else {
        datocalibrar.style.display = "none";
    }
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
motoradelante.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.direcclasificacion = 0;
    dispositivo.sentidomotor = 1;
    dispositivo.capturarengrane = false;
    motoradelantelabel.style.display = "block";
    motoratraslabel.style.display = "none";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
motoratras.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.direcclasificacion = 0;
    dispositivo.sentidomotor = -1;
    dispositivo.capturarengrane = false;
    console.log("MOTOR ATRAS");
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "block";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];

});



capturarengrane.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.sentidomotor = 0;
    dispositivo.direcclasificacion = 0;
    dispositivo.capturarengrane = true;
    console.log("imagen capturada");
    textcapturarengrane.style.display = "block";
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});

clasificacionbueno.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.sentidomotor = 0;
    dispositivo.direcclasificacion = 1;
    dispositivo.capturarengrane = false;
    console.log(" engrane bueno");
    engranebueno.style.display = "block";
    engranemalo.style.display = "none";
    arreglodispositivo.push(dispositivo);
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});

clasificacionmalo.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.sentidomotor = 0;
    dispositivo.direcclasificacion = -1;
    dispositivo.capturarengrane = false;
    console.log(" engrane malo");
    engranemalo.style.display = "block";
    engranebueno.style.display = "none";
    var d = new Date();
    var n = d.toISOString();
    socket.emit('datofecha', n);
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
//FIN PARTE MANUAL
//
console.log("Inicio del Script");
socket.on('datos', function (data) { //get button status from client
    var sizedatos = data.length;
    console.log("Tamaño del dato en Datos: " + sizedatos);
    for (i = 0; i < sizedatos; i++) {
        if (data[i].motor !== null) {
            valormotor.value = Number(data[i].motor);
            labelmotor = Number(data[i].motor);
        }
        if (data[i].presencia !== null) {
            if (data[i].presencia === true) {
                console.log("Presenciaazul");
                estadopresencia.style.backgroundColor = 'Blue';
                estadopresenciamanual.style.backgroundColor = 'Blue';
                textcongear.style.display = "block";
            } else {
                console.log("presenciaGris");
                estadopresencia.style.backgroundColor = "#f4f4f4";
                estadopresenciamanual.style.backgroundColor = "#268982";
                textsingear.style.display = "block";
            }
        }
        if (data[i].clasificador != null) {
            if (data[i].clasificador == true) {
                console.log("clasiAZUL");
                estadoclasificador.style.backgroundColor = "#6600cc";// ojo cambiar los colortes 
                texobjbueno.style.display = "block";
                texobjmalo.style.display = "none";
            } else {
                estadoclasificador.style.backgroundColor = "#f4f4f4";
                console.log("clasiGRIS");
                texobjbueno.style.display = "none";
                texobjmalo.style.display = "block";
            }
        }
        if (data[i].estado != null) {
            if (data[i].estado == -1) {
                console.log("emerROJO");
                coloremergencia.style.backgroundColor = 'Red';
                colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
                colorstop.style.backgroundColor = "#35424a";
                valorstart.disabled = true;
                valorstop.disabled = true;
                valoremergencia.checked = true;

            } else if (data[i].estado == 1) {
                console.log("startVerde");
                colorstart.style.backgroundColor = 'Green';
                colorstop.style.backgroundColor = "#35424a";
                coloremergencia.style.backgroundColor = "#f4f4f4";
                valorstart.disabled = true;
                valorstop.disabled = false;
                valoremergencia.checked = false;

            } else if (data[i].estado == 0) {
                console.log("StopTomate");
                colorstop.style.backgroundColor = 'Orange';
                colorstart.style.backgroundColor = "#f4f4f4";//gris oscuro
                coloremergencia.style.backgroundColor = "#f4f4f4";//gris oscuro
                valorstart.disabled = false;
                valorstop.disabled = true;
                valoremergencia.checked = false;
            }
        }
        //INICIO PARTE MANUAL
        if (data[i].umbral != null) {
            umbral.value = Number(data[i].umbral);
            umbralvalor = Number(data[i].umbral);
        }
        if (data[i].modofunc != null) {
            if (data[i].modofunc == true) {
                sistemamanual.disabled = false;
                motoradelante.disabled = true;
                motoratras.disabled = true;
                capturarengrane.disabled = true;
                clasificacionbueno.disabled = true;
                clasificacionmalo.disabled = true;
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
                engranebueno.style.display = "none";
                engranemalo.style.display = "none";
            } else {
                sistemamanual.disabled = true;
                valorstart.disabled = true;
                motoradelante.disabled = false;
                motoratras.disabled = false;
                capturarengrane.disabled = false;
                clasificacionbueno.disabled = false;
                clasificacionmalo.disabled = false;
                textcapturarengrane.style.display = "none";
                engranebueno.style.display = "none";
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
            }
        }
        if (data[i].sentidomotor != null) {
            if (data[i].sentidomotor == -1) {
                console.log("MOTOR ATRAS");
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "block";
            } else if (data[i].sentidomotor == 1) {
                console.log("MOTOR ADELANTE");
                motoradelantelabel.style.display = "block";
                motoratraslabel.style.display = "none";
            } else if (data[i].sentidomotor == 0) {

            }
        }
        if (data[i].direcclasificacion != null) {
            if (data[i].direcclasificacion == -1) {
                console.log(" engrane malo");
                engranemalo.style.display = "block";
                engranebueno.style.display = "none";
            } else if (data[i].direcclasificacion == 1) {
                console.log(" engrane bueno");
                engranebueno.style.display = "block";
                engranemalo.style.display = "none";

            } else if (data[i].direcclasificacion == 0) {

            }
        }
        //FIN PARTE MANUAL
    }
});
socket.on('nueva_imagen', function (data) {
    imagen1.src = data.nombreprocesada;
    infoengranaje.innerHTML = "Numero de dientes: " + data.numerodientes +
            "<br> " + "Modulo: " + data.modulo.toFixed(2) + "<br> " + "Paso: " + data.paso.toFixed(2) + "<br> " + "Diametro exterior: " + data.diametroexterno.toFixed(2) + "mm";
    if (data.errores === " ") {
        reporteerrores.innerHTML = "Pieza sin errores!";
        texobjbueno.innerHTML = "Objeto bueno";
    } else {
        reporteerrores.innerHTML = data.errores;
        texobjbueno.innerHTML = "Objeto descartado";
    }
    fecha.innerHTML = "Fecha: " + data.fecha;

});


socket.on('nuevousuario', function (data) { //get button status from client
    console.log("nuevo usuario");
    console.log(data);
    var sizedatos = data.length;
    console.log(typeof data);
    console.log("Tamaño del dato" + sizedatos);
    for (i = 0; i < sizedatos; i++) {
        if (data[i].motor != null) {
            valormotor.value = Number(data[i].motor);
            labelmotor = Number(data[i].motor);
        }
        if (data[i].presencia != null) {
            if (data[i].presencia == true) {
                console.log("Presenciaazul");
                estadopresencia.style.backgroundColor = 'Blue';
                textcongear.style.display = "block";
            } else {
                console.log("presenciaGris");
                estadopresencia.style.backgroundColor = "#35424a";
                textsingear.style.display = "block";
            }
        }
        if (data[i].clasificador != null) {
            if (data[i].clasificador == true) {
                console.log("clasiAZUL");
                estadoclasificador.style.backgroundColor = "#6600cc";// ojo cambiar los colortes 
                texobjbueno.style.display = "block";
                texobjmalo.style.display = "none";
            } else {
                estadoclasificador.style.backgroundColor = "#f4f4f4";
                console.log("clasiGRIS");
                texobjbueno.style.display = "none";
                texobjmalo.style.display = "block";
            }
        }
        if (data[i].estado != null) {
            if (data[i].estado == -1) {
                console.log("emerROJO");
                coloremergencia.style.backgroundColor = 'Red';
                colorstart.style.backgroundColor = "#35424a";//gris oscuro
                colorstop.style.backgroundColor = "#f4f4f4";
                valorstart.disabled = true;
                valorstop.disabled = true;
                valoremergencia.checked = true;

            } else if (data[i].estado == 1) {
                console.log("startVerde");
                colorstart.style.backgroundColor = 'Green';
                colorstop.style.backgroundColor = "#f4f4f4";
                coloremergencia.style.backgroundColor = "#35424a";
                valorstart.disabled = true;
                valorstop.disabled = false;
                valoremergencia.checked = false;

            } else if (data[i].estado == 0) {
                console.log("StopTomate");
                colorstop.style.backgroundColor = 'Orange';
                colorstart.style.backgroundColor = "#35424a";//gris oscuro
                coloremergencia.style.backgroundColor = "#35424a";//gris oscuro
                valorstart.disabled = false;
                valorstop.disabled = true;
                valoremergencia.checked = false;
            }
        }
        //INICIO PARTE MANUAL
        if (data[i].umbral != null) {
            umbral.value = Number(data[i].umbral);
            umbralvalor = Number(data[i].umbral);
        }
        if (data[i].modofunc != null) {
            if (data[i].modofunc == true) {
                sistemamanual.disabled = false;
                motoradelante.disabled = true;
                motoratras.disabled = true;
                capturarengrane.disabled = true;
                clasificacionbueno.disabled = true;
                clasificacionmalo.disabled = true;
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
                engranebueno.style.display = "none";
                engranemalo.style.display = "none";
            } else {
                sistemamanual.disabled = true;
                valorstart.disabled = true;
                motoradelante.disabled = false;
                motoratras.disabled = false;
                capturarengrane.disabled = false;
                clasificacionbueno.disabled = false;
                clasificacionmalo.disabled = false;
                textcapturarengrane.style.display = "none";
                engranebueno.style.display = "none";
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
            }
        }
        if (data[i].sentidomotor != null) {
            if (data[i].sentidomotor == -1) {
                console.log("MOTOR ATRAS");
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "block";
            } else if (data[i].sentidomotor == 1) {
                console.log("MOTOR ADELANTE");
                motoradelantelabel.style.display = "block";
                motoratraslabel.style.display = "none";
            } else if (data[i].sentidomotor == 0) {

            }
        }
        if (data[i].direcclasificacion != null) {
            if (data[i].direcclasificacion == -1) {
                console.log(" engrane malo");
                engranemalo.style.display = "block";
                engranebueno.style.display = "none";
            } else if (data[i].direcclasificacion == 1) {
                console.log(" engrane bueno");
                engranebueno.style.display = "block";
                engranemalo.style.display = "none";

            } else if (data[i].direcclasificacion == 0) {

            }
        }
        //FIN PARTE MANUAL
    }

});
socket.on('lastimagen', function (data) {
    imagen1.src = data;
});
socket.on('latenciaentrada', function (data) {
    var dato = JSON.parse(data);
    console.log("Dato latencia entrada");
    console.log(data);
    latenciaentrada.innerHTML = dato.latencia;
    jsonfechaentrada = {};
    jsonfechaentrada.fechacliente = dato.fechacliente;
    jsonfechaentrada.fechaservidor = dato.fechaservidor;
    jsonfechaentrada.latencia = dato.latencia;
    fechassalida.push(jsonfechaentrada);
});
socket.on('latenciasalida', function (data) {
    var dato = JSON.parse(data);
    console.log("Dato latencia salida");
    console.log(data);
    latenciasalida.innerHTML = dato.latencia;
    jsonfechasalida = {};
    jsonfechasalida.fechacliente = dato.fechacliente;
    jsonfechasalida.fechaservidor = dato.fechaservidor;
    jsonfechasalida.latencia = dato.latencia;
    fechassalida.push(jsonfechasalida);
});
socket.on('latenciaimagen', function (data) {
    var dato = JSON.parse(data);
    console.log("Dato latencia imagen");
    console.log(data);
    latenciasimagen.innerHTML = dato.latencia;
    jsonfechasimagen = {};
    jsonfechasimagen.fechacliente = dato.fechacliente;
    jsonfechasimagen.fechaservidor = dato.fechaservidor;
    jsonfechasimagen.latencia = dato.latencia;
    fechasimagen.push(jsonfechasalida);
});
function cambioPestaña(evento, modo) {
    var i, tabcontent, tablinks;
    // Get all elements with class="tabcontent" and hide them
    tabcontent = document.getElementsByClassName("contenedor");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    // Get all elements with class="tablinks" and remove the class "active"
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }

    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(modo).style.display = "block";
    evento.currentTarget.className += " active";
}
