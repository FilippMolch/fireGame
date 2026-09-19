//
// Created by FIlipp on 02.05.2024.
//

#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <functional>
#include <memory>
#include <cstdint>
#include <Event/CRC32.h>

/*
 * Еhe event system is borrowed from
 * https://github.com/denyskryvytskyi/ElvenEngine
*/

#define EVENT_TYPE(event_type)                  \
        static std::uint32_t GetStaticEventType()   \
        {                                           \
        return CRC32(event_type);               \
        }                                           \
        std::uint32_t GetEventType() const override \
        {                                           \
        return GetStaticEventType();            \
        }

class Event {
public:
    virtual ~Event() = default;
    virtual uint32_t GetEventType() const = 0;

    double dt;
};

class WindowCloseEvent : public Event {
public:
    EVENT_TYPE("WindowCloseEvent")

    WindowCloseEvent() = default;
};

class WindowResizeEvent : public Event {
public:
    EVENT_TYPE("WindowResizeEvent")

    WindowResizeEvent(int new_x, int new_y) : x(new_x), y(new_y) {}

    int x, y;
};

class CameraViewMatRecalcEvent : public Event {
public:
    EVENT_TYPE("CameraViewMatRecalcEvent")

    CameraViewMatRecalcEvent() = default;
};

class KeyboardKeyEvent : public Event {
public:
    EVENT_TYPE("KeyboardKeyEvent")

    KeyboardKeyEvent(int k, int a) : key(k), action(a) {};

    int key, action;
};
#endif //EVENT_H
