#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace FEATURES
{
	namespace MISC
	{
		class CircleStrafer
		{
		public:
			void Do(SDK::CUserCmd* cmd);

		private:
			float VerifyRotation(float ideal_rotation);
			void VisualizeRotation(float rotation, float length);
		};

		extern CircleStrafer circle_strafer;
	}
}
