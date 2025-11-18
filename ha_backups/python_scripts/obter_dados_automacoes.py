import yaml
import json
import datetime
import requests


ACCESS_TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0" 
HA_URL = "http://localhost:8123"

###############################
# AGRUPAR AUTOMACOES PELOS ID #
###############################

CAMINHO_AUTOMATIONS = "/config/automations.yaml"
obj_json = {}
atributos_dinamicos = {}

with open(CAMINHO_AUTOMATIONS, "r") as f:
    automacoes = yaml.safe_load(f)

list_json = []
list_entity_id = []

for a in automacoes:
    if not isinstance(a, dict):
        continue

    id_ = a.get("id")
    if not id_ or "Timer-" not in id_:
        continue

    dados = {
        "id": id_,
        "at": None,
        "weekday": None,
        "entity_id": None,
        "action": None
    }

    # Trigger com horário
    for trigger in a.get("triggers", []):
        if trigger.get("platform") == "time":
            dados["at"] = trigger.get("at")
            break

    # Condição com dia da semana
    for cond in a.get("conditions", []):
        if cond.get("condition") == "time" and "weekday" in cond:
            dados["weekday"] = cond["weekday"]
            break

    # Processa ações
    for acao in a.get("actions", []):
        if "action" in acao: 
            dados["action"] = acao["action"]
            if "target" in acao and "entity_id" in acao["target"]:
                dados["entity_id"] = acao["target"]["entity_id"] 
                if acao["target"]["entity_id"] not in list_entity_id: # Add ID na lista, se nao existe
                    list_entity_id.append(acao["target"]["entity_id"] )   


    list_json.append(dados)

# Exibir em formato JSON
# print(json.dumps(list_json, indent=2))
# print(list_entity_id)


############################################
# ENCONTRAR AUTOMACAO PPROXIMA DA EXECUCAO #
############################################

def obter_proxima_automacao(list_json, list_entity_id):
    atributos_dinamicos = {}
    
    for entidade in list_entity_id:
        entity_alvo = entidade
        hoje = datetime.datetime.now()

        def dia_str_para_num(dia_str):
            dias = ["mon", "tue", "wed", "thu", "fri", "sat", "sun"]
            return dias.index(dia_str)

        proxima_automacao = None
        menor_tempo = None
        dia_encontrado = None
        data_execucao_completa = None  # Nova variável para armazenar a data completa

        for auto in list_json:
            if auto["entity_id"] != entity_alvo:
                continue

            for dia_str in auto["weekday"]:
                dia_alvo = dia_str_para_num(dia_str)
                hora_alvo = datetime.datetime.strptime(auto["at"], "%H:%M:%S").time()

                # Calcula dias até a próxima execução
                dias_ate = (dia_alvo - hoje.weekday()) % 7
                if dias_ate == 0 and datetime.datetime.combine(hoje.date(), hora_alvo) <= hoje:
                    dias_ate = 7  # Se já passou hoje, agenda para próxima semana

                # Calcula a data completa de execução
                data_exec = hoje + datetime.timedelta(days=dias_ate)
                execucao = datetime.datetime.combine(data_exec.date(), hora_alvo)

                diferenca = (execucao - hoje).total_seconds()

                if diferenca >= 0:  # Considera apenas execuções futuras
                    if menor_tempo is None or diferenca < menor_tempo:
                        menor_tempo = diferenca
                        proxima_automacao = auto
                        dia_encontrado = dia_str
                        data_execucao_completa = execucao  # Armazena a data/hora completa

        if proxima_automacao:
            resultado = {
                "id": proxima_automacao["id"],
                "at": proxima_automacao["at"],
                "weekday": dia_encontrado,
                "entity_id": proxima_automacao["entity_id"],
                "action": proxima_automacao["action"],
                "execucao": data_execucao_completa.strftime("%d/%m/%Y, %H:%M:%S"),
                #"data_execucao": data_execucao_completa.date().isoformat(),  # Data no formato YYYY-MM-DD
                #"dias_restantes": (data_execucao_completa.date() - hoje.date()).days,
                #"horas_restantes": int(menor_tempo // 3600),
                #"minutos_restantes": int((menor_tempo % 3600) // 60)
            }
            
            obj_json = resultado.copy()
            atributos_dinamicos[proxima_automacao["entity_id"]] = obj_json
           
    return atributos_dinamicos


atributos_dinamicos = obter_proxima_automacao(list_json, list_entity_id)



#######################################################################
# ACTUALIZAR TIMERS DAS CARDs POR vIA DO input_text "info_dos_timers" #
#######################################################################

# Configurações do Home Assistant
ENTITY_ID = "input_text.info_dos_timers"

# Corpo da requisição para a API REST
payload = {
    "state": "",  
    "attributes": {
        "friendly_name": "Informação dos Timers",
        **atributos_dinamicos  # Desempacota os atributos dinâmicos
    }
}

# Headers da requisição
headers = {
    "Authorization": f"Bearer {ACCESS_TOKEN}",
    "Content-Type": "application/json"
}

# Envia a requisição POST para atualizar o input_text
response = requests.post(
    f"{HA_URL}/api/states/{ENTITY_ID}",
    headers=headers,
    data=json.dumps(payload)
)

# Verifica a resposta
if response.status_code == 200:
    print("Atributos atualizados com sucesso!")
    #print("Resposta:", response.json())
else:
    print("Erro:", response.status_code, response.text)






# async def send_ll_custom_event():
#     async with websockets.connect(HA_URL) as ws:
#         # Autenticação
#         await ws.recv()
#         await ws.send(json.dumps({
#             "type": "auth",
#             "access_token": ACCESS_TOKEN
#         }))
#         auth_response = await ws.recv()
#         print("Autenticação:", auth_response)

#         # Enviar evento
#         await ws.send(json.dumps({
#             "id": 1,
#             "type": "fire_event",
#             "event_type": "ll-custom",
#             "event_data": {
#                 "message": "timer_update",
#                 "data": {
#                     "at": "22:15:00",
#                     "weekday": ["mon", "thu"],
#                     "service": "light.toggle"
#                 }
#             }
#         }))

#         response = await ws.recv()
#         print("Resposta do evento:", response)

# asyncio.run(send_ll_custom_event())
   












# import yaml

# def listar_alias_automacoes(caminho_arquivo):
#     with open(caminho_arquivo, 'r', encoding='utf-8') as f:
#         dados = yaml.safe_load(f)

#     aliases = []

#     # Caso o arquivo tenha uma chave "automation:" no topo
#     if isinstance(dados, dict) and 'automation' in dados:
#         automacoes = dados['automation']
#         if isinstance(automacoes, list):
#             for automacao in automacoes:
#                 alias = automacao.get('alias')
#                 if alias:
#                     aliases.append(alias)
#     # Caso o arquivo seja diretamente uma lista de automações
#     elif isinstance(dados, list):
#         for automacao in dados:
#             alias = automacao.get('alias')
#             if alias:
#                 aliases.append(alias)
#     else:
#         print("Formato inesperado no arquivo YAML.")

#     return aliases

# # Caminho para o arquivo automations.yaml
# arquivo = '/config/automations.yaml'
# aliases = listar_alias_automacoes(arquivo)

# print("Aliases encontrados:")
# for alias in aliases:
#     print(f"- {alias}")