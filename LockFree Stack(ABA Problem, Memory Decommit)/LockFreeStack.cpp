#include <stdio.h>
#include <process.h>
#include <time.h>
#include <map>
#include "LockFreeStack.h"
#pragma comment(lib, "winmm")

unsigned WINAPI TestLockFreeStack(LPVOID);

int main()
{
	timeBeginPeriod(1);
	HANDLE hThread[4];
	for (int i = 0; i < 4; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(nullptr, 0, TestLockFreeStack, (LPVOID)i, 0, nullptr);
	}

	WaitForMultipleObjects(4, hThread, true, INFINITE);
	printf("³¡\n");
	timeEndPeriod(1);
}

int gValue = 0;
int gPopIndex = 0;
LockFreeStack<int> stack;
LockFreeHistory* history[4];
unsigned WINAPI TestLockFreeStack(LPVOID args)
{
	history[(int)args] = new LockFreeHistory[50000];
	memset(history[(int)args], 0, sizeof(LockFreeHistory) * 50000);
	srand(time(nullptr));
	int pop;
	int index = 0;
	int localValue;
	int popIndex;

	for (int k = 0; k < 2500; ++k)
	{
		for (int i = 0; i < 10; ++i)
		{
			localValue = InterlockedExchangeAdd((uint32_t*)&gValue, (uint32_t)1);
			stack.push(localValue, (history[(int)args] + index++));
			//history2[index] = history;
			//historyMap.insert({ index, history });
		}

		for (int j = 0; j < 10; ++j)
		{
			popIndex = InterlockedExchangeAdd((uint32_t*)&gPopIndex, (uint32_t)1);
			stack.pop(&pop, (history[(int)args] + index++), popIndex);
			//history2[index] = history;
			//historyMap.insert({ index, history });
		}
	}
	return 0;
}