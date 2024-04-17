
// Kode dibuat oleh Alvaro Ariel Ilanunu.

// Pengimporan pustaka.
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>

// Definisi konstan dan variabel.
#define VERSI 1 // Menetapkan versi firmware saat ini.
#define ALAMAT_IP_SERVER "192.168.1.14:5000" // Menyimpan alamat IP dan port server.
#define JALUR_PEMBARUAN "/update" // Jalur pembaruan di server di mana berkas pembaruan dapat diunduh.

// Menyimpan SSID dan kata sandi untuk jaringan Wi-Fi yang akan dihubungkan.
String SSID_Wifi = "Griya Ilham";
String sandi_Wifi = "Siput_Ngebut";

// Deklarasi fungsi.
void koneksiWiFi(); // Fungsi untuk menghubungkan Wi-Fi.
void periksaServer(); // Memeriksa pembaruan firmware
t_httpUpdate_return pembaruanFirmware(String url_pembaruan); // Melakukan pembaruan firmware.

// Fungsi yang dijalankan saat mikrokontroler pertama kali dihidupkan.
void setup()
{
  Serial.begin(9600);
  koneksiWiFi();
}

// Fungsi yang dijalankan berulang-ulang setelah fungsi setup() selesai dieksekusi.
void loop()
{
  periksaServer();
  delay(1000);
}

// Fungsi yang memeriksa pembaruan firmware.
void periksaServer() {
  Serial.println("\n\n⤫~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~⤫");
  Serial.println("|");
  Serial.println("◉➞➞➞  Mengambil Data dari Server...\n");
  Serial.println("⤫--------------------------------------------------------");
  Serial.println("|");

  // Baris ini membangun URL dengan parameter.
  String url = "http://" + String(ALAMAT_IP_SERVER) + String(JALUR_PEMBARUAN) +
               "?mac_address=" + WiFi.macAddress();
  
  // HTTPClient adalah objek untuk membuat permintaan HTTP. Kemudian, permintaan GET dibuat ke URL yang telah dibangun sebelumnya.
  HTTPClient http;
  String respon_dari_server;
  http.begin(url);
  int httpCode = http.GET();

  // Jika permintaan HTTP berhasil (kode respons lebih besar dari 0), maka respon dari server akan disimpan dan ditampilkan di serial monitor.
  if(httpCode > 0) {
    respon_dari_server = http.getString();
    Serial.println("◉➞  Respons dari Server: ");
    Serial.println("\n" + respon_dari_server);
    Serial.println("⤫--------------------------------------------------------");
    Serial.println("|");

    //Data JSON dari respon server di-parse menggunakan ArduinoJson.h. Versi dan URL pembaruan firmware diekstrak dari objek JSON.
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, respon_dari_server);
    JsonObject obj = doc.as<JsonObject>();
    
    String versi = obj[String("versi")];
    String url_pembaruan = obj[String("url")];
    String versi_di_server = url_pembaruan.substring(url_pembaruan.lastIndexOf('v') + 1, url_pembaruan.lastIndexOf('.'));

    Serial.println("◉➞➞➞  Versi Firmware Tertinggi yang Tersedia: " + versi_di_server + "\n|");
    // Serial.println("URL: " + url_pembaruan);

    // Versi firmware dari server dibandingkan dengan versi firmware yang saat ini terpasang pada perangkat.
    if (versi_di_server.toInt() > VERSI) {
      Serial.println("◉➞➞➞  Pembaruan Dimulai --- ✓");

      //Jika pembaruan tersedia, fungsi updateFirmware() akan dipanggil untuk mengunduh dan memasang pembaruan. Jika pembaruan berhasil, perangkat akan me-restart.
      if(pembaruanFirmware(url_pembaruan) == HTTP_UPDATE_OK) {
        ESP.restart();
      } else {
        Serial.println("◉➞➞➞  Pembaruan Gagal! --- ✕\n");
      }
    } else {
      Serial.println("◉➞➞➞  Firmware Versi Terbaru Belum Tersedia --- ◯\n");
      Serial.println("⤫~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~⤫\n\n");
    }

    // Jika permintaan HTTP gagal, pesan "Permintaan HTTP Gagal!" akan ditampilkan di serial monitor.
  } else {
    Serial.println("|\n◉➞➞➞  Permintaan HTTP Gagal! --- ✕\n");
  }

  http.end();
}

// Fungsi pembaruanFirmware() digunakan untuk mengunduh dan memasang pembaruan firmware. 
// Setelah memeriksa status koneksi WiFi, pembaruan dilakukan menggunakan ESPhttpUpdate.update().
// Hasil pembaruan kemudian diperiksa, dan pesan yang sesuai ditampilkan di serial monitor.
t_httpUpdate_return pembaruanFirmware(String url_pembaruan)
{
  t_httpUpdate_return ret;

  if(WiFi.status()==WL_CONNECTED){
    
    ret= ESPhttpUpdate.update(url_pembaruan);

    switch(ret)
    {
      case HTTP_UPDATE_FAILED:
        Serial.println("[update]|\n◉➞➞➞  Pembaruan Tidak Dapat Dilakukan! --- ✕\n");
        return ret;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update]|\n◉➞➞➞  Tidak Ada Pembaruan --- ◯\n");
        return ret;
        break;

      case HTTP_UPDATE_OK:
        Serial.println("[update]|\n◉➞➞➞  Pembaruan Dilakukan! --- ✓\n");
        break;
    }
  }
}

// Fungsi ini menghubungkan perangkat ke jaringan WiFi yang telah ditentukan.
void koneksiWiFi()
{
  Serial.println("\n|\n◉➞➞➞  Menghubungkan ke Wi-Fi\n");
  WiFi.begin(SSID_Wifi.c_str(), sandi_Wifi.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n|\n◉➞➞➞  Wi-Fi Terhubung! --- ✓\n");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}