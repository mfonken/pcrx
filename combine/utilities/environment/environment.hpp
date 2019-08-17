//
//  environment.hpp
//  combine_core
//
//  Created by Matthew Fonken on 1/20/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef environment_hpp
#define environment_hpp

#include <stdio.h>

#include <string>
#include <stdlib.h>
#include <stdint.h>

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "serial_wrapper.hpp"

#ifndef LOG
#define LOG(...) printf(__VA_ARGS__)
#endif

#define ENV_DEBUG

#ifdef ENV_DEBUG
#define LOG_ENV(...) LOG("<Environment> " __VA_ARGS__)
#else
#define LOG_ENV(...)
#endif

#define MAX_THREADS 3
#define MAX_FPS     60
#define OUT_FPS     60
#define OUT_UDL     1000000 / OUT_FPS

#define MAX_EVENTS 6

using namespace std;

static inline long getTime( struct timeval tv )
{
    gettimeofday(&tv, NULL);
    return long(tv.tv_usec + tv.tv_sec*1000000);
}

class TestInterface
{
protected:
    int id;
    const char * name;
public:
    virtual void    Init( void ) = 0;
    virtual void    Trigger( void ) = 0;
    virtual string  Serialize( void ) = 0;
    
    TestInterface(int id, const char * name ) : id(id), name(name) {}
    int             GetId( void ) { return id; }
    const char *    GetName( void ) { return name; }
};

typedef enum
{
    INITIALIZED = 0,
    LIVE,
    PAUSED,
    ERROR
} ENV_STATUS;

class Event
{
public:
    int id,
        rate;
    pthread_t       thread;
    pthread_mutex_t * mutex;
    SerialWriter    * sercom;
    TestInterface   * test;
    
    Event(int);
    Event( pthread_mutex_t*, TestInterface*, SerialWriter*, int );
    static void     * worker(void * data);
};

class EventList
{
private:
    int     index;
    Event * list[MAX_EVENTS];
public:
    
    EventList();
    int         Add( pthread_mutex_t*, TestInterface*, SerialWriter*, int );
    int         ValidIndex(int, int);
    Event *     Get(int);
    int         Remove(int);
    int         Length();
};

class Environment
{
private:
    pthread_mutex_t lock;
    pthread_cond_t  condition;
    
    void controlTick();
public:
    ENV_STATUS status;
    EventList events;
    pthread_t thread;
    
    Environment( TestInterface*, int );
    Environment( TestInterface*, SerialWriter*, int );
    ~Environment();
    
    void AddTest( TestInterface*, int );
    void AddTest( TestInterface*, SerialWriter*, int );
    
    void Start();
    void Pause();
    void Resume();
};

#endif /* environment_hpp */
