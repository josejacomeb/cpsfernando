#include "ArduinoJson.h" //Generador de Json para C++, el asistente se puede encontrar aca https://arduinojson.org/v5/assistant/
// INICIO Y PARO
int inicio = 7;
int paro = 9;
int ledstate = 8;


//Variables para Arduino JSON Generator/Parser
const size_t bufferSize = JSON_OBJECT_SIZE(2);
DynamicJsonBuffer Buffersalida(bufferSize);
JsonObject& datosalidajson = Buffersalida.createObject();
unsigned long tiempoinicio = 0;
char z, d;
int flag = 0;
char serialequipo2 = 0;
String datospuertoserial = "";
bool haycadena = false;
bool iniciocadena = false, fincadena = false;
void setup() {
  Serial.begin(115200);
  pinMode(ledstate, OUTPUT);
  pinMode(inicio, INPUT);
  pinMode(paro, INPUT);
  //Serial.print(1); //Indica que es el segundo dispositivo del puerto serial
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
    }
    if (d == '}') {
      fincadena = true;
      break;
    }
  }
  if (iniciocadena && fincadena) {
    iniciocadena = false, fincadena = false;
    DynamicJsonBuffer Bufferentrada(bufferSize);
    JsonObject& datojson  = Bufferentrada.parseObject(datospuertoserial);
    if (datojson.success()) {
      for (auto kv : datojson) {
        if (String(kv.key).compareTo("led") == 0) {
          if (kv.value.as<bool>() == true) {
            digitalWrite(ledstate, HIGH);
           // Serial.print("L1");
          } else {
            digitalWrite(ledstate, LOW);
           // Serial.print("L0");
          }
        }
      }
    }
    else {
      Serial.print(datospuertoserial);
    }
    datospuertoserial = "";
  }
  if (digitalRead(inicio) == HIGH) {
    flag = 1;
    delay(120);


    if (flag != 0) {
      // datosalidajson["e2"]=1;
      datosalidajson["led"] = true;
      datosalidajson.printTo(Serial);
      digitalWrite(ledstate, HIGH);
      flag = 0;
    }
  }
  if (digitalRead(paro) == HIGH) {
    flag = 1;
    delay(120);
    if (flag != 0) {
      datosalidajson["led"] = false;
      datosalidajson.printTo(Serial);
      digitalWrite(ledstate, LOW);
      flag = 0;
    }
  }

}

