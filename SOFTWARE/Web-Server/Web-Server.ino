#include <WiFi.h>

const char* ssid = "FRITZ!Box 5690 VS";
const char* password = "95798363731421596201";

NetworkServer server(80);


// Aktueller Zustand des Fahrzeugs
String fahrtrichtung = "STOP";
String lenkrichtung = "GERADEAUS";

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 startet...");

  // WLAN verbinden
  WiFi.begin(ssid, password);

  Serial.print("Verbinde mit WLAN");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WLAN verbunden!");

  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // Webserver starten
  server.begin();

  Serial.println("Webserver gestartet.");
  Serial.print("HMI erreichbar unter: http://");
  Serial.println(WiFi.localIP());
}


void loop() {

  NetworkClient client = server.accept();

  if (client) {

    Serial.println();
    Serial.println("Neuer Client verbunden.");

    String currentLine = "";

    while (client.connected()) {

      if (client.available()) {

        char c = client.read();

        if (c == '\n') {

          // Ende der HTTP-Anfrage
          if (currentLine.length() == 0) {

            // HTTP-Antwort
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();

            // HTML-Seite
            client.println("<!DOCTYPE html>");
            client.println("<html>");

            // Kopf
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>RC Car HMI</title>");

            // CSS
            client.println("<style>");

            client.println("body {");
            client.println("font-family: Arial;");
            client.println("text-align: center;");
            client.println("margin: 20px;");
            client.println("}");

            client.println("h1 {");
            client.println("font-size: 40px;");
            client.println("}");

            client.println("button {");
            client.println("width: 200px;");
            client.println("height: 80px;");
            client.println("font-size: 25px;");
            client.println("margin: 10px;");
            client.println("}");

            client.println(".stop {");
            client.println("width: 150px;");
            client.println("height: 60px;");
            client.println("}");

            client.println(".status {");
            client.println("font-size: 25px;");
            client.println("margin: 20px;");
            client.println("}");

            client.println("</style>");
            client.println("</head>");

            // Körper
            client.println("<body>");

            client.println("<h1>RC Car Steuerung</h1>");

            // Statusanzeige
            client.println("<div class=\"status\">");

            client.print("Fahrtrichtung: <b>");
            client.print(fahrtrichtung);
            client.println("</b><br>");

            client.print("Lenkrichtung: <b>");
            client.print(lenkrichtung);
            client.println("</b>");

            client.println("</div>");

            // Vorwärts
            client.println("<form action=\"/forward\">");
            client.println("<button type=\"submit\">VORWAERTS</button>");
            client.println("</form>");

            // Links / Stop / Rechts
            client.println("<form action=\"/left\" style=\"display:inline;\">");
            client.println("<button type=\"submit\">LINKS</button>");
            client.println("</form>");

            client.println("<form action=\"/stop\" style=\"display:inline;\">");
            client.println("<button class=\"stop\" type=\"submit\">STOP</button>");
            client.println("</form>");

            client.println("<form action=\"/right\" style=\"display:inline;\">");
            client.println("<button type=\"submit\">RECHTS</button>");
            client.println("</form>");

            // Rückwärts
            client.println("<form action=\"/backward\">");
            client.println("<button type=\"submit\">RUECKWAERTS</button>");
            client.println("</form>");

            client.println("</body>");
            client.println("</html>");

            client.println();

            break;
          }

          currentLine = "";

        } else if (c != '\r') {

          currentLine += c;
        }


        // ==========================
        // FAHRBEFEHLE
        // ==========================

        if (currentLine.endsWith("GET /forward")) {

          fahrtrichtung = "VORWÄRTS";

          Serial.println("Befehl: VORWÄRTS");
        }


        if (currentLine.endsWith("GET /backward")) {

          fahrtrichtung = "RÜCKWÄRTS";

          Serial.println("Befehl: RÜCKWÄRTS");
        }


        if (currentLine.endsWith("GET /stop")) {

          fahrtrichtung = "STOP";

          Serial.println("Befehl: STOP");
        }


        // ==========================
        // LENKBEFEHLE
        // ==========================

        if (currentLine.endsWith("GET /left")) {

          lenkrichtung = "LINKS";

          Serial.println("Befehl: LENKEN LINKS");
        }


        if (currentLine.endsWith("GET /right")) {

          lenkrichtung = "RECHTS";

          Serial.println("Befehl: LENKEN RECHTS");
        }
      }
    }

    client.stop();

    Serial.println("Client getrennt.");
  }
}