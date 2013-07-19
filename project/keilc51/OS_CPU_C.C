/*********************************************************************************************************
**				                               Small RTOS 51
**                                   The Real-Time Kernel For Keil c51
**
**                                  (c) Copyright 2002-2003, chenmingji
**                                           All Rights Reserved
**
**                                                  V1.20.2
**
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: OS_CPU_C.C
**��   ��   ��: ������
**����޸�����:  2004��4��22��
**��        ��: Small RTOS 51��CPU(��8051ϵ��)��ص�C���Դ���
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ������
** ��  ��: V0.50~v1.00
** �ա���: 2002��2��22��~2002��6��20��
** �衡��: �������
**
**------------------------------------------------------------------------------------------------------
** �޸���: ������
** ��  ��: V1.10~V1.12.0
** �ա���: 2002��9��1��~002��12��30��
** �衡��: ���ƴ���
**
**------------------------------------------------------------------------------------------------------
** �޸���: ������
** ��  ��: V1.20.0
** �ա���: 2003��8��17��
** �衡��: �����°汾��Ҫ�����˺���
**
**------------------------------------------------------------------------------------------------------
** �޸���: ������
** ��  ��: V1.20.2
** �ա���: 2004��4��22��
** �衡��: ��������ɾ��ʱ��һ��BUG
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���:
** �ա���:
** �衡��:
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#define  IN_OS_CPU_C
#include "CONFIG.h"

extern idata uint8 STACK[1];                    /* ��ջ��ʼλ��,��OS_CPU_A���� */
uint8 idata * data OSTsakStackBotton[OS_MAX_TASKS + 2];/* �����ջ�ײ�λ��            */

#if EN_SP2 > 0
uint8 idata  Sp2[Sp2Space];                     /* �߼��ж϶�ջ           */
#endif

#if OS_MAX_TASKS < 9            
uint8 data OSFastSwap[1];                        /* �����Ƿ���Կ����л� */
#else
uint8 data OSFastSwap[2];
#endif

extern data uint16 C_XBP;

void TaskIdle(void)   small;
/*********************************************************************************************************
** ��������: OSCPUInit
** ��������: Small RTOS ��ϵͳ��صĳ�ʼ��
** �䡡��: ��
** �䡡�� : ��
** ȫ�ֱ���: OSTaskID,OSTsakStackBotton,SP
** ����ģ��: LoadCtx
** 
** ������: ������
** �ա���: 2002��2��22��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**-------------------------------------------------------------------------------------------------------
********************************************************************************************************/
        void OSCPUInit(void)  small
{
    uint8 i;

    for (i = 0; i < OS_MAX_TASKS + 1; i++)
    {
        OSTsakStackBotton[i] = STACK;
    }
    OSTsakStackBotton[OS_MAX_TASKS + 1] = (uint8 idata *)(IDATA_RAM_SIZE % 256);
    
}

/*********************************************************************************************************
** ��������: OSTaskStkInit
** ��������: �����ջ��ʼ��
** �䡡��: ��
** �䡡�� : ��
** ȫ�ֱ���: OSTaskID,OSTsakStackBotton,SP
** ����ģ��: LoadCtx
** 
** ������: ������
** �ա���: 2002��2��22��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**-------------------------------------------------------------------------------------------------------
********************************************************************************************************/
/*
#if EN_REENTRANT > 0
#define SP_ADD_BYTE  5
#else
#define SP_ADD_BYTE  3
#endif


        void OSTaskStkInit(void (code * task)(void), void xdata *ptos, uint8 TaskID) small
{
    uint8 i;
    uint8 idata * cp;
    uint8 idata * cp1;
    
#if OS_MAX_TASKS < 9
        OSFastSwap[1] |= OSMapTbl[TaskID];
#else
        if (TaskID < 8)
        {
            OSFastSwap[1] |= OSMapTbl[TaskID];
        }
        else
        {
            OSFastSwap[0] |= OSMapTbl[TaskID & 0x07];
        }
#endif
    
    if (TaskID < OSRunningTaskID())
    {
        i =  OSRunningTaskID() - TaskID;
        cp = (uint8 idata *)(&(OSTsakStackBotton[TaskID + 1]));
        
        do
        {
            *cp += SP_ADD_BYTE;
            cp++;
        } while (--i != 0);
        
        cp1 = (uint8 idata *)SP;
        SP = SP + SP_ADD_BYTE;
        i = SP - (uint8)(OSTsakStackBotton[TaskID + 1]);
        cp = (uint8 idata *)SP;
        do
        {
            *cp-- = *cp1--;
        } while (--i != 0);
    }
    else
    {
        cp1 = (uint8 idata *)(&(OSTsakStackBotton[OSRunningTaskID() + 1]));
        
        i = TaskID - OSRunningTaskID();
        
        do
        {
            *cp1 -= SP_ADD_BYTE;
            cp1++;
        } while (--i != 0);
        
        cp = OSTsakStackBotton[OSRunningTaskID() + 1];
        i = OSTsakStackBotton[TaskID] - cp - SP_ADD_BYTE;
        cp1 = cp - SP_ADD_BYTE;
        do
        {
            *cp1++ = *cp++;
        } while (--i != 0);
    }
    cp = OSTsakStackBotton[TaskID];
    *cp++ = (uint16)task % 256;
    *cp++ = (uint16)task / 256;
#if EN_REENTRANT > 0
    *cp++ = (uint16)ptos / 256;
    *cp++ = (uint16)ptos % 256;
#endif
    *cp = 0;
}
    */


/*********************************************************************************************************
** ��������: OSTaskStkDel
** ��������: �����ջɾ��
** �䡡��: ��
** �䡡�� : ��
** ȫ�ֱ���: OSTaskID,OSTsakStackBotton,SP
** ����ģ��: LoadCtx
** 
** ������: ������
** �ա���: 2003��8��17��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2004��4��22��
**-------------------------------------------------------------------------------------------------------
********************************************************************************************************/

        void StkDelA(uint8 TaskID)
{
    uint8 idata *cp;
    uint8 idata *cp1;
    uint8 temp, i;
    
    cp = OSTsakStackBotton[TaskID];
    cp1 = OSTsakStackBotton[TaskID + 1];
    temp = cp1 - cp;
    for (i = TaskID + 1; i <= OSRunningTaskID(); i++)
    {
        OSTsakStackBotton[i] -= temp;
    }
    i = SP - (uint8)(cp1) + 1;
    do
    {
        *cp++ = *cp1++;
    } while (--i != 0);
    SP = SP - temp;
}


        void StkDelB(uint8 TaskID)
{
    uint8 idata *cp;
    uint8 idata *cp1;
    uint8 temp, i;

    cp = OSTsakStackBotton[TaskID] - 1;
    cp1 = OSTsakStackBotton[TaskID + 1] - 1;
    i = OSTsakStackBotton[TaskID] - OSTsakStackBotton[OSRunningTaskID() + 1];
    if (i != 0)
    {
        do
        {
            *cp1-- = *cp--;
        } while (--i != 0);
    }
    temp = cp1 - cp;
    for (i = OSRunningTaskID() + 1; i <= TaskID; i++)
    {
        OSTsakStackBotton[i] += temp;
    }
}

        void StkDelC(uint8 TaskID)
{
    uint8 idata *cp;
    extern void C_OSCtxSw(void);
    
    
    cp = OSTsakStackBotton[TaskID];
    *cp++ = (uint16)C_OSCtxSw % 256;
    *cp = (uint16)C_OSCtxSw / 256;
    SP = (uint8) cp;
    OSFindNextRunningTask();
}

        void OSTaskStkDel(uint8 TaskID)  small
{
    uint8 temp;

    temp = 1;
    if (TaskID == OSRunningTaskID())
    {
        temp = 2;
    }
    else if (TaskID < OSRunningTaskID())
    {
        temp = 0;
    }
    switch (temp)
    {
        case 0:
            StkDelA(TaskID);
            break;
        case 1:
            StkDelB(TaskID);
            break;
        case 2:
            StkDelC(TaskID);
            break;
        default:
            break;
    }
}

/*********************************************************************************************************
** ��������: OSTickISR
** ��������: ϵͳʱ���жϷ�����
** �䡡��: ��
** �䡡�� : ��
** ȫ�ֱ���: ��
** ����ģ��: OS_IBT_ENTER,(UserTickTimer),OSTimeTick,OSIntExit
** 
** ������: ������
** �ա���: 2002��2��22��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2002��10��23��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**-------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#pragma disable                                        /* ����������ȼ��жϣ����򣬱��������һ�� */

void OSTickISR(void) interrupt OS_TIME_ISR
{
#if TICK_TIMER_SHARING > 1 
    static uint8 TickSum = 0;
#endif

#if EN_USER_TICK_TIMER > 0 
    UserTickTimer();                                    /* �û����� */
#endif

#if TICK_TIMER_SHARING > 1 
    TickSum = (TickSum + 1) % TICK_TIMER_SHARING;
    if (TickSum != 0)
    {
        return;
    }
#endif

    OS_INT_ENTER();                                     /* �жϿ�ʼ���� */

#if EN_TIMER_SHARING > 0
    OSTimeTick();                                       /* ����ϵͳʱ�Ӵ������� */
#else
    OSIntSendSignal(TIME_ISR_TASK_ID);                  /* ����IDΪTIME_ISR_TASK_ID������ */
#endif
    
    OSIntExit();                                        /* �жϽ������� */
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/