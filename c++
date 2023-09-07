#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

const char* ssid = "SOMOS.NXT_2G";
const char* password = "3216549870";
const char* serverAddress = "http://app.somosnxt.com.br/stream_data.php";
IPAddress serverIP(192, 168, 1, 100);
const char* dbUser = "usuario";
const char* dbPassword = "senha";
const char* dbName = "nome_do_banco";

void setup() {
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando à rede Wi-Fi...");
    }
    Serial.println("Conectado à rede Wi-Fi");

    enviarDadosParaMySQL();
}

void loop() {
}

int gerarTemperaturaAleatoria() {
    return random(10, 40); 
}

int gerarUmidadeAleatoria() {
    return random(20, 80);
}

void enviarDadosParaMySQL() {
    int temperatura = gerarTemperaturaAleatoria();
    int umidade = gerarUmidadeAleatoria();

    WiFiClient client;

    if (client.connect(serverAddress, 80)) {
        String data = "temperatura=" + String(temperatura) + "&umidade=" + String(umidade);
        client.println("POST /stream_data.php HTTP/1.1");
        client.println("Host: " + String(serverAddress));
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data.length());
        client.println();
        client.println(data);

        delay(10);
        while (client.available()) {
            String linha = client.readStringUntil('\r');
            Serial.print(linha);
        }

        MySQL_Connection conn((Client *)&client);
        if (conn.connect(serverIP, 3306, dbUser, dbPassword)) {
            delay(1000);
            Serial.println("Conectado ao servidor MySQL");

            MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
            char INSERT_SQL[128];
            sprintf(INSERT_SQL, "INSERT INTO dados (temperatura, umidade) VALUES (%d, %d)", temperatura, umidade);
            cur_mem->execute(INSERT_SQL);
            delete cur_mem;

            Serial.println("Dados inseridos no MySQL");
            conn.close();
        } else {
            Serial.println("Falha ao conectar-se ao servidor MySQL");
        }

        client.stop();
    } else {
        Serial.println("Falha ao conectar-se ao servidor");
    }

    delay(5000);
}
