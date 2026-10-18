#include "ZigbeeAC.h"

ZigbeeAC::ZigbeeAC(uint8_t endpoint) : ZigbeeEP(endpoint) {
    // Configuração do endpoint (nomes corretos dos campos)
    _ep_config.app_device_id      = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID;
    _ep_config.app_device_version = 1;
    _ep_config.app_profile_id     = ESP_ZB_AF_HA_PROFILE_ID;
    // _ep_config.endpoint já é atribuído pelo construtor da classe base

    _power = false;
    _customValue[0] = 0;
    _customValue[1] = 0;

    // --- On/Off cluster ---
    esp_zb_on_off_cluster_cfg_t on_off_cfg = {};
    on_off_cfg.on_off = false;
    esp_zb_attribute_list_t *on_off_cluster =
        esp_zb_on_off_cluster_create(&on_off_cfg);

    // --- Cluster customizado 0xfc00 ---
    esp_zb_attribute_list_t *custom_cluster =
        esp_zb_zcl_attr_list_create(ZIGBEE_CUSTOM_CLUSTER);
    esp_zb_custom_cluster_add_custom_attr(
        custom_cluster,
        ZIGBEE_CUSTOM_ATTR_ID,
        ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING,
        ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        _customValue
    );

    // --- Basic cluster (obrigatório) ---
    esp_zb_basic_cluster_cfg_t basic_cfg = {};
    basic_cfg.zcl_version  = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
    basic_cfg.power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
    esp_zb_attribute_list_t *basic_cluster =
        esp_zb_basic_cluster_create(&basic_cfg);

    // --- Identify cluster (obrigatório) ---
    esp_zb_identify_cluster_cfg_t identify_cfg = {};
    identify_cfg.identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
    esp_zb_attribute_list_t *identify_cluster =
        esp_zb_identify_cluster_create(&identify_cfg);

    // --- Adicionar clusters à lista (a lista já foi criada pelo ZigbeeEP) ---
    esp_zb_cluster_list_add_basic_cluster   (_cluster_list, basic_cluster,    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_identify_cluster(_cluster_list, identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_on_off_cluster  (_cluster_list, on_off_cluster,   ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_custom_cluster  (_cluster_list, custom_cluster,   ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // NÃO criar _ep_list aqui — a ZigbeeCore::addEndpoint() faz isso.
}

ZigbeeAC::~ZigbeeAC() {}

void ZigbeeAC::setPower(bool state) {
    _power = state;
    esp_zb_zcl_set_attribute_val(
        _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
        ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, &_power, false);
}

bool ZigbeeAC::getPower() { return _power; }

void ZigbeeAC::reportState(uint8_t typeId, uint8_t value) {
    _customValue[0] = typeId;
    _customValue[1] = value;

    esp_zb_zcl_report_attr_cmd_t cmd = {};
    cmd.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
    cmd.zcl_basic_cmd.dst_addr_u.addr_short = 0x0000;
    cmd.zcl_basic_cmd.dst_endpoint = 1;
    cmd.zcl_basic_cmd.src_endpoint = _endpoint;
    cmd.clusterID    = ZIGBEE_CUSTOM_CLUSTER;
    cmd.attributeID  = ZIGBEE_CUSTOM_ATTR_ID;
    cmd.direction    = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI;
    esp_zb_zcl_report_attr_cmd_req(&cmd);
}

// Assinatura correta com 4 parâmetros
void ZigbeeAC::zbAttributeRead(uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute,
                                uint8_t src_endpoint, esp_zb_zcl_addr_t src_address) {
    if (cluster_id == ZIGBEE_CUSTOM_CLUSTER &&
        attribute->id == ZIGBEE_CUSTOM_ATTR_ID) {
        esp_zb_zcl_report_attr_cmd_t cmd = {};
        cmd.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
        cmd.zcl_basic_cmd.dst_addr_u.addr_short = 0x0000;
        cmd.zcl_basic_cmd.dst_endpoint = 1;
        cmd.zcl_basic_cmd.src_endpoint = _endpoint;
        cmd.clusterID    = ZIGBEE_CUSTOM_CLUSTER;
        cmd.attributeID  = ZIGBEE_CUSTOM_ATTR_ID;
        cmd.direction    = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI;
        esp_zb_zcl_report_attr_cmd_req(&cmd);
    }
}