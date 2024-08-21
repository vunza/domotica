import network, socket, time

tamnho_chunk = 1024*16

#############################
# Conectar/Criar rede Wi-Fi #
#############################
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.scan()
sta_if.connect("TPLINK", "gregorio@2012")
#sta_if.connect("Welwitschia Mirabilis", "tigre?2018@")

####################
# Espera a conexao #
####################
while sta_if.isconnected() == False:
    print('Conectando...')
    time.sleep(1)
    pass

#print('Coxeão a rede WiFi %s estabelecida' % ssid)
print('IP: ' + sta_if.ifconfig()[0])  # Mostra dados da conexão



############################
# Função para ler arquivos #
############################
def serve_file(file_path):
    try:
        with open(file_path, "rb") as f:
            chunk_size = tamnho_chunk
            while True:
                chunk = f.read(chunk_size)
                if not chunk:
                    break
                yield chunk
    except OSError:
        yield "404 Não encontrado.".encode('utf-8')


# Cria o socket do servidor
addr = socket.getaddrinfo("0.0.0.0", 80)[0][-1]
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(addr)
s.listen(5)


###############################################
# Tratamento de pedidos dos clientes/Browsers #
###############################################
while True:
    cl, addr = s.accept()
    #print("Cliente conectado from", addr)

    request = cl.recv(tamnho_chunk).decode()
    #print("Request:", request)

    if "GET / " in request:
        file_path = "/data/index.html"
        content_type = "text/html"
    elif 'GET /update ' in request:
        file_path = '/data/update.html'
        content_type = 'text/html'      
    elif "GET /style.css " in request:
        file_path = "/data/style.css"
        content_type = "text/css"
    elif "GET /script.js " in request:
        file_path = "/data/script.js"
        content_type = "application/javascript"
    elif "GET /favicon.ico " in request:
        file_path = "data:image/x-icon;base64,AAAB AAAAA".encode('utf-8')
        content_type = "image/x-icon"    
    else:
        file_path = None
        content_type = "text/plain"
        response = "404 Não encontrado.".encode('utf-8')

    if file_path:
        cl.send("HTTP/1.1 200 OK\r\n")
        cl.send("Content-Type: {}\r\n".format(content_type))
        cl.send("Conexão: Terminada\r\n")
        cl.send("\r\n")

        for chunk in serve_file(file_path):
            cl.send(chunk)

    cl.close()

