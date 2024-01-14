#include "scheduler.h"
#include "doctor.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

int threadsStarted = 0, threadsEnded = 0;

Scheduler* createScheduler(int numDoctors, char* file){
    Scheduler* scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    scheduler->numberDoctors = numDoctors;
    scheduler->logFile = fopen(file,"w");
    scheduler->doctors = initializeDoctors(numDoctors);
    pthread_mutex_init(&scheduler->mutexFile, NULL);
    pthread_cond_init(&scheduler->finishedWorkDay, NULL);
    scheduler->nextDoctor = 0;
    return scheduler;
}

void destroyScheduler(Scheduler* scheduler){
    fclose(scheduler->logFile);
    destroyDoctor(scheduler->doctors, scheduler->numberDoctors);
    free(scheduler);
}

Doctor* findAvailableDoctor(Doctor* doctors, int numDoctors, int *nextDoctor, FILE* file){
    // printf("In findAvailableDoctor\n");
    // printf("Number of doctors %i\n",numDoctors);
    for (int i = *nextDoctor; i < numDoctors; i++){
        // printf("In findAvailableDoctor for with i=%i\n", i);
        if(pthread_mutex_trylock(&doctors[i].mutex) == 0){
            fprintf(file, "Doctorul %i este in consultatie\n", i + 1);
            if(i == numDoctors - 1)
                *nextDoctor = 0;
            else
                *nextDoctor = i + 1;
            // printf("The next doctor is %i\n", *nextDoctor);
            return &doctors[i];
        }
    }
    nextDoctor = 0;
    // printf("Out of findAvailableDoctor\n");

    return NULL;
}

void writeLog(Scheduler* scheduler, pthread_t patient_id, time_t waitTime, time_t consultationTime){
    // printf("In writeLog\n");
    bool doneWriting = false;
    while(!doneWriting)
        if(pthread_mutex_trylock(&scheduler->mutexFile)){
            fprintf(scheduler->logFile, "Pacientul %lu a asteptat %u secunde dupa ce a ajuns ca sa intre la un medic si a stat %lu secunde in consultatie.\n", patient_id, (unsigned int)waitTime, consultationTime);
            doneWriting = true;
            pthread_mutex_unlock(&scheduler->mutexFile);
        }
    // printf("Out of writeLog\n");
}

void* patientThreadFunction(void* arg){
    // printf("In patientTHreadFunction\n");
    Scheduler* params = (Scheduler*)arg;

    srand((unsigned int)time(NULL));

    time_t startTime = time(NULL);
    time_t waitTime;
    
    bool finishedConsultation = false;
    while(!finishedConsultation){
        Doctor* doctor = findAvailableDoctor(params->doctors, (params->numberDoctors), &params->nextDoctor, params->logFile);
        if(doctor != NULL){
            waitTime = time(NULL) - startTime;  
            // printf("1 Next doctor in patientThreadFunction is %i\n", params->nextDoctor);
            pthread_cond_signal(&doctor->consultation);
            time_t consultationTime = time(NULL);
            pthread_cond_wait(&doctor->consultation, &doctor->mutex);
            // printf("2 Next doctor in patientThreadFunction is %i\n", params->nextDoctor);
            pthread_mutex_unlock(&doctor->mutex);
            finishedConsultation = true;
            writeLog(params, pthread_self(), waitTime, time(NULL) - consultationTime);
            // printf("3 Next doctor in patientThreadFunction is %i\n", params->nextDoctor);
        }
    }
    // printf("Out of the patientThreadFunction\n");
    threadsEnded++;
    return NULL;
}

void startSimulation(Scheduler* scheduler, unsigned int patientsSpawnTime, unsigned int patientsSpawnCoolDown){
    // printf("In startSsimulation\n");
    srand((unsigned int)time(NULL)); // Seed for random number generation
    time_t startTime = time(NULL);
    time_t currentTime;

    pthread_t threadsHandles[1000];
    int counterThreads = 0;
    while((currentTime = time(NULL)) - startTime < patientsSpawnTime) {
        int sleepTime = rand() % patientsSpawnCoolDown; // Sleep for a random duration (0 to 14 seconds)
        sleep(sleepTime);

        pthread_t patient;
        threadsStarted++;
        if (pthread_create(&patient, NULL, patientThreadFunction, scheduler) != 0){
            fprintf(stderr, "Error creating thread\n");
            return;
        }

        // pthread_detach(patient); // Patient is done with consultation and logging info
        threadsHandles[counterThreads++] = patient;
    }

    for(int i = 0; i < counterThreads; i++){
        pthread_join(threadsHandles[i],NULL);
    }

    terminateDoctorThreads(scheduler->doctors, scheduler->numberDoctors);
    // printf("There were %i threads started\nThere were %i thread ended\n", threadsStarted, threadsEnded);
    // pthread_mutex_trylock(&scheduler->mutexFile);
    // programFinished(&scheduler->finishedWorkDay);
    // pthread_cond_wait(&scheduler->finishedWorkDay, &scheduler->mutexFile);
    // printf("Out of the startSimulation\n");
}
