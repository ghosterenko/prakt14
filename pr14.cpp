#include <iostream>
#include <Windows.h>

#define MAX_CLIENTS 20
#define CLUB_CAPACITY 4

HANDLE hSemaphore;

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

ClubState club;

VOID WINAPI threadVisitor() {
	srand(time(NULL));
	ClientRecord client;
	client.arriveTick = GetTickCount64();

	DWORD wait = WaitForSingleObject(hSemaphore, 3000);
	if (wait == WAIT_OBJECT_0) {
		client.startTick = GetTickCount64();
		int waitTime = rand() % 5;
		Sleep(waitTime * 1000);
		ReleaseSemaphore(hSemaphore, 1, NULL);
		client.endTick = GetTickCount64();
		client.served = TRUE;
		client.timeout = FALSE;
	}
	else if(wait == WAIT_TIMEOUT){
		client.served = FALSE;
		client.timeout = TRUE;
	}
}

VOID WINAPI threadSeeker() {
	Sleep(500);
	std::cout << club.currentVisitors << std::endl;
	std::cout << club.servedCount << std::endl;
	std::cout << club.timeoutCount << std::endl;
}

int main()
{

}
