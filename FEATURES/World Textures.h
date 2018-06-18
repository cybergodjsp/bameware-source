#pragma once

namespace FEATURES
{
	namespace VISUALS
	{
		class WorldTextures
		{
		public:
			static constexpr int NUM_SKYBOXES = 25;
			char skybox_list[NUM_SKYBOXES][256] =
			{
				"cs_baggage_skybox",
				"cs_tibet",
				"clearsky",
				"clearsky_hdr",
				"embassy",
				"italy",
				"jungle",
				"nukeblank",
				"office",
				"sky_cs15_daylight01_hdr",
				"sky_cs15_daylight02_hdr",
				"sky_cs15_daylight03_hdr",
				"sky_cs15_daylight04_hdr",
				"sky_csgo_cloudy01",
				"sky_csgo_night02",
				"sky_csgo_night02b",
				"sky_csgo_night_flat",
				"sky_day02_05_hdr",
				"sky_day02_05",
				"sky_dust",
				"sky_l4d_rural02_ldr",
				"vertigo_hdr",
				"vertigoblue_hdr",
				"vertigo",
				"vietnam"
			};

			void DoFSN();
		};

		extern WorldTextures world_textures;
	}
}
