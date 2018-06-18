#pragma once

namespace SDK
{
	class CBaseEntity;
	class IMaterial;
	class KeyValues;
}

namespace FEATURES
{
	namespace VISUALS
	{
		class CChams
		{
		public:
			void DoSceneEnd();
			bool ShouldDrawModel(SDK::CBaseEntity* entity);

		private:
			bool is_in_rendering = false; /// dont draw model material twice

			SDK::IMaterial* local_player_mat;
			SDK::IMaterial* enemy_visible_mat;
			SDK::IMaterial* enemy_occluded_mat;
			SDK::IMaterial* teammate_visible_mat;
			SDK::IMaterial* teammate_occluded_mat;
			SDK::IMaterial* backtrack_visualization_mat;

			void DrawPlayer(SDK::CBaseEntity* entity);
			void DrawBacktrackPlayer(SDK::CBaseEntity* entity);
			void UpdateMaterials();

			SDK::IMaterial* CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe);
		};

		extern CChams chams;
	}
}
