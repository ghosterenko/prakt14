#include <iostream>
#include <Windows.h>

#define MAX_CLIENTS 20
#define CLUB_CAPACITY 4

struct ClientRecord {
	DWORD threadId;
	DWORD arriveTick;
	DWORD startTick;
	DWORD endTick;
	BOOL served;
	BOOL timeout;
};

struct ClubState {
	ClientRecord clients[MAX_CLIENTS];
	LONG currentVisitors;
	LONG maxVisitors = CLUB_CAPACITY;
	LONG servedCount;
	LONG timeoutCount;
};

HANDLE Visitors[MAX_CLIENTS], seeker, hSemaphore;
ClubState club;
int count = 0;
int counter = 0;

VOID WINAPI threadVisitor() {
	srand(time(NULL));
	int i = 0;
	ClientRecord client = ClientRecord();
	client.arriveTick = GetTickCount64();
	while (count <= 20)
	{
		DWORD wait = WaitForSingleObject(hSemaphore, 3000);
		if (wait == WAIT_OBJECT_0) {
			client.startTick = GetTickCount64() - client.arriveTick;

			int waitTime = rand() % 5;
			Sleep(waitTime * 1000);
			ReleaseSemaphore(hSemaphore, 1, NULL);

			client.endTick = GetTickCount64() - client.endTick;

			client.served = TRUE;
			client.timeout = FALSE;
		}
		else if (wait == WAIT_TIMEOUT) {
			client.served = FALSE;
			client.timeout = TRUE;
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
		club.clients[counter] = client;
		counter++;
	}
}

VOID WINAPI threadSeeker() {
	
	while (count <= 20)
	{
		WaitForSingleObject(hSemaphore, INFINITE);
		Sleep(500);
		std::cout << club.currentVisitors << " - Текущее число занятых мест" << std::endl;
		std::cout << club.servedCount << " - Кол-во обслуженных посетителей" << std::endl;
		std::cout << club.timeoutCount << " - Кол-во ушедших по таймауту" << std::endl;
		count++;
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}
}

int main()
{
	setlocale(LC_ALL, "ru");

	hSemaphore = CreateSemaphoreA(NULL, 1, 4, NULL);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		Visitors[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadVisitor, NULL, NULL, &club.clients[i].threadId);
	}
	for (int i = 0; i < 7; i++)
	{
		if (!SetThreadPriority(Visitors[i], THREAD_PRIORITY_NORMAL))
			return GetLastError();
	}
	for (int i = 8; i < 15; i++)
	{
		if (!SetThreadPriority(Visitors[i], THREAD_PRIORITY_BELOW_NORMAL))
			return GetLastError();
	}
	for (int i = 16; i < 19; i++)
	{
		if (!SetThreadPriority(Visitors[i], THREAD_PRIORITY_HIGHEST))
			return GetLastError();
	}

	seeker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadSeeker, NULL, NULL, NULL);
	if (!SetThreadPriority(seeker, THREAD_PRIORITY_LOWEST))
		return GetLastError();

	while (count <= 20) {}

	WaitForMultipleObjects(20, Visitors, TRUE, INFINITE);
	WaitForSingleObject(seeker, INFINITE);
	WaitForSingleObject(hSemaphore, INFINITE);
	CloseHandle(Visitors);
	CloseHandle(seeker);
	CloseHandle(hSemaphore);
}
