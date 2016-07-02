#include <stdio.h>
#include <stdlib.h>

#include "osal_api.h"

/* ***************************************************************************** */

UINT32 mut_0, mut_1, mut_2, mut_3, mut_4,mut_5, mut_6;
UINT32 mut_7, mut_8 ,mut_9, mut_10;


void InitializeMutIds(void)
{
    mut_0 = 99; mut_1 = 99; mut_2 = 99; mut_3 = 99; mut_4 = 99; mut_5 = 99;
    mut_6 = 99; mut_7 = 99; mut_8 = 99; mut_9 = 99; mut_10 = 99;
    return;
} /* end InitializeMutIds */


/* ************************************************************************************ */
int TestMutexes(void)
{
    int failMutCreatecount = 0;
    int failMutDeletecount = 0;
    int failMutGetIdcount = 0;
    int totalfailures = 0;
    int status;
    
    InitializeMutIds();

    status = OSAL_MutSemCreate( &mut_0,"Mut 0",0);
    printf("Status after creating Mut 0: %d,%d\n",status,mut_0);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_1,"Mut 1",0);
    printf("Status after creating Mut 1: %d,%d\n",status,mut_1);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_2,"Mut 2",0);
    printf("Status after creating Mut 2: %d,%d\n",status,mut_2); 
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_3,"Mut 3",0);
    printf("Status after creating Mut 3: %d,%d\n",status,mut_3);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_4,"Mut 4",0);
    printf("Status after creating Mut 4: %d,%d\n",status,mut_4);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_5,"Mut 5",0);
     printf("Status after creating Mut 5: %d,%d\n",status,mut_5);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_6,"Mut 6",0);
    printf("Status after creating Mut 6: %d,%d\n",status,mut_6); 
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_7,"Mut 7",0);
    printf("Status after creating Mut 7: %d,%d\n",status,mut_7);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_8,"Mut 8",0);
    printf("Status after creating Mut 8: %d,%d\n",status,mut_8);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_9,"Mut 9",0);
     printf("Status after creating Mut 9: %d,%d\n",status,mut_9);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_10,"Mut 10",0);
    printf("Status after creating Mut 10: %d,%d\n",status,mut_10);
    if (status == OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemDelete(mut_0);
     printf("Status after deleteing Mut 0:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_1);
    printf("Status after deleteing Mut 1:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_2);
     printf("Status after deleteing Mut 2:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_3);
    printf("Status after deleteing Mut 3:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_4);
    printf("Status after deleteing Mut 4:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_5);
    printf("Status after deleteing Mut 5:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_6);
    printf("Status after deleteing Mut 6:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_7);
    printf("Status after deleteing Mut 7:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_8);
    printf("Status after deleteing Mut 8:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_9);
    printf("Status after deleteing Mut 9:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_10);
    printf("Status after deleteing Mut 10:%d\n",status);
    if (status == OSAL_SUCCESS)
        failMutDeletecount++;
/*
* Now Create a few extra semaphores
*  to test  GetIdByName
*/
    InitializeMutIds();
    status = OSAL_MutSemCreate( &mut_0,"Mut 0",0);
    printf("Status after creating Mut 0: %d,%d\n",status,mut_0);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_1,"Mut 0",0);
    printf("Status after creating Mut 0 again: %d,%d\n",status,mut_1);
    if (status == OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_2,"Mut 2",0);
    printf("Status after creating Mut 2: %d,%d\n",status,mut_2);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemCreate( &mut_3,"Mut 3",0);
    printf("Status after creating Mut 3: %d,%d\n",status,mut_3);
    if (status != OSAL_SUCCESS)
        failMutCreatecount++;

    status = OSAL_MutSemGetIdByName(&mut_0,"Mut 0");
    if (status != OSAL_SUCCESS || mut_0 != 0)
        failMutGetIdcount++;
    
    status = OSAL_MutSemGetIdByName(&mut_1,"Mut 1");
    if (status == OSAL_SUCCESS)
        failMutGetIdcount++;
    
    status = OSAL_MutSemGetIdByName(&mut_2,"Mut 2");
    if (status != OSAL_SUCCESS || mut_2 != 1)
        failMutGetIdcount++;
    
    status = OSAL_MutSemGetIdByName(&mut_3,"Mut 3");
    if (status != OSAL_SUCCESS || mut_3 != 2)
        failMutGetIdcount++;
    
    status = OSAL_MutSemDelete(mut_0);
    printf("Status after deleteing Mut 0:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    /* this one was never created*/
    status = OSAL_MutSemDelete(mut_1);
    printf("Status after deleteing Mut 1:%d\n",status);
    if (status == OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_2);
    printf("Status after deleteing Mut 2:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;

    status = OSAL_MutSemDelete(mut_3);
    printf("Status after deleteing Mut 3:%d\n",status);
    if (status != OSAL_SUCCESS)
        failMutDeletecount++;
  
    if (failMutCreatecount != 0)
        printf("Mutex Create Failed Count %d\n", failMutCreatecount);

    if (failMutDeletecount != 0)
        printf("Mutex Delete Failed Count %d\n", failMutDeletecount);

    if (failMutGetIdcount != 0)
        printf("Mutex Get ID Failed Count %d\n", failMutGetIdcount);

    totalfailures = failMutCreatecount + failMutDeletecount + failMutGetIdcount;
    
    return totalfailures;

} /* end TestMutexes */


UINT32 bin_0, bin_1, bin_2, bin_3, bin_4,bin_5, bin_6;
UINT32 bin_7, bin_8, bin_9, bin_10;

void InitializeBinIds(void)
{
    bin_0 = 99; bin_1 = 99; bin_2 = 99; bin_3 = 99; bin_4 = 99; bin_5 = 99; 
    bin_6 = 99; bin_7 = 99; bin_8 = 99; bin_9 = 99; bin_10 = 99;
    return;
}


int TestSems(void)
{
    int failBinCreatecount = 0;
    int failBinDeletecount = 0;
    int failBinGetIdcount = 0;
    int totalfailures = 0;
    int status;
    InitializeBinIds();

    status = OSAL_SemaphoreCreate( &bin_0,"Bin 0",SEM_EMPTY);
     printf("Status after creating: %d,%d\n",status,bin_0);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_1,"Bin 1",SEM_EMPTY);
    printf("Status after creating: %d,%d\n",status,bin_1);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_2,"Bin 2",SEM_EMPTY);
     printf("Status after creating: %d,%d\n",status,bin_2);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_3,"Bin 3",SEM_EMPTY);
     printf("Status after creating: %d,%d\n",status,bin_3);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_4,"Bin 4",SEM_FULL);
      printf("Status after creating: %d,%d\n",status,bin_4);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_5,"Bin 5",SEM_FULL);
     printf("Status after creating: %d,%d\n",status,bin_5);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_6,"Bin 6",SEM_FULL);
     printf("Status after creating: %d,%d\n",status,bin_6);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_7,"Bin 7",SEM_FULL);
      printf("Status after creating: %d,%d\n",status,bin_7);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_8,"Bin 8",SEM_FULL);
     printf("Status after creating: %d,%d\n",status,bin_8);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_9,"Bin 9",SEM_FULL);
     printf("Status after creating: %d,%d\n",status,bin_9);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_10,"Bin 10",SEM_FULL);
     printf("Status after creating: %d,%d\n",status,bin_10);
    if (status == OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreDelete(bin_0);
     printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_1);
     printf("Status after deleteing:%d\n",status);
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_2);
      printf("Status after deleteing:%d\n",status);
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_3);
     printf("Status after deleteing:%d\n",status);
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_4);
     printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_5);
      printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_6);
      printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;
    status = OSAL_SemaphoreDelete(bin_7);
     printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_8);
     printf("Status after deleteing:%d\n",status);
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_9);
     printf("Status after deleteing:%d\n",status); 
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    status = OSAL_SemaphoreDelete(bin_10);
     printf("Status after deleteing:%d\n",status);
    if (status == OSAL_SUCCESS)
        failBinDeletecount++;

/*
* Now Create a few extra semaphores
*  to test  GetIdByName
*/

    InitializeBinIds();
printf("------------------\n");
    status = OSAL_SemaphoreCreate( &bin_0,"Bin 0",SEM_FULL);
    printf("Status after creating: %d,%d\n",status,bin_0); 
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_1,"Bin 0",SEM_FULL);
    printf("Status after creating: %d,%d\n",status,bin_1);
    if (status == OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_2,"Bin 2",SEM_EMPTY);
    printf("Status after creating: %d,%d\n",status,bin_2); 
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;

    status = OSAL_SemaphoreCreate( &bin_3,"Bin 3",SEM_FULL);
    printf("Status after creating: %d,%d\n",status,bin_3);
    if (status != OSAL_SUCCESS)
        failBinCreatecount++;



    
    status = OSAL_SemaphoreGetIdByName(&bin_0,"Bin 0");
     printf("Status after GETID: %d,%d\n",status,bin_0);
    if (status != OSAL_SUCCESS || bin_0 != 0)
        failBinGetIdcount++;
    
    status = OSAL_SemaphoreGetIdByName(&bin_1,"Bin 1");
     printf("Status after GETID: %d,%d\n",status,bin_1);

    if (status != OSAL_SUCCESS)
        failBinGetIdcount++;
    
    status = OSAL_SemaphoreGetIdByName(&bin_2,"Bin 2");
   printf("Status after GETID: %d,%d\n",status,bin_2); 

    if (status != OSAL_SUCCESS || bin_2 != 1)
        failBinGetIdcount++;
    
    status = OSAL_SemaphoreGetIdByName(&bin_3,"Bin 3");
     printf("Status after GETID: %d,%d\n",status,bin_3);
    if (status != OSAL_SUCCESS || bin_3 != 2)
        failBinGetIdcount++;
     
    status = OSAL_SemaphoreDelete(bin_0);
    printf("Status after deleteing:%d\n",status);
    if (status != OSAL_SUCCESS)
        failBinDeletecount++;

    /* this one was never created */
    status = OSAL_SemaphoreDelete(bin_1);
    printf("Status after deleteing:%d\n",status);
    if (status == OSAL_SUCCESS)
        failBinDeletecount++;

    if (failBinCreatecount != 0)
        printf("Bin Sem Create Failed Count %d\n", failBinCreatecount);

    if (failBinDeletecount != 0)
        printf("Bin Sem Delete Failed Count %d\n", failBinDeletecount);

    if (failBinGetIdcount != 0)
        printf("Bin Sem Get ID Failed Count %d\n", failBinGetIdcount);

    
    totalfailures = failBinCreatecount + failBinDeletecount + failBinGetIdcount;
    
    return totalfailures;

}/* end TestBinaries */


