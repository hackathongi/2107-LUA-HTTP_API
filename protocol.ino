#include <ESP8266WiFi.h>

//NOMBRE DE CRIDES
#define N_CRIDES 1


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
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
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



  entrada e1;
  e1.nom = "on";
  e1.funcio = encendre;
  entrades[0] = e1;
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
  String prova = entrades[index].funcio(parametres);
  
  
  Serial.println(prova);
  client.flush();
}

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
