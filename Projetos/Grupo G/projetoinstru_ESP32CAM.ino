#include <WiFi.h>
#include "esp_camera.h"
#include <WebServer.h>
#include "base64.h"  

// Configurações da rede Wi-Fi
const char* ssid = "****";
const char* password = "*****";

WebServer server(80);

// Configurações da câmera
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM      5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

int yellow_level_azul = -1;
int yellow_level_verde = -1;
String encodedImageAzul;
String encodedImageVerde;

void setup_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // Inicializa a câmera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Erro ao inicializar a camera");
    return;
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

int calcular_nivel_amarelo(camera_fb_t * fb) {
  // Processa a imagem para calcular o nível de amarelo
  int yellow_level = 0;
  for (int i = 0; i < fb->len; i += 3) {
    uint8_t r = fb->buf[i];
    uint8_t g = fb->buf[i + 1];
    uint8_t b = fb->buf[i + 2];

    // Detecta pixels amarelos (R e G altos, B baixo)
    if (r > 150 && g > 150 && b < 100) {
      yellow_level++;
    }
  }
  return yellow_level;
}

void handle_root() {
  String htmlContent = "<html><head><style>"
                      "body { font-family: Arial, sans-serif; text-align: center; }"
                      "h2 { color: #333; }"
                      ".button { display: inline-block; margin: 10px; padding: 10px 20px; background-color: #007BFF; color: white; text-decoration: none; border-radius: 5px; }"
                      ".image-container { margin-top: 20px; }"
                      ".analysis { margin-top: 20px; font-size: 18px; }"
                      "img { max-width: 100%; height: auto; margin-top: 10px; }"
                      "</style></head><body>";
  htmlContent += "<h2>Controle de LEDs e Captura de Imagens:</h2>";
  htmlContent += "<a href='/led_azul_on' class='button'>Ativar LED Azul e Capturar Imagem</a>";
  htmlContent += "<a href='/led_verde_on' class='button'>Ativar LED Verde e Capturar Imagem</a>";

  if (!encodedImageAzul.isEmpty()) {
    htmlContent += "<div class='image-container'><h3>Imagem capturada com LED Azul:</h3>";
    htmlContent += "<img src='data:image/jpeg;base64," + encodedImageAzul + "' /></div>";
  }

  if (!encodedImageVerde.isEmpty()) {
    htmlContent += "<div class='image-container'><h3>Imagem capturada com LED Verde:</h3>";
    htmlContent += "<img src='data:image/jpeg;base64," + encodedImageVerde + "' /></div>";
  }

  htmlContent += "<div class='analysis' id='analysis'>";

  if (yellow_level_azul != -1 && yellow_level_verde != -1) {
    htmlContent += "<h3>Comparacao dos Niveis de Amarelo:</h3>";
    htmlContent += "<p>Nivel de amarelo com LED Azul: " + String(yellow_level_azul) + "</p>";
    htmlContent += "<p>Nivel de amarelo com LED Verde: " + String(yellow_level_verde) + "</p>";

    float ratio = (float)yellow_level_azul / (yellow_level_verde + 1);  // Evita divisão por zero
    if (ratio > 1.5) {
      htmlContent += "<p><strong>Maior absorcao com LED Azul, indicando possivel presenca de bilirrubina elevada.</strong></p>";
    } else if (ratio < 0.6) {
      htmlContent += "<p><strong>Maior reflexao com LED Verde, indicando que a pele esta refletindo mais luz verde do que azul.</strong></p>";
    } else {
      htmlContent += "<p>Niveis de amarelo similares entre as iluminacoes, sem indicacoes significativas.</p>";
    }
  }

  htmlContent += "</div>";
  htmlContent += "</body></html>";
  server.send(200, "text/html", htmlContent);
}

void handle_led(String led_color, int &yellow_level, String &encodedImage, String next_url) {
  // Captura a imagem
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Erro ao capturar a imagem com LED " + led_color);
    Serial.println("Erro ao capturar a imagem com LED " + led_color);
    return;
  }

  // Calcula o nível de amarelo
  yellow_level = calcular_nivel_amarelo(fb);

  // Codifica a imagem em base64
  encodedImage = base64::encode((const uint8_t *)fb->buf, fb->len);

  // Monta a mensagem do nível de amarelo
  String yellow_message = "Nivel de amarelo detectado: " + String(yellow_level);
  if (yellow_level > 120) {
    yellow_message += "<br><strong>Alto nivel de amarelo detectado! Possivel indicacao de altos niveis de bilirrubina na pele.</strong>";
  } else {
    yellow_message += "<br>Nivel de amarelo dentro do normal.";
  }     

  String htmlContent = "<div><h3>Imagem capturada com LED " + led_color + ":</h3>";
  htmlContent += "<img src='data:image/jpeg;base64," + encodedImage + "' /><br>";
  htmlContent += "<h3>Resultado da Analise:</h3>";
  htmlContent += "<p>" + yellow_message + "</p></div>";
  htmlContent += "<a href='" + next_url + "' class='button'>Avancar para proxima etapa</a>";

  server.send(200, "text/html", htmlContent);
  esp_camera_fb_return(fb);
  Serial.println("Imagem capturada com LED " + led_color + " e analise de amarelo enviada para o navegador");
}

void handle_led_azul() {
  handle_led("Azul", yellow_level_azul, encodedImageAzul, "/led_verde_on");
}

void handle_led_verde() {
  handle_led("Verde", yellow_level_verde, encodedImageVerde, "/");
}

void setup() {
  // Configurações iniciais e inicialização da câmera e do Wi-Fi
  Serial.begin(115200);
  setup_camera();
  setup_wifi();

  server.on("/", handle_root);
  server.on("/led_azul_on", handle_led_azul);
  server.on("/led_verde_on", handle_led_verde);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}

