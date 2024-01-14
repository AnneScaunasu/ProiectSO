#include "doctor.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

bool StillPatients = true; 

void* doctorThreadFunction(void* arg) {
    printf("In doctorThreadFunction\n");
    Doctor* doctor = (Doctor*)arg;

    while(StillPatients){
        pthread_mutex_lock(&doctor->consultation_mutex);
        pthread_cond_wait(&doctor->consultation, &doctor->consultation_mutex);

        time_t consultationTime = rand() % 20;
        sleep(consultationTime); // Each patient can stay a maximum of 5 seconds in the doctors office
        pthread_mutex_unlock(&doctor->consultation_mutex);

        pthread_cond_signal(&doctor->consultation);
    }
    printf("Out of doctorThreadFunction\n");
}

Doctor* initializeDoctors(int numDoctors){
    Doctor* doctors = (Doctor*)malloc(numDoctors * sizeof(Doctor));
    
    pthread_t threadsHandles[1000];
    for (int i = 0; i < numDoctors; i++){
        pthread_mutex_init(&doctors[i].mutex, NULL);
        pthread_mutex_init(&doctors[i].consultation_mutex, NULL);
        pthread_cond_init(&doctors[i].consultation, NULL);
        doctors[i].id_doctor = i + 1;

        if (pthread_create(&doctors[i].thread, NULL, doctorThreadFunction, &doctors[i]) != 0){
            fprintf(stderr, "Error creating doctor thread\n");
            exit(EXIT_FAILURE);
        }
    }

    return doctors;
}

void destroyDoctor(Doctor* doctors, int numDoctors){
    // printf("In destructor of doctor\n");
    for (int i = 0; i < numDoctors; i++){
        pthread_mutex_destroy(&doctors[i].mutex);
    }

    for(int i = 0; i < numDoctors; i++){
        pthread_join(doctors[i].thread,NULL);
    }

    free(doctors);
}

void terminateDoctorThreads(Doctor* doctors, int numDoctors){
    StillPatients = 0;
    for(int i = 0; i < numDoctors; i++)
        pthread_cond_signal(&doctors[i].consultation);

}
