# DESARROLLO E INTEGRACIÓN DE UN SISTEMA CIBER-FISICO APLICADO AL CONTROL DE CALIDAD UTILIZANDO VISIÓN ARTIFICIAL

## Autor

* **Fernando Israel Jácome Ramirez**

## Materiales
* Raspberry Pi 3 B +
* STM32
* Pixy Cam
* Actuadores
* Servidor Web

## Software
* Netbeans

## Dependencias de Json:
* Socket.io
* Express
* http
* fs

## Dependencias de C++:
* [Rapid JSON](https://github.com/Tencent/rapidjson)
* [Websockettpp](https://github.com/zaphoyd/websocketpp)
* [Socket.IO C](https://github.com/socketio/socket.io-client-cpp)
(Ejecutar sudo ldconfig para integrar todas las librerias en el sistema)

# Roadmap
- [x] ~Crear un servidor web con Node.js~
- [x] ~Integrar envio de JSON al servidor con C++~
- [x] ~Integrar los eventos en la página Web~
- [x] ~Enviar archivos por el Socket~
- [x] ~Enviar imagenes de la camara por el Socket~
- [x] Integrar las imagenes en la pagina Web
- [x] Hacer un data base de la imagen para mostrarlas despues
- [x] Integrar el STM32 al Raspberry Pi por UART

# Formato JSON Envio - Recepcion Datos
* Datos: Ejemplo

```
{
"dispositivo": 1,
"motor": -255,
"presencia": false,
"estado": -1,
"rgb": [0,0,0],
"imagenprocesada": false
}
```
* Imagen(Ejemplo)
```{
  "cadena64": "cadenaenbase64archivobinario",
  "extension": ".jpg",
  "fecha": "Mon Jul 16 17:16:55 2018\n"
}```
