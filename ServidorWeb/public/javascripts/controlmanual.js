


//PARTE NUEVA
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

var socket = io(); // CREA UNA VARIABLE PARA ENVIAR AL CLIENTE DEPEDNIEDO DE LA VARIABLE EN HTML
var imagen1 = document.getElementById("imagen_actual");
var reporteerrores = document.getElementById("reporteerrores");
var fecha = document.getElementById('fecha');
var infoengranaje = document.getElementById("detalleengrane");
var valormotor = document.getElementById("motor");
var labelmotor = document.getElementById("motorvalor");

//AÑADIR COMANDOS MANUELAES
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
//

var mensaje;

var arreglodispositivo = []; // inicia en forma de vector

// AÑADIR SISTEMA MANUAL
sistemamanual.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.estado = 0;
    dispositivo.direcclasificacion = 0;
    motoradelante.disabled = false;
    motoratras.disabled = false;
    capturarengrane.disabled = false;
    clasificacionbueno.disabled = false;
    clasificacionmalo.disabled = false;
    engranebueno.style.display = "none";
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "none";

    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
//
valormotor.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
       dispositivo.dispositivo = 1;
     dispositivo.modofunc = false;
    dispositivo.motor = Number(valormotor.value);
    labelmotor.innerHTML = valormotor.value;
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});




//AÑADIR LOS COMANDOS PARA LOS DEMAS
umbral.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
       dispositivo.dispositivo = 1;
   dispositivo.modofunc = false;
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
    dispositivo.sentidomotor = 1;
    dispositivo.direcclasificacion = 0;
    console.log("MOTOR ADELANTE");
    motoradelantelabel.style.display = "block";
    motoratraslabel.style.display = "none";
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
motoratras.addEventListener("click", function () {
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = false;
    dispositivo.sentidomotor = -1;
    dispositivo.direcclasificacion = 0;
    console.log("MOTOR ATRAS");
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "block";
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});
capturarengrane.addEventListener("click", function () {
    var dispositivo = {};
   dispositivo.modofunc = false;
    dispositivo.dispositivo = 1;
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
    console.log(" engrane bueno");
    engranebueno.style.display = "block";
    engranemalo.style.display = "none";
    arreglodispositivo.push(dispositivo);
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
    console.log(" engrane malo");
    engranemalo.style.display = "block";
    engranebueno.style.display = "none";
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});

//
console.log("Inicio del Script");
socket.on('datos', function (data) { //get button status from client
    var sizedatos = data.length;
    console.log(data);
    console.log("Tamaño del dato en Datos: " + sizedatos);
    for (i = 0; i < sizedatos; i++) {
        if (data[i].motor != null) {
            valormotor.value = Number(data[i].motor);
            labelmotor = Number(data[i].motor);
        }



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

    }
});
socket.on('nueva_imagen', function (data) {
    imagen1.src = data.nombreprocesada;
    infoengranaje.innerHTML = "Numero de dientes: " + data.numerodientes +
            "<br> " + "Modulo: " + data.modulo.toFixed(2) + "<br> " + "Paso: " + data.paso.toFixed(2) + "<br> " + "Diametro exterior: " + data.diametroexterno.toFixed(2) + "mm";
    if (data.errores === " ") {
        reporteerrores.innerHTML = "Pieza sin errores!";

    } else {
        reporteerrores.innerHTML = data.errores;
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

        if (data[i].umbral != null) {
            umbral.value = Number(data[i].umbral);
            umbralvalor = Number(data[i].umbral);
        }
        if (data[i].modofunc != null) {
            if (data[i].modofunc == true) {
                sistemaautomatico.disabled = true;
                sistemamanual.disabled = false;
                valoremergencia.disabled = false;
                valorstart.disabled = false;
                valorstop.disabled = false;
                motoradelante.disabled = true;
                motoratras.disabled = true;
                capturarengrane.disabled = true;
                clasificacionbueno.disabled = true;
                clasificacionmalo.disabled = true;
                texobjbueno.style.display = "none";
                texobjmalo.style.display = "none";
                textcapturarengrane.style.display = "none";
                textcongear.style.display = "none";
                textemerg.style.display = "none";
                textrun.style.display = "none";
                textsingear.style.display = "none";
                textparo.style.display = "none";
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
                engranebueno.style.display = "none";
                engranemalo.style.display = "none";
            } else {
                sistemaautomatico.disabled = false;
                sistemamanual.disabled = true;
                valoremergencia.disabled = true;
                valorstart.disabled = true;
                valorstop.disabled = true;
                motoradelante.disabled = false;
                motoratras.disabled = false;
                capturarengrane.disabled = false;
                clasificacionbueno.disabled = false;
                clasificacionmalo.disabled = false;
                texobjbueno.style.display = "none";
                texobjmalo.style.display = "none";
                textcapturarengrane.style.display = "none";
                textcongear.style.display = "none";
                textemerg.style.display = "none";
                textrun.style.display = "none";
                textsingear.style.display = "none";
                textparo.style.display = "none";
                engranebueno.style.display = "none";
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "none";
            }
        }
        if (data[i].sentidomotor != null) {
            if (data[i].sentidomotor == -1) {
                dispositivo.direcclasificacion = 0;
                console.log("MOTOR ATRAS");
                motoradelantelabel.style.display = "none";
                motoratraslabel.style.display = "block";


            } else if (data[i].sentidomotor == 1) {
                dispositivo.direcclasificacion = 0;
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


    }

});
socket.on('lastimagen', function (data) {
    imagen1.src = data;
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
