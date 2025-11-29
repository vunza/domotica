#include "async_elegant_ota.h"

void ElengantOTA::begin(AsyncWebServer* server) {
    AsyncElegantOTA.begin(server);
    imprimeln(F("OTA Inicializada!"));
}
