#!/usr/bin/env python3
import sys
import json

OBJ_HUBS_IR =  sys.argv[1]
#json_text = f'{{"codigo_ir": "{CODIGO_IR}"}}'
#json_text = f'"[{OBJ_HUBS_IR}]"'
json_text = f'{OBJ_HUBS_IR}'
try:
    obj = json.loads(json_text)
    with open("/config/www/json_files/codigos_ir.json", "w") as f:
        json.dump(obj, f, indent=0)
except Exception as e:
    with open("/config/www/json_files/erro.log", "w") as f:
        f.write(str(e))
