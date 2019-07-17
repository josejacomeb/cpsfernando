#include "ArduinoJson.h" //Generador de Json para C++, el asistente se puede encontrar aca https://arduinojson.org/v5/assistant/
// INICIO Y PARO
int inicio = 7;
int paro = 8;
int ledstate = 12;


//Variables para Arduino JSON Generator/Parser
const size_t bufferSize =JSON_OBJECT_SIZE(2);
DynamicJsonBuffer Buffersalida(bufferSize);
JsonObject& datosalidajson = Buffersalida.createObject();
unsigned long tiempoinicio = 0;

String datospuertoserial = "";
bool haycadena = false;
bool iniciocadena = false, fincadena = false;
void setup() {
  Serial.begin(115200);
  pinMode(ledstate, OUTPUT);
  pinMode(inicio, INPUT);
  pinMode(paro, INPUT);


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
            ledstate = kv.value.as<boolean>();
            digitalWrite(ledstate, LOW);
          } else  {
            digitalWrite(ledstate, HIGH);
          }
        }
      }
      else {
        Serial.println(datospuertoserial);
      }
      datospuertoserial = "";
    }
    if (digitalRead(paro) == LOW) {
      if (led != 0) {
        datosalidajson["led"] = 0;
        datosalidajson.printTo(Serial);
        digitalWrite(ledstate, LOW);
      }
    }
    if (digitalRead(inicio) == HIGH) {
      if (led != 1) {
        datosalidajson["led"] = 1;
        datosalidajson.printTo(Serial);
        digitalWrite(ledstate, HIGH);
      }
    }
  }
