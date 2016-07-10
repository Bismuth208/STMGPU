#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_TASKS   0xFE
#define NULL_TASK   0xFF
#define SEC         *1000

//#define SHRINK_FACTOR 1.24733f

//------------------------ Usefull LIB Flags --------------------------------//
/*
 * If you are shure what:
 *   - you have enougth memory (bazillions bazillions bytes);
 *   - no memory leak;
 *   - you know what you do;
 *   - need a few more programm memory;
 *   - other unnown reasons.
 * When, set USE_MEM_PANIC to 0
 */
#define USE_MEM_PANIC 0

/*
 * If set this to 0, then:
 *   - memalloc;
 *   - realloc
 *   - free.
 * will be unavaliable!
 *
 * Only static memory managment!
 * Created for minimize ROM usage.
 * Use only when you know final number of tasks!
 * And tTaskStates.tTaskStatesArr must be init correctly!
 *
 * Like this:
 *  tTaskStates someName = {NULL, 0};
 *  tTaskStatesArr someNameArr[X];
 *  someName.pArr = &someNameArr;
 * where X is number of elements(Tasks);
 */
#define USE_DYNAMIC_MEM 0
  
/*
 * Set this one to 1 and it will periodically call
 * defragTasksMemory() func by default.
 * And it not depend on current task array!
 */
#define USE_AUTO_DEFRAG 0
// Set in seconds how often defragTasksMemory() will call
// NOTE: works only if USE_AUTO_DEFRAG is 1 !
#define AUTO_DEFRAG_TIMEOUT 10 SEC
  
/*
 * Set this one to 1 and it will periodically call
 * rmDuplicateTasks() func by default.
 * And it not depend on current task array!
 */
#define USE_AUTO_GEMINI 0
// Set in seconds how often rmDuplicateTasks() will call
// NOTE: works only if USE_AUTO_GEMINI is 1 !
#define AUTO_GEMINI_TIMEOUT 12 SEC
  
/*
 * Set this one to 0 and disable otuput errors on screen.
 * Remove depedencies from gfx lib.
 *
 * NOTE: place your specific way to print error codes in panic() func!
 */
#define USE_GFX_LIB 0
//---------------------------------------------------------------------------//

// PAA - pointer acess array
#define PAA pCurrentArrTasks->pArr
// PAC - pointer acess count
#define PAC pCurrentArrTasks->tasksCount

#if 1
#define TIMER_FUNC  uptime()
#else
#define TIMER_FUNC // place here your system uptime timer function
#endif
    
//----------- ERROR CODES --------------//
//  NAME:                  ERR CODE:    WHY IT`S HAPPEN:
#define OVER_LIMIT_FAIL         0x01    // more than: 0xFE
#define OVER_RANGE_FAIL         0x02    // more than: maxTasks
#define ALLOC_FAIL              0x03    // not enougth RAM
    
//                                      WHERE IT`S HAPPEN:
#define ADD_FAIL                0x10    // addTask()
#define ADD_TO_ARR_FAIL         0x20    // addTaskToArr()
#define DEFRAG_FAIL             0x30    // defragTasksMemory()
//--------------------------------------//
    
    
//------------------------ Tasks Structures ---------------------------------//
typedef struct {            // 9 bytes RAM(*)
  uint32_t previousMillis;	// when was previous func call
  void (*pTaskFunc)();      // on avr it get 2 bytes
  uint16_t timeToRunTask;   // 65,5 seconds will be enougth? (65535 millis / 1000)
  struct {
  	uint8_t execute 	:1;		// status flag; need exec or not
  	uint8_t freeRam 	:7;		// not implemented features
    //uint8_t priority 	:2;		// priority of task 0-3
  };
  // 7 or 5 bytes align here
  //whole size: avr = 9 bytes, arm = 11 bytes
} taskStatesArr_t;

typedef struct {            // (2(*) + tasksCount * taskStatesArr_t) + 1 bytes RAM
  taskStatesArr_t *pArr;     // pointer to array whith tasks
  uint8_t tasksCount;       // Note: 0xFF reserved as NULL !
  // 1 or 3 bytes align here
  // whole size: avr = 4 bytes, arm = 8 bytes.
} taskStates_t;
  
// * On AVR arch only.
//---------------------------------------------------------------------------//
  
//------------------------ Function Prototypes ------------------------------//
void addTaskToArr(taskStates_t *pTasksArr, void (*task)(void), uint16_t timeToCheckTask, bool exec);
void addTask(void (*task)(void), uint16_t timeToCheckTask, bool exec);
void replaceTask(void (*oldTask)(void), void (*newTask)(void), uint16_t timeToCheckTask, bool exec);
void updateTaskStatus(void (*oldTask)(void), bool exec);
void updateTaskTimeCheck(void (*task)(void), uint16_t timeToCheckTask);
void disableTask(void (*task)(void));
void deleteTask(void (*task)(void));
void deleteAllTasks(void);
void runTasks(void);

void rmDuplicateTasks();  // still not ready
void defragTasksMemory();
taskStates_t *setTaskArray(taskStates_t *pNewTasksArr);
uint16_t *getCurrentTaskArray();
void setMaxTasks(uint8_t maximumTasks);
uint8_t searchTask(void (*task)(void));
uint8_t avalibleTasks(void);
uint16_t avalibleRam(void);
void panic(uint8_t errorCode);
  
void printTasksMem(uint16_t offset);
//---------------------------------------------------------------------------//
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif
