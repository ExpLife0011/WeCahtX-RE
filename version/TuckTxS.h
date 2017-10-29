#ifndef __DEF_TUCKTXS_MON__
#define __DEF_TUCKTXS_MON__

#include "windows.h"

#include <vector> 
using namespace std;
#include <atlstr.h>

class TuckMsg
{
public:	
	VOID Start();
   

public:
	static TuckMsg* Instance()
	{
		static TuckMsg s_Instance;
		return &s_Instance;
	}
	
private:
	TuckMsg::TuckMsg() :
		// ԭʼģ����
		m_hProcess(NULL),
		m_exe_msver(0),
		m_exe_lsver(0),
		m_Init(0),
		m_exe_size(0),
		m_FirstHitTime(0)
	{
		;
	}


	VOID _StartImpl();
	VOID StartHook();
    


private:
	HMODULE m_hProcess;        // ע���ģ��ĵ�ַ
	DWORD m_exe_msver;         // ���汾��
	DWORD m_exe_lsver;         // �ΰ汾��
	BOOL  m_Init;              // HOOK������ʼ��Ok
	DWORD m_exe_size;          // ע��Ľ��̵��ļ���С
	CString m_cs_filename;
	DWORD m_FirstHitTime;
};






#endif