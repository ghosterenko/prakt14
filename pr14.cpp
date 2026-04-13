#include <iostream>
#include <windows.h>

#define MAX_CLIENTS 20
#define CLUB_CAPACITY 4

struct ClientRecord {
    DWORD arriveTick;
    DWORD startTick;
    BOOL served;
    BOOL timeout;
};

struct ClubState {
    ClientRecord clients[MAX_CLIENTS];
    LONG currentVisitors;
    LONG maxVisitors;
    LONG servedCount;
    LONG timeoutCount;
};

ClubState club;
HANDLE semaphore;
int idCur = 0;
HANDLE threads[MAX_CLIENTS];

void ClientThread() {
    int id = idCur;
    idCur++;

    DWORD arrive = GetTickCount64();

    club.clients[id].arriveTick = arrive;
    club.clients[id].served = FALSE;
    club.clients[id].timeout = FALSE;

    std::cout << "Клиент " << id + 1 << " пришел" << std::endl;

    if (WaitForSingleObject(semaphore, 3000) == WAIT_OBJECT_0) {
        DWORD start = GetTickCount64();
        int work = 2000 + rand() % 3001;

        club.currentVisitors++;
        if (club.currentVisitors > club.maxVisitors) {
            club.maxVisitors = club.currentVisitors;
        }

        club.clients[id].startTick = start;
        club.clients[id].served = TRUE;

        std::cout << "Клиент " << id + 1 << " начал (мест: " << club.currentVisitors << ")" << std::endl;

        Sleep(work);

        club.currentVisitors--;
        club.servedCount++;

        std::cout << "Клиент " << id + 1 << " закончил" << std::endl;
        ReleaseSemaphore(semaphore, 1, NULL);
    }
    else {
        club.clients[id].timeout = TRUE;
        club.timeoutCount++;
        std::cout << "Клиент " << id + 1 << " ушел" << std::endl;
    }
}

void seeker() {
    while (true) {
        Sleep(500);

        bool f = true;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (WaitForSingleObject(threads[i], 0) == WAIT_TIMEOUT) {
                f = false;
                break;
            }
        }

        std::cout << "Статус: занято=" << club.currentVisitors << " обслужено=" << club.servedCount << " ушло=" << club.timeoutCount << std::endl;

        if (f)
            break;
    }
}

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "ru");

    semaphore = CreateSemaphore(NULL, CLUB_CAPACITY, CLUB_CAPACITY, NULL);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientThread, NULL, 0, NULL);
        if (i < 8) SetThreadPriority(threads[i], THREAD_PRIORITY_NORMAL);
        else if (i < 16) SetThreadPriority(threads[i], THREAD_PRIORITY_BELOW_NORMAL);
        else SetThreadPriority(threads[i], THREAD_PRIORITY_HIGHEST);
    }

    HANDLE obs = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)seeker, NULL, 0, NULL);
    SetThreadPriority(obs, THREAD_PRIORITY_LOWEST);

    WaitForSingleObject(obs, INFINITE);

    std::cout << "\nОбслужено: " << club.servedCount << " Ушло: " << club.timeoutCount << " Максимум мест: " << club.maxVisitors << std::endl;

    LONG totalWait = 0;
    int served = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (club.clients[i].served) {
            totalWait += (club.clients[i].startTick - club.clients[i].arriveTick);
            served++;
        }
    }
    if (served > 0) {
        std::cout << "Среднее ожидание: " << totalWait / served << " мс" << std::endl;
    }

    std::cout << "Не обслужены: ";
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (club.clients[i].timeout) {
            std::cout << i + 1 << " ";
        }
    }
    std::cout << std::endl;

    for (int i = 0; i < MAX_CLIENTS; i++) 
        CloseHandle(threads[i]);
    CloseHandle(obs);
    CloseHandle(semaphore);

    return 0;
}
