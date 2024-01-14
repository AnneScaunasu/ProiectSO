#include <pthread.h>
#include <stdio.h>

#ifndef DOCTORS_H_INCLUDED
#define DOCTORS_H_INCLUDED

// Doctor struct
typedef struct {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_mutex_t consultation_mutex;
    pthread_cond_t consultation;
    int id_doctor;
} Doctor;

Doctor* initializeDoctors(int numDoctors);
void destroyDoctor(Doctor* doctor, int numDoctors);
void terminateDoctorThreads(Doctor* doctors, int numDoctors);

#endif
