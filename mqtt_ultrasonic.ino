//This is the Ar code for snding data from Ultrasonic sensor to node-red via MQTT connection
//This program aim to measuring the volume of wash dish in a bottle

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int trig = 5 ; //pin D1 on ESP8266
const int echo = 4 ;// pin D2 on ESP8266
long durasi, cm1, volume;

// Change the credentials below, so your ESP8266 connects to your router

const char* ssid = "Galaxy A21s9B84"; //wifi address that I connect to 
const char* password =  "wjbc1007"; // password of wifi adress

// Change the variable to your Raspberry Pi IP address or personal computer, so it connects to your MQTT broker
const char* mqtt_server = "192.168.5.72";//in my case this is 

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.publish("volume", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
 pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
  digitalWrite(trig, LOW);
delayMicroseconds(5);
digitalWrite(trig, HIGH);
delayMicroseconds(10);
digitalWrite(trig, LOW);


// reading signal from sensor: HIgH pulse
// durasi is duration  (in ms) from sender
// from ping for Echo off from the object
pinMode(echo, INPUT);

durasi = pulseIn(echo, HIGH);

cm1 = (durasi/2) / 29.1; //distance from sensor to surface of the water

volume = (21+ 6.5 - (cm1)) * 275 ;

    }

    static char volume_sabun_cuci_piring [7];
    dtostrf(volume,7,3,volume_sabun_cuci_piring);
    

    // publish the measured volume
    client.publish("volume", volume_sabun_cuci_piring);

    
    Serial.print("volume: ");
    Serial.print(cm1);
  }
