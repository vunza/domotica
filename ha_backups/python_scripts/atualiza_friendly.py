import sys
import yaml
import requests

YOUR_LONG = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0"
ENTITY_ID =  sys.argv[1]
FRIENDLY_NAME = sys.argv[2]
ENTITY_STATE = sys.argv[3]
CONFIG_FILE = "/config/customize.yaml"  # ajuste conforme seu path

with open(CONFIG_FILE, "r") as f:
    data = yaml.safe_load(f) or {}

if ENTITY_ID not in data:
    data[ENTITY_ID] = {}

data[ENTITY_ID]["friendly_name"] = FRIENDLY_NAME

with open(CONFIG_FILE, "w") as f:
    yaml.dump(data, f)

#print(f"Atualizado: {ENTITY_ID} -> {FRIENDLY_NAME}")    

# Recarrega o customize via API REST
headers = {
    "Authorization": "Bearer " + YOUR_LONG,
    "Content-Type": "application/json"
}

# Recarrega o customize via API REST
requests.post("http://localhost:8123/api/services/homeassistant/reload_core_config", headers=headers)


# Atualiza o estado da entidade (incluindo friendly_name)
requests.post(
    f"http://localhost:8123/api/states/{ENTITY_ID}",
    headers=headers,
    json={
        "state": ENTITY_STATE,  # Mantém o estado atual ou ajuste conforme necessário
        "attributes": {"friendly_name": FRIENDLY_NAME}
    }
)

