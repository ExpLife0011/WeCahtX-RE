#ifdef WIN32
#    ifndef WIN32_LEAN_AND_MEAN 
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>
#    ifndef PAGE_SIZE
#        define PAGE_SIZE 0x1000
#    endif
#else
#    include <ntifs.h>
#    ifndef MAX_PATH
#        define MAX_PATH 260
#    endif
#endif

#include <emmintrin.h>
#include "FSearch.h"

// 1�������ַ���
//    SigPattern = "This is a null terminated string."
//    SigMask = NULL or "xxxxxxxxxxx" or "x?xx????xxx"
//
// 2���������롢����������
//    SigPattern = "\x8B\xCE\xE8\x00\x00\x00\x00\x8B"
//    SigMask = "xxxxxxxx" or "xxx????x"
//
// Mask �е� ? ������ģ��ƥ�䣬�ڱ���������Ƭ�����ж�̬�仯������ʱʹ��(��ָ������ĵ�ַ�����ݵ�)
//
// ���������������ڴ��Ӧ���Ӻ����������ڴ�������棬�����и��˵ķ�������������޹ؾ�ʡ����
//

// ����˵������ַ��������С�������룬ƥ����
// ƥ������ָ��ôƥ�������룬x���뾫ȷƥ�䣬������ģ��ƥ��

// ����ֵ,����λ��
ULONG FastSearchVirtualMemory(ULONG VirtualAddress, ULONG VirtualLength, PUCHAR SigPattern, PCHAR SigMask)
{
	// SigMask δָ��ʱ�Զ����ɼ򻯵��ã���ֻ����������ַ�����
	CHAR TmpMask[PAGE_SIZE];
	if (SigMask == NULL || SigMask[0] == 0) {
		ULONG SigLen = (ULONG)strlen((PCHAR)SigPattern);
		if (SigLen > PAGE_SIZE - 1) SigLen = PAGE_SIZE - 1;
		memset(TmpMask, 'x', SigLen);
		TmpMask[SigLen] = 0;
		SigMask = TmpMask;
	}

	// �������
	PUCHAR MaxAddress = (PUCHAR)(VirtualAddress + VirtualLength);
	PUCHAR BaseAddress;
	PUCHAR CurrAddress;
	PUCHAR CurrPattern;
	PCHAR CurrMask;
	BOOLEAN CurrEqual;
	register UCHAR CurrUChar;

	// SSE ������ر���
	__m128i SigHead = _mm_set1_epi8((CHAR)SigPattern[0]);
	__m128i CurHead, CurComp;
	ULONG MskComp, IdxComp;
	ULONGLONG i, j;

	//
	// ��һ�����ʹ�� SSE �����ֽڼ���Ϊ�� 16 �ֽ�ÿ�Σ����ռ��� 12 ��������Ҫ��Դ��ˣ�
	//
	// �ڶ����Ӵ�ƥ�䲻��ʹ�� SSE ���٣�ԭ������
	//     1. SSE ��Ϊ��ָ������ݣ�������ָ�� CPU ���ڱȳ���ָ��Ҫ��
	//
	//     2. �Ӹ�������˵���Ӵ�ƥ��ʱ��һ���ֽ�����ʧ���� SSE һ���ԶԱ� 16 ���ֽ�����ʧ���ڸ����ϼ������
	//
	//     3. ����ʵ����� SSE �Ż��ڶ����Ӵ�ƥ�佫�����������ղ����ٶ�
	//
	//     4. �����ϣ���ʹ SSE ����ָ���볣��ָ�����ͬ����CPU���ڣ����Ҳֻ�ܼ��� 16 ��
	//
	for (i = 0; i <= VirtualLength - 16; i += 16)
	{
		CurHead = _mm_loadu_si128((__m128i*)(VirtualAddress + i));
		CurComp = _mm_cmpeq_epi8(SigHead, CurHead);
		MskComp = _mm_movemask_epi8(CurComp);

		BaseAddress = (PUCHAR)(VirtualAddress + i);
		j = 0;
		while (_BitScanForward(&IdxComp, MskComp))
		{
			CurrAddress = BaseAddress + j + IdxComp;
			CurrPattern = SigPattern;
			CurrMask = SigMask;
			for (; CurrAddress <= MaxAddress; CurrAddress++, CurrPattern++, CurrMask++)
			{
				// ��Ϊ�Ǳ�����������ϵͳ�������ڴ棬������������Ķ�ջ����ȻҲ�������������ڴ��һ����
				// ���Ϊ�˱���ƥ�䵽���� SigPattern ��������������Ӧ���˲������粻��Ҫ�������м� 2 ��
				CurrUChar = *CurrPattern;
				// *CurrPattern = CurrUChar + 0x1;
				CurrEqual = (*CurrAddress == CurrUChar);
				// *CurrPattern = CurrUChar;

				if (!CurrEqual) { if (*CurrMask == 'x') break; }
				if (*CurrMask == 0) { return (ULONG)(BaseAddress+ j + IdxComp); }
			}

			++IdxComp;
			MskComp = MskComp >> IdxComp;
			j += IdxComp;
		}
	}

	return 0x0;
}






// ������������
PVOID FindTarget(PVOID addr, DWORD len, PUCHAR target, DWORD target_len)
{
    PVOID status = NULL;
    DWORD i = 0, j = 0;
    PUCHAR cur = (PUCHAR)addr;
    PUCHAR target_cur = (PUCHAR)target;
    BOOL bBadPoint = FALSE;

	for (i = 0; i < len; i++)
	{
		for (j = 0; j < target_len && (i + j) < len; j++)
		{
			//�ж�ָ���ַ�Ƿ���Ч
			if (IsBadReadPtr((const void*)&cur[i + j], 1))
			{
				bBadPoint = TRUE;
				break;
			}

			if (cur[i + j] != target_cur[j] && 0xcb != target_cur[j])
			{
				break;
			}
		}

		if (bBadPoint)
			break;

		if (j == target_len)
		{
			status = &cur[i];
			break;
		}
	}
    return status;
}