# Consulta de CEP com ESP32 e BrasilAPI

Projeto desenvolvido em C/C++ com o framework Arduino, utilizando PlatformIO, para a atividade **Aula 63 – Consumo de API Livre**. O programa conecta o ESP32 a uma rede Wi-Fi, consome uma API pública de CEP por meio de uma requisição HTTP, trata a resposta em formato JSON e exibe o endereço correspondente no Monitor Serial.

## Sumário

1. [Objetivo](#objetivo)
2. [API utilizada](#api-utilizada)
3. [Requisitos](#requisitos)
4. [Estrutura do projeto](#estrutura-do-projeto)
5. [Configuração](#configuração)
6. [Execução](#execução)
7. [Funcionamento do programa](#funcionamento-do-programa)
8. [Descrição das funções](#descrição-das-funções)
9. [Tratamento de erros](#tratamento-de-erros)
10. [Saída esperada](#saída-esperada)
11. [Limitações](#limitações)
12. [Autor](#autor)

## Objetivo

Desenvolver um programa embarcado capaz de:

- selecionar uma API gratuita e identificar as informações que ela disponibiliza;
- realizar uma requisição HTTP para obter os dados;
- receber a resposta em formato JSON e realizar a desserialização, com filtro e tratamento dos dados;
- apresentar as informações obtidas no Monitor Serial.

## API utilizada

**BrasilAPI – Consulta de CEP (v1)**

| Item | Descrição |
|---|---|
| Endpoint | `https://brasilapi.com.br/api/cep/v1/{cep}` |
| Método | `GET` |
| Autenticação | Não requerida |
| Formato de resposta | JSON |
| Parâmetro | `cep`: oito dígitos, sem hífen |

Campos de interesse da resposta:

| Campo no JSON | Informação exibida |
|---|---|
| `street` | Rua |
| `neighborhood` | Bairro |
| `city` | Cidade |
| `state` | Estado (UF) |

Os demais campos retornados pela API são descartados pelo programa.

## Requisitos

### Hardware

- Placa de desenvolvimento ESP32 (configuração `esp32dev`)
- Cabo USB para gravação e comunicação serial
- Rede Wi-Fi com acesso à internet, na frequência de 2,4 GHz

### Software

- [Visual Studio Code](https://code.visualstudio.com/)
- Extensão [PlatformIO IDE](https://platformio.org/)
- Biblioteca [ArduinoJson](https://arduinojson.org/) versão 7 (instalada automaticamente pelo PlatformIO)

As bibliotecas `WiFi` e `HTTPClient` já acompanham o framework Arduino para ESP32.

## Estrutura do projeto

```
.
├── platformio.ini
├── README.md
└── src
    └── main.cpp
```

## Configuração

### Arquivo `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    bblanchon/ArduinoJson@^7
```

O valor de `monitor_speed` deve ser igual ao informado em `Serial.begin()` no código, para que o Monitor Serial exiba os caracteres corretamente.

### Credenciais de rede

No início do arquivo `src/main.cpp`, informe o nome e a senha da rede Wi-Fi:

```cpp
const char *nomeRedeWiFi = "NOME_DA_REDE";
const char *senhaRedeWiFi = "SENHA_DA_REDE";
```

> **Atenção:** não publique credenciais reais em repositórios públicos. Substitua-as por valores genéricos antes de enviar o código ao GitHub.

### CEP consultado

O CEP é definido diretamente no código, na chamada de `pedirParaApiCep()` dentro de `setup()`:

```cpp
Cep = pedirParaApiCep("09510200");
```

Para consultar outro endereço, basta alterar o valor informado, mantendo oito dígitos e sem hífen.

## Execução

1. Abra o projeto no Visual Studio Code com a extensão PlatformIO instalada.
2. Conecte o ESP32 ao computador por USB.
3. Configure as credenciais da rede Wi-Fi em `src/main.cpp`.
4. Compile e grave o programa na placa (`PlatformIO: Upload`).
5. Abra o Monitor Serial (`PlatformIO: Serial Monitor`) com velocidade de 115200 baud.
6. Acompanhe a conexão e o resultado da consulta. Para repetir a consulta, pressione o botão **EN** (reset) da placa.

## Funcionamento do programa

1. O ESP32 inicia a comunicação serial e a conexão com a rede Wi-Fi, exibindo o símbolo `#` enquanto aguarda.
2. Estabelecida a conexão, o programa envia uma requisição `GET` à BrasilAPI com o CEP definido.
3. Se a resposta for bem-sucedida (código HTTP 200), o JSON recebido é armazenado em uma `String`.
4. O JSON é desserializado com um filtro que preserva apenas os campos `street`, `neighborhood`, `city` e `state`.
5. As informações são impressas no Monitor Serial.

A consulta é executada uma única vez, na função `setup()`. A função `loop()` permanece vazia.

## Descrição das funções

### `void setup()`

Inicializa a comunicação serial, conecta o ESP32 à rede Wi-Fi, solicita os dados do CEP e, caso a resposta não esteja vazia, chama `mostrarEndereco()`.

### `void loop()`

Não executa nenhuma operação, pois a consulta é realizada apenas uma vez.

### `String pedirParaApiCep(String codigo)`

Realiza a requisição HTTP.

- **Parâmetro:** `codigo`, o CEP com oito dígitos.
- **Retorno:** o corpo da resposta em formato JSON, ou uma `String` vazia em caso de falha.
- **Comportamento:** monta a URL concatenando o endereço da API ao CEP, executa o `GET` e verifica o código HTTP retornado. Libera os recursos da conexão com `http.end()` antes de retornar.

### `void mostrarEndereco(String json)`

Desserializa o JSON e exibe o endereço.

- **Parâmetro:** `json`, o texto retornado pela API.
- **Filtro:** um documento auxiliar (`JsonDocument`) define os campos que devem ser mantidos, reduzindo o consumo de memória do microcontrolador.
- **Valores padrão:** o operador `|` do ArduinoJson atribui o texto `Não informado` quando um campo está ausente ou nulo, situação comum em CEPs de cidades pequenas.
- **Saída:** imprime rua, bairro, cidade e estado no Monitor Serial.

## Tratamento de erros

| Situação | Comportamento |
|---|---|
| Falha na conexão Wi-Fi | O programa permanece em espera, exibindo `#` até conectar |
| CEP inexistente (HTTP 404) | Exibe `CEP não encontrado.` |
| Outros erros HTTP ou de conexão | Exibe `Deu erro. Código: <valor>` |
| Resposta vazia | A função `mostrarEndereco()` não é chamada |
| JSON inválido ou incompleto | Exibe `Erro no JSON: <descrição>` |
| Campo ausente ou nulo no JSON | Exibe `Não informado` no campo correspondente |

## Saída esperada

Formato da saída no Monitor Serial para uma consulta bem-sucedida:

```
Iniciando conexão WiFi...
####
Conectado com sucesso!

--- Endereço encontrado ---
Rua: <logradouro>
Bairro: <bairro>
Cidade: <cidade>
Estado: <UF>
```

## Limitações

- O CEP é fixo no código; alterá-lo exige nova compilação e gravação.
- A consulta é realizada apenas uma vez a cada inicialização da placa.
- O ESP32 não se conecta a redes Wi-Fi de 5 GHz.
- O funcionamento depende da disponibilidade da BrasilAPI e de acesso à internet na rede utilizada.

## Autor

**Gabriel Pedrão Catapane**
Curso Técnico em Desenvolvimento de Sistemas – SENAI São Caetano do Sul
