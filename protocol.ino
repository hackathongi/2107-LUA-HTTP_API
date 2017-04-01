#include <ESP8266WiFi.h>


//NOMBRE DE CRIDES
#define N_CRIDES 2


const char* ssid = "tarla";
const char* password = "tarla2017hack";

int ledPin = 13; // GPIO13
WiFiServer server(80);

struct parametre{
  String nom;
  String valor;
};

struct entrada{
  String nom;
  String (*funcio)(parametre params[]);
};

entrada entrades[N_CRIDES]; 

String encendre(parametre params[]){
  Serial.println(sizeof(params));
  Serial.println((sizeof(params)/sizeof(parametre)));
  for(int i = 0; i < (sizeof(params)/sizeof(parametre)); i++){
    Serial.print("Parametre trobat, nom: ");
    Serial.print(params[i].nom);
    Serial.print(" valor: ");
    Serial.print(params[i].valor);
  }
  return "TOT PERFECT";
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

//is mandatory to implement this function with your device
String status(parametre params[]){
  String myDeviceIsA = "Light";
  return "I'm the :" + myDeviceIsA + ", my IP is " + ipToString(WiFi.localIP());
}
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  IPAddress ip(192, 168, 2, 3);
  IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  //don't overwrite status function, don't forget to implement it.
  entrada e0;
  e0.nom = "status";
  e0.funcio = status;
  entrades[0] = e0;

  //customize your functions here:
  entrada e1;
  e1.nom = "on";
  e1.funcio = encendre;
  entrades[1] = e1;

}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  String url = request.substring(request.indexOf(' ')+1, request.lastIndexOf(' '));
  Serial.println(url);
  
  int indexParam = url.indexOf("?");
  String nomFuncio = url.substring(url.lastIndexOf("/")+1, indexParam);
  Serial.print("Nom funcio ");
  Serial.println(nomFuncio);
  
  int nParams = 0;
  if(url.indexOf('?') != -1){
    nParams++;
  }
  if(nParams > 0){
    Serial.println("dins");
    int posicio = 0;
    while((posicio = url.indexOf('&', posicio+1)) != -1){Serial.print("posicio");Serial.println(posicio);nParams++;}
  }
  
  parametre parametres[nParams];  
    
  if(nParams > 0){
    String p = url.substring(indexParam+1, url.length());
    int fi = p.indexOf('&');
    for(int i = 0; i < nParams; i++){
      Serial.println("url");
      Serial.println(p);
      if(fi == -1){
        fi = p.length();
      }
      int posIgual = p.indexOf('=');
      parametres[i] = {p.substring(0, posIgual),p.substring(posIgual+1, fi)};
      p = p.substring(fi+1, url.length());
      fi = p.indexOf('&');
      
      Serial.print("Parametre nom:");
      Serial.print(parametres[i].nom);
      Serial.print(" valor: ");
      Serial.println(parametres[i].valor);
      Serial.println();
    } 
  }
  
  int index = 0;
  while(index < N_CRIDES && nomFuncio != entrades[index].nom){
    index++;
  } 

  if (index < N_CRIDES) {
    String resposta = entrades[index].funcio(parametres);
  
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");  
    client.println("");  
    client.println(resposta);    
    
  }

  client.flush();
  client.stop();
}


