//
//  environment.cpp
//  combine_core
//
//  Created by Matthew Fonken on 1/20/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "environment.hpp"

Environment::Environment( TestInterface * test, int rate ) : Environment(test, NULL, rate) {}

Environment::Environment( TestInterface * test, SerialWriter * sercom, int rate )
{
    LOG_ENV("Initializing Test Environment.\n");
    if (pthread_mutex_init(&lock, NULL) != 0) {LOG_ENV("\n mutex init failed\n");}
    if(events.add( &lock, test, sercom, rate ))
        test->init();
    status = INITIALIZED;
}

void Environment::addTest( TestInterface * test, int rate)
{
    if(events.add( &lock, test, NULL, rate ))
        test->init();
}

void Environment::addTest( TestInterface* test, SerialWriter* sercom, int rate)
{
    if(events.add( &lock, test, sercom, rate ))
        test->init();
}

Environment::~Environment()
{
    pthread_mutex_destroy(&lock);
}

void Environment::start()
{
    resume();
}

void Environment::pause()
{
    LOG_ENV("Pausing environment\n");
    pthread_mutex_unlock(&lock);
    status = PAUSED;
}

void Environment::resume()
{
    LOG_ENV("Resuming environment\n");
    pthread_mutex_lock(&lock);
    for( int i = 0; i < events.length(); i++ )
    {
        Event * e = events.get(i);
        pthread_create(&thread, NULL, &e->worker, (void*)e);
    }
    status = LIVE;
}

Event::Event( int id )
{
    this->id = id;
}

Event::Event( pthread_mutex_t * mutex, TestInterface * test, SerialWriter * sercom, int rate )
{
    this->id = 0;
    this->mutex = mutex;
    this->test = test;
    this->sercom = sercom;
    this->rate = rate;
}

void * Event::worker( void * data )
{
    Event e = *(Event*)data;
//    const char * n = e.test->name;
    if( e.mutex == NULL)
    {
        LOG_ENV("ALERT: Event %s has no mutex!\n", n);
        return NULL;
    }
    
    if( e.rate <= 0)
    {
        LOG_ENV("ALERT: Event %s has invalid rate.\n", n);
        return NULL;
    }
    int sl = 1000000/e.rate;
    
    struct timeval time;
    long curr_time, end_time;
    while( pthread_mutex_trylock(e.mutex) )
    {
        end_time = getTime(time) + sl;
    
        e.test->trigger();
        if(e.sercom != NULL)
            e.sercom->write(e.test->serialize());
        
        e.id++;
        
        while( (curr_time = getTime(time)) < end_time );
    }
    pthread_mutex_unlock(e.mutex);
    LOG_ENV("Event %s has triggered %d times\n", e.test->name, e.id);
    return NULL;
}

EventList::EventList()
{
    this->index = 0;
    
    for(int i = 0; i < MAX_EVENTS; i++)
    {
        list[i] = (Event *)malloc(sizeof(Event));
    }
}

int EventList::add( pthread_mutex_t* mutex, TestInterface* test, SerialWriter* sercom, int rate )
{
    if( !validIndex(this->index, MAX_EVENTS-1) ) return 0;
    Event * e = get(this->index++);
    if(e == NULL) return 0;
    e->id = this->index-1;
    e->mutex = mutex;
    e->test = test;
    e->sercom = sercom;
    e->rate = rate;
    
    return 1;
}

int EventList::validIndex( int i, int m )
{
    if( i >= m || i < 0 ) return 0;
    return 1;
}

Event * EventList::get(int i)
{
    if( !validIndex(i, index) ) return NULL;
    return list[i];
}
int EventList::remove(int i)
{
    if( !validIndex(i, index)  ) return 0;
    for(; i < index-1; i++)
        list[i] = list[i+1];
    index--;
    return 1;
}

int EventList::length()
{
    return index;
}
