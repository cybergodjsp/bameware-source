#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace FEATURES
{
	namespace MISC
	{
		void Misc();

		void FixMovement(SDK::CUserCmd*);
		void AntiUntrusted(SDK::CUserCmd*);

		void UpdateAngles(SDK::CUserCmd*);
		void ThirdpersonOV();
		void ThirdpersonFSN();

		void Fakelag(SDK::CUserCmd* cmd);
		void Bhop(SDK::CUserCmd* cmd);
		void AutoRevolver(SDK::CUserCmd* cmd);
		void AutoPistol(SDK::CUserCmd* cmd);
		void ClantagChanger();
		void Radar();

		void LoadUserConfig();

		void DrawMisc();

		class BulletTracers
		{
		public:
			struct BulletTracer
			{
				BulletTracer(CColor color, Vector start, Vector end)
				{
					m_time = UTILS::GetCurtime();
					m_start = start;
					m_end = end;
					m_color = color;
				}

				float m_time;
				Vector m_start, m_end;
				CColor m_color;
			};

		public:
			void Do();

			void AddTracer(BulletTracer tracer)
			{
				tracers.insert(tracers.begin(), tracer);
			}

		private:
			std::vector<BulletTracer> tracers;

		private:
			const float max_bullet_trace_time = 1.5f;
		};

		extern BulletTracers bullet_tracers;

		class Hitmarkers
		{
		public:
			void Do();
			void AddHitmarker(int damage);

		private:
			struct Hitmarker
			{
				Hitmarker(float time, int damage)
				{
					m_time = time;
					m_damage = damage;
				}

				float m_time;
				int m_damage;
			};

		private:
			std::vector<Hitmarker> hitmarker_vector;
		private:
			const float hitmarker_fade_time = 0.2f;
			const float hitmarker_damage_fade_time = 2.f;
			const int hitmarker_damage_float_height = 150;
		};

		extern Hitmarkers hitmarkers;
	}
}
