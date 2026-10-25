#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *nomeRedeWiFi = "SENAI EDUCACIONAL";
const char *senhaRedeWiFi = "info@134";
const char *enderecoApiCep = "https://brasilapi.com.br/api/cep/v1/";

String pedirParaApiCep(String codigo);
void mostrarEndereco(String json);

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

  Cep = pedirParaApiCep("09510200");

  if (!Cep.isEmpty())
  {
    mostrarEndereco(Cep);
  }
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
  else if (codigoHTTP == 404)
  {
    Serial.println("CEP não encontrado.");
  }
  else
  {
    Serial.printf("Deu erro. Código: %d\n", codigoHTTP);
  }

  http.end();
  return resposta;
}

void mostrarEndereco(String json)
{
  JsonDocument filtro;
  filtro["street"] = true;
  filtro["neighborhood"] = true;
  filtro["city"] = true;
  filtro["state"] = true;

  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, json, DeserializationOption::Filter(filtro));

  if (erro)
  {
    Serial.printf("Erro no JSON: %s\n", erro.c_str());
    return;
  }
  
  const char *rua = doc["street"] | "Não informado";
  const char *bairro = doc["neighborhood"] | "Não informado";
  const char *cidade = doc["city"] | "Não informado";
  const char *estado = doc["state"] | "Não informado";

  Serial.println("\n--- Endereço encontrado ---");
  Serial.printf("Rua: %s\n", rua);
  Serial.printf("Bairro: %s\n", bairro);
  Serial.printf("Cidade: %s\n", cidade);
  Serial.printf("Estado: %s\n", estado);
}