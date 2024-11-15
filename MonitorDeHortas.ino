#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "**";
const char *password = "**";

WebServer server(80);
DHT dht(15, DHT11);

void handleRoot() {
  char msg[2000];  // Adjusted buffer size if needed

  snprintf(msg, 2000,
           "<html lang='pt-BR'>\
  <head>\
    <meta http-equiv='refresh' content='5'>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>Monitor de Horta</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>Monitor de Horta</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperatura</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Umidade</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
      <h1>Nascer e Por do Sol</h1>\
        <p id='sunrise'>Carregando...</p>\
        <p id='sunset'>Carregando...</p>\
 <script>\
 const apiURL = 'https://api.sunrisesunset.io/json?lat=-5.0874608&lng=-42.8049571&timezone=America/Sao_Paulo&date=today';\
 function updateSunTimes() {\
   fetch(apiURL)\
   .then(res => res.json())\
   .then(data => {\
     const sunrise = data.results.sunrise;\
     const sunset = data.results.sunset;\
     document.getElementById('sunrise').innerText = 'Nascer do sol: ' + sunrise;\
     document.getElementById('sunset').innerText = 'Por do sol: ' + sunset;\
   })\
   .catch(error => console.error('Erro:', error));\
 }\
 updateSunTimes();\
 setInterval(updateSunTimes, 10000);  // Update every 10 seconds\
 </script>\
  </body>\
</html>",
           readDHTTemperature(), readDHTHumidity());
  server.send(200, "text/html", msg);
}


void setup(void) {

  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks

  if ((WiFi.status() == WL_CONNECTED)) {
    delay(5000);
  }
}


float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(h);
    return h;
  }
}
