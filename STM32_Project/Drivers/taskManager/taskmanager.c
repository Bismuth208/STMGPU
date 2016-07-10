/* 
 * Each task execute on it timeout.
 * 
 * Povide time-shifted multitask.
 * 
 * It allows you to run undemanding tasks on time,
 * and left more CPU resources to difficult tasks,
 * which will has low influence to other tasks, 
 * if call timeout is low.
 * 
 * System uptime time couter requred.
 * You can change it in TIMER_FUNC define,
 * but it must return milliseconds!
 * 
 * Minimal time unit: 1 millis
 * Maximum tasks:     254 (0xFF reserved as NULL)
 * RAM per task:      9 bytes
 * Language:          C
 * 
 * Author: Antonov Alexandr (Bismuth208)
 * Date:   7 Dec, 2015 
 * e-mail: bismuth20883@gmail.com
 * 
 * 1 tab = 2 spaces
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

#include "taskmanager.h"

#if USE_GFX_LIB
#include <gfx.h>   // Core graphics LCD library
#endif
#include <systicktimer.h>


#if !USE_DYNAMIC_MEM
// Used only in static mem managment
static uint8_t maxTasks = 0;
#endif

static taskStates_t *pCurrentArrTasks = NULL;  // pointer to current tasks array

#ifdef USE_MEM_PANIC
const uint8_t textPanic[] PROGMEM = "Sorry...\nMemory panic :(\n\n";
const uint8_t textError[] PROGMEM = "Error code: ";
#endif
//===========================================================================//

// Main loop reincarnation.
void runTasks(void)
{
  uint32_t currentMillis;
  volatile uint8_t count;
  
#if USE_AUTO_DEFRAG
  uint32_t defragPreviousMs=0;
#endif
#if USE_AUTO_GEMINI
  uint32_t geminiPreviousMs=0;
#endif

  for(;;) {
#if USE_AUTO_DEFRAG
    if ((TIMER_FUNC - defragPreviousMs) > AUTO_DEFRAG_TIMEOUT) {
      defragPreviousMs = TIMER_FUNC;
      defragTasksMemory();
    }
#endif
#if USE_AUTO_GEMINI
    if ((TIMER_FUNC - geminiPreviousMs) > AUTO_GEMINI_TIMEOUT) {
      geminiPreviousMs = TIMER_FUNC;
      rmDuplicateTasks();
    }
#endif
    //if (count > PAC) count =0;
    
    for(count=0; count < PAC; count++) {
      // Have func and need execute?
      if(PAA[count].pTaskFunc && PAA[count].execute) {
        currentMillis = TIMER_FUNC;
        
        // check timeout
        if((currentMillis - PAA[count].previousMillis) >
                                       PAA[count].timeToRunTask) {
          //PAA[count].previousMillis = currentMillis;
          PAA[count].pTaskFunc(); // execute
          PAA[count].previousMillis = TIMER_FUNC;
        } 
      }
      //++count;
    //if (++count > PAC) count =0;
    }
  }
}

// 1 param - task function
// 2 param - time period, when run task
// 3 param - need execute or not (aka reserve mem for future)
void addTask(void (*task)(void), uint16_t timeToCheckTask, bool exec) 
{
  // Add task by reallocate memory
  // for dynamic struct array with pointers to funtions.
  // After, place task and timeout to new index in array.
    
#if USE_MEM_PANIC
  if (PAC < MAX_TASKS) { // less than 254 tasks
      PAC++;   // increase total tasks
  } else {
      panic(OVER_LIMIT_FAIL | ADD_FAIL);
  }
#else
  PAC++;   // increase total tasks
#endif

#if USE_DYNAMIC_MEM
  // reallocate block of RAM for new task
  PAA = (taskStatesArr_t*) realloc (PAA, PAC * sizeof(taskStatesArr_t));
#else
  if(PAC > maxTasks) {
      panic(OVER_RANGE_FAIL | ADD_FAIL);
  }
#endif

  if (PAA != NULL) {
    // aaand place params to new index
    // why -1? because we can`t allocate 0 bytes :)
    PAA[PAC-1].pTaskFunc = task;
    PAA[PAC-1].timeToRunTask = timeToCheckTask;
    PAA[PAC-1].previousMillis = 0;
    PAA[PAC-1].execute = exec;

  // So sad what this is C++ style :(
  /* PAA[PAC-1] = {task, timeToCheckTask, 0, exec}; */
    
#if USE_MEM_PANIC   
  } else {
    deleteAllTasks();
    panic(ALLOC_FAIL | ADD_FAIL);
  }
#else
  }
#endif    
}

// 1 param - pointer to array whith tasks
// 2 param - task function
// 3 param - time period, when run task
// 4 param - need execute or not (aka reserve mem for future)
void addTaskToArr(taskStates_t *pTasksArr, void (*task)(void),
                                  uint16_t timeToCheckTask, bool exec) 
{
  // Add task by reallocate memory
  // for dynamic struct array with pointers to funtions.
  // After, place task and timeout to new index in array.

#if USE_MEM_PANIC
  if (pTasksArr->tasksCount < MAX_TASKS) { // less than 254 tasks
      pTasksArr->tasksCount++;   // increase total tasks
  } else {
      panic(OVER_LIMIT_FAIL | ADD_TO_ARR_FAIL);
  }
#else
  pTasksArr->tasksCount++;   // increase total tasks
#endif

#if USE_DYNAMIC_MEM
  // reallocate block of RAM for new task
  pTasksArr->pArr = (taskStatesArr_t*) realloc (pTasksArr->pArr,
                                  pTasksArr->tasksCount * sizeof(taskStatesArr_t));
#else
  if(pTasksArr->tasksCount > maxTasks) {
    panic(OVER_RANGE_FAIL | ADD_TO_ARR_FAIL);
  }
#endif

  if (pTasksArr->pArr != NULL) {
    // aaand place params to new index
    // why -1? because we can`t allocate 0 bytes :)
    pTasksArr->pArr[pTasksArr->tasksCount-1].pTaskFunc = task;
    pTasksArr->pArr[pTasksArr->tasksCount-1].timeToRunTask = timeToCheckTask;
    pTasksArr->pArr[pTasksArr->tasksCount-1].previousMillis = 0;
    pTasksArr->pArr[pTasksArr->tasksCount-1].execute = exec;
    
    // So sad what this is C++ style :(
    /* pTasksArr->pArr[pTasksArr->tasksCount-1] = {task, timeToCheckTask, 0, exec}; */
#if USE_MEM_PANIC
  } else {
    deleteAllTasks();
    panic(ALLOC_FAIL | ADD_TO_ARR_FAIL);
  }
#else
  }
#endif
}

void deleteAllTasks(void)
{
#if USE_DYNAMIC_MEM
  free(PAA);
  PAA = NULL;
#else
  for(uint8_t count = 0; count < maxTasks; count++) {
    PAA[count].pTaskFunc = NULL;
  }
  //memset(PAA, 0x00, maxTasks * sizeof(tTaskStatesArr));
#endif
  PAC = 0;
}

void deleteTask(void (*task)(void))
{
  uint8_t taskId = searchTask(task);

  if(taskId < NULL_TASK) {
    PAA[taskId].pTaskFunc = NULL;    // remove pointer
    //PAA[taskId].execute = false;     // clear exec flag
    //defragTasksMemory();
  }
}

void disableTask(void (*task)(void))
{
  uint8_t taskId = searchTask(task);

  if(taskId < NULL_TASK) {
    PAA[taskId].execute = false;
  }
}

void updateTaskStatus(void (*task)(void), bool exec)
{
  uint8_t taskId = searchTask(task);

  if(taskId < NULL_TASK) {
    PAA[taskId].execute = exec;
  }

  // If we get here, when no such func in pCurrentArrTasks.
  // Need add task or return 0?
}

void updateTaskTimeCheck(void (*task)(void), uint16_t timeToCheckTask)
{
  uint8_t taskId = searchTask(task);

  if(taskId < NULL_TASK) {
    PAA[taskId].timeToRunTask = timeToCheckTask;
    PAA[taskId].previousMillis = TIMER_FUNC;
  }

  // If we get here, when no such func in pCurrentArrTasks.
  // Need add task or return 0?
}

void replaceTask(void (*oldTask)(void), void (*newTask)(void),
                    uint16_t timeToCheckTask, bool exec)
{
  // Also it combine deleteTask().
  // Just call like this: replaceTask(taskToDelete, NULL, 0, false);

  uint8_t taskId = searchTask(oldTask);

  if(taskId < NULL_TASK) {
    PAA[taskId].pTaskFunc = newTask;
    PAA[taskId].previousMillis = 0;
    PAA[taskId].timeToRunTask = timeToCheckTask;
    PAA[taskId].execute = exec;
  }

  // If we get here, when no such func or free holes left in pCurrentArrTasks.
  // Oh! Or maybe recursive call replaceTask(..) whith NULL?
  //addTask(newTask, timeToCheckTask, true);
}

void defragTasksMemory(void)
{
  // After some time of work may appear many holes
  // more than need. It eat much of memory, and that
  // is why need to remove them.

  bool defraged = false;
  uint8_t nullCount =0;
  uint8_t i=0, j=0;
  
  taskStatesArr_t tmpTask;
  
  // First: find all NULL pointers and move them to end of array
#if 1
  // bubble sort
  for(i = 0; i < (PAC); i++) {
    for(j = 0; j < (PAC - i - 1); j++) {
      if(PAA[j].pTaskFunc == NULL) {
        
        tmpTask = PAA[j];
        PAA[j] = PAA[j + 1];
        PAA[j + 1] = tmpTask;
        //memcpy(&tmpTask, &PAA[j], sizeof(tTaskStatesArr));
        //memcpy(&PAA[j], &PAA[j + 1], sizeof(tTaskStatesArr));
        //memcpy(&PAA[j + 1], &tmpTask, sizeof(tTaskStatesArr));
        defraged = true;
      }
    }
  }
#else
  // another type of sorting
#endif
  // Second: if some NULL was finded
  // when cut them off.
  if(defraged) {
    // search all NULL ponters from end of array
    for(i=PAC-1; i > 1; i--) {
      if(PAA[i].pTaskFunc != NULL) {
        break;
      } else {
        nullCount++; // count how much NULL`s need to cut off
      }
    }
    
    PAC -= nullCount; // Remove waste NULL`s
    
#if USE_DYNAMIC_MEM
    // free some RAM
    PAA = (taskStatesArr_t*)realloc(PAA, PAC * sizeof(taskStatesArr_t));
#if USE_MEM_PANIC
    if(PAA == NULL) {
      deleteAllTasks();
      panic(ALLOC_FAIL | DEFRAG_FAIL);
    }
#endif
#endif
  }
}

// still work unstable
void rmDuplicateTasks(void)
{
  // If by some reasons in task arr are two or more
  // same tasks - remove them
  
  uint8_t i, j;
  
  //printTasksMem(0);
  //defragTasksMemory();
  //printTasksMem(40);

  for (i=0; i<PAC; i++) { //
    for (j=1; j<(PAC-i); j++) {
      if((PAA[i].pTaskFunc > 0) && (PAA[i+j].pTaskFunc > 0)) {
        if(PAA[i].pTaskFunc == PAA[i+j].pTaskFunc) {
          
          PAA[i+j].pTaskFunc = NULL;
        }
      }
    }
  }
  
  //printTasksMem(80);
  //defragTasksMemory();
  //printTasksMem(120);
}

// Search task in current tasks array
// 1 param - task function
uint8_t searchTask(void (*task)(void))
{
  for(uint8_t count=0; count < PAC; count++) {
    if(PAA[count].pTaskFunc == task) {
      return count; // ok, this is it!
    }
  }

  return NULL_TASK;  // no such func
}

taskStates_t *setTaskArray(taskStates_t *pNewTasksArr)
{
  taskStates_t *oldTaskArr = pCurrentArrTasks; // make a copy

  pCurrentArrTasks = pNewTasksArr; // replace pointers

  return oldTaskArr; // previous pointer
}

uint16_t *getCurrentTaskArray(void)
{
  return (uint16_t *)pCurrentArrTasks;
}

#if !USE_DYNAMIC_MEM
// Use only when USE_DYNAMIC_MEM is 0
void setMaxTasks(uint8_t maximumTasks)
{
  maxTasks = maximumTasks;
}
#endif

// ------------------------------- helpfull -------------------------- //
void panic(uint8_t errorCode)
{
  char errBuf[3];
  
  itoa(errorCode, errBuf, 16);
  
#if USE_GFX_LIB
  setTextSize(3);
  setCursor(0, 0);
    
  tftFillScreen(COLOR_BLUE);
  tftPrintPGR(textPanic);
  tftPrintPGR(textError);
  
  print(errBuf);
#else
  // place here some another way to print error code
#endif
  
  while(1); // panic mode: on
}

uint8_t avalibleTasks(void)
{
  return PAC;
}

uint16_t avalibleRam(void) // space between the heap and the stack
{
#ifdef __AVR__
  // GCC unicorn magic...
  extern uint16_t __heap_start, *__brkval;
  uint16_t v;
  return (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval);
#else // another arch, don`t know how to check
  return 0;
#endif
}

#if USE_GFX_LIB
void printTasksMem(uint16_t offset)
{
  // for debug only!
  
  char buf[10];
  char adrBuf[5];
  
  setTextSize(1);
  tftFillRect(offset, 0, 36, 8*PAC, COLOR_BLUE);
  tftDrawFastVLine(offset+37, 0, 8*PAC, COLOR_WHITE);
  
  for (uint8_t i=0; i<PAC; i++) {
    memset(buf, 0x00, 10);
    memset(adrBuf, 0x00, 5);

    setCursor(offset, 8*i);
    
    strcat(buf, "0x");
    strcat(buf, itoa(PAA[i].pTaskFunc, adrBuf, 16));
    strcat(buf, "\n");
    
    print(buf);
  }
}
#endif
// ------------------------------------------------------------------- //
