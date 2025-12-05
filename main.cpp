/*
INTEGRANTES:

TOMAS AMARANTE MASSON
MARCELO HENRIQUE MARTINS CUNHA BARBOSA
ALEX SASAKI
DANILO MAGALHÃES VALOIS

INSTRUÇÕES:

use um mouse (de preferência) para facilitar a identificação do driver.
Cique para gerar objetos, no máximo 100.


*/





#include <iostream>

// rand
#include <stdlib.h>
#include <time.h>


// classes herdadadas (figura implicito)
#include "circulo.h"
#include "triangulo.h"
#include "quadrado.h"

#define MAX_OBJS 100
#define LARGURA 80
#define ALTURA 24

struct Velocidade {
    int vx;
    int vy;
};

#ifdef _WIN32
    // detectar mouse windows
    #include <windows.h>
    static bool hold_previous = false;

    int detect_mouse(int fd)
    {
        bool clicked = false;
        bool hold = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (hold && !hold_previous)
        {
            clicked = true;
        }

        return clicked;
    }
    void global_sleep(int t)
    {
        t = t/1000;
        Sleep(t);
    }

    int open_mouse()
    {
        return 1;
    }

    void close_mouse(int fd)
    {
        return ;
    }
#else
    // detectar mouse linux
    #include <fcntl.h>
    #include <unistd.h>
    #include <linux/input.h>
    #include <cstring>
    #include <dirent.h>

    std::string find_mouse_input();

    int detect_mouse(int fd)
    {
        struct input_event ev;

        bool click = false;
        while (read(fd, &ev, sizeof(struct input_event)) > 0)
        {
            if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1)
                click = true;
        }

        return click;
    }

    void global_sleep(int t)
    {
        usleep(t);
    }

    int open_mouse()
    {
        std::string path_str = find_mouse_input();
        
        if (path_str.empty()) 
            return -1;

        const char *dev = path_str.c_str();
        int fd = open(dev, O_RDONLY | O_NONBLOCK);

        return fd;
    }

    void close_mouse(int fd)
    {
        close(fd);
    }

    std::string find_mouse_input()
    {
        const char *directory = "/dev/input/by-id/";
        DIR *dir = opendir(directory);

        if (dir == nullptr)
        {
            // Se falhar no by-id, tenta diretório raiz (fallback)
            directory = "/dev/input/";
            dir = opendir(directory);
             if (dir == nullptr) {
                std::cerr << "Erro crítico: Input folder not found." << std::endl;
                return "";
             }
        }

        struct dirent* entry;
        std::string mouse_path = "";

        while ((entry = readdir(dir)) != nullptr)
        {
            // Procura "event-mouse" (by-id) ou apenas "event" (fallback genérico)
            if (strstr(entry->d_name, "event-mouse") || strstr(entry->d_name, "event"))
            {
                // Ignora keyboard se cair no fallback
                if (strstr(entry->d_name, "kbd")) continue; 

                mouse_path = std::string(directory) + std::string(entry->d_name);
                // Se achou um com "mouse" no nome, prioridade total e sai
                if (strstr(entry->d_name, "mouse")) break;
            }
        }

        closedir(dir);
        return mouse_path;
    }
#endif

void mover_figura(Figura* f, Velocidade &v) {
    if (!f) return;

    //Pega posição atual usando os métodos originais
    int x = f->get_x();
    int y = f->get_y();

    // Aplica a velocidade externa
    x += v.vx;
    y += v.vy;

    //Colisão com as bordas (Lógica de Rebote)
    
    // Eixo X
    if (x >= LARGURA) {
        x = LARGURA;
        v.vx = -v.vx; // Inverte velocidade no struct auxiliar
    } else if (x <= 1) {
        x = 1;
        v.vx = -v.vx;
    }

    // Eixo Y
    if (y >= ALTURA) {
        y = ALTURA;
        v.vy = -v.vy;
    } else if (y <= 1) {
        y = 1;
        v.vy = -v.vy;
    }

    // 4. Devolve a nova posição para o objeto
    f->set_posicao(x, y);
}

void DisplayRefresh(Figura* figs[], int qtd) {
    // 1. Limpa a tela (ANSI escape code)
    std::cout << "\033[2J\033[H"; 

    // 2. Percorre e desenha usando Polimorfismo
    for (int i = 0; i < qtd; i++) {
        if (figs[i] != nullptr) {
            // Posiciona o cursor (você pode encapsular isso na classe ou fazer aqui)
            std::cout << "\033[" << figs[i]->get_y() << ";" << figs[i]->get_x() << "H";
            figs[i]->desenhar(); // Chamada polimórfica
        }
    }
    
    // Força o output
    std::cout << std::flush;
}

int main(int argc, char *argv[])
{
    // incialização de rand
    srand(time(NULL));

    // dados do vetor mãe
    Figura *ptr[MAX_OBJS];
    int size = 0;

    Velocidade vels[MAX_OBJS];

    // teste de sudo
    int fd = open_mouse();
    if (fd == -1)
    {

        std::cerr << "Rode o programa com: sudo ./main" << std::endl;
        return 1;
    }

    short int end = 0, m_output = 0;

    // main loop do jogo
    while (!end)
    {
        
        
        if (detect_mouse(fd))
        {
            if (size < MAX_OBJS)
            {
                // Posição inicial aleatória
                int startX = (rand() % (LARGURA - 2)) + 2;
                int startY = (rand() % (ALTURA - 2)) + 2;

                // Cria a Figura
                int rn = rand() % 3;
                if (rn == 0)
                    ptr[size] = new Circulo(startX, startY);
                else if (rn == 1)
                    ptr[size] = new Triangulo(startX, startY);
                else 
                    ptr[size] = new Quadrado(startX, startY);

                // Define a Velocidade no array paralelo
                // (gera 1 ou -1 aleatoriamente)
                vels[size].vx = (rand() % 2 == 0) ? 1 : -1; 
                vels[size].vy = (rand() % 2 == 0) ? 1 : -1;

                size++;
            }
        }

        // 3. Atualiza e Desenha
        for (int i = 0; i < size; i++)
        {
            mover_figura(ptr[i], vels[i]);
        }

        DisplayRefresh(ptr, size);

        // Força o output a aparecer
        std::cout << std::flush;

        // Delay para animação fluida (50ms)
        global_sleep(50000);
    }
    
    close_mouse(fd);
    std::cout << "\033[?25h"; // Restaura cursor
    
    // Limpeza de memória
    for(int i=0; i<size; i++) delete ptr[i];

    return 0;
}

