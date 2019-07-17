/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Vision.h
 * Author: Fernando Jácome
 *
 * Created on August 3, 2018, 5:26 AM
 */


#ifndef VISION_H
#define VISION_H

#include <iostream>
#include <opencv2/core.hpp>             //Librerias de OpenCV para el procesamiento
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <math.h>
#include <vector>
#include <opencv2/highgui.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>


//Variables de OpenCV
cv::Mat imagencontours;
cv::Mat imagengris;
cv::Mat canny_output;
cv::Mat roi, roired;
cv::Mat datosprueba;
cv::Mat salida(1, 1, CV_32FC1); //Datos para el procesamiento red
cv::Mat lineasexternas;
cv::Point2f puntoanterior;
cv::Mat lineasinternas;

std::vector<std::vector<cv::Point> > crestastotales, vallestotales;
std::vector<cv::Point> cresta, valle;
std::vector<std::vector<cv::Point> > contours;
std::vector<cv::Point> circulointerno;
std::vector<cv::Vec4i> hierarchy;
std::vector<std::vector<cv::Point> > contours_poly;
int kernelsize = 3;
int tope = 180;
cv::Rect rectaobjetivo;
int offset = 7;
int aciertos = 0;
float distanciaminima = 1000;
float distanciacentropunto = 0;
cv::Point2f centrocirculo;
cv::Point menorradio;
float radioexterior;
int puntosadd = 0;
cv::Point2f centrocirculointerno;
float radiocirculointerno;
float modulo = 0.0;
float paso = 0.0;
cv::Mat imagen;
std::string errores = "";
int numeroerrores = 0;

cv::VideoCapture cap;

#ifdef __x86_64 //Solo se muestran las capturas cuando corre en un Ordenador de Escritorio
#define mostrarcapturas
#endif

int numerocrestas = 0;
int numerovalles = 0;
int indicemayor = 0; //Indice del contorno mayor
int areamayor = 0;
int area = 0;
double areapromediovalles = 0.0, areapromediocrestas = 0.0;
double areacresta = 0.0, areavalle = 0;
double desviacionestandarcresta = 0.0, desviacionestandarvalle = 0.0;
//cv::Ptr<cv::ml::ANN_MLP> red; //Puntero de la red
cv::Size entradared(32, 32); //Entrada de 32x32 a la red
int dientestotales = 0;
int dientesperdidos = 0;
float constanteconversionpxacm = 0.2083333;
float limitesuperior = 0, limiteinferior = 0;
float sumatotalcircunferencia = 0;

struct areasrectas {
    double area = 0.0;
    cv::Rect Recta;
    double anguloentrecrestas = 0.0;
    cv::Point2f centroide;
};

bool preprocesamiento() {   
    //Inicializacion de variables
    std::cout << "Inicio pre procesamiento" << std::endl;
    crestastotales.clear();
    vallestotales.clear();
    contours.clear();
    hierarchy.clear();
    areamayor = 0;
    indicemayor = 0;
    area = 0;
    numeroerrores = 0;
    dientesperdidos = 0;
    sumatotalcircunferencia = 0;
    imagencontours = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    lineasexternas = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    lineasinternas = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    roi = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0));
    errores = "";
    numeroerrores = 0;
    imagengris.release();
    cv::cvtColor(imagen, imagengris, cv::COLOR_BGR2GRAY); //Transformo a escala de grises
    normalize(imagengris, imagengris, 0, 255, cv::NORM_MINMAX, -1); //Normalizo las imagenes para compensar luces y sombras
    medianBlur(imagengris, imagengris, kernelsize); //difuminacion de la imagen
    cv::threshold(imagengris, imagengris, tope, 255, cv::THRESH_OTSU | cv::THRESH_BINARY); //Umbralizacion OTSU de la imagen
    cv::Mat kernelerosion = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::erode(imagengris, imagengris, kernelerosion); //Erosion para eliminar puntos cercanos
    cv::findContours(imagengris, contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0)); //Buscar contornos
    contours_poly.resize(contours.size());
    if (contours.size() == 0) {
        return false;
    }
    //Hallar el contorno con el indice mayor para procesearlo
    std::cout << "Numero contornos: " << contours.size() << std::endl;
    for (int i = 0; i < contours.size(); i++) {
        area = contourArea(contours[i]);
        if (area > areamayor) {
            areamayor = area;
            indicemayor = i;
        }
    }
    cv::approxPolyDP(contours[indicemayor], contours_poly[indicemayor], 3, true); //Aproximo un poligono al contorno
    cv::minEnclosingCircle(contours[indicemayor], centrocirculo, radioexterior); //Busco el circulo que se aproxima al contorno externo
    rectaobjetivo = cv::boundingRect(cv::Mat(contours_poly[indicemayor])); //Busco la recta que contiene al contorno
    cv::drawContours(imagencontours, contours, indicemayor, cv::Scalar(255, 255, 255), -1);
    roi = imagencontours(rectaobjetivo);
    return true;
}


/*
 * Regresa 1 cuando hay engrane
 * Regresa 0 cuando no hay engrane
 * Regresa -1 cuando hay un error en el procesamiento 
 */
int procesarengrane(bool centrado) {
    std::cout << "Inicia procesamiento engrane" << std::endl;
    if(!preprocesamiento()){
        return -1;
    }
    if (rectaobjetivo.x == 0 || rectaobjetivo.x + rectaobjetivo.width == imagen.cols) {
        std::cout << "Imagen no centrada, centrando..." << std::endl;
    }
    if (centrado) {
#ifdef __arm__
        while (rectaobjetivo.x == 0 || rectaobjetivo.x + rectaobjetivo.width == imagen.cols) { //Si el contorno mayor esta topando en cero o esta topando en el borde mayor en y de la imagen
            std::cout << "Posicion y: " << rectaobjetivo.y << " Posicion x: " << rectaobjetivo.x << std::endl;
            std::cout << " Posicion final: " << rectaobjetivo.x + rectaobjetivo.width << " ancho " << imagen.cols << std::endl;
            if (rectaobjetivo.x == 0) {
                serialPutchar(fd, 'a');
                std::cout << "Motor para adelante" << std::endl;
            } else if (rectaobjetivo.x + rectaobjetivo.width == imagen.cols) {
                serialPutchar(fd, 'd');
                std::cout << "Motor para atras" << std::endl;
            }
            while (!serialDataAvail(fd)) {
            } // Espero hasta que haya datos en el puerto serial
            char dato = serialGetchar(fd); //Variable para que se transfiera del buffer el dato
            std::cout << "Dato bucle while: " << dato << std::endl;
            // if(dato == '2') return; /*Error 1, cuando se para cuando está cuadrando, debe salir de la funcion, el Arduino se restablece*/
            //Cuando la camara tiene compensacion automatica de exposicion capturamos siete imagenes hasta que se estabilice
            imagen.release();
            for (int k = 0; k < 7; k++) {
                cap >> imagen; //Capturamos una imagen
            }
            preprocesamiento();
        }
        std::cout << "t" << std::endl;
        serialPutchar(fd, '9');
        while (!serialDataAvail(fd)) {
        } // Espero hasta que haya datos en el puerto serial
        char dato = serialGetchar(fd); //Variable para que se transfiera del buffer el dato
        while (dato != '2') {
            serialPutchar(fd, 't');
            while (!serialDataAvail(fd)) {
            } // Espero hasta que haya datos en el puerto serial
            dato = serialGetchar(fd); //Variable para que se transfiera del buffer el dato
            std::cout << "Dato bucle while envio t: " << dato << std::endl;
        }
        std::cout << "Finalizo centrado" << dato << std::endl;
        std::cout << "Posicion y: " << rectaobjetivo.y << " Posicion x: " << rectaobjetivo.x << std::endl;
        std::cout << " Posicion otro costado: " << rectaobjetivo.y + rectaobjetivo.height << " alto " << imagen.rows << std::endl;
#endif
        /* //Proceso la imagen en la red para saber si es un engranaje
         float probabilidadengrane = clasificar();
         std::cout << "Probabilidad engrane: " << probabilidadengrane << std::endl;
         if (probabilidadengrane > 0.2) {
             std::cout << "Es un engrane! Sigue el proceso" << std::endl;
         } else {
             std::cout << "La imagen no pertenece a un engrane, abortando proceso" << std::endl;
             //errores += "No es engrane!";
             //return false;
         }*/
    }
    imagencontours = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    roi = cv::Mat(imagen.rows, imagen.cols, CV_8UC3, cv::Scalar(0));
    cv::drawContours(imagencontours, contours, indicemayor, cv::Scalar(0, 255, 0), 1);
    //Buscar minima distancia entre contornos
    cv::Point2f direccion;

    struct puntosmenores {
        int distancia = 1000;
        cv::Point punto = cv::Point(0, 0);
    };
    std::vector<puntosmenores> indicespuntos(6);
    float angulo;
    //Busco la minima distancia cada 60º para sacar el diametro interior del engrane
    for (int j = 0; j < contours[indicemayor].size(); j++) {
        direccion = cv::Point2f(contours.at(indicemayor).at(j).x, contours.at(indicemayor).at(j).y) - centrocirculo;
        angulo = atan2f((float) direccion.y, (float) direccion.x);
        distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(j).x, contours.at(indicemayor).at(j).y) - centrocirculo);
        if (angulo >= 0 && angulo < (M_PI / 3)) {
            if (distanciacentropunto < indicespuntos.at(0).distancia) {
                indicespuntos.at(0).distancia = distanciacentropunto;
                indicespuntos.at(0).punto = contours.at(indicemayor).at(j);
            }
        } else if (angulo >= (M_PI / 3) && angulo < (2 * M_PI / 3)) {
            if (distanciacentropunto < indicespuntos.at(1).distancia) {
                indicespuntos.at(1).distancia = distanciacentropunto;
                indicespuntos.at(1).punto = contours.at(indicemayor).at(j);
            }
        } else if (angulo >= (2 * M_PI / 3) && angulo <= M_PI) {
            if (distanciacentropunto < indicespuntos.at(2).distancia) {
                indicespuntos.at(2).distancia = distanciacentropunto;
                indicespuntos.at(2).punto = contours.at(indicemayor).at(j);
            }
        } else if (angulo >= -M_PI && angulo < -(2 * M_PI / 3)) {
            if (distanciacentropunto < indicespuntos.at(3).distancia) {
                indicespuntos.at(3).distancia = distanciacentropunto;
                indicespuntos.at(3).punto = contours.at(indicemayor).at(j);
            }
        } else if (angulo >= -2 * M_PI / 3 && angulo < -M_PI / 3) {
            if (distanciacentropunto < indicespuntos.at(4).distancia) {
                indicespuntos.at(4).distancia = distanciacentropunto;
                indicespuntos.at(4).punto = contours.at(indicemayor).at(j);
            }
        } else if (angulo >= -M_PI / 3 && angulo < 0) {
            if (distanciacentropunto < indicespuntos.at(5).distancia) {
                indicespuntos.at(5).distancia = distanciacentropunto;
                indicespuntos.at(5).punto = contours.at(indicemayor).at(j);
            }
        }
    }
    circulointerno.clear();
    //Añado al vector circulo intenro
    for (int j = 0; j < indicespuntos.size(); j++) {
        circulointerno.push_back(indicespuntos.at(j).punto);
    }
    minEnclosingCircle(circulointerno, centrocirculointerno, radiocirculointerno); //Saco el circulo interno que entra con esos puntos

    bool iniciocadena = false;
    bool estado = false;
    //Calculo el radio intermedio para separar entre cresas y valles
    float radiointermedio = radiocirculointerno + (radioexterior - radiocirculointerno) / 2;
    int maximovector = contours[indicemayor].size();
    //Añado las cadenas de contornos separandolas si son crestas o valles
    for (int i = 0; i < maximovector; i++) {
        distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(i).x, contours.at(indicemayor).at(i).y) - centrocirculo);
        if (distanciacentropunto > radiointermedio) estado = true;
        else estado = false;
        if (!iniciocadena) {
            if (estado) {
                int iterador = maximovector - 1;
                distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(iterador).x, contours.at(indicemayor).at(iterador).y) - centrocirculo);
                while (distanciacentropunto > radiointermedio) {
                    distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(iterador).x, contours.at(indicemayor).at(iterador).y) - centrocirculo);
                    iterador -= 1;
                }
                for (int j = iterador; j < maximovector; j++) {
                    cresta.push_back(contours.at(indicemayor).at(j));
                }
                cresta.push_back(contours.at(indicemayor).at(i));
                maximovector = iterador + 1;
                iniciocadena = true;
            } else {
                int iterador = maximovector - 1;
                distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(iterador).x, contours.at(indicemayor).at(iterador).y) - centrocirculo);
                while (distanciacentropunto < radiointermedio) {
                    distanciacentropunto = cv::norm(cv::Point2f(contours.at(indicemayor).at(iterador).x, contours.at(indicemayor).at(iterador).y) - centrocirculo);
                    iterador -= 1;
                }
                for (int j = iterador; j < maximovector; j++) {
                    valle.push_back(contours.at(indicemayor).at(j));
                }
                valle.push_back(contours.at(indicemayor).at(i));
                maximovector = iterador + 1;
                iniciocadena = true;
            }
        } else if (estado && iniciocadena) {
            if (valle.size() > 0) {
                //Verifica si el punto que se añade es demasiado pequeño para considerarse diente
                if (cv::contourArea(valle) > 2) {
                    vallestotales.push_back(valle);
                }
                valle.clear();
            }
            cresta.push_back(contours.at(indicemayor).at(i));
        } else if (!estado && iniciocadena) {
            if (cresta.size() > 0) {
                if (cv::contourArea(cresta) > 2) {
                    crestastotales.push_back(cresta);
                }
                cresta.clear();
            }
            valle.push_back(contours.at(indicemayor).at(i));

        }
    }
    if (valle.size()) {
        vallestotales.push_back(valle);
        valle.clear();
    }
    if (cresta.size()) {
        crestastotales.push_back(cresta);
        cresta.clear();
    }

    numerocrestas = crestastotales.size();
    numerovalles = vallestotales.size();

    std::vector<areasrectas> areascrestas, areasvalles;
    cv::Rect rectaerrores;
    contours_poly.resize(numerocrestas);
    areasrectas estructuracrestas, estructuravalles;
    cv::Moments momentoscrestas;
    //Calculo las areas de las crestas y el centro de masa de las mimsmas
    for (int i = 0; i < numerocrestas; i++) {
        cv::drawContours(lineasexternas, crestastotales, i, cv::Scalar(244, 200, 254), 1, cv::LINE_AA);
        areacresta = cv::contourArea(crestastotales.at(i));
        areapromediocrestas += areacresta;
        estructuracrestas.area = areacresta;
        approxPolyDP(crestastotales[i], contours_poly[i], 3, true);
        estructuracrestas.Recta = cv::boundingRect(contours_poly[i]);
        momentoscrestas = cv::moments(crestastotales.at(i), true);
        estructuracrestas.centroide = cv::Point2f((momentoscrestas.m10 / momentoscrestas.m00), (momentoscrestas.m01 / momentoscrestas.m00));
        cv::circle(lineasexternas, estructuracrestas.centroide, 1, cv::Scalar(0, 125, 255), -1);
        areascrestas.push_back(estructuracrestas);
    }
    areapromediocrestas /= (double) numerocrestas;
    contours_poly.resize(numerovalles);
    //Calculo el area de los valles y los cenetroides de las mismas
    for (int i = 0; i < numerovalles; i++) {
        cv::drawContours(lineasinternas, vallestotales, i, cv::Scalar(244, 255, 204), 1, cv::LINE_AA);
        areavalle = cv::contourArea(vallestotales.at(i));
        areapromediovalles += areavalle;
        estructuravalles.area = areavalle;
        approxPolyDP(vallestotales[i], contours_poly[i], 3, true);
        estructuravalles.Recta = cv::boundingRect(contours_poly[i]);
        areasvalles.push_back(estructuravalles);

    }
    areapromediovalles /= (double) numerovalles;
    //Sacar la desviacion estandar de las crestas
    for (int i = 0; i < numerocrestas; i++) {
        desviacionestandarcresta += powf((areapromediocrestas - areascrestas.at(i).area), 2);
    }
    desviacionestandarcresta = sqrtf(desviacionestandarcresta / numerocrestas);
    for (int i = 0; i < numerovalles; i++) {
        desviacionestandarvalle += powf((areapromediovalles - areasvalles.at(i).area), 2);
    }
    desviacionestandarvalle = sqrtf(desviacionestandarvalle / numerovalles);
    std::cout << "Desviacion estandar areas de crestas: " << desviacionestandarcresta \
            << " Area de crestas " << areapromediocrestas << " nie - 2*de " \
            << areapromediocrestas - 2 * desviacionestandarcresta \
            << " niu + 2*de " << areapromediocrestas + 2 * desviacionestandarcresta << std::endl;
    std::cout << "Desviacion estandar areas de valles: " << desviacionestandarvalle \
            << " Area de crestas " << areapromediovalles << " nie - 2*de " \
            << areapromediovalles - 2 * desviacionestandarvalle \
            << " niu + 2*de " << areapromediovalles + 2 * desviacionestandarvalle << std::endl;
    //Aplicar la distribucion normal estandar para encontrar areas con dientes partidos
    for (int i = 0; i < numerocrestas; i++) {
        if (areascrestas.at(i).area < areapromediocrestas - 2 * desviacionestandarcresta && areascrestas.at(i).area > areapromediocrestas + 2 * desviacionestandarcresta) {
            numeroerrores += 1;
            errores += "P" + std::to_string(numeroerrores) + " Diente partido\n";
            cv::rectangle(imagencontours, areascrestas.at(i).Recta, cv::Scalar(0, 0, 255));
            cv::putText(imagencontours, "P" + std::to_string(numeroerrores), cv::Point(areascrestas.at(i).Recta.x + areascrestas.at(i).Recta.width / 2, areascrestas.at(i).Recta.y + areascrestas.at(i).Recta.height / 2), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255));
        }
    }
    //Aplicar la distribucion normal estandar para encontrar areas con acumulacion de material
    for (int i = 0; i < numerovalles; i++) {
        if (areasvalles.at(i).area < areapromediovalles - 2 * desviacionestandarvalle && areasvalles.at(i).area > areapromediovalles + 2 * desviacionestandarvalle) {
            numeroerrores += 1;
            errores += "P" + std::to_string(numeroerrores) + " Acumulacion material\n";
            cv::rectangle(imagencontours, areasvalles.at(i).Recta, cv::Scalar(0, 0, 255));
            cv::putText(imagencontours, "P" + std::to_string(numeroerrores), cv::Point(areasvalles.at(i).Recta.x + areasvalles.at(i).Recta.width / 2, areasvalles.at(i).Recta.y + areasvalles.at(i).Recta.height / 2), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255));
        }
    }
    //Verifica si no hay dientes
    double anguloactual = 0.0, anguloanterior = 0.0;
    double angulomenor = 360, diferencia = 0.0;
    for (int i = 0; i < numerocrestas; i++) {
        if (i == 0) anguloanterior = atan2f(centrocirculointerno.y - areascrestas.at(numerocrestas - 1).centroide.y, areascrestas.at(numerocrestas - 1).centroide.x - centrocirculointerno.x);
        else anguloanterior = atan2f(centrocirculointerno.y - areascrestas.at(i - 1).centroide.y, areascrestas.at(i - 1).centroide.x - centrocirculointerno.x);
        anguloactual = atan2f(centrocirculointerno.y - areascrestas.at(i).centroide.y, areascrestas.at(i).centroide.x - centrocirculointerno.x);
        anguloactual = (anguloactual > 0.0 ? anguloactual : (2 * M_PI + anguloactual));
        anguloanterior = (anguloanterior > 0.0 ? anguloanterior : (2 * M_PI + anguloanterior));
        diferencia = std::abs(anguloactual - anguloanterior)* 180.0 / M_PI;
        if (diferencia > 90) diferencia = 360 - diferencia; //cuando pasa un diente antes de 360 y el otro despues de cero me aseguro que no sume toda la vuelta
        areascrestas.at(i).anguloentrecrestas = diferencia;
    }
    //Saco la maxima frecuencia de angulos
    int frecuencia = 0, frecuenciamaxima = 0;
    float promediofrecuencia = 0.0;
    for (int i = 0; i < numerocrestas; i++) {
        frecuencia = 0;
        promediofrecuencia = 0;
        for (int j = 0; j < numerocrestas; j++) {
            if ((int) areascrestas.at(i).anguloentrecrestas == (int) areascrestas.at(j).anguloentrecrestas) {
                frecuencia += 1;
                promediofrecuencia += areascrestas.at(i).anguloentrecrestas;
            }
        }
        if (frecuencia > frecuenciamaxima) {
            frecuenciamaxima = frecuencia;
            angulomenor = promediofrecuencia / (float) frecuencia;
        }
    }
    std::cout << "Angulo con mas frecuencia: " << angulomenor << std::endl;
    float sumaangulos = 0.0;
    float anguloactualgrados = 0.0;
    float anguloactualcorregido = 0.0;
    //Busco desde el inicio de los angulos los dientes a traves de la suma del angulo con mas frecuencia
    for (int i = 0; i < numerocrestas; i++) {
        if (i == 0) {
            anguloactual = atan2f(centrocirculointerno.y - areascrestas.at(i).centroide.y, areascrestas.at(i).centroide.x - centrocirculointerno.x);
            anguloactualgrados = 180 * anguloactual / M_PI;
            anguloactualcorregido = (anguloactualgrados > 0.0 ? anguloactualgrados : (360.0 + anguloactualgrados));
            sumaangulos = anguloactualcorregido;
            sumatotalcircunferencia += areascrestas.at(i).anguloentrecrestas;
        } else {
            sumaangulos += angulomenor;
            if (sumaangulos > 360) sumaangulos -= 360;
            anguloactual = atan2f(centrocirculointerno.y - areascrestas.at(i).centroide.y, areascrestas.at(i).centroide.x - centrocirculointerno.x);
            anguloactualgrados = 180 * anguloactual / M_PI;
            anguloactualcorregido = (anguloactualgrados > 0.0 ? anguloactualgrados : (360.0 + anguloactualgrados));
            if (anguloactualcorregido <= 2 * offset) anguloactualcorregido += 360;
            if (sumaangulos <= 2 * offset) sumaangulos += 360;
            limitesuperior = sumaangulos + offset;
            limiteinferior = sumaangulos - offset;
            if (anguloactualcorregido <= limitesuperior && anguloactualcorregido >= limiteinferior) {
                sumaangulos = anguloactualcorregido;
                sumatotalcircunferencia += areascrestas.at(i).anguloentrecrestas;
            } else {
                numeroerrores += 1;
                dientesperdidos += 1;
                cv::Point2f puntoerror = centrocirculo + cv::Point2f(radiointermedio * cosf(sumaangulos * M_PI / 180.0), -radiointermedio * sinf(sumaangulos * M_PI / 180.0));
                std::cout << "Punto error: [" << puntoerror.x << "," << puntoerror.y << "]" << std::endl;
                errores += "P" + std::to_string(numeroerrores) + " No hay diente\n";
                cv::circle(imagencontours, puntoerror, 5, cv::Scalar(0, 0, 255), -1);
                cv::putText(imagencontours, "P" + std::to_string(numeroerrores), puntoerror, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 255));
                std::cout << "\033[1;31m No hay diente en esta ubicacion \033[0m" << std::endl;
                sumaangulos = anguloactualcorregido;
                sumatotalcircunferencia += angulomenor;
            }
        }
    }
    if (sumatotalcircunferencia <= 360 - offset && dientesperdidos == 0) { //Si no hay dientes perdidos reviso que la circunferencia total cumpla los 360º
        sumaangulos += angulomenor;
        numeroerrores += 1;
        dientesperdidos += 1;
        cv::Point2f puntoerror = centrocirculo + cv::Point2f(radiointermedio * cosf(sumaangulos * M_PI / 180.0), -radiointermedio * sinf(sumaangulos * M_PI / 180.0));
        std::cout << "Punto error: [" << puntoerror.x << "," << puntoerror.y << "]" << std::endl;
        errores += "P" + std::to_string(numeroerrores) + " No hay diente\n";
        cv::circle(imagencontours, puntoerror, 5, cv::Scalar(0, 0, 255), -1);
        cv::putText(imagencontours, "P" + std::to_string(numeroerrores), puntoerror, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 255));
        std::cout << "\033[1;31m No hay diente en esta ubicacion \033[0m" << std::endl;
    }
    modulo = (2 * radioexterior * constanteconversionpxacm) / (crestastotales.size() + dientesperdidos + 2); //M = (De)/(Z + 2)
    if ((modulo - (int) modulo) <= 0.7 && (modulo - (int) modulo) >= 0.3) {
        modulo = (int) modulo + 0.5;
    } else if ((modulo - (int) modulo) > 0.7) {
        modulo = (int) modulo + 1.0;
    } else if ((modulo - (int) modulo) < 0.3) {
        modulo = (int) modulo + 0.0;
    }
    paso = modulo * M_PI;
    roi = imagencontours(rectaobjetivo);
    std::cout << "*****Informacion del engrane*****" << std::endl;
    std::cout << "Hay " << dientesperdidos << " dientes perdidos" << std::endl;
    std::cout << "* Diametro exterior(De): " << 2 * radioexterior * constanteconversionpxacm << " mm" << std::endl;
    std::cout << "* Diametro interior(Di): " << 2 * radiocirculointerno * constanteconversionpxacm << " mm" << std::endl;
    std::cout << "* Modulo(M): " << modulo << std::endl;
    std::cout << "* Paso(P): " << paso << std::endl;
    std::cout << "* Numero de dientes(Z): " << crestastotales.size() + dientesperdidos << std::endl;
    std::cout << "*********************************" << std::endl;
    cv::putText(imagencontours, "Crestas: " + std::to_string(numerocrestas), centrocirculo - cv::Point2f(20, 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 125, 255));
    cv::putText(imagencontours, "Valles: " + std::to_string(numerovalles), centrocirculo - cv::Point2f(20, -10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(125, 0, 255));
    cv::circle(imagencontours, centrocirculo, radioexterior, cv::Scalar(255, 100, 100));
    cv::line(imagencontours, centrocirculo - cv::Point2f(20, 30), centrocirculo - cv::Point2f(-25, 30), cv::Scalar(255, 100, 100), 3);
    cv::putText(imagencontours, "D.E.", centrocirculo - cv::Point2f(45, 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 100, 100));
    cv::circle(imagencontours, centrocirculointerno, radiocirculointerno, cv::Scalar(255, 255, 255));
    cv::line(imagencontours, centrocirculo - cv::Point2f(15, 50), centrocirculo - cv::Point2f(-25, 50), cv::Scalar(255, 255, 255), 3);
    cv::putText(imagencontours, "D.I.", centrocirculo - cv::Point2f(45, 50), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
    cv::circle(imagencontours, centrocirculo, (int) radiointermedio, cv::Scalar(125, 255, 255));
    cv::line(imagencontours, centrocirculo - cv::Point2f(15, 70), centrocirculo - cv::Point2f(-25, 70), cv::Scalar(125, 255, 255), 3);
    cv::putText(imagencontours, "D.P.", centrocirculo - cv::Point2f(45, 70), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(125, 255, 255));
    std::cout << errores << std::endl;
#ifdef mostrarcapturas
    cv::namedWindow("ENGRANAJE ORIGINAL", cv::WINDOW_NORMAL);
    cv::imshow("ENGRANAJE ORIGINAL", imagen);
    cv::imshow("Lineas externas", lineasexternas);
    cv::imshow("Lineas internas", lineasinternas);
    cv::imshow("GEAR VERSION IMAGEN GRIS INVERSA: OTSU: ", imagengris);
    cv::imshow("Contorno del engranaje", imagencontours);
    cv::namedWindow("ROI", cv::WINDOW_NORMAL);
    cv::imshow("ROI", roi);
    cv::waitKey();
    cv::destroyAllWindows();
#endif
    bool erroresfinales = false;
    if (errores.compare("") == 0) erroresfinales = true;
    else erroresfinales = false;
    if(erroresfinales) return 1;
    else return 0;
}
#endif /* VISION_H */
