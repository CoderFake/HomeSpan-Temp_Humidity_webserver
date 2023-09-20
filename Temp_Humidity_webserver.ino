#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WebServer server(80);

const char* ssid = "Dieu";
const char* password = "Hoangdieu2002";
const char* apiKey = "ee590aac9737cddd44c5da1f18e951f2";

float temperature = 0.0;
float humidity = 0.0;
String lat = "21.027169";
String lon = "105.867890";

void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>Weather Monitor</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
          h1 { margin-bottom: 1.2em; }
          h2 { margin: 0; }
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em; font-size: 2em; text-decoration: none }
          .btn.OFF { background-color: #333; }
        </style>
      </head>
      <body>
        <h1>Weather Monitor</h1>
        <div>
          <h2>Temperature: <span id="temp">0</span>&deg;C</h2>
          <h2>Humidity: <span id="humidity">0</span>%</h2>
        </div>
        <script>
          function updateWeather() {
            fetch('/weather')
              .then(response => response.json())
              .then(data => {
                document.getElementById('temp').textContent = data.temperature;
                document.getElementById('humidity').textContent = data.humidity;
              });
          }
          setInterval(updateWeather, 5000); 
          updateWeather(); 
        </script>
      </body>
    </html>
  )";

  server.send(200, "text/html", response);
}

void setup(void) {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);

  server.on("/weather", HTTP_GET, []() {
    String payload = GET_Request();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    temperature = doc["main"]["temp"];
    humidity = doc["main"]["humidity"];
    String weatherData = "{\"temperature\": " + String(temperature, 1) + ", \"humidity\": " + String(humidity, 1) + "}";
    server.send(200, "application/json", weatherData);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);
  delay(5000); 
}

String GET_Request() {
  String url = "http://api.openweathermap.org/data/2.5/weather?";
  HTTPClient http;

  url += "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + apiKey;

  http.begin(url);
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}
