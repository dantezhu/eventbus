#ifndef __EVENTBUS_H_20141014190353__
#define __EVENTBUS_H_20141014190353__

#include <iostream>
#include <set>
#include <list>
#include <pthread.h>

namespace eventbus {
    class BaseEvent
    {
    public:
        BaseEvent() {
            what = 0;
        }
        // 如果不标明虚函数，进行继承后delete会有bug
        virtual ~BaseEvent() {}

    public:
        int what;
    };

    class IHandler
    {
    public:
        virtual ~IHandler(){}
        virtual void onEvent(BaseEvent* event)=0;
    };

    class EventBus
    {
    public:
        EventBus();
        virtual ~EventBus();

        void start();
        void stop();
        bool isRunning();

        void addHandler(IHandler* handler);
        void delHandler(IHandler* handler);
        void postEvent(BaseEvent* event);
        void clearEvents();
    private:
        void onEvent(BaseEvent* event);
        void loopEvents();
    private:
        std::list<BaseEvent*> m_events;
        std::set<IHandler*> m_handlers;

        pthread_mutex_t m_visitMutex;
    };

}

#endif /* end of include guard: __EVENTBUS_H_20141014190353__ */
