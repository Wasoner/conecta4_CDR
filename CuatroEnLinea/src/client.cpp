#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <limits>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server_ip> <port>" << endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Crear el socket del cliente
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        cerr << "Invalid address" << endl;
        return 1;
    }

    // Conectar al servidor
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    char buffer[1024];
    while (true) {
        // Recibir el estado del juego del servidor
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
            break;

        cout << buffer;

        // Verificar si se ha ganado o terminado el juego
        if (strstr(buffer, "Ganaste!") != nullptr || strstr(buffer, "Perdiste!") != nullptr || strstr(buffer, "Empate!") != nullptr) {
            break;
        }

        int columna;
        while (true) {
            // Solicitar al usuario que introduzca una columna válida
            cout << "Introduce la columna (1-7): ";
            cin >> columna;
            if (cin.fail() || columna < 1 || columna > 7) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Columna invalida, intente de nuevo" << endl;
            }
            else
            {
                break;
            }
        }
        // Enviar la columna seleccionada al servidor
        sprintf(buffer, "%d", columna);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Cerrar el socket del cliente al terminar el juego
    close(client_socket);
    return 0;
}