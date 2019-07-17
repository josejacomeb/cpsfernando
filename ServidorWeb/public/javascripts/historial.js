var socket = io(); // CREA UNA VARIABLE PARA ENVIAR AL CLIENTE DEPEDNIEDO DE LA VARIABLE EN HTML
var myObj = {};
var x = 0;
socket.on('nueva_imagen', function (data) {
    console.log("Recargando todo!");
    dibujar();
});
function dibujar() {

    var body = document.getElementById("divtabla");
    var tabla = document.getElementById("tablahistorial"); //Cargo la tabla del ID
    // Crea un elemento <table> y un elemento <tbody>
    var tblBody = document.createElement("tbody");
    var xmlhttp = new XMLHttpRequest();
    var tamañox = 0, tamañoy = 0;
    xmlhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            myObj = JSON.parse(this.responseText);
            console.log(myObj);
            var celda = document.createElement("td");
            var hilera = document.createElement("tr");
            var imagen = document.createElement('img');
            for (x; x < myObj.length; x++) {
                imagen = document.createElement('img');
                celda = document.createElement("td");
                hilera = document.createElement("tr");
                var textoCelda = document.createTextNode(x);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                imagen.src = myObj[x].nombreprocesada;
                imagen.height = 60;
                imagen.onmouseover = bigImg;
                imagen.onmouseout = normalImg;
                celda.appendChild(imagen);
                hilera.appendChild(celda);
                imagen = document.createElement('img');
                imagen.src = myObj[x].nombre;
                imagen.height = 50;
                imagen.onmouseover = bigImg;
                imagen.onmouseout = normalImg;
                celda = document.createElement("td");
                celda.appendChild(imagen);
                hilera.appendChild(celda);
                var celda = document.createElement("td");
                var infoengranje = "Z: " + myObj[x].numerodientes + "\u000a" + "Modulo: " + myObj[x].modulo + "\u000a" + "Paso: " + myObj[x].paso.toFixed(2) + '\n' + "øext: " + myObj[x].diametroexterno.toFixed(2) + "mm";
                textoCelda = document.createTextNode(infoengranje);
                celda.appendChild(textoCelda);
                celda.style = "white-space: pre;"
                hilera.appendChild(celda);
                celda = document.createElement("td");
                var d = new Date(myObj[x].fecha);
                var textoCelda = document.createTextNode(d.toUTCString());
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                celda = document.createElement("td");
                if (myObj[x].errores == "")
                    var textoCelda = document.createTextNode("¡Pieza sin errores!    ");
                else
                    var textoCelda = document.createTextNode(myObj[x].errores);
                celda.appendChild(textoCelda);
                celda.style = "white-space: pre";
                var celda = document.createElement("td");
                textoCelda = document.createTextNode(myObj[x].latencia);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                var celda = document.createElement("td");
                textoCelda = document.createTextNode(myObj[x].latencia);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                var celda = document.createElement("td");
                textoCelda = document.createTextNode(myObj[x].tiempoprocesamiento);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                var celda = document.createElement("td");
                textoCelda = document.createTextNode(myObj[x].procesador);
                celda.appendChild(textoCelda);
                hilera.appendChild(celda);
                hilera.appendChild(celda);
                tblBody.appendChild(hilera);
            }
            tabla.appendChild(tblBody);
            body.appendChild(tabla);
        }
    };
    xmlhttp.open("GET", "/images/imagenes.json", true);
    xmlhttp.send();
    console.log("Dibujar");
}
function bigImg(data) {
    data.target.height = 230;
}
function normalImg(data) {
    data.target.height = 50;
}