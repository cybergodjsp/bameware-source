#pragma once

#include "../SDK/AnimLayer.h"
#include "../SDK/CBaseAnimState.h"
#include "../FEATURES/Resolver.h"

namespace SDK
{
	class CBaseEntity;
	class NetChannel;
}

namespace FEATURES
{
	namespace RAGEBOT
	{
		#define ENABLE_INTERPOLATION 1
		#define DISABLE_INTERPOLATION 0

		static constexpr float MAX_UNLAG = 1.f;
		static constexpr float MAX_BACKTRACK_RANGE = 0.2f;

		struct Backtracking_Record
		{
			Backtracking_Record() {};
			explicit Backtracking_Record(SDK::CBaseEntity*);

			Vector vec_origin;
			Vector eye_angles;
			Vector abs_angles;
			Vector render_angles;
			Vector bbmin;
			Vector bbmax;
			Vector velocity;

			Vector previous_vec_origin;

			float simulation_time;
			float previous_simulation_time;
			float curtime;
			float previous_curtime;
			bool is_exploiting;
			int flags;

			SDK::CBaseEntity* player;
			SDK::CAnimationLayer anim_layers[15];
			SDK::CBaseAnimState animstate;
			matrix3x4_t bone_matrix[128];
			float pose_params[24];

			FEATURES::RAGEBOT::Resolver::PlayerResolveRecord resolve_record;
		};

		struct Incoming_Sequence_Record
		{
			Incoming_Sequence_Record(int in_reliable, int out_reliable, int in_sequence, float realtime)
			{
				in_reliable_state = in_reliable;
				out_reliable_state = out_reliable;
				in_sequence_num = in_sequence;

				time = realtime;
			}

			int in_reliable_state;
			int out_reliable_state;
			int in_sequence_num;

			float time;
		};

		class CBacktracking
		{
		public:
			void Store();
			void Restore(SDK::CBaseEntity*, Backtracking_Record, int extrapolate_ticks = 0);
			void ApplyRestore(SDK::CBaseEntity* entity, float curtime);
			void RestoreToCurrentRecord(SDK::CBaseEntity*);

			void UpdateIncomingSequences();
			void AddLatency(SDK::NetChannel* net_channel, float latency);

			std::vector<Backtracking_Record> GetPriorityRecords(SDK::CBaseEntity* entity); // gets the records in order of most usable, lby update, etcs
			std::vector<Backtracking_Record> GetValidRecords(SDK::CBaseEntity* entity);
			std::vector<Backtracking_Record> GetRecords(SDK::CBaseEntity* entity);
			Backtracking_Record* GetCurrentRecord(SDK::CBaseEntity* entity);

			std::vector<Backtracking_Record>& GetShotRecords()
			{
				return shot_at_records;
			}

			void AddShotRecord(Backtracking_Record record)
			{
				shot_at_records.insert(shot_at_records.begin(), record);

				if (shot_at_records.size() > 3)
					shot_at_records.pop_back();
			}

			bool GetExtrapolatedRecord(SDK::CBaseEntity* entity, Backtracking_Record& record);
			bool Extrapolate(SDK::CBaseEntity* entity, Backtracking_Record& record, float time);

			static float GetDeltaTime(Backtracking_Record record);
			static int GetLerpTicks();
			static int GetTickCount(Backtracking_Record);
			static void SetInterpolationFlags(SDK::CBaseEntity* entity, int flag);

		private:
			bool needs_to_restore[64];
			int last_incoming_sequence = 0;
			std::deque<Incoming_Sequence_Record> sequence_records;
			std::vector<Backtracking_Record> player_records[64];
			std::vector<Backtracking_Record> shot_at_records;

			static void InvalidateBoneCache(SDK::CBaseEntity* entity);
		};

		extern CBacktracking backtracking;
	}
}
