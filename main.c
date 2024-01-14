#include <stdio.h>
#include <time.h>
#include "doctor.h"
#include "scheduler.h"

int main(int argc, char *argv[], char* envp[]) {
    Scheduler* scheduler;

    char* file;
    int numberDoctors;
    int patientsSpawnCoolDown, patientsSpawnTime;
    time_t startTime = time(NULL);

    if(argv[1] != NULL){
        file = argv[1];
    } else {
        file = "schedulerLog.txt";
    }

    if(argv[2] != NULL) {
        sscanf(argv[2], "%u", &numberDoctors);
    } else {
        numberDoctors = 3;
    }

    if(argv[3] != NULL) {
        sscanf(argv[3], "%u", &patientsSpawnTime);
    } else {
        patientsSpawnTime = 60;
    }

    if(argv[4] != NULL) {
        sscanf(argv[4], "%u", &patientsSpawnCoolDown);
    } else {
        patientsSpawnCoolDown = 10;
    }

    scheduler = createScheduler(numberDoctors, file);
    startSimulation(scheduler, patientsSpawnTime, patientsSpawnCoolDown);
    destroyScheduler(scheduler);
    
    // printf("The proram run for %li\n",time(NULL) - startTime);
    
    return 0;
}
