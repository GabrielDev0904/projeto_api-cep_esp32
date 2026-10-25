#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *nomeRedeWiFi = "SENAI EDUCACIONAL";
const char *senhaRedeWiFi = "info@134";
const char *enderecoApiCep = "https://brasilapi.com.br/api/cep/v1/";

String pedirParaApiCep(String codigo);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(nomeRedeWiFi, senhaRedeWiFi);

  Serial.print("\n\rIniciando conexão WiFi...\n\r");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("#");
    delay(500);
  }

  Serial.print("\n\rConectado com sucesso!\n\r");

  String Cep;

  Cep = pedirParaApiCep("09560500");
}

void loop()
{
}

String pedirParaApiCep(String codigo)
{
  HTTPClient http;
  http.begin(enderecoApiCep + codigo);

  int codigoHTTP = http.GET();
  String resposta;
  
  if (codigoHTTP == 200)
  {
    resposta = http.getString();
  }
  else
  {
    Serial.printf("Deu erro. Código: %d", codigoHTTP);
  }

  http.end();
  return resposta;
}