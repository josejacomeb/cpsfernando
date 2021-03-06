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
#include <opencv2/highgui.hpp>
#include "ArduinoJson.h"                //Libreria para el Manejo de Cadenas Json en C++


#ifdef __arm__                       //Compila solo si se encuentra dentro del Raspberry Pi(procesador arm)

#include <wiringPi.h>
#include <wiringSerial.h>
#endif
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl   //Colorear las funciones en el terminal
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl
int fd;
int fd2;
int fdm; //Aplicacion Mobiles
#include "Vision.h"                   //Libreria y funciones de vision para engranes

using namespace sio; //Workspace de Sockets.io
using namespace std; //Workspace de std
using namespace cv; //Workspace de opEnCV

std::mutex _lock; //Workspace de multithreading
std::condition_variable_any _cond;
bool connect_finish = false;
//Variables que se pueden modificar a través de los argumentos de C
string servidorIP = "127.0.0.1";
string puerto = "3000";
string puertoserial = "/dev/ttyUSB0"; // para arduinos chinos sistema principal
//string puertoserial = "/dev/ttyACM0"; // arduinos originales  sistema principal
string puertoserialesp = "/dev/ttyAMA0"; // comunicacion directa rxtx del ESP8622
int idcamara = 0; // La primera camara que se encuentra en /dev/video0
vector<uchar> buf; //Buffer para guardar los datos de la imagen
std::string encoded, encodedprocesada, s, tipo;
std::time_t tt;
bool datos = false;
bool modfun;
//Variables para el envio de datos
const size_t bufferSize = JSON_OBJECT_SIZE(11);
DynamicJsonBuffer Bufferentrada(bufferSize);
DynamicJsonBuffer Buffersalida(bufferSize);
JsonObject& datoenvioarduino = Buffersalida.createObject();
JsonObject& datoenvioarduinomanual = Buffersalida.createObject();
JsonObject& datoenvioarduinomotor = Buffersalida.createObject();
std::string cadenapararduino;
std::string cadenapararduinovid;
std::string cadenapararduinomanual;
std::string cadenapararduinomotor;
std::string cadenatotaldesdearduinomo; //Cadena para ESP
bool capturarengrane;

const size_t bufferSizeeq2 = JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2);
DynamicJsonBuffer Bufferentradaeq2(bufferSizeeq2);
DynamicJsonBuffer Buffersalidaeq2(bufferSizeeq2);
JsonObject& datoenvioarduinoeq2 = Buffersalidaeq2.createObject();

std::string cadenapararduinoeq2;


const size_t bufferSizemo = JSON_OBJECT_SIZE(11);
DynamicJsonBuffer Bufferentradamo(bufferSizemo);
DynamicJsonBuffer Buffersalidamo(bufferSizemo);
JsonObject& datoenvioarduinomo = Buffersalidamo.createObject();

const size_t bufferSizeesp1 = JSON_OBJECT_SIZE(11);
DynamicJsonBuffer Bufferentradaesp1(bufferSizeesp1);
DynamicJsonBuffer Buffersalidaesp1(bufferSizeesp1);
JsonObject& datoenvioarduinoesp1 = Buffersalidaesp1.createObject();
std::string cadenapararduinoesp1;

const size_t bufferSizeesp2 = JSON_OBJECT_SIZE(11);
DynamicJsonBuffer Bufferentradaesp2(bufferSizeesp2);
DynamicJsonBuffer Buffersalidaesp2(bufferSizeesp2);
JsonObject& datoenvioarduinoesp2 = Buffersalidaesp2.createObject();
std::string cadenapararduinoesp2;

const size_t bufferSizeespmotor = JSON_OBJECT_SIZE(11);
DynamicJsonBuffer Bufferentradaespmotor(bufferSizeespmotor);
DynamicJsonBuffer Buffersalidaespmotor(bufferSizeespmotor);
JsonObject& datoenvioarduinoespmotor = Buffersalidaespmotor.createObject();
std::string cadenapararduinoespmotor;


bool iniciocadena = false, fincadena = false;
bool iniciocadenaeq2 = false, fincadenaeq2 = false;
bool iniciocadenamo = true, fincadenamo = false; //Variables ESP

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

void eventos() {
    current_socket->on("datos", sio::socket::event_listener_aux([&](string
            const& name, message::ptr const& data, bool isAck, message::list & ack_resp) {
        _lock.lock(); //Bloquear la ejecucion hasta realizar una opcion
        std::vector<std::shared_ptr < message>> vectoringresojson = data -> get_vector(); //Creo vector para el ingreso de datos

        for (int i = 0; i < vectoringresojson.size(); i++) {
            std::map<std::string, message::ptr> mapa = vectoringresojson.at(i)->get_map();
                    std::cout << "Vector ingreso json a mapadatos" << std::endl;
            if (mapa.find("motor") != mapa.end()) {
                std::cout << "Motor: " << mapa.find("motor")->second->get_int() << std::endl;
                        datoenvioarduinomotor["motor"] = mapa.find("motor")->second->get_int();

                        std::cout << "Cadena para Equipo 1 motor: ";
                        cadenapararduinomotor = "";

                        datoenvioarduinomotor.printTo(cadenapararduinomotor);
                        // cadenapararduino += std::string("\0"); //Fin de cadena
                        const char *cstr = cadenapararduinomotor.c_str();
                        std::cout << cstr << std::endl;
#ifdef __arm__
                        serialPuts(fd, cstr);
#endif
                        std::cout << "cadenaresetea" << std::endl;

                        cadenapararduinomotor = "";
                        std::cout << "cadenaresetea" << cadenapararduinomotor << std::endl;






            }
            if (mapa.find("dispositivo") != mapa.end()) {
                if (mapa.find("dispositivo") != mapa.end()) {
                    std::cout << "dispositivo : " << mapa.find("dispositivo")->second->get_int() << std::endl;
                            datoenvioarduino["dispositivo"] = mapa.find("dispositivo")->second->get_int();
                            datoenvioarduinomanual["dispositivo"] = mapa.find("dispositivo")->second->get_int();

                }
                if (mapa.find("modofunc") != mapa.end()) {
                    std::cout << "modofunc: " << mapa.find("modofunc")->second->get_bool() << std::endl;
                            modfun = mapa.find("modofunc")->second->get_bool();

                }
                if (modfun == true) {
                    datoenvioarduino["modofunc"] = mapa.find("modofunc")->second->get_bool();
                    if (mapa.find("motor") != mapa.end()) {
                        std::cout << "Motor: " << mapa.find("motor")->second->get_int() << std::endl;
                                datoenvioarduino["motor"] = mapa.find("motor")->second->get_int();
                    }

                    if (mapa.find("estado") != mapa.end()) {
                        std::cout << "estado: " << mapa.find("estado")->second->get_int() << std::endl;
                                signed int datoestado = mapa.find("estado")->second->get_int();
                                datoenvioarduino["estado"] = datoestado;
                    }
                    std::cout << "Cadena para Equipo 1 automatico: ";
                            cadenapararduino = "";

                            datoenvioarduino.printTo(cadenapararduino);
                            // cadenapararduino += std::string("\0"); //Fin de cadena
                            const char *cstr = cadenapararduino.c_str();
                            std::cout << cstr << std::endl;
#ifdef __arm__
                            serialPuts(fd, cstr);
#endif
                            std::cout << "cadenaresetea" << std::endl;

                            cadenapararduino = "";
                            std::cout << "cadenaresetea" << cadenapararduino << std::endl;

                } else {
                    datoenvioarduinomanual["modofunc"] = mapa.find("modofunc")->second->get_bool();
                    if (mapa.find("sentidomotor") != mapa.end()) {
                        std::cout << "sentidomotor: " << mapa.find("sentidomotor")->second->get_int() << std::endl;
                                datoenvioarduinomanual["sentidomotor"] = mapa.find("sentidomotor")->second->get_int();
                    }

                    if (mapa.find("direcclasificacion") != mapa.end()) {
                        std::cout << "direcclasificacion: " << mapa.find("direcclasificacion")->second->get_int() << std::endl;
                                datoenvioarduinomanual["direcclasificacion"] = mapa.find("direcclasificacion")->second->get_int();
                    }
                    if (mapa.find("umbral") != mapa.end()) {
                        std::cout << "Umbral IF: " << mapa.find("umbral")->second->get_int() << std::endl;
                                datoenvioarduinomanual["umbral"] = mapa.find("umbral")->second->get_int();
                    }
                    if (mapa.find("capturarengrane") != mapa.end()) {
                        std::cout << "capturarengrane: " << mapa.find("capturarengrane")->second->get_bool() << std::endl;
                                datoenvioarduinomanual["capturarengrane"] = mapa.find("capturarengrane")->second->get_bool();
                                bool capturarengrane = mapa.find("capturarengrane")->second->get_bool();
                                datoenvioarduinomanual.printTo(cadenapararduinomanual);
                                // cadenapararduino += std::string("\0"); //Fin de cadena
                                const char *cstr = cadenapararduinomanual.c_str();
                                std::cout << cstr << std::endl;
#ifdef __arm__
                                serialPuts(fd, cstr);
#endif                      
                                if (capturarengrane) {
                            std::cout << "La captura esta abierta?: " << cap.isOpened() << std::endl;
                                    cap >> imagen; //Guardo la imagen de la cámara del PC en la matriz
                                    std::cout << "La imagen esta vacia?" << imagen.empty() << std::endl;
                                    // imshow("Imagen", imagen);
                                    waitKey(1000);
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
                                    sio::message::ptr mensajeimagenindividual(sio::object_message::create());
                                    std::map<std::string, sio::message::ptr>& mapimagenindividual = mensajeimagenindividual->get_map();

                                    tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
                                    mapimagenindividual.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
                                    mapimagenindividual.insert(std::make_pair("fecha", sio::string_message::create(ctime(&tt)))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane
                                    current_socket->emit("imagen", mensajeimagenindividual); //Envio los datos
                                    mapimagenindividual.clear();
                                    std::cout << "Fin envio imagenes" << std::endl;
                        }
                    }





                    std::cout << "Cadena para Equipo 1 maunal: ";
                            cadenapararduinomanual = "";

                            datoenvioarduinomanual.printTo(cadenapararduinomanual);
                            // cadenapararduino += std::string("\0"); //Fin de cadena
                            const char *cstr = cadenapararduinomanual.c_str();
                            std::cout << cstr << std::endl;
#ifdef __arm__
                            serialPuts(fd, cstr);
#endif
                            std::cout << "cadenaresetea" << std::endl;

                            cadenapararduinomanual = "";
                            std::cout << "cadenaresetea" << cadenapararduinomanual << std::endl;
                }
            }




            /* NUEVO EQUIPO*/
            if (mapa.find("equipo2") != mapa.end()) {
                if (mapa.find("equipo2") != mapa.end()) {
                    std::cout << "equipo2 : " << mapa.find("equipo2")->second->get_int() << std::endl;
                            datoenvioarduinoeq2["equipo2"] = mapa.find("equipo2")->second->get_int();
                }
                if (mapa.find("led") != mapa.end()) {
                    std::cout << "led : " << mapa.find("led")->second->get_bool() << std::endl;
                            datoenvioarduinoeq2["led"] = mapa.find("led")->second->get_bool();
                }
                if (mapa.find("rgb") != mapa.end()) {
                    std::cout << "a" << std::endl;
                            std::vector<sio::message::ptr> vectorrgb = mapa.find("rgb")->second->get_vector();
                            std::cout << "Vector rgb: " << vectorrgb.size();
                    if (vectorrgb.size() == 3) {
                        std::cout << vectorrgb.at(0)->get_int() << std::endl;
                                JsonArray& rgb = datoenvioarduino["rgb"];
                                int rgb0 = vectorrgb.at(0)->get_int(); // 255
                                int rgb1 = vectorrgb.at(1)->get_int(); // 255
                                int rgb2 = vectorrgb.at(2)->get_int(); // 255
                    }
                }
                /*FIN DE NUEVO EQUIPO*/

                std::cout << "Cadena para Equipo 2: ";
                        cadenapararduinoeq2 = "";
                        datoenvioarduinoeq2.printTo(cadenapararduinoeq2);
                        cadenapararduinoeq2 += std::string("\0"); //Fin de cadena
                        const char *cstr2 = cadenapararduinoeq2.c_str();
                        std::cout << cstr2 << std::endl;
#ifdef __arm__
                        //serialPuts(fd2, cstr2);
#endif
            }

        }

        _lock.unlock(); //Desbloquear la ejecucion para seguir con el programa
    }));
    current_socket->on("imagen", sio::socket::event_listener_aux([&](string
            const& name, message::ptr const& data, bool isAck, message::list & ack_resp) {

        _lock.lock(); //Bloquear la ejecucion hasta realizar una opcion
        std::cout << "Llego una imagen" << std::endl;
        _lock.unlock(); //Desbloquear la ejecucion para seguir con el programa
    }));


    current_socket->on("nuevousuario", sio::socket::event_listener_aux([&](string
            const& name, message::ptr const& data, bool isAck, message::list & ack_resp) {
        _lock.lock(); //Bloquear la ejecucion hasta realizar una opcion
        std::vector<std::shared_ptr < message>> vectoringresojson = data -> get_vector(); //Creo vector para el ingreso de datos
        std::cout << "Inicializando datos" << std::endl;

        for (int i = 0; i < vectoringresojson.size(); i++) {
            std::map<std::string, message::ptr> mapa = vectoringresojson.at(i)->get_map();
                    std::cout << "Vector ingreso json a mapadatos" << std::endl;
            if (mapa.find("motor") != mapa.end()) {
                std::cout << "Motor: " << mapa.find("motor")->second->get_int() << std::endl;
                        datoenvioarduinomotor["motor"] = mapa.find("motor")->second->get_int();

                        std::cout << "Cadena para Equipo 1 motor: ";
                        cadenapararduinomotor = "";

                        datoenvioarduinomotor.printTo(cadenapararduinomotor);
                        // cadenapararduino += std::string("\0"); //Fin de cadena
                        const char *cstr = cadenapararduinomotor.c_str();
                        std::cout << cstr << std::endl;
#ifdef __arm__
                        serialPuts(fd, cstr);
#endif
                        std::cout << "cadenaresetea" << std::endl;

                        cadenapararduinomotor = "";
                        std::cout << "cadenaresetea" << cadenapararduinomotor << std::endl;






            }
            if (mapa.find("dispositivo") != mapa.end()) {
                if (mapa.find("dispositivo") != mapa.end()) {
                    std::cout << "dispositivo : " << mapa.find("dispositivo")->second->get_int() << std::endl;
                            datoenvioarduino["dispositivo"] = mapa.find("dispositivo")->second->get_int();
                            datoenvioarduinomanual["dispositivo"] = mapa.find("dispositivo")->second->get_int();
                }
                if (mapa.find("motor") != mapa.end()) {
                    std::cout << "Motor: " << mapa.find("motor")->second->get_int() << std::endl;
                            datoenvioarduino["motor"] = mapa.find("motor")->second->get_int();
                            datoenvioarduinomanual["motor"] = mapa.find("motor")->second->get_int();

                }

                if (mapa.find("modofunc") != mapa.end()) {
                    std::cout << "modofunc: " << mapa.find("modofunc")->second->get_bool() << std::endl;
                            modfun = mapa.find("modofunc")->second->get_bool();

                }
                if (modfun == true) {
                    datoenvioarduino["modofunc"] = mapa.find("modofunc")->second->get_bool();
                    if (mapa.find("motor") != mapa.end()) {
                        std::cout << "Motor: " << mapa.find("motor")->second->get_int() << std::endl;
                                datoenvioarduino["motor"] = mapa.find("motor")->second->get_int();
                    }

                    if (mapa.find("estado") != mapa.end()) {
                        std::cout << "estado: " << mapa.find("estado")->second->get_int() << std::endl;
                                signed int datoestado = mapa.find("estado")->second->get_int();
                                datoenvioarduino["estado"] = datoestado;
                    }
                    std::cout << "Cadena para Equipo 1 automatico: ";
                            cadenapararduino = "";

                            datoenvioarduino.printTo(cadenapararduino);
                            // cadenapararduino += std::string("\0"); //Fin de cadena
                            const char *cstr = cadenapararduino.c_str();
                            std::cout << cstr << std::endl;
#ifdef __arm__
                            serialPuts(fd, cstr);
#endif
                            std::cout << "cadenaresetea" << std::endl;

                            cadenapararduino = "";
                            std::cout << "cadenaresetea" << cadenapararduino << std::endl;



                } else {
                    datoenvioarduinomanual["modofunc"] = mapa.find("modofunc")->second->get_bool();
                    if (mapa.find("sentidomotor") != mapa.end()) {
                        std::cout << "sentidomotor: " << mapa.find("sentidomotor")->second->get_int() << std::endl;
                                datoenvioarduinomanual["sentidomotor"] = mapa.find("sentidomotor")->second->get_int();
                    }

                    if (mapa.find("direcclasificacion") != mapa.end()) {
                        std::cout << "direcclasificacion: " << mapa.find("direcclasificacion")->second->get_int() << std::endl;
                                datoenvioarduinomanual["direcclasificacion"] = mapa.find("direcclasificacion")->second->get_int();
                    }
                    if (mapa.find("umbral") != mapa.end()) {
                        std::cout << "Umbral IF: " << mapa.find("umbral")->second->get_int() << std::endl;
                                datoenvioarduinomanual["umbral"] = mapa.find("umbral")->second->get_int();
                    }
                    if (mapa.find("capturarengrane") != mapa.end()) {
                        std::cout << "capturarengrane: " << mapa.find("capturarengrane")->second->get_bool() << std::endl;
                                datoenvioarduinomanual["capturarengrane"] = mapa.find("capturarengrane")->second->get_bool();
                                bool capturarengrane = mapa.find("capturarengrane")->second->get_bool();
                                datoenvioarduinomanual.printTo(cadenapararduinomanual);
                                // cadenapararduino += std::string("\0"); //Fin de cadena
                                const char *cstr = cadenapararduinomanual.c_str();
                                std::cout << cstr << std::endl;
#ifdef __arm__
                                serialPuts(fd, cstr);
#endif                      
                                if (capturarengrane) {
                            std::cout << "La captura esta abierta?: " << cap.isOpened() << std::endl;
                                    cap >> imagen; //Guardo la imagen de la cámara del PC en la matriz
                                    std::cout << "La imagen esta vacia?" << imagen.empty() << std::endl;
                                    //  imshow("Imagen", imagen);
                                    waitKey(1000);
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
                                    sio::message::ptr mensajeimagenindividual(sio::object_message::create());
                                    std::map<std::string, sio::message::ptr>& mapimagenindividual = mensajeimagenindividual->get_map();

                                    tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
                                    mapimagenindividual.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
                                    mapimagenindividual.insert(std::make_pair("fecha", sio::string_message::create(ctime(&tt)))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane
                                    current_socket->emit("imagen", mensajeimagenindividual); //Envio los datos
                                    mapimagenindividual.clear();
                        }
                    }





                    std::cout << "Cadena para Equipo 1 maunal: ";
                            cadenapararduinomanual = "";

                            datoenvioarduinomanual.printTo(cadenapararduinomanual);
                            // cadenapararduino += std::string("\0"); //Fin de cadena
                            const char *cstr = cadenapararduinomanual.c_str();
                            std::cout << cstr << std::endl;
#ifdef __arm__
                            serialPuts(fd, cstr);
#endif
                            std::cout << "cadenaresetea" << std::endl;

                            cadenapararduinomanual = "";
                            std::cout << "cadenaresetea" << cadenapararduinomanual << std::endl;
                }


                /* NUEVO EQUIPO*/
                if (mapa.find("equipo2") != mapa.end()) {
                    if (mapa.find("equipo2") != mapa.end()) {
                        std::cout << "equipo2 : " << mapa.find("equipo2")->second->get_int() << std::endl;
                                datoenvioarduinoeq2["equipo2"] = mapa.find("equipo2")->second->get_int();
                    }
                    if (mapa.find("led") != mapa.end()) {
                        std::cout << "led : " << mapa.find("led")->second->get_bool() << std::endl;
                                datoenvioarduinoeq2["led"] = mapa.find("led")->second->get_bool();
                    }
                    if (mapa.find("rgb") != mapa.end()) {
                        std::cout << "a" << std::endl;
                                std::vector<sio::message::ptr> vectorrgb = mapa.find("rgb")->second->get_vector();
                                std::cout << "Vector rgb: " << vectorrgb.size();
                        if (vectorrgb.size() == 3) {
                            std::cout << vectorrgb.at(0)->get_int() << std::endl;
                                    JsonArray& rgb = datoenvioarduino["rgb"];
                                    int rgb0 = vectorrgb.at(0)->get_int(); // 255
                                    int rgb1 = vectorrgb.at(1)->get_int(); // 255
                                    int rgb2 = vectorrgb.at(2)->get_int(); // 255
                        }
                    }
                    /*FIN DE NUEVO EQUIPO*/

                    std::cout << "Cadena para Equipo 2: ";
                            cadenapararduinoeq2 = "";
                            datoenvioarduinoeq2.printTo(cadenapararduinoeq2);
                            cadenapararduinoeq2 += std::string("\0"); //Fin de cadena
                            const char *cstr2 = cadenapararduinoeq2.c_str();
                            std::cout << cstr2 << std::endl;
#ifdef __arm__
                            //serialPuts(fd2, cstr2);
#endif
                }

            }
        }

        _lock.unlock(); //Desbloquear la ejecucion para seguir con el programa
    }));
}

//Separar string con delimitador
// manejamos por interfaz de usuario ejemplo: motor,225, led,0.......

std::vector<std::string> split(std::string strToSplit, char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter)) {

        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

void ayuda() {

    HIGHLIGHT("Ayuda, no hay suficientes argumentos");
    std::cout << "Opciones validas" << std::endl;
    std::cout << "--s <ip> especifica la dirección IP servidor" << std::endl;
    std::cout << "--p <puerto> especifica puerto del servidor" << std::endl;
    std::cout << "--ps <puertoserial> especifica la direccion del puerto donde se encuentra conectado la comunicacion serial" << std::endl;
    std::cout << "--c <id camara> especifica el numero entero que identifica la cámara en el openCV " << std::endl;

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
                else ayuda();
            } else if (strcmp(args[j], "--ps") == 0) {
                std::cout << "Cambio puerto serial" << std::endl;
                if ((j + 1) <= argc) puertoserial = std::string(args[j + 1]);
                else ayuda();
            } else if (strcmp(args[j], "--c") == 0) {
                std::cout << "Cambio  de camara" << std::endl;
                if ((j + 1) <= argc) idcamara = atoi(args[j + 1]);
                else ayuda();
            }

        }
    } else ayuda();
#ifdef __arm__
    if ((fd = serialOpen(puertoserial.c_str(), 115200)) < 0) {
        fprintf(stderr, "No se puede abrir el dispositivo serial EQUIPO1, revise los permisos!: %s\n", strerror(errno));
        return -1; //ttyUSB0" Regresa con error
    }
    //Puerto para abrir el ESP
    if ((fdm = serialOpen(puertoserialesp.c_str(), 115200)) < 0) {
        fprintf(stderr, "No se puede abrir el dispositivo serial para MOBILES, revise los permisos!: %s\n", strerror(errno));
        return 1; //ttyUSB0"
    }

    /*  if ((fd2 = serialOpen("/dev/ttyACM0", 115200)) < 0) {

          fprintf(stderr, "No se puede abrir el EQUIPO 2 serial, revise los permisos!: %s\n", strerror(errno));
          return 1;
      }*/

    if (wiringPiSetup() == -1) {
        // fprintf(stdout, "No se puede inicializar wiringPi: %s\n", strerror(errno));
        return -11;
    }
#endif

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

    eventos(); //mandar a llamar una vez, obligatorio
    const String rutared = "parametrosred.xml";
    /*red = cv::ml::ANN_MLP::load(rutared);
    if (red->empty()) {
        std::cout << "No hay red" << std::endl;
        return -1;
    }*/
    datosprueba = Mat(1, entradared.height * entradared.width, CV_32F, Scalar(0));
    cap.open(idcamara);
    if (!cap.isOpened()) {
        std::cout << "\033[1;31m No esta conectada la camara al dispositivo, conectela por favor \033[0m" << std::endl;
        return -1;
    }
#ifdef __arm__
    std::cout << "Corriendo sobre un dispostivo arm" << std::endl;
    std::string cadenatotaldesdearduino = "";
    std::string cadenatotaldesdearduinoeq2 = "";
    std::string cadenatotaldesdearduinomo = "";
    bool sensorpresencia = false;
    char c;
    char z;
    char y;
    bool llegodatos = false;
    bool llegodatoseq2 = false;
    bool llegodatosmo = false;
    bool modofuncionamientoesp = false;
    while (1) {
        cap >> imagen;
        llegodatos = false;
        while (serialDataAvail(fd) > 0) {
            c = serialGetchar(fd);
            std::cout << c;
            if (c == '{') {
                iniciocadena = true;
                cadenatotaldesdearduino = "";
            }
            if (iniciocadena) {
                cadenatotaldesdearduino += std::string(1, c);
                if (c == '}') {
                    fincadena = true;
                    std::cout << "Caracter }" << std::endl;
                    break;
                }
            }
        }
        if (iniciocadena && fincadena) {
            iniciocadena = false, fincadena = false;
            std::cout << "Procesando JSON" << std::endl;
            llegodatos = false;
            JsonObject& datojson = Bufferentrada.parseObject(cadenatotaldesdearduino);
            std::cout << "Cadena que llega Arduino: " << cadenatotaldesdearduino << std::endl;
            cadenatotaldesdearduino = "";
            if (datojson.success()) {
                map.clear();
                for (auto kv : datojson) {
                    if (strcmp(kv.key, ("presencia")) == 0) {
                        map.insert(std::make_pair("presencia",
                                sio::bool_message::create(kv.value.as<bool>())));
                        if (kv.value.as<bool>()) {
                            std::cout << "Presencia: true" << std::endl;
                            sensorpresencia = true;
                        } else {
                            std::cout << "Presencia: false" << std::endl;
                            sensorpresencia = false;
                        }
                    }
                    if (strcmp(kv.key, ("estado")) == 0) {
                        map.insert(std::make_pair("estado",
                                sio::int_message::create(kv.value.as<signed int>())));
                    }
                }
                vectortotal.push_back(mensajeunico);
                current_socket->emit("datos", mensajetotal);
                vectortotal.clear();
            }
        }
        if (sensorpresencia) {
            //Cuando la camara tiene compensacion automatica de exposicion capturamos siete imagenes hasta que se estabilice
            for (int k = 0; k < 4; k++) {
                cap >> imagen;
            }
            int resultado = procesarengrane(true);
            imencode(".png", imagen, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
            s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
            encoded = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
            tipo = "png";
            imencode(".png", roi, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
            s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
            encodedprocesada = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
            mapimagen.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
            chrono::system_clock::time_point today = chrono::system_clock::now(); //Obtengo el tiempo actual
            tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
            std::cout << "today is: " << ctime(&tt);
            mapimagen.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
            mapimagen.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
            mapimagen.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
            mapimagen.insert(std::make_pair("fecha", sio::string_message::create(ctime(&tt)))); //Guardo como formato estandar compatible con JavaScript
            mapimagen.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
            mapimagen.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
            mapimagen.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
            mapimagen.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
            mapimagen.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane

            current_socket->emit("imagen", mensajeimagen); //Envio los datos
            mapimagen.clear();
            JsonObject& datoenvioarduinovid = Buffersalida.createObject();

            datoenvioarduinovid["imagenprocesada"] = true;
            std::cout << "Resultado: " << resultado << endl;
            if (resultado == true) datoenvioarduinovid["clasificador"] = true;
            else datoenvioarduinovid["clasificador"] = false;
            datoenvioarduinovid.printTo(cadenapararduinovid);
            cadenapararduinovid += std::string("\0"); //Caracter terminacion
            const char *cstr = cadenapararduinovid.c_str();
            std::cout << "Cadena de imagen procesada " << cstr << std::endl;
            serialPuts(fd, cstr); //Envio que realizo la vision
            std::cout << "Fin Vision" << std::endl;
            sensorpresencia = false;
            cstr = "";
            cadenapararduinovid = "";
        }

        //NUEVO DISPOSITIVO
        llegodatoseq2 = false;
        /*while (serialDataAvail(fd2) > 0) {
            z = serialGetchar(fd2);
            std::cout << z;
            if (z == '{') {
                iniciocadenaeq2 = true;
                cadenatotaldesdearduinoeq2 = "";
            }
            if (iniciocadena) {
                cadenatotaldesdearduinoeq2 += std::string(1, z);
                if (c == '}') {
                    fincadenaeq2 = true;
                    std::cout << "Caracter }" << std::endl;
                    break;
                }
            }
        }*/
        if (iniciocadenaeq2 && fincadenaeq2) {
            iniciocadenaeq2 = false, fincadenaeq2 = false;
            std::cout << "Procesando JSON" << std::endl;
            llegodatoseq2 = false;
            JsonObject& datojson = Bufferentradaeq2.parseObject(cadenatotaldesdearduinoeq2);
            std::cout << "Cadena que llega Arduino: " << cadenatotaldesdearduinoeq2 << std::endl;
            cadenatotaldesdearduinoeq2 = "";
            if (datojson.success()) {
                map.clear();
                for (auto kv : datojson) {
                    if (strcmp(kv.key, ("led")) == 0) {
                        map.insert(std::make_pair("led",
                                sio::bool_message::create(kv.value.as<bool>())));
                        if (kv.value.as<bool>()) {
                            std::cout << "led: true" << std::endl;
                        } else {
                            std::cout << "led: false" << std::endl;

                        }
                    }
                }
                vectortotal.push_back(mensajeunico);
                current_socket->emit("datos", mensajetotal);
                vectortotal.clear();
            }
        }

        // AÑADO LA PARTE DEL ESP8622;


        llegodatosmo = false;
        while (serialDataAvail(fdm) > 0) {
            y = serialGetchar(fdm);
            std::cout << y;
            if (y == '{') {
                iniciocadenamo = true;
                cadenatotaldesdearduinomo = "";
            }
            if (iniciocadenamo) {
                cadenatotaldesdearduinomo += std::string(1, y);
                if (y == '}') {
                    fincadenamo = true;
                    std::cout << "Caracter }" << std::endl;
                    break;
                }
            }
        }

        if (iniciocadenamo && fincadenamo) {
            iniciocadenamo = false, fincadenamo = false;
            std::cout << "Procesando JSON EPS8622" << std::endl;
            llegodatosmo = false;
            JsonObject& datojson = Bufferentradamo.parseObject(cadenatotaldesdearduinomo);
            std::cout << "Cadena que llega ESP8622: " << cadenatotaldesdearduinomo << std::endl;
            cadenatotaldesdearduinomo = "";
            if (datojson.success()) {
                map.clear();
                for (auto kv : datojson) {
                    if (strcmp(kv.key, ("modofunc")) == 0) {
                        map.insert(std::make_pair("modofunc",
                                sio::bool_message::create(kv.value.as<bool>())));
                        modofuncionamientoesp = kv.value.as<bool>();
                    }
                     if (strcmp(kv.key, ("motor")) == 0) {
                        map.insert(std::make_pair("motor",
                                sio::bool_message::create(kv.value.as<signed int>())));
                        datoenvioarduinoespmotor["motor"]=(kv.value.as<signed int>());
                                                std::cout << "Cadena para Equipo 1 motor auto: ";
                        cadenapararduinoespmotor = "";

                        datoenvioarduinoespmotor.printTo(cadenapararduinoespmotor);
                         const char *cstr = cadenapararduinoespmotor.c_str();
                        std::cout << cstr << std::endl;
                        serialPuts(fd, cstr); //Envio que realizo el ESP8622
                        cadenapararduinoespmotor = "";
                    }



                    if (modofuncionamientoesp) {
                       // std::cout << "estyo en modofuncioamiento automatico ";
                        datoenvioarduinoesp1["modofunc"] = (kv.value.as<bool>());

                        if (strcmp(kv.key, ("estado")) == 0) {
                            map.insert(std::make_pair("estado",
                                    sio::int_message::create(kv.value.as<signed int>())));
                            std::cout << "estado: nvalor" << std::endl;
                            datoenvioarduinoesp1["estado"] = (kv.value.as<signed int>());
                        }
                        std::cout << "Cadena para Equipo 1 motor auto: ";
                        cadenapararduinoesp1 = "";

                        datoenvioarduino.printTo(cadenapararduinoesp1);
                        // cadenapararduino += std::string("\0"); //Fin de cadena
                        const char *cstr = cadenapararduinoesp1.c_str();
                        std::cout << cstr << std::endl;
                        serialPuts(fd, cstr); //Envio que realizo el ESP8622
                        cadenapararduinoesp1 = "";
                    } else {
                         std::cout << "estyo en modo funciinaonamiento manual ";
                        datoenvioarduinoesp2["modofunc"] = (kv.value.as<bool>());
                        if (strcmp(kv.key, ("sentidomotor")) == 0) {
                            map.insert(std::make_pair("sentidomotor",
                                    sio::int_message::create(kv.value.as<signed int>())));
                            std::cout << "sentidomotor: nvalor" << std::endl;
                            datoenvioarduinoesp2["sentidomotor"] = (kv.value.as<signed int>());
                            datoenvioarduinoesp2["umbral"] = 491;
                        }
                        if (strcmp(kv.key, ("direcclasificacion")) == 0) {
                            map.insert(std::make_pair("direcclasificacion",
                                    sio::int_message::create(kv.value.as<signed int>())));
                            std::cout << "sentido: nvalor" << std::endl;
                            datoenvioarduinoesp2["direcclasificacion"] = (kv.value.as<signed int>());
                             datoenvioarduinoesp2["umbral"] = 491;
                        }
                        
                         if (strcmp(kv.key, ("capturarengrane")) == 0) {
                            map.insert(std::make_pair("capturarengrane",
                                    sio::int_message::create(kv.value.as<bool>())));
                            std::cout << "sentido: nvalor" << std::endl;
                            datoenvioarduinoesp2["capturarengrane"] = (kv.value.as<bool>());
                             datoenvioarduinoesp2["umbral"] = 491;
                             bool capturarengraneesp= (kv.value.as<bool>());
                               if (capturarengraneesp) {
                            std::cout << "La captura esta abierta?: " << cap.isOpened() << std::endl;
                                    cap >> imagen; //Guardo la imagen de la cámara del PC en la matriz
                                    std::cout << "La imagen esta vacia?" << imagen.empty() << std::endl;
                                    // imshow("Imagen", imagen);
                                    waitKey(1000);
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
                                    sio::message::ptr mensajeimagenindividual(sio::object_message::create());
                                    std::map<std::string, sio::message::ptr>& mapimagenindividual = mensajeimagenindividual->get_map();

                                    tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
                                    mapimagenindividual.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
                                    mapimagenindividual.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
                                    mapimagenindividual.insert(std::make_pair("fecha", sio::string_message::create(ctime(&tt)))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
                                    mapimagenindividual.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
                                    mapimagenindividual.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane
                                    current_socket->emit("imagen", mensajeimagenindividual); //Envio los datos
                                    mapimagenindividual.clear();
                                    std::cout << "Fin envio imagenes" << std::endl;
                        }
                        }

                       
                        std::cout << "Cadena para Equipo 1 manual esp8266: ";
                        cadenapararduinoesp2 = "";

                        datoenvioarduinoesp2.printTo(cadenapararduinoesp2);
                        // cadenapararduino += std::string("\0"); //Fin de cadena
                        const char *cstr = cadenapararduinoesp2.c_str();
                        std::cout << cstr << std::endl;
                        serialPuts(fd, cstr); //Envio que realizo el ESP8622
                        cadenapararduinoesp2 = "";
                    }


                }
                vectortotal.push_back(mensajeunico);
                current_socket->emit("datos", mensajetotal);
                vectortotal.clear();
            }
        }





    }
#else
    std::cout << "Corriendo en un computador x86-64" << std::endl;
    HIGHLIGHT("Manual: Ponga el identificador seguido del valor, ejemplo:");
    EM(" motor,255; estado, 0; presencia,0; clasificador,0; Valores permitidos:motor(0-255), clasificador(0,1), presencia(1,0), led(1,0), imagen ,ruta/a/imagen.gif;");
    for (std::string line; std::getline(std::cin, line);) {
        cap >> imagen;
        if (line.length() > 0) {
            if (line == "$exit") {
                std::cout << "Salir" << std::endl;
                //  break;
            } else if (line.length() > 4 && line.length() < 300) { //Si tiene un tamaño estándar
                cadenaseparada = split(line, ','); //Separe las opciones
                if (cadenaseparada.size() % 2 == 0) { //Si las opciones son divisibles para dos quiere decir que se puso el identificador y el valor
                    for (int i = 0; i < cadenaseparada.size(); i += 2) {
                        if (cadenaseparada.at(i) == "dispositivo") { //Si es el identificador motor
                            datos = true;
                            map.insert(std::make_pair("dispositivo",
                                    sio::int_message::create(stoi(cadenaseparada.at(i + 1))))); //Crea el Par nombre:actuadorMotor
                            std::cout << "Dispositivo" << std::endl;
                        } else if (cadenaseparada.at(i) == "motor") {
                            datos = true;
                            map.insert(std::make_pair("motor",
                                    sio::int_message::create(stoi(cadenaseparada.at(i + 1)))));
                            std::cout << "Motor" << std::endl;
                        } else if (cadenaseparada.at(i) == "presencia") {
                            datos = true;
                            if (cadenaseparada.at(i + 1) == "true") {
                                map.insert(std::make_pair("presencia",
                                        sio::bool_message::create(true)));
                            } else {
                                map.insert(std::make_pair("presencia",
                                        sio::bool_message::create(false)));
                            }
                            std::cout << "Presencia" << std::endl;
                        } else if (cadenaseparada.at(i) == "estado") {
                            datos = true;
                            map.insert(std::make_pair("estado",
                                    sio::int_message::create(stoi(cadenaseparada.at(i + 1)))));
                            std::cout << "Estado" << std::endl;
                        } else if (cadenaseparada.at(i) == "imagenprocesada") {
                            datos = true;
                            if (cadenaseparada.at(i + 1) == "true") {
                                map.insert(std::make_pair("imagenprocesada",
                                        sio::bool_message::create(true)));
                            } else {
                                map.insert(std::make_pair("imagenprocesada",
                                        sio::bool_message::create(false)));
                            }
                        } else if (cadenaseparada.at(i) == "clasificador") {
                            datos = true;
                            if (cadenaseparada.at(i + 1) == "true") {
                                map.insert(std::make_pair("clasificador",
                                        sio::bool_message::create(true)));
                            } else {
                                map.insert(std::make_pair("clasificador",
                                        sio::bool_message::create(false)));
                            }
                        } else if (cadenaseparada.at(i) == "imagen") { //Si es imagen
                            if (cadenaseparada.at(i + 1) == "0") { //Si es cero saca imagen de la camara
                                std::cout << "camara" << std::endl;
                                //Saque imagen de la camara del Computador
                                cap.open(0);
                                cap >> imagen; //Guardo la imagen de la cámara del PC en la matriz
                                procesarengrane(false);
                                imencode(".png", imagen, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
                                s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
                                encoded = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
                                tipo = "png";
                                imencode(".png", roi, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
                                s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
                                encodedprocesada = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
                                cap.release();
                            } else {
                                std::cout << "Archivo " << std::endl;
                                std::cout << cadenaseparada.at(i + 1) << std::endl;
                                tipo = split(cadenaseparada.at(i + 1), '.').at(1); //Obtengo la extension del archivo
                                std::cout << "Tipo: " << tipo << endl;
                                if (tipo == "png" || tipo == "jpg" || tipo == "jpeg" || tipo == "bmp") {
                                    imagen = imread(cadenaseparada.at(i + 1));
                                    if (imagen.empty()) {
                                        std::cout << "Mala ruta de la imagen" << std::endl;
                                        return -1;
                                    }
                                    procesarengrane(false);
                                    imencode(".png", imagen, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
                                    s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
                                    encoded = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
                                    tipo = "png";
                                    imencode(".png", roi, buf); //Codifico la imagen en PNG y le paso a un Buffer de memoria
                                    s = std::string(buf.begin(), buf.end()); //Transformo el buffer a string
                                    encodedprocesada = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); //Convierto esa String a String en Base64
                                } else {
                                    ifstream in(cadenaseparada.at(i + 1), ios::binary); //Abro el archivo en forma binaria, en la ruta especificada
                                    in.seekg(0, ios::end); //Itero sobre todo el archivo
                                    int iSize = in.tellg(); //Obtengo el tamaño del Archivo
                                    in.seekg(0, ios::beg);
                                    std::vector<char> pBuff(iSize); //Creo un buffer del tamaño del archivo
                                    if (iSize > 0)
                                        in.read(&pBuff[0], iSize); //Pongo los datos de la imagen en el buffer
                                    in.close();
                                    std::string s(pBuff.begin(), pBuff.end()); //Creo un String con todos los elementos del Buffer
                                    encoded = base64_encode(reinterpret_cast<const unsigned char*> (s.c_str()), s.length()); ///Convierto la cadena a un String de Base64
                                }

                            }
                            chrono::system_clock::time_point today = chrono::system_clock::now(); //Obtengo el tiempo actual
                            tt = chrono::system_clock::to_time_t(today); //Convierto a tiempo estandar
                            mapimagen.insert(std::make_pair("cadena64", sio::string_message::create(encoded))); //Incluyo el valor de la cadena, con un identificador cadena64
                            mapimagen.insert(std::make_pair("cadena64procesada", sio::string_message::create(encodedprocesada))); //Incluyo el valor de la cadena, con un identificador cadena64
                            mapimagen.insert(std::make_pair("extension", sio::string_message::create(tipo))); //Incluyo la extension adecuada
                            mapimagen.insert(std::make_pair("fecha", sio::string_message::create(ctime(&tt)))); //Guardo como formato estandar compatible con JavaScript
                            mapimagen.insert(std::make_pair("errores", sio::string_message::create(errores))); //Guardo como formato estandar compatible con JavaScript
                            mapimagen.insert(std::make_pair("numerodientes", sio::int_message::create(crestastotales.size() + dientesperdidos))); //Dientes engrane
                            mapimagen.insert(std::make_pair("modulo", sio::double_message::create(modulo))); //Dientes engrane
                            mapimagen.insert(std::make_pair("paso", sio::double_message::create(paso))); //Dientes engrane
                            mapimagen.insert(std::make_pair("diametroexterno", sio::double_message::create(2 * radioexterior * constanteconversionpxacm))); //Dientes engrane
                            current_socket->emit("imagen", mensajeimagen); //Envio los datos
                            mapimagen.clear();
                        }
                    }
                }
                if (datos) {
                    vectortotal.push_back(mensajeunico); //Ingresa al vector como ultimo de la cola
                    std::cout << "*=======sacando valores del vector final=======*" << std::endl; //Si hay valores en el vector, llene
                    std::cout << "tamaño: " << vectortotal.size() << std::endl;
                    current_socket->emit("datos", mensajetotal); //Envie los datos en un solo vector
                    vectortotal.clear();
                    map.clear(); //Reinicio las variables
                    datos = false;
                }
                cadenaseparada.clear();
                _lock.lock();
                _lock.unlock();
            } else {
                vectortotal.clear();
                map.clear();
                cadenaseparada.clear();
            }
        }
    }
#endif
    h.close(); //Cerrar servidor
    return 0;
}