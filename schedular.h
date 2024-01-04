#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "doctor.h"

#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

typedef struct {
    Doctor* doctors;
    FILE* logFile;
    int numberDoctors;
} Scheduler;

Scheduler* createScheduler(int numDoctors, char* file);
void destroyScheduler(Scheduler* scheduler);
void startSimulation(Scheduler* scheduler, unsigned int patientsSpawnTime, unsigned int patientsSpawnCoolDown);
void* patientThreadFunction(void* arg);

#endif
