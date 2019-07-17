#include "ArduinoJson.h" //Generador de Json para C++, el asistente se puede encontrar aca https://arduinojson.org/v5/assistant/
//LED MULTICOLOR
int red = 6;
int blue = 3;
int green = 5;
int objbueno = 13;
int objmalo = 8;

// INICIO Y PARO
int emerg = 9;
int start = 7;


//Variables para iniciar proceso y parar el proceso

bool modofuncionamiento = true;
int datosentidomotor = 0;
int datotoclasificar = 0;
int datoestado = 0;
bool stanby = false;
//Variables LED rgb
int rgb0 = 255;   //Rojo
int rgb1 = 255;   //Verde
int rgb2 = 255;   //Azul
bool sis_activado = false; /////////////////////
bool sis_activado2 = false; /////////////////////
bool resetdatosentido = false; /////////////////////////
// IF
int receptor = 0;
int umbralprograma = 50;
int datoumbral = 0;
//clasificador
#include <Servo.h>
Servo clasificador;
int pinservo = 10;
#define motorpwm 11
int motordir = 12;
bool actd = false;
bool antd = false;
bool vidprocesado = false;
bool clasificadorservo = false;
int dato = 80;
int datoclasificacion = 80;
int valorinicial = 0; //Valor de la entrada infrarrojo
int valor = 0; //Valor de la entrada infrarrojo
int dispositivo = 0; //Identificador dispositivo
char c, d;
bool centrado = false;
//Variables para Arduino JSON Generator/Parser
const size_t bufferSize = JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(8);
DynamicJsonBuffer Buffersalida(bufferSize);
JsonObject& datosalidajson = Buffersalida.createObject();
unsigned long tiempoinicio = 0;

String datospuertoserial = "";
bool haycadena = false;
bool iniciocadena = false, fincadena = false;
int estado = 0;
#define debug 1
void setup() {
  Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(objmalo, OUTPUT);
  pinMode(objbueno, OUTPUT);
  pinMode(motorpwm, OUTPUT);
  pinMode(motordir, OUTPUT);
  pinMode(emerg, INPUT);
  pinMode(start, INPUT);
  digitalWrite(motordir, HIGH);
  clasificador.attach(pinservo);
  clasificador.write(90);
  analogWrite(motorpwm, 0);
  analogWrite(red, 255 - rgb0);
  analogWrite(blue, 255 - rgb1);
  analogWrite(green, 255 - rgb2);
  Serial.setTimeout(1000);
  //Serial.println(valor);
  valor = analogRead(receptor);
  //Serial.println(valor);
  if ( (valorinicial >= 0) || (valorinicial <= 1000)) {
    sis_activado = false;
  }
  else {
    sis_activado = true;
  }
}

void loop() {
  ////////////////

  // actd=antd;
  //Serial.println(valor);
  // datosalidajson["estadoumbral"] = sis_activado;
  ////////
  // datosalidajson.printTo(Serial);
  while (Serial.available() > 0) { //Siempre leo los datos del Serial
    haycadena = true;
    d = Serial.read();
    //Verifico que me lleguen las llaves { y } antes de procesar el objeto
    if (d == '{') {
      datospuertoserial = "";
      iniciocadena = true;
    }
    if (iniciocadena) {
      datospuertoserial.concat(d);
      rgb0 = 20, rgb1 = 20, rgb2 = 20;
      analogWrite(red, 255 - rgb0);
      analogWrite(blue, 255 - rgb1);
      analogWrite(green, 255 - rgb2);
    }
    if (d == '}') {
      fincadena = true;
      break;
    }
  }////Fin del Serial Available
  if (iniciocadena && fincadena) {
    iniciocadena = false, fincadena = false;
    rgb0 = 255, rgb1 = 255, rgb2 = 0;
    analogWrite(red, 255 - rgb0);
    analogWrite(blue, 255 - rgb1);
    analogWrite(green, 255 - rgb2);
    delay(200);
#ifdef debug
    Serial.println(datospuertoserial);
#endif
    DynamicJsonBuffer Bufferentrada(bufferSize);
    JsonObject& datojson  = Bufferentrada.parseObject(datospuertoserial);
    if (datojson.success()) {
      rgb0 = 255, rgb1 = 255, rgb2 = 0;
      analogWrite(red, 255 - rgb0);
      analogWrite(blue, 255 - rgb1);
      analogWrite(green, 255 - rgb2);
      delay(200);
      for (auto kv : datojson) {
#ifdef debug
        Serial.println("inicioJSON");
#endif
        if (String(kv.key).compareTo("umbral") == 0) {
          datoumbral = kv.value.as<signed int>();
#ifdef debug
          Serial.println("dato umbral: ");
          Serial.println( datoumbral);
#endif
        }
        if (String(kv.key).compareTo("modofunc") == 0) {
          modofuncionamiento = kv.value.as<bool>();
#ifdef debug
          if (modofuncionamiento) Serial.println("Modo automático");
          else Serial.println("Modo manual");
#endif
        }
        if (String(kv.key).compareTo("motor") == 0) {
          dato = kv.value.as<signed int>();
          datoclasificacion = dato;
#ifdef debug
          Serial.print("Valor motor: ");
          Serial.println(dato);
#endif
        }
        if (String(kv.key).compareTo("sentidomotor") == 0  ) {
          datosentidomotor = kv.value.as<signed int>();
#ifdef debug
          Serial.print("Dato sentido motor: ");
          Serial.println( datosentidomotor);
#endif
        }
        if (String(kv.key).compareTo("direcclasificacion") == 0  ) {
          datotoclasificar = kv.value.as<signed int>();
#ifdef debug
          Serial.print("Direccion clasificacion: %d");
          Serial.println(datotoclasificar);
#endif
        }
        datospuertoserial = "";
      }//Fin del For
    }//Fin del procesamiento de JSOn
    else {
      Serial.println(datospuertoserial); //Si por algun motivo no procesa bien, devuelva al dispositivo que genero la señal
    }
    datospuertoserial = "";
  }

  if (!sis_activado) {
    if (datoumbral >= 400) {
      sis_activado2 = true;
    } else sis_activado2 = false;
  }
  /* if (sis_activado2) {
     valor = analogRead(receptor);
     if ( valor <= datoumbral) {
       actd = true;
     }
     else {
       actd = false;
     }
     delay(200);
     if ( actd != antd) {
       datosalidajson["presencia"] = actd;
     }
     datosalidajson.printTo(Serial);
     antd=actd;
    }*/
  if (modofuncionamiento == false ) {        //  Aqui Hago el modo manual
    /*
      while (modofuncionamiento == false ) {        //  Aqui Hago el modo manual

      if (modofuncionamiento == true ) break;*/

    if (sis_activado2 == true) {

      // Serial.println("ACTIVADO,");

      analogWrite(motorpwm, 0);
      tiempoinicio = millis();
      while (datosentidomotor == 1) {
        Serial.println("ADELANTE: ");
        analogWrite(motorpwm, dato);
        digitalWrite(motordir, HIGH);
        delay(1000);
        break;
        if ((millis() - tiempoinicio) > 4000) break;
      }
      while (datosentidomotor == -1) {
        Serial.println("ATRAS: ");
        analogWrite(motorpwm, dato);
        digitalWrite(motordir, LOW);
        delay(1000);
        break;
        if ((millis() - tiempoinicio) > 4000) break;
      }

      if ((datotoclasificar == 1) || (datotoclasificar == 1 && datosentidomotor == 0) ) {
        digitalWrite(objbueno, HIGH);
        digitalWrite(objmalo, LOW);
        clasificador.write(30);
        Serial.println("bueno");

      }
      else if (datotoclasificar == -1 || (datotoclasificar == -1 && datosentidomotor == 0))  {
        digitalWrite(objmalo, HIGH);
        digitalWrite(objbueno, LOW);
        clasificador.write(150);
        Serial.println("malo");
      } else if (datotoclasificar == 0) {

      }
      sis_activado == false;
      datosentidomotor = 0;
      datotoclasificar = 0;


    }
    else {
      analogWrite(motorpwm, 0);
      analogWrite(red, 255 - rgb0);
      analogWrite(green, 255 - rgb1);
      analogWrite(blue, 255 - rgb2);

    }

  }
  else {
   // Serial.println("AUTOMATICO");
    rgb0 = 0, rgb1 = 0, rgb2 = 0;
    analogWrite(red, 255 - rgb0);
    analogWrite(blue, 255 - rgb1);
    analogWrite(green, 255 - rgb2);
  }

  /*while (modofuncionamiento == true)
    {
    if (modofuncionamiento ==false) break;
        Serial.println("AUTOMATICO");
      rgb0 = 0, rgb1 = 0, rgb2 = 0;
      analogWrite(red, 255 - rgb0);
      analogWrite(blue, 255 - rgb1);
      analogWrite(green, 255 - rgb2);
    }*/

}

