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
void red();
void yellow();
void reset();

void cyan(){
    printf("\033[0;36m");
}
void red () {
  printf("\033[1;31m");
}
void yellow () {
  printf("\033[1;33m");
}
void reset () {
  printf("\033[0m");
}

void printSchedule(schedule *ps){
    yellow();

    if (isScheduleUnfinished(ps)){
        LLNode* currentNode = (LLNode*)malloc(sizeof(LLNode));

        if ( (!isEmpty(ps->foreQueue))){
            
            currentNode = ps->foreQueue->qFront;
            printf("\n---Stuff in the FOREGROUND ---\n");
            printf("%s...\n", currentNode->qt->name);
            
            while(!(currentNode->pNext == NULL)){
                currentNode = currentNode->pNext;
                printf("%s...\n", currentNode->qt->name);            }
        }
        
        if ( (!isEmpty(ps->backQueue))){
        
            currentNode = ps->backQueue->qFront;
            printf("---Stuff in the BACKGROUND ---\n");
            printf("%s\n", currentNode->qt->name);
            
            while(!(currentNode->pNext == NULL)){
                currentNode = currentNode->pNext;
                printf("%s\n", currentNode->qt->name);
            }        
        }
        
        printf("-------------------------\n\n");
        
        free(currentNode);
    }
    reset();

}

char* runNextProcessInSchedule( schedule *ps ) {
    /* TODO: complete this function.
    The function "runProcess", "promoteProcess", "loadProcessData", and "freeProcessData"
    in processSimulator.c will be useful in completing this.
    You may want to write a helper function to handle promotion */
     
    char *ret = NULL;
    int maxSteps;
    bool isComplete;

        
    /*
        while the forequeue (FOREGROUND) is NOT empty, then we need to
        run whatever processes are in it
    */
    while ( !isEmpty(ps->foreQueue) ){

        isComplete = false;
        
        // malloc some space for the process data and the process we are about to use        
        processData* pData = (processData*)malloc(sizeof(processData));
        process *removedProcess = (process *) malloc(sizeof(process));

        // get a pointer to the process data at the front of the queue, and pass it
        // to loadProcessData() to be loaded
        pData = ps->foreQueue->qFront->qt->data;
        loadProcessData(pData);

        // set the max number of steps a FOREGROUND process can run to 50, per the
        // instructions
        maxSteps = FOREGROUND_QUEUE_STEPS;
        
        /* 
            pass the process at the front of the forequeue (FOREGROUND) to the
            runProcess() function, as well as memory addresses to store 
            - any systemcall strings that might be started mid-process -> &ret
            - the number of steps run prior to finishing or a system call-> &maxSteps 

            since the runProcess() function returns a bool as to whether or not the
            process completed, we will store that into isComplete
        */

        isComplete = runProcess(ps->foreQueue->qFront->qt->name, &ret, &maxSteps);
        
        /*
            if the process completed, we need to remove it from the queue and free
            anything related to it

            if the process is NOT complete, we need to move it to the back of
            the forequeue (FOREGROUND)
        */
        if (isComplete){
            cyan();
            printf("%s is complete. Removing from forqueue.\n", ps->foreQueue->qFront->qt->name);
            reset();

            removedProcess = dequeue(ps->foreQueue); 
            freeProcessData();
            free(removedProcess);
        }else{
            cyan();
            printf("%s is not complete. Moving to back of forqueue.\n", ps->foreQueue->qFront->qt->name);
            reset();

            removedProcess = dequeue(ps->foreQueue);
            enqueue(ps->foreQueue,removedProcess); 
        }

        // printSchedule(ps);
    }// end of forequeue while loop

    
    while ( !isEmpty(ps->backQueue) && 
            isEmpty(ps->foreQueue) 
        ){

        isComplete = false;

        // processData* pData = (processData*)malloc(sizeof(processData));
        // process *removedProcess = (process *) malloc(sizeof(process));

        // pData = ps->backQueue->qFront->qt->data;
        loadProcessData(ps->backQueue->qFront->qt->data);

        int current_time = getCurrentTimeStep();
        int processSteps = ps->backQueue->qFront->qt->data->heap[1];
        int timeInQueue = current_time - ps->backQueue->qFront->qt->timeInserted;
        
        
        if (timeInQueue + processSteps >= STEPS_TO_PROMOTION )
            maxSteps = STEPS_TO_PROMOTION - timeInQueue;
            

        // maxSteps = processSteps;

        while (!isComplete){
            
            isComplete = runProcess(ps->backQueue->qFront->qt->name, &ret, &maxSteps);
            current_time = getCurrentTimeStep();
            timeInQueue = current_time - ps->backQueue->qFront->qt->timeInserted;

            if ( (!isComplete) && (timeInQueue >= STEPS_TO_PROMOTION)){
                cyan();
                printf("%s is not complete, but it's been in the backqueue too long. Must promote.\n", ps->backQueue->qFront->qt->name);
                reset();
                
                promoteProcess(ps->backQueue->qFront->qt->name, ps->backQueue->qFront->qt->data);
                enqueue(ps->foreQueue, dequeue(ps->backQueue));
                
                break;
            }

            if ( isComplete ){
                cyan();
                printf("%s is complete. Removing from backqueue.\n", ps->backQueue->qFront->qt->name);
                reset();

                
                freeProcessData();
                free(dequeue(ps->backQueue));
                
            }

        }
        //only free when complete
        
        // printSchedule(ps);

    }//while (!isEmpty(ps->backQueue))
    
        
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
