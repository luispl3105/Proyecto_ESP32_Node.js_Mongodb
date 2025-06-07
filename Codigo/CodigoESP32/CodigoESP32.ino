#include <WiFi.h>         // Librería para gestionar la conexión WiFi en el ESP32
#include <HTTPClient.h>   // Librería para realizar solicitudes HTTP
#include <Ultrasonic.h>   // Librería para manejar el sensor ultrasónico HC-SR04
#include "time.h" // Para sincronizar con NTP

// Configuración de la red WiFi a la que se conectará el ESP32
const char* ssid = "TUUNET-FO-3548";     // Nombre de la red WiFi
const char* password = "P3043BAR7";      // Contraseña de la red WiFi

// URL del servidor Node.js al que se enviarán los datos
const char* serverUrl = "http://192.168.100.8:3000/api/sensor";

// Definición de los pines utilizados por el sensor ultrasónico HC-SR04
#define TRIG_PIN 5   // Pin de disparo (Trigger)
#define ECHO_PIN 18  // Pin de eco (Echo)

// Creación del objeto sensor con los pines configurados
Ultrasonic sensor(TRIG_PIN, ECHO_PIN);

// Servidor NTP 
const char* ntpServer = "pool.ntp.org";

// Ajuste de zona horaria (ejemplo: México central -6 horas)
const long gmtOffset_sec = -6 * 3600; // UTC-6
const int daylightOffset_sec = 0;     // Sin horario de verano


void setup() {
    Serial.begin(115200);  // Inicia la comunicación serie a 115200 baudios

    // Conexión a la red WiFi
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi...");

    // Esperar hasta que el ESP32 se conecte al WiFi
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
  
    Serial.println("\nConectado a WiFi");

      // Configurar el NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Esperar un momento para que se sincronicen la fecha y hora
    delay(2000);

    // Comprobación opcional de la hora actual
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Error al obtener la hora");
    } else {
        Serial.print("Fecha/Hora actual: ");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
    }

    
}

void loop() {
    // Medición de la distancia utilizando el sensor ultrasónico
    float distancia = sensor.read();
    Serial.print("Distancia: ");
    Serial.println(distancia);  // Imprime la distancia medida en cm

    // Solo enviamos los datos si la distancia es mayor a 0 y menor a 50
    if (distancia > 0  && distancia <50) {
        // Verifica si el ESP32 sigue conectado a la red WiFi
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;  // Crea un objeto para realizar solicitudes HTTP

            http.begin(serverUrl);  // Especifica la URL del servidor
            http.addHeader("Content-Type", "application/json");  // Especifica el tipo de contenido como JSON

            // Construcción del JSON con los datos a enviar
            String payload = "{";
            payload += "\"sensorKey\": \"ESP32_001\",";  // Identificador del sensor
            payload += "\"distance\": " + String(distancia) + ",";  // Valor de la distancia medida
            payload += "\"timestamp\": \"" + getFormattedTime() + "\",";  // Fecha y hora actual
            payload += "\"latitude\": 19.4326,";  // Coordenada de latitud (ejemplo)
            payload += "\"longitude\": -99.1332"; // Coordenada de longitud (ejemplo)
            payload += "}";

            // Envío de la solicitud HTTP POST al servidor
            int httpResponseCode = http.POST(payload);

            Serial.println("Enviando datos...");
            Serial.println(payload);  // Muestra el JSON que se envía al servidor

            // Verifica si el envío fue exitoso
            if (httpResponseCode > 0) {
                Serial.println("Datos enviados con éxito: " + String(httpResponseCode));
            } else {
                Serial.println("Error en el envío: " + String(httpResponseCode));
            }

            http.end();  // Finaliza la conexión HTTP
        } else {
            Serial.println("Error: No conectado a WiFi");  // Mensaje de error si la conexión falla
        }
    } else {
        Serial.println("Distancia inválida, no se enviarán datos.");
    }

    delay(5000);  // Esperar 5 segundos antes de la siguiente medición
}


// Función para obtener la fecha y hora actual (simulada, sin RTC ni NTP)
String getFormattedTime() {
    time_t now = time(nullptr);         // Obtiene el tiempo actual en segundos desde 1970
    struct tm* t = localtime(&now);     // Convierte el tiempo a formato de fecha y hora
    char buffer[30];                    
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);  // Formatea la fecha y hora
    return String(buffer);              // Devuelve la fecha y hora como un string
}



