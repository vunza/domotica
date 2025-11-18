# Este script liga uma lâmpada se estiver desligada e vice-versa
lamp = "switch.0xa4c138164da7cfb7_left"
if hass.states.get(lamp).state == "off":
    hass.services.call("switch", "turn_on", {"entity_id": lamp})
elif hass.states.get(lamp).state == "on":
    hass.services.call("switch", "turn_off", {"entity_id": lamp})    

# lamp = "light.0x28dba7fffe1af100"
# if hass.states.get(lamp).state == "off":
#     hass.services.call("light", "turn_on", {"entity_id": lamp})
# elif hass.states.get(lamp).state == "on":
#     hass.services.call("light", "turn_off", {"entity_id": lamp})    