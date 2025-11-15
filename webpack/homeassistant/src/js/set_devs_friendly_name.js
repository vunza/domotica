
/**
 * Altera o Nome amigavel de entidades, atraves de "shell_command/atualiza_friendly.py"
 * @param {String} ip_porta IP e Porta do Servidor
 * @param {String} entityId ID do Dspositivo/Entidade
 * @param {String} friendlyName Novo nome amigavel para o Dispositivo
 * @param {String} entityState Estado do Dspositivo/Entidade
 * @param {String} entityType Tipo do Dspositivo/Entidade
 * @param {String} token Token de acesso ao HA
 * @returns {void}
 */
async function alterarFriendlyNome(ip_porta, entityId, friendlyName, entityState, entityType, token) {

    try {      
        
        // Executa o script para alteração do Nome
        const url = `${ip_porta}/api/services/shell_command/atualiza_friendly`;

        fetch(url, {
        method: "POST",
        headers: {
            "Authorization": `Bearer ${token}`,
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            entity_id: entityId,
            friendly_name: friendlyName, 
            entity_state: entityState,
            entity_type: entityType
        })
        })
        .then(response => {
        if (!response.ok) {
            throw new Error("Erro na requisição");
        }
        return response.json();
        })
        .then(data => {
        console.log("Comando enviado com sucesso:", data);
        })
        .catch(error => {
        console.error("Erro:", error);
        });
            
        } catch (error) {
            console.error('Erro:', error);
        }
}


export  {alterarFriendlyNome}