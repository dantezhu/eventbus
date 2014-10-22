#include <algorithm>
#include <iterator>
#include "EventBus.h"

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
#pragma comment(lib,"pthreadVSE2.lib")
#endif

namespace eventbus {

    EventBus::EventBus() {
        pthread_mutex_init(&m_visitMutex, NULL);
    }

    EventBus::~EventBus() {
        pthread_mutex_destroy(&m_visitMutex);
        clearEvents();
        m_handlers.clear();
    }

    void EventBus::addHandler(IHandler* handler) {
        pthread_mutex_lock(&m_visitMutex);

        m_handlers.insert(handler);

        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::delHandler(IHandler *handler) {
        pthread_mutex_lock(&m_visitMutex);

        m_handlers.erase(handler);

        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::onEvent(BaseEvent* event) {
        pthread_mutex_lock(&m_visitMutex);

        std::set<IHandler*> handlers=m_handlers;

        for(auto& handler: handlers) {
            if (m_handlers.find(handler) != m_handlers.end()) {
                handler->onEvent(event);
            }
        }

        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::pushEvent(BaseEvent* event) {
        pthread_mutex_lock(&m_visitMutex);

        m_events.push_back(event);

        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::loopEvents() {
        pthread_mutex_lock(&m_visitMutex);
        // 复制下来，防止访问冲突
        std::list<BaseEvent*> events = m_events;
        pthread_mutex_unlock(&m_visitMutex);

        for (auto& event: events) {
            onEvent(event);
            event->_done = true;
        }

        pthread_mutex_lock(&m_visitMutex);
        for(auto it = m_events.begin(); it != m_events.end();)
        {
            auto& event = (*it);
            if (event->_done)
            {
                it = m_events.erase(it);
                // event在用完了之后就要删掉
                delete event;
            }
            else
            {
                ++it;
            }
        }
        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::clearEvents() {
        pthread_mutex_lock(&m_visitMutex);

        for (auto& e: m_events) {
            delete e;
        }
        m_events.clear();

        pthread_mutex_unlock(&m_visitMutex);
    }

}
