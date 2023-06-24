#pragma once
#ifndef __LOCK_FREE_STACK_H__
#define	__LOCK_FREE_STACK_H__
#include <windows.h>

enum eLockFreeFuncType
{
	PUSH, POP
};

struct LockFreeHistory {
	eLockFreeFuncType funType;
	DWORD threadID;
	int value;
	int nextValue;
	int ccurrentNextValue;
	__int64* top;
	__int64* topNext;
	__int64* topCurrentNext;
	int gPopIndex;
};

template <typename T>
class LockFreeStack
{
private:
	struct Node
	{
		Node* next;
		T _value;
	};
public:
	LockFreeStack()
		: top(nullptr)
		, size(0)
		, debugIndex(0)
		, lock(false)
	{
	}

	~LockFreeStack()
	{
	}

	void push(T input, LockFreeHistory* history)
	{
		Node* _localScopeTop;
		Node* newNode = new Node;
		newNode->_value = input;
		int localNextValue = -1;
		do {
			_localScopeTop = top;
			newNode->next = _localScopeTop;
			if (_localScopeTop != nullptr)
			{
				localNextValue = _localScopeTop->_value;
			}
		} while (InterlockedCompareExchange((uintptr_t*)&top, (uintptr_t)newNode, (uintptr_t)_localScopeTop) != (uintptr_t)_localScopeTop);
		InterlockedIncrement(&size);

		//if (history == nullptr)
		//{
		//	return;
		//}
			

		history->funType = PUSH;
		history->threadID = GetCurrentThreadId();
		history->top = (__int64*)newNode;
		history->topNext = (__int64*)_localScopeTop;
		history->value = input;
		history->nextValue = localNextValue;
	}

	bool pop(T* input, LockFreeHistory* history, int gPopIndex)
	{
		Node* _localScopeTop;
		Node* _localNodeNext;
		Node* _currentTopNext;

		int localCurrentNextValue = -1;
		int localNextValue = -1;
		do {
			if ((_localScopeTop = top) == nullptr)
			{
				return false;
			}
			*input = _localScopeTop->_value;
			_localNodeNext = _localScopeTop->next;
			_currentTopNext = top->next;
			if (_currentTopNext != nullptr)
			{
				localCurrentNextValue = _currentTopNext->_value;
			}
				
			if (_localNodeNext != nullptr)
			{
				localNextValue = _localNodeNext->_value;
			}
		} while (InterlockedCompareExchange((uintptr_t*)&top, (uintptr_t)_localNodeNext, (uintptr_t)_localScopeTop) != (uintptr_t)_localScopeTop);
		if (_localScopeTop->next)
		InterlockedDecrement(&size);
		
		//if (history == nullptr)
		//{
		//	return;
		//}

		history->funType = POP;
		history->threadID = GetCurrentThreadId();
		history->top = (__int64*)_localScopeTop;
		history->topNext = (__int64*)_localNodeNext;
		history->value = *input;
		history->nextValue = localNextValue;
		history->ccurrentNextValue = localCurrentNextValue;
		history->topCurrentNext = (__int64*)_currentTopNext;
		history->gPopIndex = gPopIndex;

		delete _localScopeTop;
		return true;
	}

private:
	Node* top;
	unsigned __int64 debugIndex;
	size_t size;
	unsigned __int64 lock;
};
#endif // !__LOCK_FREE_STACK_H__
