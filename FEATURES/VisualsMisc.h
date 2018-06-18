#pragma once

namespace FEATURES
{
	namespace VISUALS
	{
		class VisualsMisc
		{
		public:
			void Do();

			void DisableFlashDuration();

		private:
			void DrawCrosshair();
			void DrawGrenadePrediction();
			void DrawSnaplines();
			void DrawSpreadCircle();
			void DrawAutowallCrosshair();
		};

		extern VisualsMisc visuals_misc;
	}
}
