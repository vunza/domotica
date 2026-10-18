/*
 * ESP32-C6 — Zigbee Nó com texto bidirecional
 * Cluster AnalogInput (0x000C) — atributo Description (0x001C)
 * Recebe comandos JSON do HA/Z2M e controla o AC via BC7215A.
 */

/*
    ESP8266:
    ========
    GPIO5 --> BC7215A TX
    GPIO16 -> BC7215A RX
    GPIO14 -> BC7215A MOD
    GPIO4 --> BC7215A BUSY
    3.3V ---> BC7215A VCC

    ESP32 WROOM:
    ============
    GPIO25 -> BC7215A TX
    GPIO33 -> BC7215A RX
    GPIO27 -> BC7215A MOD
    GPIO26 -> BC7215A BUSY
    3.3V ---> BC7215A VCC

    SEED STUDIO ESP32 C6:
    =====================
    GPIO17 -> BC7215A TX
    GPIO16 -> BC7215A RX
    GPIO[1|2] -> BC7215A MOD
    GPIO21 -> BC7215A BUSY
    3.3V ---> BC7215A VCC
*/

#include <Arduino.h>
#include <Preferences.h>
#include <bc7215ac.h>
#include "ctrl_debug.h"
#include <eeprom_manager_pro.h>
#include <ArduinoJson.h>
#include "Zigbee.h"
#include "ep/ZigbeeDimmableLight.h"
#include "esp_zigbee_core.h"

#ifndef ZIGBEE_MODE_ED
#error "Por favor, selecione Zigbee end device em Tools->Zigbee mode"
#endif

#define ZIGBEE_ENDPOINT_ID 1
#define BUTTON_PIN         9
#define CUSTOM_CLUSTER_ID  0x000C          // Analog Input
#define CUSTOM_ATTR_ID     0x001C          // Description (string)

const uint8_t MOD_PIN  = 1;
const uint8_t BUSY_PIN = 21;
const uint8_t TX_PIN   = 16;
const uint8_t RX_PIN   = 17;

#if defined(ESP32)
HardwareSerial bc721Serial(1);
#endif

BC7215   bc7215Board(bc721Serial, MOD_PIN, BUSY_PIN);
BC7215AC ac(bc7215Board);

static uint8_t  g_text_attr[256] = {0};
static uint16_t g_cluster_rev    = 1;      // Cluster Revision obrigatório
Preferences prefs;

void verificarBotaoFactoryReset();
void enviarTextoParaHomeAssistant(const char* texto);
void emparelharAc();
void descodificarSinalAc();
void processarComandoJson(const String &jsonString);
void enviaDadosAc(int temp, int mode_index, int fan_index, int key);


class ZigbeeTextEP : public ZigbeeDimmableLight {
public:
    ZigbeeTextEP(uint8_t ep) : ZigbeeDimmableLight(ep) {}
    esp_zb_cluster_list_t* getClusterList() { return _cluster_list; }

    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override {
        if (message->info.cluster == CUSTOM_CLUSTER_ID &&
            message->attribute.id == CUSTOM_ATTR_ID) {
            const uint8_t *raw = (const uint8_t *)message->attribute.data.value;
            if (raw != nullptr) {
                uint8_t len = raw[0];
                if (len > 250) len = 250;
                char texto[252];
                memcpy(texto, raw + 1, len);
                texto[len] = '\0';
                g_text_attr[0] = len;
                memcpy(&g_text_attr[1], texto, len);
                imprimef(F("\n[ZIGBEE] Recebido do HA/Z2M: %s\n"), texto);

                char text[256];
                sprintf(text, "{\"temp\": %d, \"mode\": %d, \"fan\": %d, \"power\": %d}",
                        texto, 0, 0, 0);
                enviarTextoParaHomeAssistant(texto);
                                
                processarComandoJson(String(texto));
            }
        }
    }
};

ZigbeeTextEP zbTextEndpoint = ZigbeeTextEP(ZIGBEE_ENDPOINT_ID);

void setup() {
    Serial.begin(115200);
    delay(1000);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

#if defined(ESP32)
    bc721Serial.begin(19200, SERIAL_8N2, RX_PIN, TX_PIN);
#endif

    delay(100);
    bc7215Board.setRx();
    delay(50);

    lerDadosEEPROM(esp_cfg_data);
    ac.startCapture();

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    prefs.begin("ac_prefs", false);

    // Valor inicial no formato Zigbee [len][bytes...]
    const char* initMsg = "Inicio Node";
    uint8_t initLen = strlen(initMsg);
    g_text_attr[0] = initLen;
    memcpy(&g_text_attr[1], initMsg, initLen);

    zbTextEndpoint.setManufacturerAndModel("CustomESP32", "C6_Text_Node");
    Zigbee.addEndpoint(&zbTextEndpoint);

    // ============================================================
    // CRIAÇÃO MANUAL DO CLUSTER ANALOG INPUT
    // (evita duplicar o atributo description interno do SDK)
    // ============================================================
    esp_zb_attribute_list_t *analog_input_cluster =
        esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT);

    // 1. Cluster Revision (obrigatório por especificação ZCL) — 0xFFFD
    esp_zb_custom_cluster_add_custom_attr(
        analog_input_cluster,
        0xFFFD,                                       // Cluster Revision
        ESP_ZB_ZCL_ATTR_TYPE_U16,
        ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
        &g_cluster_rev
    );

    // 2. Atributo Description (CHAR_STRING) — único ponteiro para g_text_attr
    esp_zb_custom_cluster_add_custom_attr(
        analog_input_cluster,
        ESP_ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID,  // 0x001C
        ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING,
        ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        (void *)g_text_attr
    );

    esp_zb_cluster_list_add_analog_input_cluster(
        zbTextEndpoint.getClusterList(),
        analog_input_cluster,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE
    );

    if (!Zigbee.begin(ZIGBEE_END_DEVICE)) {
        ESP.restart();
    }

    imprimeln(F("Aguardando conexão Zigbee..."));
    unsigned long t = millis();
    while (!Zigbee.connected() && (millis() - t < 20000)) delay(500);

    imprimeln(F("Firmware pronto."));
}

void enviarTextoParaHomeAssistant(const char* texto) {
    uint8_t len = strlen(texto);
    if (len > 250) len = 250;

    esp_zb_lock_acquire(portMAX_DELAY);

    g_text_attr[0] = len;
    memcpy(&g_text_attr[1], texto, len);

    esp_zb_zcl_set_attribute_val(
        ZIGBEE_ENDPOINT_ID,
        CUSTOM_CLUSTER_ID,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
        CUSTOM_ATTR_ID,
        (void*)g_text_attr,
        false
    );

    esp_zb_zcl_report_attr_cmd_t report_cmd = {};
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = 0x0000;
    report_cmd.zcl_basic_cmd.dst_endpoint = 1;
    report_cmd.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI;
    report_cmd.dis_default_resp = 0x00;
    report_cmd.manuf_specific = 0x00;
    report_cmd.clusterID = CUSTOM_CLUSTER_ID;
    report_cmd.attributeID = CUSTOM_ATTR_ID;
    report_cmd.zcl_basic_cmd.src_endpoint = ZIGBEE_ENDPOINT_ID;

    esp_err_t err = esp_zb_zcl_report_attr_cmd_req(&report_cmd);
    esp_zb_lock_release();

    if (err == ESP_OK) {
        imprimef(F("[ZIGBEE] Report enviado: %s\n"), texto);
        // TODO: Ver se é necessário enviar comando IR correspondente para o BC7215A
    } else {
        imprimef(F("[ZIGBEE] Erro no report: %s"), esp_err_to_name(err));
    }
}

void loop() {
    verificarBotaoFactoryReset();

    if (!paired && !esp_cfg_data.configurado) {
        emparelharAc();
        return;
    }
    else if (!paired && esp_cfg_data.configurado) {
        if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)){
            paired = true;
            return;
        }
        else {
            esp_cfg_data.configurado = false;
            emparelharAc();
        }
    }
    else if (paired && esp_cfg_data.configurado) {
        descodificarSinalAc();
    }

    delay(10);
}

void verificarBotaoFactoryReset() {
    static unsigned long pressTime = 0;
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (pressTime == 0) {
            pressTime = millis();
        } else if (millis() - pressTime > 5000) {
            imprimeln(F("\n[RESET] Botão BOOT pressionado. A limpar configurações..."));
            esp_cfg_data.configurado = false;
            salvarDadosEEPROM(esp_cfg_data);
            prefs.clear();
            Zigbee.factoryReset();
            delay(1000);
            ESP.restart();
        }
    } else {
        pressTime = 0;
    }
}

void emparelharAc(){
    if (ac.signalCaptured()) {
        ac.stopCapture();
        if (ac.init()){
            paired = true;
            esp_cfg_data.configurado = true;
            esp_cfg_data.formatPkt = *ac.getFormatPkt();
            esp_cfg_data.dataPkt = *((bc7215DataMaxPkt_t *)ac.getDataPkt());
            salvarDadosEEPROM(esp_cfg_data);
            ac.startCapture();
        }
        else{
            ac.startCapture();
        }
    }
}

void descodificarSinalAc(){
    if (ac.signalCaptured()) {
        ac.stopCapture();
        int temp = -1, mode = -1, fan = -1, power = -1;
        if (ac.parse(temp, mode, fan, power)) {
            char texto[256];
            sprintf(texto, "{\"temp\": %d, \"mode\": %d, \"fan\": %d, \"power\": %d}",
                    temp, mode, fan, power);
            enviarTextoParaHomeAssistant(texto);
        }
        ac.startCapture();
    }
}



void enviaDadosAc(int temp, int mode_index, int fan_index, int key){
  unsigned long startTime = millis();
  
  ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt);
  ac.startCapture();
  delay(50); 

  bc7215Board.setTx();
  delay(50);
  ac.setTo(temp, mode_index, fan_index, -1);
  //imprimef("[SOFTWARE] >> POWER: %s TEP: %d MODE: %s FAN: %s FOR_TX: %d\n", PWR_STATUS[1], temp, mode_index, fan_index, 1);
  
  while (ac.isBusy() && (millis() - startTime < 3000)){
    delay(10);
  }
  bc7215Board.setRx();

} 


void processarComandoJson(const String &jsonString) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        imprimef("[JSON] Erro ao processar o JSON: %s\n", error.c_str());
        return;
    }

    imprimeln(F("[JSON] Comando válido recebido!"));

    int8_t power = doc["power"];
    int8_t temp = doc["temp"];
    int8_t mode = doc["mode"];
    int8_t fan = doc["fan"];

    imprimef("[JSON] Comando recebido: %s\n", jsonString.c_str());
    // [JSON] Comando recebido: {"temp":27,"mode":1,"fan":2,"power":0}
    // TODO: Enviar comando IR correspondente para o BC7215A
    
    //enviaDadosAc(temp, mode, fan, 1);

    if (power != -1) {
        imprimef(" -> Power: %d\n", power);        
    }
    if (temp != -1) {
        imprimef(" -> Temperatura: %d\n", temp);
    }
    if (mode != -1) {
        imprimef(" -> Modo: %d\n", mode);
    }
    if (fan != -1) {
        imprimef(" -> Ventilador: %d\n", fan);
    }
}