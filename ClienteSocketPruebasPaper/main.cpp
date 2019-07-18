/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: Fernando Jácome
 *
 * Created on July 9, 2018, 12:55 PM
 */


#include "sio_client.h"                 //Librería de Sockets io
#include "base64.h"                     //Libreria para transformar strings a base64
#include <functional>                   //Librerías estándar
#include <iostream>
#include <thread>                       //Librerías de Multriprocesamiento
#include <mutex>
#include <condition_variable>
#include <string>                       //Librerías de cadena de caracteres
#include <sstream>
#include <fstream>                      //Libreria para abrir archivos
#include <vector>                       //Manejo de vectores
#include <chrono>                       //Libreria para fecha y año
#include <cstdlib>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "Vision.h"


#ifdef __arm__                       //Compila solo si se encuentra dentro del Raspberry Pi(procesador arm)

#include <wiringPi.h>
#include <wiringSerial.h>
#endif
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl   //Colorear las funciones en el terminal
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl
int fd;
int fd2;

using namespace sio; //Workspace de Sockets.io
using namespace std; //Workspace de std

std::mutex _lock; //Workspace de multithreading
std::condition_variable_any _cond;
bool connect_finish = false;
//Variables que se pueden modificar a través de los argumentos de C
string servidorIP = "127.0.0.1";
string puerto = "3000";
int idcamara = 0; // La primera camara que se encuentra en /dev/video0

std::string encoded, encodedprocesada, s, tipo;
std::time_t tt;
bool datos = false;
bool pruebaconimagen = false;
bool pruebaconsensor = true;
int contador = 200;
std::string rutaimagen = "";
vector<uchar> buf; //Buffer para guardar los datos de la imagen
sio::message::ptr mensajeimagenindividual(sio::object_message::create());
std::map<std::string, sio::message::ptr>& mapimagenindividual = mensajeimagenindividual->get_map();

/*
 *  Clase para conectar el listener, para aguardar por eventos
 */
class connection_listener {
    sio::client &handler;

public:

    connection_listener(sio::client& h) :
    handler(h) {
    }

    void on_connected() {
        _lock.lock();
        _cond.notify_all();
        connect_finish = true;
        _lock.unlock();
        std::cout << "Servidor conectado correctamente" << std::endl;
    }

    void on_close(client::close_reason const& reason) {
        std::cout << "sio closed " << std::endl;
        exit(0);
    }

    void on_fail() {
        std::cout << "TFM- SISTEMAS CYBERFISICOSio failed " << std::endl;
        exit(0);
    }
};

socket::ptr current_socket; //Api de socket, el Socket que maneja todos los eventos de Socket.io


std::string GetLocalTime() {
    auto now(std::chrono::system_clock::now());
    auto seconds_since_epoch(
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()));

    // Construct time_t using 'seconds_since_epoch' rather than 'now' since it is
    // implementation-defined whether the value is rounded or truncated.
    std::time_t now_t(
            std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::time_point(seconds_since_epoch)));

    char temp[20];
    if (!std::strftime(temp, 20, "%Y-%m-%d", std::localtime(&now_t)))
        return "";
    char horas[20];
    if (!std::strftime(horas, 20, "%H:%M:%S.", std::localtime(&now_t)))
        return "";
    std::string nanoseconds = std::to_string(
            (std::chrono::duration<long long, std::nano>(
            now.time_since_epoch() - seconds_since_epoch)).count());

    std::string tiempoISO = std::string(temp) + "T" + std::string(horas) + std::string(9 - nanoseconds.length(), '0') + nanoseconds + "-05:00";
    cout << tiempoISO << endl;
    return tiempoISO;
}

void ayuda() {

    HIGHLIGHT("Ayuda, no hay suficientes argumentos");
    std::cout << "Opciones validas" << std::endl;
    std::cout << "--s <ip> especifica la dirección IP servidor" << std::endl;
    std::cout << "--p <puerto> especifica puerto del servidor" << std::endl;
    std::cout << "--n <numeros pruebas> especifica el número de pruebas" << std::endl;
    std::cout << "--s <prueba sensor True> Inicializar las pruebas del sensor" << std::endl;
    std::cout << "--i <prueba sensor> Inicializar las pruebas del sensor" << std::endl;
}

int main(int argc, const char* args[]) {
    if (argc > 2) { //Para cambiar el puerto y el servidor desde el terminal
        for (int j = 0; j < argc; j++) {
            if (strcmp(args[j], "--s") == 0) {
                std::cout << "Cambio servidor" << std::endl;
                if ((j + 1) <= argc) servidorIP = std::string(args[j + 1]);
                else ayuda();
                std::cout << "Servidor IP: " << servidorIP << std::endl;
            } else if (strcmp(args[j], "--p") == 0) {
                std::cout << "Cambio puerto" << std::endl;
                if ((j + 1) <= argc) puerto = std::string(args[j + 1]);
                else {
                    ayuda();
                    return -1;
                }
                std::cout << "Servidor IP: " << servidorIP << std::endl;
            } else if (strcmp(args[j], "--n") == 0) {
                if ((j + 1) <= argc) contador = atoi(args[j + 1]);
                else {
                    ayuda();
                    return -1;
                }
                std::cout << "***Cambio de numero de pruebas: " << contador << " ***" << std::endl;
            } else if (strcmp(args[j], "--i") == 0) {
                pruebaconsensor = false;
                pruebaconimagen = true;
                if ((j + 1) <= argc) {
                    rutaimagen = std::string(args[j + 1]);
                    if (contador == 200) contador = 1;
                    std::cout << "***Modo prueba imagenes*** " << rutaimagen << std::endl;
                } else {
                    ayuda();
                    return -1;
                }

            } else if (strcmp(args[j], "--s") == 0) {
                pruebaconsensor = true;
                pruebaconimagen = false;
                std::cout << "***Modo prueba sensor*** " << servidorIP << std::endl;
            }
        }
    } else ayuda();

    sio::client h; //Creación de un nuevo cliente de Sockets.io
    connection_listener l(h); //Añado al cliente un listener para obtener eventos
    h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
    h.set_close_listener(std::bind(&connection_listener::on_close, &l, std::placeholders::_1));
    h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));
    std::cout << "Corriendo sobre el servidor " + servidorIP + ":" + puerto << std::endl;
    h.connect("http://" + servidorIP + ":" + puerto); //Conexión  al servidor Web

    _lock.lock();
    if (!connect_finish) {
        _cond.wait(_lock);
    }
    _lock.unlock();
    current_socket = h.socket(); //Socket para inicio de comunicacion

    std::vector<std::string> cadenaseparada, extensionarchivo; //Variables para separar string, una para separar la entrada de texto, otra para obtener la extension del archivo
    sio::message::ptr mensajeunico(sio::object_message::create()); //Objetos para enviar datos en Formato JSON
    sio::message::ptr mensajeimagen(sio::object_message::create());

    std::map<std::string, sio::message::ptr>& map = mensajeunico->get_map(); //Punteros hacia los objetos anteriores
    std::map<std::string, sio::message::ptr>& mapimagen = mensajeimagen->get_map();

    array_message::ptr mensajetotal(sio::array_message::create()); //Crear un array de objetos, para enviar varios datos a la vez
    std::vector<std::shared_ptr < sio::message>> &vectortotal = mensajetotal->get_vector(); //Variable estandar con puntero al anterior objeto array_message

    std::cout << "Prueba 100 valores" << std::endl;
    HIGHLIGHT("Manual: Ponga el identificador seguido del valor, ejemplo:");
    EM(" motor,255; estado, 0; presencia,0; clasificador,0; Valores permitidos:motor(0-255), clasificador(0,1), presencia(1,0), led(1,0), imagen ,ruta/a/imagen.gif;");
    bool datosalida = true;
    datos = true;
    int contadorparcial = 0;
    while (contadorparcial < contador) {
        cout << "==================================================" << endl;
        cout << "                   Contador: " << contadorparcial  << endl;
        cout << "==================================================" << endl;
        if (pruebaconsensor) {
            map.insert(std::make_pair("presencia",
                    sio::bool_message::create(datosalida)));
            chrono::system_clock::time_point today = chrono::system_clock::now(); //Obtengo el tiempo actual
            tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
            std::cout << GetLocalTime() << '\n';
            map.insert(std::make_pair("fecha", sio::string_message::create(GetLocalTime()))); //Guardo como formato estandar compatible con JavaScript

            std::cout << "Presencia" << std::endl;

            _lock.lock();
            vectortotal.push_back(mensajeunico); //Ingresa al vector como ultimo de la cola
            std::cout << "*=======sacando valores del vector final=======*" << std::endl; //Si hay valores en el vector, llene
            std::cout << "tamaño: " << vectortotal.size() << std::endl;
            current_socket->emit("datos", mensajetotal); //Envie los datos en un solo vector
            vectortotal.clear();
            map.clear(); //Reinicio las variables
            datos = false;
            _lock.unlock();
            cadenaseparada.clear();
            sleep(1);
        } else if (pruebaconimagen) {
            if (rutaimagen.compare("0") == 0) {
                cap.open(0);
                for (int k = 0; k < 20; k++) {
                    cap >> imagen;
                }
                cap >> imagen;
            } else if (rutaimagen.compare("1") == 0) {
                cap.open(1);
                for (int k = 0; k < 20; k++) {
                    cap >> imagen;
                }
                cap >> imagen;
            } else imagen = cv::imread(rutaimagen);
            if (imagen.empty()) {
                cout << "Imagen vacía, por favor ingrese una ruta de imagen válida" << endl;
                break;
                return -1;
            }
            auto start = chrono::steady_clock::now();
            procesarengrane(false);

            std::cout << "Si" << std::endl;
            imencode(".png", imagen, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
            s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
            encoded = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
            tipo = "png";
            imencode(".png", roi, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
            s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
            encodedprocesada = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
            std::cout << "Si" << std::endl;
            chrono::system_clock::time_point today = chrono::system_clock::now(); //Obtengo el tiempo actual
            //Mensajes para enviar

            auto end = chrono::steady_clock::now();
            auto duracion = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            cout << "Tiempo finalizacion Tarea: " << duracion << " ms" << endl;
            tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
            _lock.lock();
            mapimagenindividual.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
            mapimagenindividual.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
            mapimagenindividual.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
            mapimagenindividual.insert(std::make_pair("fecha", sio::string_message::create(GetLocalTime()))); //Guardo como formato estandar compatible con JavaScript
            mapimagenindividual.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
            mapimagenindividual.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
            mapimagenindividual.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
            mapimagenindividual.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
            mapimagenindividual.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane
            string dispositivo = "Computador";
#ifdef __arm__
            dispositivo = "RPi";
#else
            dispositivo = "Computador";
#endif
            mapimagenindividual.insert(std::make_pair("procesador", sio::string_message::create(dispositivo))); //Guardo como formato estandar compatible con JavaScript
            mapimagenindividual.insert(std::make_pair("tiempoprocesamiento", sio::string_message::create(to_string(duracion)))); //Guardo como formato estandar compatible con JavaScript
            current_socket->emit("imagen", mensajeimagenindividual); //Envio los datos
            mapimagenindividual.clear();
            sleep(3);
            _lock.unlock();
        }
        contadorparcial += 1;
        if(datosalida) datosalida = false;
        else datosalida = true;
    }
    h.close(); //Cerrar servidor
    cout << "Finalización programa" << endl;
    return 0;
}
