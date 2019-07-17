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
bool inicioproceso = false;
bool paroemergencia = false;
int datomanual = 40;
int modofuncionamiento = 0;

//Variables LED rgb
int rgb0 = 255;   //Rojo
int rgb1 = 255;   //Verde
int rgb2 = 255;   //Azul

// IF
int receptor = 0;
int umbral = 200;
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
int valor = 0; //Valor de la entrada infrarrojo
char c, d;
bool centrado = false;
//Variables para Arduino JSON Generator/Parser
const size_t bufferSize = JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(6);
DynamicJsonBuffer Buffersalida(bufferSize);
JsonObject& datosalidajson = Buffersalida.createObject();
unsigned long tiempoinicio = 0;

String datospuertoserial = "";
bool haycadena = false;
bool iniciocadena = false, fincadena = false;
int estado = 0;
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
  valor = analogRead(receptor);
  Serial.setTimeout(1000);
  //Serial.print("0");//Envia 0
  if ( valor <= umbral) {
    actd = true;
  }
  else {
    actd = false;
  }
  datosalidajson["presencia"] = actd;
  if (digitalRead(emerg) == LOW) {
    paroemergencia = true;
  }
  else paroemergencia = false;
  if (paroemergencia == true) {
    estado = -1;
    datosalidajson["estado"] = -1;
  }
  else {
    estado = 0;
    datosalidajson["estado"] = 0;
  }
  datosalidajson.printTo(Serial);
  delay(1000);
}

void loop() {
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
      rgb0 = 0, rgb1 = 0, rgb2 = 0;
      analogWrite(red, 255 - rgb0);
      analogWrite(blue, 255 - rgb1);
      analogWrite(green, 255 - rgb2);
    }
    if (d == '}') {
      fincadena = true;
      break;
    }
  }
  if (iniciocadena && fincadena) {
    iniciocadena = false, fincadena = false;
    rgb0 = 255, rgb1 = 255, rgb2 = 0;
    analogWrite(red, 255 - rgb0);
    analogWrite(blue, 255 - rgb1);
    analogWrite(green, 255 - rgb2);
    delay(200);
    DynamicJsonBuffer Bufferentrada(bufferSize);
    JsonObject& datojson  = Bufferentrada.parseObject(datospuertoserial);
    if (datojson.success()) {
      rgb0 = 255, rgb1 = 255, rgb2 = 0;
      analogWrite(red, 255 - rgb0);
      analogWrite(blue, 255 - rgb1);
      analogWrite(green, 255 - rgb2);
      delay(200);
      for (auto kv : datojson) {
        if (String(kv.key).compareTo("modofunc") == 0) {
          modofuncionamiento = kv.value.as<signed int>();
        }
        if (modofuncionamiento == 1) {
          Serial.println("modofuncionamiento 1:");
          if (String(kv.key).compareTo("motor") == 0) {
            dato = kv.value.as<signed int>();
            datoclasificacion = dato;
            /**/ Serial.print("dato del motor:  ");
             /**/ Serial.print(dato);
          }
          if (String(kv.key).compareTo("estado") == 0) {
            if (kv.value.as<signed int>() == -1) {
              analogWrite(motorpwm, 0);
              /**/ Serial.print("ESTADO --1");
              if (estado != -1) {
                estado = -1;
                datosalidajson["estado"] = -1;
                datosalidajson.printTo(Serial);
              }
              paroemergencia = true;
              inicioproceso = false;
              rgb0 = 100, rgb1 = 0, rgb2 = 0; //Rojo
            }
            else if (kv.value.as<signed int>() == 0) {
              analogWrite(motorpwm, dato);
              /**/ Serial.print("ESTADO 0");
              if (estado != 0) {
                estado = 0;
                datosalidajson["estado"] = 0;
                datosalidajson.printTo(Serial);
              }
              paroemergencia = false;
              inicioproceso = false;
              centrado = false;
              rgb0 = 255, rgb1 = 69, rgb2 = 0; //Naranja
            }
            else if (kv.value.as<signed int>() == 1) {
              /**/ Serial.print("ESTADO 1");
              if (estado != 1) {
                estado = 1;
                datosalidajson["estado"] = 1;
                datosalidajson.printTo(Serial);
              }
              centrado = false;
              paroemergencia = false;
              inicioproceso = true;
              dato = datoclasificacion;
              rgb0 = 0, rgb1 = 100, rgb2 = 0; //Verde
            }
          }
          if (String(kv.key).compareTo("clasificador") == 0) {
            clasificadorservo = kv.value.as<boolean>();
          }
          if (String(kv.key).compareTo("imagenprocesada") == 0) {
            rgb0 = 255, rgb1 = 255, rgb2 = 255;
            analogWrite(red, 255 - rgb0);
            analogWrite(blue, 255 - rgb1);
            analogWrite(green, 255 - rgb2);
            delay(200);
            vidprocesado = kv.value.as<boolean>();
          }
          if (String(kv.key).compareTo("rgb") == 0) {
            JsonArray& rgb = kv.value.as<JsonArray>();
            rgb0 = rgb[0]; // 255
            rgb1 = rgb[1]; // 255
            rgb2 = rgb[2]; // 255
            analogWrite(red, 255 - rgb0);
            analogWrite(green, 255 - rgb1);
            analogWrite(blue, 255 - rgb2);
            delay(500);
          }
        }
        else if (modofuncionamiento == -1 ) {
          Serial.println("modofuncionamiento ---1:");
          if (String(kv.key).compareTo("motor") == 0) {
            datomanual = kv.value.as<signed int>();
              Serial.print("MOTOR MANUAL ES  :");
                Serial.println(datomanual);
          }
          if (String(kv.key).compareTo("sentidomotor") == 0) {
            if (kv.value.as<signed int>() == 1) {
              analogWrite(motorpwm, datomanual);
              digitalWrite(motordir, HIGH);
               Serial.println("ADELANTE");
            }
            else if (kv.value.as<signed int>() == -1) {
              analogWrite(motorpwm, datomanual);
              digitalWrite(motordir, LOW);
               Serial.println("ATRAS");
            }
            else if (kv.value.as<signed int>() == 0) {
              analogWrite(motorpwm, 0);
              digitalWrite(motordir, HIGH);
            }
          }
          if (String(kv.key).compareTo("direcclasificacion") == 0) {
            if (kv.value.as<signed int>() == 1) {
              digitalWrite(objbueno, HIGH);
              digitalWrite(objmalo, LOW);
              Serial.println("bueno");
            }
            else  {
              digitalWrite(objmalo, HIGH);
              digitalWrite(objbueno, LOW);
              Serial.println("malo");
            }
          }
        }
        else {
          analogWrite(motorpwm, 0);
         digitalWrite(objmalo, LOW);
              digitalWrite(objbueno, LOW);
        }
      }
    } 
    else {
      Serial.print(datospuertoserial);
    }
    datospuertoserial = "";
  }




  // datos que envia el arduino al servidor
  if (digitalRead(emerg) == LOW) {
    centrado = false;
    if (estado != -1) {
      datosalidajson["estado"] = -1;
      datosalidajson.printTo(Serial);
      estado  = -1;
    }
    inicioproceso = false;
    paroemergencia = true;
  }
  else if (paroemergencia == true && digitalRead(emerg) == HIGH) paroemergencia = true;
  else paroemergencia = false; 
  if (paroemergencia) {
    analogWrite(motorpwm, 0);
    inicioproceso = false;
    rgb0 = 120, rgb1 = 0, rgb2 = 0;
    analogWrite(red, 255 - rgb0);
    analogWrite(green, 255 - rgb1);
    analogWrite(blue, 255 - rgb2);
  }
  else {
    if (inicioproceso && estado != 1 && !paroemergencia) {
      estado = 1;
      datosalidajson["estado"] = 1;
      datosalidajson.printTo(Serial);
    }
    else if (!inicioproceso && estado != 0 && !paroemergencia) {
      estado = 0;
      datosalidajson["estado"] = 0;
      datosalidajson.printTo(Serial);
    }
    if (digitalRead(start) == HIGH) {
      delay(500);
      if ( digitalRead(start) == HIGH) {
        rgb0 = 255, rgb1 = 69, rgb2 = 0; //Naranja
        inicioproceso = false;
        centrado = false;
        datosalidajson["estado"] = 0;
        delay(1000);
      }
      else {
        delay(500);
        inicioproceso = true;
        dato = 80;
        datosalidajson["estado"] = 1;
        rgb0 = 0, rgb1 = 100, rgb2 = 0;
      }
    }
    if (inicioproceso) {
      analogWrite(red, 255 - rgb0);
      analogWrite(green, 255 - rgb1);
      analogWrite(blue, 255 - rgb2);
      analogWrite(motorpwm, dato);
      valor = analogRead(receptor);
      if ( valor <= umbral) {
        actd = true;
      }
      else {
        actd = false;
      }
      delay(200);
      if ( actd != antd) {
        datosalidajson["presencia"] = actd;
        if (inicioproceso) {
          datosalidajson["estado"] = 1;
        }
        else datosalidajson["estado"] = 0;
        datosalidajson.printTo(Serial);
      }
      if (actd == true && vidprocesado == false) {
        dato = 0;
        analogWrite(motorpwm, dato);
        if (!centrado) {
          centrado = true;
          c = '0';
          tiempoinicio = millis();
          while ( c != '9') {
            if (millis() - tiempoinicio > 10000) break;
            rgb0 = 100, rgb1 = 100, rgb2 = 100;
            analogWrite(red, 255 - rgb0);
            analogWrite(green, 255 - rgb1);
            analogWrite(blue, 255 - rgb2);
            if (digitalRead(emerg) == LOW) {
              centrado = false;
              paroemergencia = true;
              if (estado != - 1) {
                estado = -1;
                datosalidajson["estado"] = -1;
                datosalidajson.printTo(Serial);
              }
              break;
            }
            if (digitalRead(start) == HIGH) {
              delay(500);
              if ( digitalRead(start) == HIGH) {
                rgb0 = 255, rgb1 = 69, rgb2 = 0; //Naranja
                inicioproceso = false;
                centrado = false;
                datosalidajson["estado"] = 0;
                delay(1000);
              }
              else {
                delay(500);
                inicioproceso = true;
                dato = 80;
                datosalidajson["estado"] = 1;
                rgb0 = 0, rgb1 = 100, rgb2 = 0;
              }
              break;
            }
            if (Serial.available()) {
              c = Serial.read();
              if (c == 'a') {
                digitalWrite(motordir, HIGH);
                analogWrite(motorpwm, 80);
                rgb0 = 0, rgb1 = 255, rgb2 = 255;
                analogWrite(red, 255 - rgb0);
                analogWrite(green, 255 - rgb1);
                analogWrite(blue, 255 - rgb2);
                delay(100);
              }
              else if (c == 'd') {
                rgb0 = 125, rgb1 = 125, rgb2 = 125;
                analogWrite(red, 255 - rgb0);
                analogWrite(green, 255 - rgb1);
                analogWrite(blue, 255 - rgb2);
                digitalWrite(motordir, LOW);
                analogWrite(motorpwm, 80);
                delay(100);
              }
              Serial.print('1');
              analogWrite(motorpwm, 0);
              digitalWrite(motordir, HIGH);
              delay(5);
            }
          }
          Serial.print('2');
          rgb0 = 0, rgb1 = 255, rgb2 = 0;
          analogWrite(red, 255 - rgb0);
          analogWrite(green, 255 - rgb1);
          analogWrite(blue, 255 - rgb2);
          c = 'f';
        }
      }
      else if (vidprocesado == true) {
        vidprocesado = false;
        centrado = false;
        rgb0 = 125, rgb1 = 125, rgb2 = 125;
        analogWrite(red, 255 - rgb0);
        analogWrite(green, 255 - rgb1);
        analogWrite(blue, 255 - rgb2);
        dato = datoclasificacion;

        if (clasificadorservo)  clasificador.write(30);
        else clasificador.write(150);
        while (actd == true) {
          valor = analogRead(receptor);
          if ( valor <= umbral) {
            actd = true;
          }
          else {
            actd = false;
          }
          analogWrite(motorpwm, dato);
          digitalWrite(motordir, HIGH);
          delay(200);
        }
        rgb0 = 0, rgb1 = 255, rgb2 = 0;
      }
      antd = actd;
    }
    else {
      analogWrite(motorpwm, 0);
      analogWrite(red, 255 - rgb0);
      analogWrite(green, 255 - rgb1);
      analogWrite(blue, 255 - rgb2);
    }
  }
}
