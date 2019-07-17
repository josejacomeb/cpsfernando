var socket = io(); // CREA UNA VARIABLE PARA ENVIAR AL CLIENTE DEPEDNIEDO DE LA VARIABLE EN HTML
var mensaje;
var led1 = document.getElementById("ledon");
var ledsq1 = document.getElementById("ledcolor");
var led0 = document.getElementById("ledoff");
var ledsq0 = document.getElementById("ledcolor2");
var arreglodispositivo= [];
// NUEVO DISPOSITIVO


led1.addEventListener("click", function () {
    var equipo2 = {};
    equipo2.equipo2=1;
    equipo2.led = true;
    ledsq1.style.backgroundColor = 'Green';
    ledsq0.style.backgroundColor = "#f4f4f4";
    //led1.disabled = false;
    //led0.disabled = true;
    arreglodispositivo.push(equipo2);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);//ojo revisar los datos 
    arreglodispositivo = [];
});
led0.addEventListener("click", function () {
    var equipo2 = {};
    equipo2.equipo2=1;
    equipo2.led = false;
    ledsq1.style.backgroundColor = "#f4f4f4";
    ledsq0.style.backgroundColor = "#35424a";
    //led1.disabled = false;
    //led0.disabled = true;
    arreglodispositivo.push(equipo2);
    console.log(arreglodispositivo);
    socket.emit('datos', arreglodispositivo);//ojo revisar los datos 
    arreglodispositivo = [];
});

//HASTA AQUI LA PARTE DEL EJERCICIO

console.log("Inicio del Script");
socket.on('datos', function (data) { //get button status from client
    var sizedatos = data.length;
    console.log(data);
    console.log("Tamaño del dato en Datos: " + sizedatos);
    for (i = 0; i < sizedatos; i++) {
                //PARTE AÑADIDA PARA EL EJERCICIO//
        if (data[i].led != null) {
            if (data[i].led == true) {
                console.log("led encendido");
                ledsq1.style.backgroundColor = 'Green';
                ledsq0.style.backgroundColor = "#f4f4f4";
            } else {
                ledsq1.style.backgroundColor = "#f4f4f4";
                ledsq0.style.backgroundColor = "#35424a";
            }
            
        }
        //FIN DE LA PARTE AÑADIDA
    }
    });
    socket.on('nuevousuario', function (data) { //get button status from client
    console.log("nuevo usuario");
    console.log(data);
    var sizedatos = data.length;
    console.log(typeof data);
    console.log("Tamaño del dato" + sizedatos);
    for (i = 0; i < sizedatos; i++) {
        
                 //PARTE AÑADIDA PARA EL EJERCICIO//
        if (data[i].led != null) {
            if (data[i].led == true) {
                console.log("led encendido");
                ledsq1.style.backgroundColor = 'Green';
                ledsq0.style.backgroundColor = "#f4f4f4";
            } else {
                ledsq1.style.backgroundColor = "#f4f4f4";
                ledsq0.style.backgroundColor = "#35424a";
            }
            
        }
        //FIN DE LA PARTE AÑADIDA
    }
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
