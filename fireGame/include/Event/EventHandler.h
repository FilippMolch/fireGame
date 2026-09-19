//
// Created by FIlipp on 02.05.2024.
//

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <Event/Event.h>


template<typename EventType>
using EventHandler = std::function<void(const EventType& e)>;

class EventHandlerWrapperInterface {
public:

    virtual ~EventHandlerWrapperInterface() = default;

    void Ecex(const Event& e) { Call(e); }

    virtual std::string GetType() const = 0;

private:
    virtual void Call(const Event& e) = 0;
};

template<typename EventType>
class EventHandlerWrapper : public EventHandlerWrapperInterface {
public:
    explicit EventHandlerWrapper(const EventHandler<EventType>& handler) :
    m_handler(handler),
    handlerName(handler.target_type().name()) {};

private:
    EventHandler<EventType> m_handler;
    std::string handlerName;

    void Call(const Event &e) override {
        if (e.GetEventType() == EventType::GetStaticEventType())
            m_handler(static_cast<const EventType&>(e));
    }

    std::string GetType() const override { return handlerName; }
};

#endif //EVENTHANDLER_H
