//
// Created by simon on 28/03/2026.
//

#ifndef NITRONIC_EVENTBUS_HPP
#define NITRONIC_EVENTBUS_HPP

#include <entt/entt.hpp>

#include "core/Macros.hpp"

NAMESPACE {

    class EventBus
    {
    public:
        // Immediate - fires handlers... immediately.
        template<typename T>
        void dispatch(const T& event)
        {
            m_Dispatcher.trigger(event);
        }

        // Queued - fires on flush() calls.
        template<typename T>
        void enqueue(const T& event)
        {
            m_Dispatcher.enqueue(event);
        }

        // Execute queued events.
        void flush() const
        {
            m_Dispatcher.update();
        }

        // Subscribe with a free function or lambda
        template<typename T, auto Fn>
        void subscribe()
        {
            m_Dispatcher.sink<T>().template connect<Fn>();
        }

        // Subscribe with a member function
        template<typename T, auto Fn, typename Instance>
        void subscribe(Instance& instance)
        {
            m_Dispatcher.sink<T>().template connect<Fn>(instance);
        }

        // Unsubscribe with a free function or lambda
        template<typename T, auto Fn>
        void unsubscribe()
        {
            m_Dispatcher.sink<T>().template disconnect<Fn>();
        }

        // Unsubscribe with a member function
        template<typename T, auto Fn, typename Instance>
        void unsubscribe(Instance& instance)
        {
            m_Dispatcher.sink<T>().template disconnect<Fn>(instance);
        }

    private:
        entt::dispatcher m_Dispatcher;
    };

}

#endif //NITRONIC_EVENTBUS_HPP