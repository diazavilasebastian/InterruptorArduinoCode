#include <Button.h>
#include <SoftwareSerial.h>

//********Serial Comunicación***********
const int TX = 2;
const int RX = 3;
SoftwareSerial SerialArduino(RX, TX); 
//********Parametros***********
String id = "1";
String nombre = "Ninguno";
String grupo = "Ninguno";
int estado = 1;
int luzOn = 0;
int distancia = 100;
int luz = 1;
int tiempoencendida = 1000; //10 seg
//********sensores***********
//Movimiento
int pirPin = 6;

//distancia
const int echo = 8;
const int trig = 9;

//ampolletas
const int pinAmpolleta = 4;
const int pinAmpolleta2 = 5;


//luz
const int LDR =A0;

//botones
Button button = Button(12);
Button button2 = Button(11);

//tiempo transcurrido para sensores
unsigned long tiemposensor1=0;
unsigned long tiemposensor2=0;  
//********estado y botones luz***********
bool ampolleta = false;
bool ampolleta2 = false;

unsigned long time;

void onPress(Button& b){
    grupo= "Ninguno";
    estado=1;
    if(ampolleta){
      ampolleta=false;  
    }else{
      ampolleta=true; 
    }
    if(!ampolleta && !ampolleta2){
       luzOn = false; 
    }else{
       luzOn = true; 
    }
    SerialSendData();
}

void onPress2(Button& b){
    grupo= "Ninguno";
    estado=1;
    if(ampolleta2){
      ampolleta2=false;  
    }else{
      ampolleta2=true; 
    }
    if(!ampolleta && !ampolleta2){
       luzOn = false; 
    }else{
       luzOn = true; 
    }
    SerialSendData();
    
}

void setup(){
  Serial.begin(9600);
  SerialArduino.begin(9600);
  pinMode(pirPin, INPUT);

  pinMode(pinAmpolleta, OUTPUT);
  pinMode(pinAmpolleta2, OUTPUT);

  pinMode(trig, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(echo, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/
  // Assign callback function
  button.pressHandler(onPress);
  button2.pressHandler(onPress2);
}

void loop(){
  unsigned long currentMillis = millis();
  switch(estado){
    case 1:
      imprimirInterruptor();
      Serial.println(analogRead(LDR));
      break;

    case 2:
      //Movimiento
      imprimirInterruptor();
      if(analogRead(LDR) <= luz){
       if(digitalRead(pirPin) == HIGH){
         ampolleta=true; 
         ampolleta2=true;   
        }else{
            if ((unsigned long)(currentMillis - tiemposensor1) > tiempoencendida) {
                ampolleta=false; 
                ampolleta2=false;  
                tiemposensor1 = currentMillis;
             }
        }
      }
      break;

    default:
       //distancia
       imprimirInterruptor();
      if(analogRead(LDR) <= luz){
        if(calcularDistancia() <= distancia){
           ampolleta=true; 
           ampolleta2=true;  
        }else{
            if ((unsigned long)(currentMillis - tiemposensor2) > tiempoencendida) {
                ampolleta=false; 
                ampolleta2=false;  
                tiemposensor2 = currentMillis;
             }
        }
      }
       break;
  }
  encenderLuz();
  SerialInputData();
  // update the buttons' internalz
  button.process();
  button2.process();
}

void encenderLuz(){
  if(ampolleta){
    digitalWrite(pinAmpolleta,HIGH);
  }else{
    digitalWrite(pinAmpolleta,LOW);
  }
  if(ampolleta2){
    digitalWrite(pinAmpolleta2,HIGH);
  }else{
    digitalWrite(pinAmpolleta2,LOW);
  }
  
}

void imprimirInterruptor(){
  Serial.println("Estado:"+String(estado)+" Grupo:"+grupo);
}


void SerialInputData(){
  char caracter;
  String data="";
  while(SerialArduino.available()){
    caracter = SerialArduino.read();
    data.concat(caracter);
    delay(10);
  }
  if(data!=""){
    //1&esadmaskndpasndpasnd&Ninguno&1&1&1&1&1
    //id&nombre&grupo&estado&encendido&distancia&luz&retardo;
     data.trim();
     Serial.println(data);
     nombre = valueString(1,data);
     grupo = valueString(2,data);
     estado = valueString(3,data).toInt();
     luzOn = valueString(4,data).toInt();
     distancia = valueString(5,data).toInt();
     luz = valueString(6,data).toInt();
     int tiempoencendidatmp = valueString(7,data).toInt(); 
     tiempoencendida = tiempoencendidatmp*1000;
     if(estado == 1){
        if(luzOn == 1){
          ampolleta = true;
          ampolleta2 = true;
        }else{
          ampolleta = false;
          ampolleta2 = false;
        }
     }
  }
}

void SerialSendData(){
  Serial.println("Enviando datos");
  String data = String(id)+"&"+nombre+"&"+grupo+"&"+String(estado)+"&"+String(luzOn)+"&"+String(distancia)+"&"+String(luz)+"&"+String(tiempoencendida);
  SerialArduino.println(data);
}


String valueString(int posicion,String data){  
  for(int i=0;i< posicion;i++){
      data = data.substring(data.indexOf("&")+1);
    }
   return data.substring(0,data.indexOf("&"));  
}


long calcularDistancia(){
  long distancia2;
  long tiempo;
  
  digitalWrite(trig,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  
  digitalWrite(trig, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo=pulseIn(echo, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 8 empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/
  
  distancia2= int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
  /*Monitorización en centímetros por el monitor serial*/
  return distancia2;
  
}
