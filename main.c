#include <stdio.h>
#include "doctor.h"
#include "schedular.h"

int main(int argc, char *argv[], char* envp[]) {
    Scheduler* scheduler;

    char* file;
    unsigned int numberDoctors;
    unsigned int patientsSpawnCoolDown, patientsSpawnTime;

    switch (sscanf(argv[1], "%ms %u %u %u", &file, &numberDoctors, &patientsSpawnTime, &patientsSpawnCoolDown))
    {
    case 1:
        file = "schedulerLog.txt";
        numberDoctors = 3;
        patientsSpawnTime = 60;
        patientsSpawnCoolDown = 10;
        break;
    
    case 2:
        numberDoctors = 3;
        patientsSpawnTime = 60;
        patientsSpawnCoolDown = 10;
        break;

    case 3:
        patientsSpawnTime = 60;
        patientsSpawnCoolDown = 10;
        break;

    case 4:
        patientsSpawnCoolDown = 10;
        break;

    default:
        break;
    }

    scheduler = createScheduler(numberDoctors, file);
    startSimulation(scheduler, patientsSpawnTime, patientsSpawnCoolDown);
    destroyScheduler(scheduler);
    
    return 0;
}
