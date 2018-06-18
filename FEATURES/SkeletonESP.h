#pragma once

namespace SDK
{
	class CBaseEntity;
}

namespace FEATURES
{
	namespace VISUALS
	{
		class SkeletonESP
		{
		public:
			void Do();
			void DrawSkeleton(SDK::CBaseEntity* entity, CColor color, const matrix3x4_t* bone_matrix = nullptr); // public function since backtrack visalization uses it
		};

		extern SkeletonESP skeleton_esp;
	}
}
