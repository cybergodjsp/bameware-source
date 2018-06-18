#pragma once

namespace SDK
{
	// forward declare shit
	class CBaseEntity;
}

namespace FEATURES
{
	namespace VISUALS
	{
		class CBoxESP
		{
		public:
			void Do();

		private:
			void DrawBox(SDK::CBaseEntity* entity, CColor text_color, CColor box_color, bool draw_box, bool draw_name, bool draw_weapon, bool draw_resolver, bool draw_healthbar);
			void DrawBoxInfo(SDK::CBaseEntity* entity, CColor text_color, Vector* corners, bool draw_name, bool draw_weapon, bool draw_resolver);
			void DrawHealthBar(SDK::CBaseEntity* entity, Vector* corners, int alpha);
		};

		extern CBoxESP box_esp;
	}
}
