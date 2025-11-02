# HOMEASSISTANT
* O arquivo ***customize.yaml*** contem os nomes amigaveis (***friendlyname***) dos dispositivos;

* O arquivo ***automatios.yaml*** que contem as automações dinamicamente criadas;

* O arquivo ***scripts.yaml*** e ***configuration.yaml*** contêm lógicas para a criação de automações dinamicamente;

  * ## ***<span style="color:blue;">***scripts.yaml***:</span>***

```yaml
gerar_automacao_dinamica:
  alias: Script para gerar automação com horário e weekday
  sequence:
  - variables:
      horario: '{{ states(''input_datetime.horario_automacao'') }}'
      entidade: '{{ states(''input_text.identificador_do_dispositivo'') }}'
      accao: '{{ states(''input_text.accao_do_timer'') }}'
      dias: '[ {% if is_state(''input_boolean.domingo'', ''on'') %}"sun",{% endif
        %} {% if is_state(''input_boolean.segunda'', ''on'') %}"mon",{% endif %} {%
        if is_state(''input_boolean.terca'', ''on'') %}"tue",{% endif %} {% if is_state(''input_boolean.quarta'',
        ''on'') %}"wed",{% endif %} {% if is_state(''input_boolean.quinta'', ''on'')
        %}"thu",{% endif %} {% if is_state(''input_boolean.sexta'', ''on'') %}"fri",{%
        endif %} {% if is_state(''input_boolean.sabado'', ''on'') %}"sat",{% endif
        %}             ]

        '
  - service: rest_command.criar_automacao_dinamica
    data:
      horario: '{{ horario }}'
      entidade: '{{ entidade }}'
      accao: '{{ accao }}'
      dias: '{{ dias | tojson }}'
```

 * ## ***<span style="color:blue;">***configuration.yaml***:</span>***

 ```yaml
rest_command: # rest api
  criar_automacao_dinamica:
    url: "http://localhost:8123/api/config/automation/config/Timer-{{ entidade | regex_replace('^.*?\\.', '') }}"  
    method: POST
    headers:
      authorization: "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJjMDUwYTVkNTMzZDU0ZjBmODJlY2NjZmQyZDgwZDRlYyIsImlhdCI6MTczNjcxNzU1NSwiZXhwIjoyMDUyMDc3NTU1fQ.oLIiyFN8c4lPCAgIPL_sFEWI12P3IcsLsid4KL8A2D0"
      content-type: "application/json"
    payload: >-
      {
        "alias": "Timer-{{ entidade | regex_replace('^.*?\\.', '') }}",
        "trigger": [
          {
            "platform": "time",
            "at": "{{ horario }}"
          }
        ],
        "condition": [
           {
            "condition": "time",           
            "weekday": {{ dias }}
          }
        ],
        "action": [
          {
            "service": "{{ accao }}",
            "target": {
              "entity_id": "{{ entidade[:-2]}}"
            }
          }
        ],
        "mode": "single"
      }
 ```




# DOMOTICA

### **<span style="color:red;">NOTA:</span>** O código CSS ***inline*** tem proiridade, a seguir o CSS interno (descritos na ***tag style*** dentro da ***tag heade***) e por ultimo o ***CSS*** num arquivo ***".css"***;

A propriedade ***display: block*** faz com que o elemento ocupe a linha completa e quado combinada com a propridade ***margin: auto*** o elemento fica posicionado no centro, horizontalmente;

***height: 100vh*** altura correspondente a toda area visivel da página.

***width: 100vw*** largura correspondente a toda area visivel da página.

### **<span style="color:red;">NOTA:</span>** Há situções que o uso ***width: 100vw*** exibe a barra de rolagem (***scrol***), nestes casos convem usar ***width: 100%***;



***display: flex*** Permite  usar ***align-items: center*** e ***justify-content: center*** que permitem alinhar os itens na vertical e horizontal respectivamente.

# ***<span style="color:blue;">Exemplo:</span>***

```css
*{
    margin: 0;
    padding: 0; 
    box-sizing: border-box;
}

body{
    display: flex;
    align-items: center;
    justify-content: center;
}
```