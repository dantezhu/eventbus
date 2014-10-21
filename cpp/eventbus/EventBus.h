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
            _done = false;
        }
        // 如果不标明虚函数，进行继承后delete会有bug
        virtual ~BaseEvent() {}

    public:
        int what;

        // 是否完成，外界不要调用
        bool _done;
    };

    class IHandler
    {
    public:
        virtual ~IHandler(){}
        virtual void onEvent(BaseEvent* e)=0;
    };

    class EventBus
    {
    public:
        EventBus();
        virtual ~EventBus();
        void addHandler(IHandler* handler);
        void delHandler(IHandler* handler);
        void pushEvent(BaseEvent* e);
        void loopEvents();
    private:
        void onEvent(BaseEvent* e);
        void clearEvents();
    private:
        std::list<BaseEvent*> m_events;
        std::set<IHandler*> m_handlers;

        pthread_mutex_t m_visit_mutex;
    };

}

#endif /* end of include guard: __EVENTBUS_H_20141014190353__ */
