#include <algorithm>
#include <iterator>
#include "EventBus.h"
#include "cocos2d.h"

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

    void EventBus::start() {
        auto func = [this](float dt){
            loopEvents();
        };
        cocos2d::Director::getInstance()->getScheduler()->schedule(func, this, 0, false, __FILE__);
    }

    void EventBus::stop() {
        cocos2d::Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
    }

    bool EventBus::isRunning() {
        return cocos2d::Director::getInstance()->getScheduler()->isScheduled(__FILE__, this);
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

    void EventBus::postEvent(BaseEvent* event) {
        pthread_mutex_lock(&m_visitMutex);

        m_events.push_back(event);

        pthread_mutex_unlock(&m_visitMutex);
    }

    void EventBus::loopEvents() {
        pthread_mutex_lock(&m_visitMutex);
        // 防止访问冲突
        std::list<BaseEvent*> events;
        // 相当于直接剪切，速度快很多
        events.splice(events.end(), m_events);
        pthread_mutex_unlock(&m_visitMutex);

        for (auto& event: events) {
            onEvent(event);
            delete event;
        }
    }

    void EventBus::clearEvents() {
        pthread_mutex_lock(&m_visitMutex);

        for (auto& event: m_events) {
            delete event;
        }
        m_events.clear();

        pthread_mutex_unlock(&m_visitMutex);
    }

}
