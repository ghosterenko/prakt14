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
	LONG maxVisitors;
	LONG servedCount;
	LONG timeoutCount;
};

HANDLE Visitors[20], seeker, hSemaphore;
ClubState club;
int count = 0;

VOID WINAPI threadVisitor() {
	srand(time(NULL));
	while (count <= 20)
	{
		club.clients[0].arriveTick = GetTickCount64();
		club.clients[0];
		DWORD wait = WaitForSingleObject(hSemaphore, 3000);
		if (wait == WAIT_OBJECT_0) {
			club.clients[0].startTick = GetTickCount64() - club.clients[0].arriveTick;

			int waitTime = rand() % 5;
			Sleep(waitTime * 1000);
			ReleaseSemaphore(hSemaphore, 1, NULL);

			club.clients[0].endTick = GetTickCount64() - club.clients[0].endTick;

			club.clients[0].served = TRUE;
			club.clients[0].timeout = FALSE;
		}
		else if (wait == WAIT_TIMEOUT) {
			club.clients[0].served = FALSE;
			club.clients[0].timeout = TRUE;

			club.timeoutCount++;
		}
	}
}

VOID WINAPI threadSeeker() {
	
	while (count <= 20)
	{
		Sleep(500);
		std::cout << club.currentVisitors << std::endl;
		std::cout << club.servedCount << std::endl;
		std::cout << club.timeoutCount << std::endl;
		count++;
	}
}

int main()
{
	hSemaphore = CreateSemaphoreA(NULL, 1, 3, NULL);

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
	CloseHandle(Visitors);
	CloseHandle(seeker);
	CloseHandle(hSemaphore);
}
