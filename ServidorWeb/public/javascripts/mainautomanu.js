


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


// AÑADIR PARTE MANUAL
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

var mensaje;

var arreglodispositivo = []; // inicia en forma de vector

sistemaautomatico.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
    dispositivo.modofunc = true;
    dispositivo.sentidomotor = 0;
    dispositivo.direcclasificacion = 0;
    motoradelante.disabled=true;
    motoratras.disabled=true;
    clasificacionbueno.disabled=true;
    capturarengrane.disabled = true;
    clasificacionmalo.disabled=true;
    motoradelantelabel.style.display = "none";
    motoratraslabel.style.display = "none";
    textcapturarengrane.style.display = "none";
    engranebueno.style.display = "none";
    engranemalo.style.display = "none";
   
    
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
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});

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
    arreglodispositivo.push(dispositivo);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);
    arreglodispositivo = [];
});



valormotor.addEventListener("click", function () { // AÑADE UNA LISTA EL MOTOR DESPUE DE DAR UN CLICK
    var dispositivo = {};
    dispositivo.dispositivo = 1;
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
        colorstart.style.backgroundColor = "#35424a";//gris oscuro
        colorstop.style.backgroundColor = "#f4f4f4";
    } else {
        dispositivo.dispositivo = 1;
        dispositivo.modofunc = true;
        dispositivo.estado = 0;
        console.log("StopTomate");
        colorstop.style.backgroundColor = 'Orange';
        colorstart.style.backgroundColor = "#35424a";//gris oscuro
        coloremergencia.style.backgroundColor = "#35424a";//gris oscuro
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
    colorstart.style.backgroundColor = "#35424a";//gris oscuro
    colorstop.style.backgroundColor = "#f4f4f4";
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
    colorstop.style.backgroundColor = "#f4f4f4";
    coloremergencia.style.backgroundColor = "#35424a";//gris oscuro;
    textrun.style.display = "block";
    textparo.style.display = "none";
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
    colorstart.style.backgroundColor = "#35424a";//gris oscuro
    coloremergencia.style.backgroundColor = "#35424a";//gris oscuro
    textparo.style.display = "block";
    textrun.style.display = "none";
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
