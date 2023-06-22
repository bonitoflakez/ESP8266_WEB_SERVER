#include <ESP8266WiFi.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WebServer.h>

const char* apSsid = "=￣ω￣=";
const char* apPassword = "12345678";

ESP8266WebServer server(80);

void handleScan() {
  int numNetworks = WiFi.scanNetworks();

  String html = "<html><head><style>";
  html += "table { border-collapse: collapse; width: 100%; }";
  html += "th, td { text-align: left; padding: 8px; }";
  html += "th { background-color: #f2f2f2; }";
  html += "tr:nth-child(even) { background-color: #f2f2f2; }";
  html += "</style></head><body><h1>Wi-Fi Networks</h1><table>";
  html += "<tr><th>Sr. No.</th><th>Network</th><th>Signal Strength</th><th>Encryption Type</th><th>MAC Address</th></tr>";

  for (int i = 0; i < numNetworks; i++) {
    html += "<tr>";
    html += "<td>" + String(i + 1) + "</td>";
    html += "<td>" + WiFi.SSID(i) + "</td>";
    html += "<td>" + String(WiFi.RSSI(i)) + "</td>";
    html += "<td>" + String(WiFi.encryptionType(i)) + "</td>";
    html += "<td>" + WiFi.BSSIDstr(i) + "</td>";
    html += "</tr>";
  }

  html += "</table></body></html>";

  server.send(200, "text/html", html);
}

void handleRoot() {
  String html = "<h1>Welcome to ESP8266 Web Server!</h1>";
  html += "<ul><li><a href='/scan'>Scan for networks</a></li>";
  html += "<li><a href='/connected'>Check connected devices</a></li></ul>";

  server.send(200, "text/html", html);
}

void handleConnectedDevices() {
  String html = "<html><head><style>";
  html += "table { border-collapse: collapse; width: 100%; }";
  html += "th, td { text-align: left; padding: 8px; }";
  html += "th { background-color: #f2f2f2; }";
  html += "tr:nth-child(even) { background-color: #f2f2f2; }";
  html += "</style></head><body><h1>Connected Devices</h1><table>";
  html += "<tr><th>Sr. No.</th><th>MAC Address</th><th>IP Address</th></tr>";

  struct station_info* stationList = wifi_softap_get_station_info();
  int stationCount = 0;
  while (stationList != NULL) {
    html += "<tr>";
    html += "<td>" + String(++stationCount) + "</td>";
    html += "<td>";
    for (int i = 0; i < 6; ++i) {
      if (i > 0) {
        html += ":";
      }
      html += String(stationList->bssid[i], HEX);
    }
    html += "</td>";
    html += "<td>" + IPAddress(stationList->ip.addr).toString() + "</td>";
    html += "</tr>";

    stationList = STAILQ_NEXT(stationList, next);
  }
  wifi_softap_free_station_info();

  html += "</table></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Create Access Point
  WiFi.softAP(apSsid, apPassword);

  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/connected", handleConnectedDevices);

  server.begin();

  Serial.println();
  Serial.println("Access Point created:");
  Serial.print("SSID: ");
  Serial.println(apSsid);
  Serial.print("Password: ");
  Serial.println(apPassword);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
