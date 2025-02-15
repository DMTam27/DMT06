#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "tam";
const char* password = "27112006";

// Create WebServer object on port 80
WebServer server(80);

// Create servo objects
Servo myServos[4];

// Servo pins
#define SERVO_PIN_1 13
#define SERVO_PIN_2 12
#define SERVO_PIN_3 14
#define SERVO_PIN_4 26

// Target angles for the servos
int targetAngles[4] = {0, 0, 0, 0};

// HTML to control servos
const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Robot Arm Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body {font-family: Arial, sans-serif; text-align: center; padding: 50px;}
  h1 {color: #333;}
  input[type=range] {
    -webkit-appearance: none;
    width: 80%;
    margin: 20px 0;
    height: 25px; /* Height of the slider */
    background: #ddd;
    outline: none;
    opacity: 0.9;
    -webkit-transition: .2s;
    transition: opacity .2s;
  }
  input[type=range]:hover {
    opacity: 1;
  }
  input[type=range]::-webkit-slider-thumb {
    -webkit-appearance: none;
    appearance: none;
    width: 25px; /* Width of the thumb */
    height: 25px; /* Height of the thumb */
    background: #4CAF50;
    cursor: pointer;
  }
  input[type=range]::-moz-range-thumb {
    width: 25px; /* Width of the thumb */
    height: 25px; /* Height of the thumb */
    background: #4CAF50;
    cursor: pointer;
  }
</style>
<script>
function updateServo(index, angle) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/servo?index=" + index + "&angle=" + angle, true);
  xhr.send();
}
</script>
</head>
<body>
<h1>Robot Arm Control</h1>
<label for="servo1">Servo 1:</label><br>
<input type="range" id="servo1" name="servo1" min="0" max="180" oninput="updateServo(0, this.value)">
<br><br>
<label for="servo2">Servo 2:</label><br>
<input type="range" id="servo2" name="servo2" min="0" max="180" oninput="updateServo(1, this.value)">
<br><br>
<label for="servo3">Servo 3:</label><br>
<input type="range" id="servo3" name="servo3" min="0" max="180" oninput="updateServo(2, this.value)">
<br><br>
<label for="servo4">Servo 4:</label><br>
<input type="range" id="servo4" name="servo4" min="0" max="180" oninput="updateServo(3, this.value)">
</body>
</html>)rawliteral";

// Handle servo control request
void handleServo() {
  if (server.hasArg("index") && server.hasArg("angle")) {
    int index = server.arg("index").toInt();
    int angle = server.arg("angle").toInt();
    if (index >= 0 && index < 4) {
      targetAngles[index] = angle;
      myServos[index].write(angle);
      server.send(200, "text/plain", "Servo " + String(index + 1) + " target angle set to: " + String(angle));
    } else {
      server.send(400, "text/plain", "Invalid servo index");
    }
  } else {
    server.send(200, "text/html", html);
  }
}

// Handle root URL request
void handleRoot() {
  server.send(200, "text/html", html);
}

// Setup function
void setup() {
  Serial.begin(115200);
  myServos[0].attach(SERVO_PIN_1);
  myServos[1].attach(SERVO_PIN_2);
  myServos[2].attach(SERVO_PIN_3);
  myServos[3].attach(SERVO_PIN_4);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  // Check for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  // Print local IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define the handle for the servo control
  server.on("/servo", handleServo);
  
  // Define the handle for the root URL
  server.on("/", handleRoot);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}
