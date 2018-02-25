#pragma once

#include <weq/utility/NumberTypes.hpp>
#include <memory>

namespace weq{
class EventManager;
class EntityManager;
class SystemManager;

template<typename>
class Signal;

using EventSignal = Signal<void(const void*)>;
using EventSignalPtr = std::shared_ptr<EventSignal>;
using EventSignalWeakPtr = std::weak_ptr<EventSignal>;

using EntityId    = u64;
using ComponentId = u64;

}
