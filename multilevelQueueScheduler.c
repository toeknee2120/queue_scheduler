#include <stdlib.h>

#include "multilevelQueueScheduler.h"

int min( int x, int y );

static const int STEPS_TO_PROMOTION = 50;
static const int FOREGROUND_QUEUE_STEPS = 5;

/* createSchedule
 * input: none
 * output: a schedule
 *
 * Creates and return a schedule struct.
 */
schedule* createSchedule( ) {
    /* TODO: initialize data in schedule */
    schedule *ps = (schedule *) malloc( sizeof(schedule));

    if( ps!=NULL )
    {
        ps->foreQueue = createQueue();
        ps->backQueue = createQueue();
    }

    /*****TODO**************************
    Remember to free this at some point!
    ************************************/
    return ps;
}

/* isScheduleUnfinished
 * input: a schedule
 * output: bool (true or false)
 *
 * Check if there are any processes still in the queues.
 * Return TRUE if there is.  Otherwise false.
 */
bool isScheduleUnfinished( schedule *ps ) {
    /* TODO: check if there are any process still in a queue.  Return TRUE if there is. */
    
    if ( isEmpty(ps->foreQueue) && isEmpty(ps->backQueue)){
        return false;
    }else{
        return true;
    }

    // return false; /* TODO: Replace with your return value */
}

/* addNewProcessToSchedule
 * input: a schedule, a string, a priority
 * output: void
 *
 * Create a new process with the provided name and priority.
 * Add that process to the appropriate queue
 */
void addNewProcessToSchedule( schedule *ps, char *processName, priority p ) {
    /* TODO: complete this function.
    The function "initializeProcessData" in processSimulator.c will be useful in completing this. */

    processData *p_intialData = initializeProcessData(processName);
    
    //Create process struct object
    process *newProcess = (process *) malloc(sizeof(process));

    newProcess->name = processName;
    newProcess->priority = p;
    newProcess->data = p_intialData;
    newProcess->timeInserted = getCurrentTimeStep();

    //add process to foreground queue in schedule or
    //the background queue in schedule
    if (newProcess->priority == FOREGROUND){
        enqueue(ps->foreQueue, newProcess);
    }else if (newProcess->priority == BACKGROUND){
        enqueue(ps->backQueue, newProcess);
    }
        
    // free( processName ); /* TODO: This is to prevent a memory leak but you should remove it once you create a process to put processName into */
}

/* runNextProcessInSchedule
 * input: a schedule
 * output: a string
 *
 * Use the schedule to determine the next process to run and for how many time steps.
 * Call "runProcess" to attempt to run the process.  You do not need to print anything.
 * You should return the string "runProcess" returns.  You do not need to use/modify this string in any way.
 */

void printSchedule(schedule *ps){
    if (isScheduleUnfinished(ps)){
        if ( (!isEmpty(ps->foreQueue))){
            
            LLNode* currentNode = (LLNode*)malloc(sizeof(LLNode));
            currentNode = ps->foreQueue->qFront;
            printf("\n---Stuff in the FOREGROUND ---\n");
            printf("%s...%d\n", currentNode->qt->name, currentNode->qt->data->heap[11]);
            
            while(!currentNode->pNext == NULL){
                currentNode = currentNode->pNext;
                printf("%s...%d\n", currentNode->qt->name, currentNode->qt->data->heap[11]);
            }
        }
        
        if ( (!isEmpty(ps->backQueue))){
            LLNode* currentNode = (LLNode*)malloc(sizeof(LLNode));
            currentNode = ps->backQueue->qFront;
            printf("---Stuff in the BACKGROUND ---\n");
            printf("%s\n", currentNode->qt->name);
            
            while(!currentNode->pNext == NULL){
                currentNode = currentNode->pNext;
                printf("%s\n", currentNode->qt->name);
            }        
        }
        
        printf("-------------------------\n\n");
    }
}

char* runNextProcessInSchedule( schedule *ps ) {
    /* TODO: complete this function.
    The function "runProcess", "promoteProcess", "loadProcessData", and "freeProcessData"
    in processSimulator.c will be useful in completing this.
    You may want to write a helper function to handle promotion */
    //int numSteps = 0;
    
    /* TODO: Delete the following printf once you get the infinite loop fixed */
    // printf("IMPORTANT NOTE: There will be an intinite loop in runNextProcessInSchedule if you get isScheduleUnfinished and addNewProcessToSchedule working correctly\n"); 
    /* TODO: Uncomment the code below to dequeue elements from the two Queues and break your code out of the infinite loop
    if( !isEmpty(ps->foreQueue) )
        dequeue(ps->foreQueue);
    else if( !isEmpty(ps->backQueue) )
        dequeue(ps->backQueue);
    */
    
    /* your call to runProcess will look something like this: */
    // bool b = runProcess( /* name of process */, &ret, &numSteps );
    

  
    char *ret = NULL;
    int stepsCompleted;
    char **ppSystemCall;
    int *pNumSteps;

    ppSystemCall = &ret;
    pNumSteps = &stepsCompleted;

    while (isScheduleUnfinished){
        
    
       while ( !isEmpty(ps->foreQueue) ){
            bool complete;
            processData* pData = (processData*)malloc(sizeof(processData));
            process *removedProcess = (process *) malloc(sizeof(process));
    
            pData = ps->foreQueue->qFront->qt->data;
            loadProcessData(pData);
    
            stepsCompleted = 5 /*pData->heap[1]*/;
           
            complete = runProcess(ps->foreQueue->qFront->qt->name, ppSystemCall, pNumSteps);
            
            if (complete){
                //remove process
                removedProcess = dequeue(ps->foreQueue);
                freeProcessData();
                free(removedProcess);
            }else{
                //move to back of queue
                removedProcess = dequeue(ps->foreQueue);
                enqueue(ps->foreQueue,removedProcess); 
            }
    
           printSchedule(ps);
    
        }

        
        while ( !isEmpty(ps->backQueue) && 
                isEmpty(ps->foreQueue) 
            ){
            bool complete;
            processData* pData = (processData*)malloc(sizeof(processData));
            process *removedProcess = (process *) malloc(sizeof(process));
    
            pData = ps->backQueue->qFront->qt->data;
            loadProcessData(pData);

            //////////////////////
            int current_time = getCurrentTimeStep();
            int processSteps = pData->heap[1];
            int timeInQueue = current_time - ps->backQueue->qFront->qt->timeInserted;
            
            
            if (timeInQueue + processSteps >= STEPS_TO_PROMOTION )
                processSteps = STEPS_TO_PROMOTION - timeInQueue;
                

            stepsCompleted = processSteps;
    
            complete = false;
            while (!complete){
                complete = runProcess(ps->backQueue->qFront->qt->name, ppSystemCall, pNumSteps);
                current_time = getCurrentTimeStep();
                timeInQueue = current_time - ps->backQueue->qFront->qt->timeInserted;

                if ( (!complete) && (timeInQueue >=50)){
                    promoteProcess(ps->backQueue->qFront->qt->name, pData);
                    removedProcess = dequeue(ps->backQueue);
                    enqueue(ps->foreQueue, removedProcess);
                    break;
                }
                freeProcessData();
                free(removedProcess);
            }
            //only free when complete
            
            printSchedule(ps);
    
        }//while (!isEmpty(ps->backQueue))
        
    }//while (isScheduleUnfinished)





    freeSchedule(ps);

    return ret; /* TODO: be sure to store the value returned by runProcess in ret */
}

/* freeSchedule
 * input: a schedule
 * output: none
 *
 * Free all of the memory associated with the schedule.
 */
void freeSchedule( schedule *ps ) {
    /* TODO: free any data associated with the schedule as well as the schedule itself.
    the function "freeQueue" in queue.c will be useful in completing this. */

    //free LL stuff
    free(ps->foreQueue->qFront);
    free(ps->foreQueue->qRear);
    free(ps->backQueue->qFront);
    free(ps->backQueue->qRear);

    //free Queue
    freeQueue(ps->foreQueue);
    freeQueue(ps->backQueue);

    //free Schedule
    free(ps);
}

int min( int x, int y ){
    if( x<y )
        return x;
    return y;
}
