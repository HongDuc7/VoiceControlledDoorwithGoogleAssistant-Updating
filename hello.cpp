#include <WiFi.h>

// ----------------------
// 1. THIẾT LẬP WIFI
// ----------------------
// THAY THẾ bằng SSID và Mật khẩu WiFi của bạn
const char* ssid     = "#IDNS";
const char* password = "6284db356663";

// Web server chạy ở cổng 80 (cổng HTTP mặc định)
WiFiServer server(80);

// ----------------------
// 2. THIẾT LẬP GPIO
// ----------------------
// Chân GPIO 2 (thường là LED tích hợp trên ESP32 DevKit)
const int output2 = 2;

// Lưu HTTP request và trạng thái LED
String header;
String output2State = "off"; // Trạng thái ban đầu: Tắt


void setup() {
  Serial.begin(115200);

  // Khởi tạo GPIO và đặt trạng thái ban đầu: TẮT LED.
  // Vì LED tích hợp thường là Active-LOW, nên dùng HIGH để TẮT.
  pinMode(output2, OUTPUT);
  digitalWrite(output2, HIGH); 

  // Bắt đầu kết nối WiFi
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ----------------------
// 3. XỬ LÝ HTTP REQUEST
// ----------------------
void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    unsigned long previousTime = millis();

    while (client.connected() && (millis() - previousTime <= 5000)) {
      if (client.available()) {
        char c = client.read();
        // Serial.write(c); // Bỏ comment nếu muốn xem TOÀN BỘ request từ trình duyệt
        header += c;
        
        if (c != '\r') { // Chỉ thêm vào currentLine nếu không phải là ký tự \r
          currentLine += c;
        }

        if (c == '\n') {
          // ******************************************************
          // *** ĐÂY LÀ DÒNG SỬA LỖI QUAN TRỌNG NHẤT ***
          // Kiểm tra xem có phải là một dòng trống (chỉ chứa \n) hay không
          // (Vì ta đã bỏ qua \r, nên dòng trống "\r\n" sẽ chỉ còn là "\n")
          // ******************************************************
          if (currentLine.equals("\n")) {
            // Dòng trống báo hiệu kết thúc tiêu đề HTTP
            Serial.println("--- End of HTTP Header ---");

            // Xử lý điều khiển
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("GPIO 2 ON");
              output2State = "on";
              digitalWrite(output2, LOW);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 OFF");
              output2State = "off";
              digitalWrite(output2, HIGH);
            }

            // --- Gửi phản hồi HTTP và Giao diện Web ---
            Serial.println("Sending HTML response...");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println(); 

            // Giao diện web
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">"); // Thêm dòng này để tránh trình duyệt hỏi icon
            client.println("<style>");
            client.println("html{font-family:Helvetica;text-align:center;}");
            client.println(".button{border:none;color:white;padding:16px 40px;text-decoration:none;font-size:30px;margin:2px;cursor:pointer;}");
            client.println(".buttonOn{background-color:#195B6A;}"); 
            client.println(".buttonOff{background-color:#77878A;}"); 
            client.println("</style></head><body>");
            client.println("<h1>ESP32 Web Server</h1>");
            client.println("<p>GPIO 2 - LED State: <strong>" + output2State + "</strong></p>");
            
            if (output2State == "off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button buttonOn\">TURN ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button buttonOff\">TURN OFF</button></a></p>");
            }

            client.println("</body></html>");
            client.println();
            
            break; // Thoát vòng lặp nhận dữ liệu
          } else {
            // Đây là một dòng header, reset currentLine cho dòng tiếp theo
            currentLine = "";
          }
        }
      }
    }

    // Xóa header và đóng kết nối
    header = "";
    client.stop();
    Serial.println("Client disconnected.\n");
  }
}
