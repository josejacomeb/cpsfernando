var socket = io(); // CREA UNA VARIABLE PARA ENVIAR AL CLIENTE DEPEDNIEDO DE LA VARIABLE EN HTML
var myObj = {};
var x = 0;
var lista = document.getElementById("opciones");
var body = document.getElementById("divestadistica");
var tiempofuncionamiento = document.getElementById("tiempofuncionamiento");
lista.disabled = true;
var categorias = [];
var coincidencia = false;
var buenas = 0;
var malas = 0;
var opciondientes = 0;
var fechamenor = new Date();
var fechamayor = new Date();
var fechas = [];
var pruebahistograma = [];
var valoresgrafica = {
    fecha: fechamayor,
    buenas: 0,
    malas: 0
}
var fechasgrafica = [];
socket.on('nueva_imagen', function (data) {
    console.log("Recargando todo!");
});

function iniciar() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            myObj = JSON.parse(this.responseText);
            for (x = 0; x < myObj.length; x++) {
                fechas.push(new Date(myObj[x].fecha));
                coincidencia = false;
                for (var i = 0; i < categorias.length; i++) {
                    if (myObj[x].numerodientes === categorias[i]) {
                        coincidencia = true;
                    }
                }
                if (!coincidencia) {
                    categorias.push(myObj[x].numerodientes);
                    console.log(myObj[x].numerodientes);
                }
            }
            console.log(categorias);
            categorias.sort(function (a, b) {
                return a - b
            });
            fechas.sort(function (a, b) {
                return a > b
            });
            console.log(fechas[0]);
            valoresgrafica.fecha = new Date(fechas[0]);
            valoresgrafica.buenas = 0;
            valoresgrafica.malas = 0;
            fechasgrafica.push(valoresgrafica);
            console.log(fechasgrafica);
            valoresgrafica = {};
            valoresgrafica.fecha = fechas[fechas.length / 4];
            valoresgrafica.buenas = 0;
            valoresgrafica.malas = 0;
            fechasgrafica.push(valoresgrafica);
            valoresgrafica = {};
            valoresgrafica.fecha = fechas[fechas.length / 2];
            valoresgrafica.buenas = 0;
            valoresgrafica.malas = 0;
            fechasgrafica.push(valoresgrafica);
            valoresgrafica = {};
            valoresgrafica.fecha = fechas[3 * fechas.length / 4];
            valoresgrafica.buenas = 0;
            valoresgrafica.malas = 0;
            fechasgrafica.push(valoresgrafica);
            valoresgrafica = {};
            valoresgrafica.fecha = fechas[fechas.length - 1];
            valoresgrafica.buenas = 0;
            valoresgrafica.malas = 0;
            fechasgrafica.push(valoresgrafica);
            valoresgrafica = {};
            console.log(fechasgrafica);
            console.log(fechasgrafica[0].fecha);
            if (categorias.length === 0) {
                alert("No hay datos previos, ponga a funcionar el sistema primero");
            } else {
                for (var i = 0; i < categorias.length; i++) {
                    var opcion = document.createElement("option");
                    opcion.text = categorias[i] + " dientes";
                    opcion.value = categorias[i];
                    lista.appendChild(opcion);
                }
            }
        }
    };
    xmlhttp.open("GET", "/images/imagenes.json", true);
    xmlhttp.send();
}

google.charts.load('current', {'packages': ['corechart']}).then(function () {
    lista.disabled = false;
    lista.addEventListener("click", function () {
        buenas = 0, malas = 0;
        var valorselector = parseInt(lista.value);
        if (valorselector === 0) {
            var btn = document.createElement("br");
            var salida = "El sistema ha estado funcionando por: ";
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getFullYear() -
                    fechasgrafica[0].fecha.getFullYear() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getFullYear() -
                        fechasgrafica[0].fecha.getFullYear() + " años ";
            }
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getMonth() -
                    fechasgrafica[0].fecha.getMonth() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getMonth() -
                        fechasgrafica[0].fecha.getMonth() + " meses ";
            }
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getDate() -
                    fechasgrafica[0].fecha.getDate() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getDate() -
                        fechasgrafica[0].fecha.getDate() + " dias ";
            }
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getHours() -
                    fechasgrafica[0].fecha.getHours() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getDate() -
                        fechasgrafica[0].fecha.getHours() + " horas ";
            }
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getMinutes() -
                    fechasgrafica[0].fecha.getMinutes() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getMinutes() -
                        fechasgrafica[0].fecha.getMinutes() + " minutos ";
            }
            if (fechasgrafica[fechasgrafica.length - 1].fecha.getSeconds() -
                    fechasgrafica[0].fecha.getSeconds() > 0) {
                salida += fechasgrafica[fechasgrafica.length - 1].fecha.getSeconds() -
                        fechasgrafica[0].fecha.getSeconds() + " segundos ";
            }
            tiempofuncionamiento.innerHTML = salida;
        }
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.onreadystatechange = function () {
            if (this.readyState === 4 && this.status === 200) {
                myObj = JSON.parse(this.responseText);
                pruebahistograma = [];
                var vector = [];
                for (x = 0; x < myObj.length; x++) {
                    if (valorselector === 0) {
                        if (myObj[x].errores === "") {
                            vector[0] = 1;
                            vector[1] = null;
                            vector[2] = new Date(myObj[x].fecha);
                        } else {
                            vector[0] = null;
                            vector[1] = 1;
                            vector[2] = new Date(myObj[x].fecha);
                        }
                        pruebahistograma.push(vector);
                        vector = [];
                        vector[new Date(myObj[x].fecha)]
                        for (var i = 0; i < fechasgrafica.length - 1; i++) {
                            if (new Date(myObj[x].fecha) >= fechasgrafica[i].fecha
                                    && new Date(myObj[x].fecha) < fechasgrafica[i + 1].fecha) {
                                if (myObj[x].errores === "") {
                                    fechasgrafica[i + 1].buenas += 1;
                                } else {
                                    fechasgrafica[i + 1].malas += 1;
                                }
                            }
                        }
                    } else {
                        if (myObj[x].numerodientes === (valorselector)) {
                            if (myObj[x].errores === "") {
                                console.log("si");
                                buenas += 1;
                            } else {
                                malas += 1;
                            }
                        }
                    }
                }
                console.log(pruebahistograma);
            }
            if (valorselector === 0) {
                var data = new google.visualization.DataTable();
                data.addColumn('number', 'Buenas');
                data.addColumn('number', 'Malas');
                data.addColumn('date', 'Fechas');
                for (var i = 0; i < pruebahistograma.length; i++) {
                    data.addRows([pruebahistograma[i]]);
                }
                var options = {
                    title: 'Datos totales',
                    legend: {position: 'none'},
                };

                var csv = google.visualization.dataTableToCsv(data);
                console.log("CSV");
                console.log(csv);
                var chart = new google.visualization.Histogram(document.getElementById('piechart'));
                chart.draw(data, options);


            } else {
                var data = google.visualization.arrayToDataTable([
                    ['Piezas', 'Probabilidad'],
                    ['Buenas', buenas],
                    ['Malas', malas]
                ]);
                var options = {
                    title: 'Piezas'
                };
                var chart = new google.visualization.PieChart(document.getElementById('piechart'));
                chart.draw(data, options);
            }
        };
        xmlhttp.open("GET", "/images/imagenes.json", true);
        xmlhttp.send();
    });
});

