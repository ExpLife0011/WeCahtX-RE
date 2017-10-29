#include "stdafx.h"
#include "DecryptDB.h"

extern HANDLE g_DbSleepEvent;

DWORD WINAPI DecryptDBStart(LPVOID lpParam)
{
	int dwSleepQueryTime = 30 * 1000;  // 30s
	if (!g_DbSleepEvent)
	{
		return 0;
	}

	while (WaitForSingleObject(g_DbSleepEvent, dwSleepQueryTime) != WAIT_OBJECT_0)
	{
		// Copy ���е����� ����;
		//

		SetEvent(g_DbSleepEvent);
		
	}
	return 1;
}