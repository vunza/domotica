#pragma once
#include "ZigbeeEP.h"

#define ZIGBEE_AC_ENDPOINT     1
#define ZIGBEE_CUSTOM_CLUSTER  0xfc00
#define ZIGBEE_CUSTOM_ATTR_ID  0x0000

class ZigbeeAC : public ZigbeeEP {
public:
    ZigbeeAC(uint8_t endpoint);
    ~ZigbeeAC();

    void setPower(bool state);
    bool getPower();
    void reportState(uint8_t typeId, uint8_t value);

    // Assinatura correta com 4 parâmetros
    void zbAttributeRead(uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute,
                         uint8_t src_endpoint, esp_zb_zcl_addr_t src_address) override;

private:
    bool    _power;
    uint8_t _customValue[2];
};