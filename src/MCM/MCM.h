#pragma once

namespace MCM
{
	class Settings
	{
	public:
		class Pipboy
		{
		public:
			inline static REX::INI::Bool bEnable{ "Pipboy", "bEnable", true };
			inline static REX::INI::Bool bQuickBoyKey{ "Pipboy", "bQuickBoyKey", false };
			inline static REX::INI::Bool bDisableFX{ "Pipboy", "bDisableFX", true };
			inline static REX::INI::Bool bDisableFXBoth{ "Pipboy", "bDisableFXBoth", true };
			inline static REX::INI::Bool bUseColor{ "Pipboy", "bUseColor", true };
			inline static REX::INI::Bool bUseColorPA{ "Pipboy", "bUseColorPA", true };
			inline static REX::INI::Bool bBackground{ "Pipboy", "bBackground", false };
			inline static REX::INI::Bool bBackgroundBlur{ "Pipboy", "bBackgroundBlur", true };
			inline static REX::INI::Bool bBackgroundSmall{ "Pipboy", "bBackgroundSmall", false };
			inline static REX::INI::Bool bKeepLowHealthIMod{ "Pipboy", "bKeepLowHealthIMod", false };
			inline static REX::INI::Bool bPowerArmorOnly{ "Pipboy", "bPowerArmorOnly", false };

			inline static REX::INI::F32 fBackgroundAlpha{ "Pipboy", "fBackgroundAlpha", 1.0f };
			inline static REX::INI::F32 fPipboyMenuX{ "Pipboy", "fPipboyMenuX", 35.0f };
			inline static REX::INI::F32 fPipboyMenuY{ "Pipboy", "fPipboyMenuY", 0.0f };
			inline static REX::INI::F32 fPipboyMenuScaleX{ "Pipboy", "fPipboyMenuScaleX", 0.92f };
			inline static REX::INI::F32 fPipboyMenuScaleY{ "Pipboy", "fPipboyMenuScaleY", 0.0f };
			inline static REX::INI::F32 fPipboyBackgroundX{ "Pipboy", "fPipboyBackgroundX", 42.0f };
			inline static REX::INI::F32 fPipboyBackgroundY{ "Pipboy", "fPipboyBackgroundY", 0.0f };
			inline static REX::INI::F32 fPipboyBackgroundScale{ "Pipboy", "fPipboyBackgroundScale", 0.90f };
			inline static REX::INI::F32 fTerminalMenuX{ "Pipboy", "fTerminalMenuX", 135.0f };
			inline static REX::INI::F32 fTerminalMenuY{ "Pipboy", "fTerminalMenuY", 0.0f };
			inline static REX::INI::F32 fTerminalMenuScale{ "Pipboy", "fTerminalMenuScale", 0.92f };

			inline static REX::INI::F32 fPipboy3DItemScale{ "Pipboy", "fPipboy3DItemScale", 0.6f };
			inline static REX::INI::F32 fPipboy3DItemScreenPosX{ "Pipboy", "fPipboy3DItemScreenPosX", 0.68f };
			inline static REX::INI::F32 fPipboy3DItemScreenPosY{ "Pipboy", "fPipboy3DItemScreenPosY", 0.28f };
		};

		class Input
		{
		public:
			inline static REX::INI::I32 iQuickBoyKey{ "Input", "iQuickBoyKey", 0 };
		};

		class Runtime
		{
		public:
			inline static bool         bQuickBoy{ true };
			inline static bool         bQuickBoyTransition{ false };
			inline static bool         bPlayClose{ false };
			inline static bool         bWeaponDrawn{ false };
			inline static std::int32_t QuickBoyKey{ 0 };
		};

		static void Update()
		{
			ResetStateInit();

			const auto ini = REX::INI::SettingStore::GetSingleton();
			ini->Init(
				"Data/MCM/Config/BakaFullscreenPipboy/settings.ini",
				"Data/MCM/Settings/BakaFullscreenPipboy.ini");
			ini->Load();

			HandleKeybinds();
		}

		static bool QQuickBoy()
		{
			return (MCM::Settings::Pipboy::bEnable && MCM::Settings::Runtime::bQuickBoy);
		}

	private:
		static void ResetStateInit()
		{
			if (auto UI = RE::UI::GetSingleton())
			{
				if (UI->GetMenuOpen("PipboyMenu"sv))
				{
					if (auto UIMessageQueue = RE::UIMessageQueue::GetSingleton())
					{
						UIMessageQueue->AddMessage("PipboyMenu"sv, RE::UI_MESSAGE_TYPE::kHide);
					}
				}
			}

			if (auto Renderer = RE::Interface3D::Renderer::GetByName("PipboyScreenModel"sv))
			{
				Renderer->Disable();
				Renderer->Release();
			}

			if (auto Renderer = RE::Interface3D::Renderer::GetByName("PipboyBackgroundScreenModel"sv))
			{
				Renderer->Disable();
				Renderer->Release();
			}
		}

		static void HandleKeybinds()
		{
			MCM::Settings::Runtime::QuickBoyKey = MCM::Settings::Input::iQuickBoyKey;

			struct keybinds_t
			{
				struct keybind_t
				{
					std::string  id;
					std::int32_t keycode;
					std::string  modName;
					std::int32_t modifiers;
				};

				std::vector<keybind_t> keybinds;
				std::int32_t           version;
			};

			keybinds_t data{};
			if (!glz::read_file_json(data, "Data/MCM/Settings/Keybinds.json", std::string{}))
			{
				for (auto& iter : data.keybinds)
				{
					if (iter.id == "ToggleQuickBoy"sv && iter.modName == "BakaFullscreenPipboy"sv)
					{
						MCM::Settings::Runtime::QuickBoyKey = iter.keycode;
						break;
					}
				}
			}
		}
	};
}
