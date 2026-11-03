#pragma once

#include "RE/B/BSTEvent.h"

#include <filesystem>

namespace RE
{
	class UIAdvanceMenusFunctionCompleteEvent
	{
	private:
		using EventSource_t = BSTGlobalEvent::EventSource<UIAdvanceMenusFunctionCompleteEvent>;

	public:
		[[nodiscard]] static EventSource_t* GetEventSource()
		{
			static REL::Relocation<EventSource_t**> singleton{ ID::UIAdvanceMenusFunctionCompleteEvent::GetEventSource };
			if (REL::IDDB::IsNG()) {
				static bool logged = false;
				if (!logged) {
					logged = true;
					REX::INFO("NGDIAG: UIAdvance event source slot={:#x} value={:#x}", reinterpret_cast<std::uintptr_t>(singleton.get()), reinterpret_cast<std::uintptr_t>(*singleton));
				}
				if (std::filesystem::exists("Data\\F4SE\\Plugins\\BakaFullscreenPipboy.nosink")) {
					return nullptr;
				}
			}
			if (!*singleton) {
				if (REL::IDDB::IsNG()) {
					return nullptr;
				}
				*singleton = new EventSource_t(&BSTGlobalEvent::GetSingleton()->eventSourceSDMKiller);
			}
			return *singleton;
		}
	};
	static_assert(std::is_empty_v<UIAdvanceMenusFunctionCompleteEvent>);
}
