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
long tiempoencendida = 1000; //1seg
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

int luzReal=0;


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

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 

  button.pressHandler(onPress);
  button2.pressHandler(onPress2);
}

void loop(){
  unsigned long currentMillis = millis();
  int value= digitalRead(pirPin);
  
  if(!ampolleta && !ampolleta2){
      luzReal  = analogRead(LDR);
  }

  switch(estado){
    case 1:
      imprimirInterruptor();
      break;

    case 2:
      //Movimiento
      imprimirInterruptor();
      if(luzReal <= luz){
       if(value == HIGH){
         ampolleta=true; 
         ampolleta2=true;   
        }else{
            if ((unsigned long)(currentMillis - tiemposensor1) > tiempoencendida+1000) {
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
      if(luzReal <= luz){
        if(calcularDistancia() <= distancia){
           ampolleta=true; 
           ampolleta2=true;  
        }else{
            if ((unsigned long)(currentMillis - tiemposensor2) > tiempoencendida+1000) {
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

  button.process();
  button2.process();
}

void encenderLuz(){
  if(ampolleta){
    digitalWrite(pinAmpolleta,LOW);
  }else{
    digitalWrite(pinAmpolleta,HIGH);
  }
  if(ampolleta2){
    digitalWrite(pinAmpolleta2,LOW);
  }else{
    digitalWrite(pinAmpolleta2,HIGH);
  }
  
}

void imprimirInterruptor(){
  Serial.println("Nombre:"+nombre+" Estado:"+String(estado)+" Grupo:"+grupo+" encendida:"+luzOn+" distancia:"+distancia+" luz:"+luz+" retardo:"+tiempoencendida);
  
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
     tiempoencendida = tiempoencendidatmp;
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
  
  digitalWrite(trig,LOW); 
  delayMicroseconds(5);
  
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10);
  tiempo=pulseIn(echo, HIGH);
  
  distancia2= int(0.017*tiempo); 
  return distancia2;
  
}
