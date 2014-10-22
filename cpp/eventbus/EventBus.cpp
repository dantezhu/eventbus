#include <algorithm>
#include <iterator>
#include "EventBus.h"

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
#pragma comment(lib,"pthreadVSE2.lib")
#endif

namespace eventbus {

    EventBus::EventBus() {
        pthread_mutex_init(&m_visit_mutex, NULL);
    }

    EventBus::~EventBus() {
        pthread_mutex_destroy(&m_visit_mutex);
        clearEvents();
        m_handlers.clear();
    }

    void EventBus::addHandler(IHandler* handler) {
        pthread_mutex_lock(&m_visit_mutex);

        m_handlers.insert(handler);

        pthread_mutex_unlock(&m_visit_mutex);
    }

    void EventBus::delHandler(IHandler *handler) {
        pthread_mutex_lock(&m_visit_mutex);

        m_handlers.erase(handler);

        pthread_mutex_unlock(&m_visit_mutex);
    }

    void EventBus::onEvent(BaseEvent* event) {
        pthread_mutex_lock(&m_visit_mutex);

        std::set<IHandler*> handlers=m_handlers;

        for(auto& handler: handlers) {
            if (m_handlers.find(handler) != m_handlers.end()) {
                handler->onEvent(event);
            }
        }

        pthread_mutex_unlock(&m_visit_mutex);
    }

    void EventBus::pushEvent(BaseEvent* event) {
        pthread_mutex_lock(&m_visit_mutex);

        m_events.push_back(event);

        pthread_mutex_unlock(&m_visit_mutex);
    }

    void EventBus::loopEvents() {
        pthread_mutex_lock(&m_visit_mutex);
        // 复制下来，防止访问冲突
        std::list<BaseEvent*> events = m_events;
        pthread_mutex_unlock(&m_visit_mutex);

        for (auto& event: events) {
            onEvent(event);
            event->_done = true;
        }

        pthread_mutex_lock(&m_visit_mutex);
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
        pthread_mutex_unlock(&m_visit_mutex);
    }

    void EventBus::clearEvents() {
        pthread_mutex_lock(&m_visit_mutex);

        for (auto& e: m_events) {
            delete e;
        }
        m_events.clear();

        pthread_mutex_unlock(&m_visit_mutex);
    }

}
