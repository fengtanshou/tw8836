/*********************************************************************************************************
**				                               Small RTOS(51)
**                                   The Real-Time Kernel(For Keil c51)
**
**                                  (c) Copyright 2002-2003, chenmingji
**                                           All Rights Reserved
**
**                                                  V1.20.0
**
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: OS_mem.c
**��   ��   ��: ������
**����޸�����:  2003��8��3��
**�衡     ��: �ڴ涯̬����ģ�飬�����ڷ�Small RTOS(51)��ʹ�á���Small RTOS(51)
**             �±�������Ϊ֧������ջ��
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ������
** ��  ��: V1.20.0
** �ա���: 2003��8��3��
** �衡��: ԭʼ�汾
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���: 
** �ա���:
** �衡��:
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#define IN_OS_MEM
#include "config.h"

#ifndef EN_OS_MEM
#define EN_OS_MEM       0
#endif

#if EN_OS_MEM > 0
static struct _FreeMem xdata *OSFreeMem;                /* �����ڴ���˫��������ͷ */


/*********************************************************************************************************
** ��������: UserMemChkErr
** ��������: �ڴ����ʱ����������������EN_UserMemChkErr����Ϊ1ʱ,�û������Լ��������������
** �䡡��: Addr: Ӧ�����յ��ڴ��
** �䡡��: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��8��3��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#if EN_OS_MEM_CHK > 0 && EN_UserMemChkErr == 0
        void UserMemChkErr(void xdata *Addr) reentrant
{
    Addr = Addr;
    while (1);
}
#endif

/*********************************************************************************************************
** ��������: OSMemInit
** ��������: ��ʼ����̬�ڴ����ģ�顣
** �䡡��: Addr: ָ����пռ���ʼλ�õ�ָ��
**        MemSize: ���пռ�Ĵ�С 
** �䡡��: TRUE: �ɹ�
**        FALSE: ʧ��
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��8��3��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
        uint8 OSMemInit(void xdata *Addr, unsigned int MemSize)
{
    struct _FreeMem xdata *MemFree;
    
    MemSize = MemSize & ~(sizeof(int) - 1);     /* �ڴ水����(word)Ϊ��λ���� */
                                                /* ����MemSizeΪ��(word)�������� */

    /* �ж����ɿռ��Ƿ���Ч */
    if (Addr != NULL && MemSize > sizeof(struct _UsingMem))
    {
        /* ����ֻ��һ���ڵ��˫����� */
        OS_ENTER_CRITICAL();
        MemFree = (struct _FreeMem xdata *)Addr;
        OSFreeMem = MemFree;
        MemFree->Size = MemSize;
        MemFree->Next = NULL;
        MemFree->Last = NULL;
        OS_EXIT_CRITICAL();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*********************************************************************************************************
** ��������: OSMemNew
** ��������: �����ڴ档
** �䡡��: Size: �����ڴ�Ĵ�С
** �䡡��: ָ���ѷ�����ڴ濪ʼ�ĵ�ַ��ָ�룬NULLΪʧ�ܡ�
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��8��3��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
        void xdata *OSMemNew(unsigned int Size) reentrant
{
    struct _FreeMem xdata *ThisFreeMem;
    struct _UsingMem xdata *Rt;
    
    OS_ENTER_CRITICAL();
    ThisFreeMem = OSFreeMem;
    
    /* ����Ӧ������Ĵ�С������Ϊ��(word)��������������Ҫ��������ͷ�Ŀռ� */
    Size = ((Size + sizeof(int) - 1) & ~(sizeof(int) - 1)) + 
           sizeof(struct _UsingMem);
    
    /* �����㹻��������ڴ�� */
    while(ThisFreeMem != NULL)
    {
        if ((ThisFreeMem->Size) >= Size)
        {
            break;
        }
        ThisFreeMem = ThisFreeMem->Next;
    }
    if (ThisFreeMem != NULL)
    {
        if ((ThisFreeMem->Size) < (Size + sizeof(struct _FreeMem)))
        {
            /* �����ʣ���ڴ�̫С�����������ڴ�����ȥ */
            /* ����˫��������ɾ������ڵ� */
            if (ThisFreeMem->Next != NULL)
            {
                (ThisFreeMem->Next)->Last = ThisFreeMem->Last;
            }
            if (ThisFreeMem->Last)
            {
                (ThisFreeMem->Last)->Next = ThisFreeMem->Next;
            }
            Size = ThisFreeMem->Size;
            Rt = (struct _UsingMem xdata *)(ThisFreeMem);
        }
        else
        {
            /* ����������ڴ��ĸߵ�ַ�˷���һ���ڴ� */
            /* ��������������ڵ������ */
            ThisFreeMem->Size -= Size;
            Rt = (struct _UsingMem xdata *)((uint8 xdata *)ThisFreeMem + 
                                             ThisFreeMem->Size);
        }
#if EN_OS_MEM_CHK > 0
        Rt->HeadChk = 0xa55a;
        Rt->EndChk = 0x5aa5;
#endif
        Rt->Size = Size;
        OS_EXIT_CRITICAL();
        return ( void *)((uint8 xdata *)Rt + sizeof(struct _UsingMem));
    }
    else
    {
        OS_EXIT_CRITICAL();
        return NULL;
    }
    
}

/*********************************************************************************************************
** ��������: OSMemFree
** ��������: ����OSMemNew����Ŀռ�黹��ϵͳ
** �䡡��: Addr: ��ָOSMemNew���ص�ָ��
** �䡡��: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��8��3��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_OS_MEM_FREE
#define EN_OS_MEM_FREE      1
#endif

#if EN_OS_MEM_FREE > 0

        void OSMemFree(void xdata *Addr) reentrant
{
    struct _FreeMem xdata *ThisFreeMem;
    struct _UsingMem xdata *UserMem;
    unsigned int Size;

    OS_ENTER_CRITICAL();
    
    ThisFreeMem = OSFreeMem;
    /* �ҵ��ڴ��Ŀ�ʼλ�� */
    UserMem = (struct _UsingMem xdata *)((uint8 xdata *)Addr - 
                                          sizeof(struct _UsingMem));
#if EN_OS_MEM_CHK > 0
    if (UserMem->HeadChk != 0xa55a || UserMem->EndChk != 0x5aa5)
    {
        UserMemChkErr(Addr);
        OS_EXIT_CRITICAL();
        return;
    }
#endif
    /* ��������ڴ�Ӧ����˫�������в����λ�� */    
    /* ˫���������ڴ���׵�ַ�Ĵ�С˳��Ϊ�ڵ������˳�� */
    while (1)
    {
        if (ThisFreeMem > (struct _FreeMem xdata *)UserMem)
        {
            ThisFreeMem = ThisFreeMem->Last;
            break;
        }
        if (ThisFreeMem->Next == NULL)
        {
            break;
        }
        
        ThisFreeMem = ThisFreeMem->Next;
    }

    if (((uint8 xdata *)ThisFreeMem + (ThisFreeMem->Size)) == (uint8 xdata *)UserMem)
    {
        /* ���յ��ڴ������һ�������ڴ�����ڣ���ֱ���޸�����ڵ����� */
        ThisFreeMem->Size += UserMem->Size;
    }
    else
    {
        /* �������һ���ڵ� */
        Size = UserMem->Size;
        ((struct _FreeMem xdata *)UserMem)->Last = ThisFreeMem;
        ((struct _FreeMem xdata *)UserMem)->Next = ThisFreeMem->Next;
        ((struct _FreeMem xdata *)UserMem)->Size = Size;
        ThisFreeMem->Next = (struct _FreeMem xdata *)UserMem;
        ThisFreeMem = (struct _FreeMem xdata *)UserMem;
        if (ThisFreeMem->Next != NULL)
        {
            (ThisFreeMem->Next)->Last = ThisFreeMem;
        }
    }
    if (((uint8 xdata *)ThisFreeMem + (ThisFreeMem->Size)) ==
         (uint8 xdata *)(ThisFreeMem->Next))
    {
        /* ����������ڴ�����ڣ���ϲ�֮ */
        ThisFreeMem->Size += (ThisFreeMem->Next)->Size;
        ThisFreeMem->Next = (ThisFreeMem->Next)->Next;
        if(ThisFreeMem->Next != NULL)
        {
            (ThisFreeMem->Next)->Last = ThisFreeMem;
        }
    }
    OS_EXIT_CRITICAL();
    return;
}
#endif
#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/