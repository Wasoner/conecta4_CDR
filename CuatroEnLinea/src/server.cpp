#include <iostream>
#include <vector>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

using namespace std;

const int n = 6;
const int m = 7;
vector<vector<char>> board(n, vector<char>(m, '.'));

// Función para imprimir el tablero
void ImprimirTablero() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cout << board[i][j] << ' ';
        }
        cout << endl;
    }
    cout << "-------------" << endl;
    for (int j = 0; j < m; ++j) {
        cout << j + 1 << ' ';
    }
    cout << endl;
}

// Verificar si hay espacios libres en el tablero
bool sobranEspaciosLibres() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (board[i][j] == '.') {
                return true;
            }
        }
    }
    return false;
}

// Colocar ficha en la columna especificada
int ColocarFichaEn(int columna, char ficha) {
    for (int i = n - 1; i >= 0; --i) {
        if (board[i][columna] == '.') {
            board[i][columna] = ficha;
            return i;
        }
    }
    return -1;
}

// Verificar si hay un ganador
bool ganador(int fila, int columna, char ficha) {
    // Verificar vertical
    int count = 0;
    for (int i = 0; i < n; ++i) {
        if (board[i][columna] == ficha) {
            count++;
            if (count == 4) return true;
        } else {
            count = 0;
        }
    }

    // Verificar horizontal
    count = 0;
    for (int j = 0; j < m; ++j) {
        if (board[fila][j] == ficha) {
            count++;
            if (count == 4) return true;
        } else {
            count = 0;
        }
    }

    // Verificar diagonal (\)
    count = 0;
    int start_row = fila, start_col = columna;
    while (start_row > 0 && start_col > 0) {
        start_row--;
        start_col--;
    }
    while (start_row < n && start_col < m) {
        if (board[start_row][start_col] == ficha) {
            count++;
            if (count == 4) return true;
        } else {
            count = 0;
        }
        start_row++;
        start_col++;
    }

    // Verificar diagonal (/)
    count = 0;
    start_row = fila;
    start_col = columna;
    while (start_row > 0 && start_col < m - 1) {
        start_row--;
        start_col++;
    }
    while (start_row < n && start_col >= 0) {
        if (board[start_row][start_col] == ficha) {
            count++;
            if (count == 4) return true;
        } else {
            count = 0;
        }
        start_row++;
        start_col--;
    }

    return false;
}

// Manejar la conexión con un cliente
void handleClient(int client_socket) {
    char buffer[1024];
    char ficha_cliente = 'C', ficha_servidor = 'S';
    bool turno_servidor = false;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (!turno_servidor) {
            // Enviar mensaje al cliente para que juegue
            send(client_socket, "Tu turno: ", 10, 0);
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) break;

            int columna = atoi(buffer) - 1; // Ajustar a índice basado en cero
            if (columna >= 0 && columna < m && board[0][columna] == '.') {
                int fila = ColocarFichaEn(columna, ficha_cliente);
                ImprimirTablero();
                // Verificar si el cliente ganó
                if (ganador(fila, columna, ficha_cliente)) {
                    send(client_socket, "Ganaste!\n", 9, 0);
                    cout << "Cliente ganó!" << endl; // Mensaje de depuración
                    break;
                }
                turno_servidor = true;
            } else {
                send(client_socket, "Columna invalida. Intente de nuevo.\n", 36, 0);
            }
        } else {
            // Turno del servidor (jugada aleatoria)
            int columna = rand() % m;
            while (board[0][columna] != '.') columna = rand() % m;
            int fila = ColocarFichaEn(columna, ficha_servidor);
            ImprimirTablero();
            // Verificar si el servidor ganó
            if (ganador(fila, columna, ficha_servidor)) {
                send(client_socket, "Perdiste! (UnU) \n", 10, 0);
                cout << "Servidor ganó!" << endl; // Mensaje de depuración
                break;
            }
            turno_servidor = false;
        }

        // Verificar si hay empate
        if (!sobranEspaciosLibres()) {
            send(client_socket, "Empate! \n", 8, 0);
            break;
        }
    }
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Modo de uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);

    // Crear el socket del servidor
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "UPS hubo un error al crear el socket" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "UPS hubo un error en vincular el socket" << endl;
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        cerr << "No puedo escuchar el socket" << endl;
        return 1;
    }

    cout << "Estare escuchando en el puerto:  " << port << endl;

    // Esperar conexiones de clientes
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            cerr << "Error al permitir la conexion con el cliente" << endl;
            return 1;
        }

        // Crear un nuevo hilo para manejar la conexión del cliente
        thread t(handleClient, client_socket);
        t.detach();
    }

    close(server_socket);
    return 0;
}