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

bool sobranEspaciosLibres() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (board[i][j] == '.') {
                return true;
            }
        }
    }
    cout << "No hay más espacios: empate" << endl;
    return false;
}

int ColocarFichaEn(int columna, char ficha) {
    for (int i = n - 1; i >= 0; --i) {
        if (board[i][columna] == '.') {
            board[i][columna] = ficha;
            return i;
        }
    }
    return -1;
}

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

void handleClient(int client_socket) {
    char buffer[1024];
    char ficha_cliente = 'C', ficha_servidor = 'S';
    bool turno_servidor = false;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if (!turno_servidor) {
            send(client_socket, "Tu turno: ", 10, 0);
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) break;

            int columna = atoi(buffer) - 1;
            if (columna >= 0 && columna < m && board[0][columna] == '.') {
                int fila = ColocarFichaEn(columna, ficha_cliente);
                ImprimirTablero();
                if (ganador(fila, columna, ficha_cliente)) {
                    send(client_socket, "Ganaste!\n", 9, 0);
                    break;
                }
                turno_servidor = true;
            }
        } else {
            // Lógica del turno del servidor
            int columna = rand() % m;
            while (board[0][columna] != '.') columna = rand() % m;
            int fila = ColocarFichaEn(columna, ficha_servidor);
            ImprimirTablero();
            if (ganador(fila, columna, ficha_servidor)) {
                send(client_socket, "Perdiste!\n", 10, 0);
                break;
            }
            turno_servidor = false;
        }
        if (!sobranEspaciosLibres()) {
            send(client_socket, "Empate!\n", 8, 0);
            break;
        }
    }
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket" << endl;
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        cerr << "Error listening on socket" << endl;
        return 1;
    }

    cout << "Server listening on port " << port << endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            cerr << "Error accepting connection" << endl;
            return 1;
        }

        thread t(handleClient, client_socket);
        t.detach();
    }

    close(server_socket);
    return 0;
}