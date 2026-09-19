//
// Created by Filipp on 02.05.2024.
//

#ifndef EVENTSYSTEM_EVENTMANAGER_H
#define EVENTSYSTEM_EVENTMANAGER_H

#include <Event/Event.h>
#include <Event/EventHandler.h>

class HEngine;

class EventManager {
public:
    EventManager() = default;
    EventManager(const EventManager&) = delete;
    const EventManager& operator=(const EventManager&) = delete;

    void shutdown() {
        subscribers.clear();
    }

    void subscribe(const uint32_t eventID, std::unique_ptr<EventHandlerWrapperInterface>&& handler) {
        auto subs = subscribers.find(eventID);

        if (subs != subscribers.end()) {
            auto& handlers = subs->second;
            //for (auto& han : handlers) {
                //if (han->GetType() == handler->GetType())
                //    return;
            //}
            handlers.emplace_back(std::move(handler));
        } else {
            subscribers[eventID].emplace_back(std::move(handler));
        }
    }

    void unsubscribe(const uint32_t eventID, const std::string handlerName) {
        auto subs = subscribers.find(eventID);

        if (subs != subscribers.end()) {
            auto& handlers = subs->second;

            for (auto it = handlers.begin(); it != handlers.end(); ++it) {
                if (it->get()->GetType() == handlerName) {
                    it = handlers.erase(it);
                    return;
                }
            }

        }
    }

    void triggerEvent(const Event& e) {
        for (auto& handlers : subscribers[e.GetEventType()]) {
            handlers->Ecex(e);
        }
    }

    void queueEvent(std::unique_ptr<Event>&& e) {
        eventsQueue.emplace_back(std::move(e));
    }

    void dispatchEvents() {
        for (auto& ev : eventsQueue)
            triggerEvent(*ev);

        eventsQueue.clear();
    }

private:
    std::vector<std::unique_ptr<Event>> eventsQueue;
    std::unordered_map<uint32_t, std::vector<std::unique_ptr<EventHandlerWrapperInterface>>> subscribers;
};

extern std::unique_ptr<EventManager> mainEventManager;
namespace HEevent {

    template<typename EventType>
    void Subscribe(const EventHandler<EventType>& callback) {
        uint32_t eventID = EventType::GetStaticEventType();

        std::unique_ptr<EventHandlerWrapperInterface> handler =
            std::make_unique<EventHandlerWrapper<EventType>>(callback);

        mainEventManager->subscribe(eventID, std::move(handler));
    }

    template<typename EventType>
    void Unsubscribe(const EventHandler<EventType>& callback) {
        uint32_t eventID = EventType::GetStaticEventType();
        std::string handlerName = callback.target_type().name();

        mainEventManager->unsubscribe(eventID, handlerName);
    }

    inline void TriggerEvent(const Event& e){
        mainEventManager->triggerEvent(e);
    }

    inline void QueueEvent(std::unique_ptr<Event>&& e) {
        mainEventManager->queueEvent(std::move(e));
    }

}


#endif //EVENTSYSTEM_EVENTMANAGER_H
