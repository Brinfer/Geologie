/**
 * @file bookkeeper.c
 *
 * @brief  TODO
 *
 * @version 2.0
 * @date 12-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "bookkeeper.h"

#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

#include "../common.h"
#include "../tools.h"
#include "../Scanner/scanner.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MQ_LABEL "/MQ_Bookkeeper"

#define MQ_MAX_MESSAGES (2)

#define T_MAJ_LOAD (0.5)

#define MAX_ERROR_READING (2)

typedef enum {
    STOP = 0,
    GET_VALUE
} MqFlags;

typedef struct {
    MqFlags flag;
} MqMsg;

static float percentMemoryUsed;

static float percentProcessorUsed;

static pthread_t myThreadMq;

static pthread_t myThreadUpdate;

static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

static mqd_t myMq;

static bool keepGoing = false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calcule la charge processeur et met a jour #percentProcessorUsed.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t updateProcessorLoad(void);

/**
 * @brief Calcule la charge mémoire et mets a jour #percentMemoryUsed.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t updateMemoryLoad(void);

static int8_t setUpMq(void);

static int8_t tearDownMq(void);

static int8_t sendMsgMq(MqMsg* msg);

static int8_t readMsgMq(MqMsg* dest);

static float getPercentMemoryLoad(void);

static void setPercentMemoryLoad(float newValue);

static float getPercentProcessorLoad(void);

static void setPercentProcessorLoad(float newValue);

static void setKeepGoing(bool newValue);

static bool getKeepGoing(void);

static void* runMq(void* _);

static void* runUpdate(void* _);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Bookkeeper_new(void) {
    TRACE("Set up Bookkeeper%s", "\n");

    int8_t returnError = 0;

    returnError = setUpMq();

    if (returnError >= 0) {
        returnError = pthread_mutex_init(&myMutex, NULL);
        ERROR(returnError < 0, "[Bookkeeper] Error when initialising the mutex");
    }

    ERROR(returnError < 0, "[Bookkeeper] Error during the initialization");

    return returnError;
}

extern int8_t Bookkeeper_free(void) {
    TRACE("Tear down Bookkeeper%s", "\n");

    int8_t returnError = 0;

    returnError = tearDownMq();

    if (returnError >= 0) {
        returnError = pthread_mutex_destroy(&myMutex);
        ERROR(returnError < 0, "[Bookkeeper] Error when distroying the mutex");
    }

    ERROR(returnError < 0, "[Bookkeeper] Error during the destruction");

    return returnError;
}

extern int8_t Bookkeeper_askStartBookkeeper(void) {
    TRACE("Start Bookkeeper%s", "\n");

    int8_t returnError = 0;

    setKeepGoing(true);
    returnError = pthread_create(&myThreadMq, NULL, &runMq, NULL);

    if (returnError < 0) {
        ERROR(true, "[Bookkeeper] Error when creating the processus for the queue");
    } else {
        returnError = pthread_create(&myThreadUpdate, NULL, &runUpdate, NULL);
        ERROR(returnError < 0, "[Bookkeeper] Error when creating the processus for update the value");
    }

    ERROR(returnError < 0, "[Bookkeeper] Error when starting the processus");

    return returnError;
}

extern int8_t Bookkeeper_askStopBookkeeper(void) {
    TRACE("Stop Bookkeeper%s", "\n");

    int8_t returnError = 0;

    setKeepGoing(false);

    MqMsg msg = { .flag = STOP };
    returnError = sendMsgMq(&msg);
    if (returnError >= 0) {
        returnError = pthread_join(myThreadMq, NULL);

        if (returnError < 0) {
            ERROR(true, "[Bookkeeper] Error when closing the processus for the queue");
        } else {
            returnError = pthread_join(myThreadUpdate, NULL);
            ERROR(true, "[Bookkeeper] Error when closing the processus for update the value");
        }
    } else {
        ERROR(true, "[Bookkeeper] Error when sending the stop message");
    }

    ERROR(returnError < 0, "[Bookkeeper] Error when closing the processus");

    return returnError;
}

extern int8_t Bookkeeper_ask4CurrentProcessorAndMemoryLoad() {
    TRACE("Ask value to Bookkeeper%s", "\n");

    int8_t returnError = 0;

    MqMsg msg = { .flag = GET_VALUE };

    returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Bookkeeper] Error when sending the get value message");

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t updateMemoryLoad(void) {
    struct sysinfo memInfo;
    int8_t returnError = 0;

    returnError = sysinfo(&memInfo);

    if (returnError < 0) {
        ERROR(true, "[Bookkeeper] Error when reading the system information");
    } else {
        uint64_t virtualMemUsed = memInfo.totalram - memInfo.freeram;
        virtualMemUsed += memInfo.totalswap - memInfo.freeswap;

        setPercentMemoryLoad((virtualMemUsed * 100) / (memInfo.totalswap + memInfo.totalram));
    }

    return returnError;
}

static int8_t updateProcessorLoad(void) {
    static uint64_t previousTotalUser;
    static uint64_t previousTotalUserLow;
    static uint64_t previousTotalSys;
    static uint64_t previousTotalIdle;

    FILE* file = NULL;
    int8_t returnError = 0;
    uint64_t currentTotalUser;
    uint64_t currentTotalUserLow;
    uint64_t currentTotalSys;
    uint64_t currentTotalIdle;
    uint64_t currentTotal;

    file = fopen("/proc/stat", "r");
    if (file == NULL) {
        ERROR(true, "[Bookkeeper] Error when opening /proc/stat file");
    } else {
        returnError = fscanf(file, "cpu %lu %lu %lu %lu", &currentTotalUser, &currentTotalUserLow, &currentTotalSys, &currentTotalIdle);
        returnError = fclose(file);

        if (returnError < 0) {
            ERROR(true, "[Bookkeeper] Error when closing /proc/stat file");
        } else {
            if (currentTotalUser < previousTotalUser || currentTotalUserLow < previousTotalUserLow
                || currentTotalSys < previousTotalSys || currentTotalIdle < previousTotalIdle) {
                // NOTE can arrive at the start up when it's the first runMq
                ERROR(true, "[Bookkeeper] Overflown detection ... Skip the value and continue");
            } else {
                currentTotal = (currentTotalUser - previousTotalUser) + (currentTotalUserLow - previousTotalUserLow)
                    + (currentTotalSys - previousTotalSys);

                setPercentProcessorLoad((currentTotal / (currentTotal + (currentTotalIdle - previousTotalIdle))) * 100);
            }

            previousTotalUser = currentTotalUser;
            previousTotalUserLow = currentTotalUserLow;
            previousTotalSys = currentTotalSys;
            previousTotalIdle = currentTotalIdle;
        }
    }

    return returnError;
}

static float getPercentMemoryLoad(void) {
    bool returnValue;

    pthread_mutex_lock(&myMutex);
    returnValue = percentMemoryUsed;
    pthread_mutex_unlock(&myMutex);

    return returnValue;
}

static void setPercentMemoryLoad(float newValue) {
    pthread_mutex_lock(&myMutex);
    percentMemoryUsed = newValue;
    pthread_mutex_unlock(&myMutex);
}

static float getPercentProcessorLoad(void) {
    bool returnValue;

    pthread_mutex_lock(&myMutex);
    returnValue = percentProcessorUsed;
    pthread_mutex_unlock(&myMutex);

    return returnValue;

}

static void setPercentProcessorLoad(float newValue) {
    pthread_mutex_lock(&myMutex);
    percentProcessorUsed = newValue;
    pthread_mutex_unlock(&myMutex);
}

static int8_t setUpMq(void) {
    struct mq_attr attr;
    int8_t returnError;

    mq_unlink(MQ_LABEL);
    errno = 0;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MqMsg);
    attr.mq_curmsgs = 0;

    myMq = mq_open(MQ_LABEL, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

    returnError = myMq < 0 ? -1 : 0;
    ERROR(returnError < 0, "[Bookkeeper] Error when initializing the queue");

    return returnError;
}

static int8_t tearDownMq(void) {
    int returnError = 0;

    returnError = mq_close(myMq);

    if (returnError >= 0) {
        returnError = mq_unlink(MQ_LABEL);
        ERROR(true, "[Bookkeeper] Error when destroying the queue");
    } else {
        ERROR(true, "[Bookkeeper] Error when closing the queue");
    }
    return returnError;
}

static int8_t sendMsgMq(MqMsg* msg) {
    int8_t returnError = 0;

    returnError = mq_send(myMq, (char*) msg, sizeof(MqMsg), 0); // put char to avoid a warning
    ERROR(returnError < 0, "[Bookkeeper] Error when sending the message in the queue");

    return returnError;
}

static int8_t readMsgMq(MqMsg* dest) {
    int8_t returnError = 0;

    returnError = mq_receive(myMq, (char*) dest, sizeof(MqMsg), NULL); // put char to avoid a warning
    ERROR(returnError < 0, "[Bookkeeper] Error when reading the message in the queue");

    return returnError;
}

static void setKeepGoing(bool newValue) {
    pthread_mutex_lock(&myMutex);
    keepGoing = newValue;
    pthread_mutex_unlock(&myMutex);
}

static bool getKeepGoing(void) {
    bool returnValue;

    pthread_mutex_lock(&myMutex);
    returnValue = keepGoing;
    pthread_mutex_unlock(&myMutex);

    return returnValue;
}

static void* runMq(void* _) {
    int8_t returnError = 0;

    while (getKeepGoing()) {
        MqMsg msg;
        returnError = readMsgMq(&msg);
        if (returnError < 0) {
            LOG("[Bookkeeper] Can't read the message in the queue ... Re set up the queue.%s", "\n");
            returnError = setUpMq();

            if (returnError < 0) {
                LOG("[Bookkeeper] Can't re set-up the message queue ... Stop the processus.%s", "\n");
                setKeepGoing(false);
            } else {
                returnError = readMsgMq(&msg);
                if (returnError < 0) {
                    LOG("[Bookkeeper] Can't read the message in the queue ... Stop the processus.%s", "\n");
                    setKeepGoing(false);
                }
            }
        }

        if (returnError) {
            if (msg.flag == GET_VALUE) {
                ProcessorAndMemoryLoad procAndMem = {
                    .processorLoad = getPercentProcessorLoad(),
                    .memoryLoad = getPercentMemoryLoad()
                };
                Scanner_setCurrentProcessorAndMemoryLoad(&procAndMem);
            } else if (msg.flag == STOP) {
                setKeepGoing(false);
            } else {
                LOG("[Bookkeeper] Unknown message in the message queue, flag's value: %d ... Ignore it.%s", msg.flag, "\n");
            }
        }
    }
    return NULL;
}

static void* runUpdate(void* _) {
    int8_t alreadyFailMemory = 0;
    int8_t alreadyFailProcessor = 0;

    while (getKeepGoing()) {
        usleep(T_MAJ_LOAD * 1000 * 1000);

        if (alreadyFailMemory && updateMemoryLoad() < 0) {
            LOG("[Bookeeper] Error when reading the memory load%s", "\n");
            alreadyFailMemory++;

            if (alreadyFailMemory >= MAX_ERROR_READING) {
                LOG("[Bookeeper] To many fail to access to the memory load ... Stopping read this value%s", "\n");
                setPercentMemoryLoad(-1);
            }

        } else if (updateProcessorLoad() < 0) {
            LOG("[Bookeeper] Error when reading the memory load%s", " \n");
            alreadyFailProcessor++;

            if (alreadyFailProcessor >= MAX_ERROR_READING) {
                LOG("[Bookeeper] To many fail to access to the memory load ... Stopping read this value%s", "\n");
                setPercentProcessorLoad(-1);
            }

        } else if (alreadyFailMemory >= MAX_ERROR_READING && alreadyFailProcessor >= MAX_ERROR_READING){
            LOG("[Bookeeper] Error when reading the systement ressource ... Stopping the processus%s", "\n");
            break; // Not setKeepGoing to let the Mq processus to run
        }
    }

    return NULL;
}
