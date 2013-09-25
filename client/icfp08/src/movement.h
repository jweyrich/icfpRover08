#pragma once

#include <string>
#include "protocol.h"
#include "lock.h"
#include "vector2.h"

#define DECLARE_ENUM_OPERATORS(_TYPE) \
	inline _TYPE& \
	operator++(_TYPE& v1) \
	{ v1 = _TYPE(static_cast<int>(v1) + 1); return v1; } \
	\
	inline _TYPE& \
	operator--(_TYPE& v1) \
	{ v1 = _TYPE(static_cast<int>(v1) - 1); return v1; }

namespace Movement {

	enum MoveState {
		BREAKING		= -1,
		ROLLING			= 0,
		ACCELERATING	= 1
	};
	DECLARE_ENUM_OPERATORS(MoveState)

	enum TurnState {
		HARD_LEFT	= -2,
		LEFT		= -1,
		STRAIGHT	= 0,
		RIGHT		= 1,
		HARD_RIGHT	= 2
	};
	DECLARE_ENUM_OPERATORS(TurnState)

	using namespace Communication;

	class ControllerState {
		friend class Controller;
		friend class PathFind;
		public:
			void update(const Protocol::Message& message) {
				ScopeLock lock(&current.lock);
				switch (message.tag) {
					case Protocol::TAG_INITIALIZATION:
						current.map_size.x		= message.initialization.dx;
						current.map_size.y		= message.initialization.dy;
						current.time_limit		= message.initialization.time_limit;
						current.sensor_range.x	= message.initialization.min_sensor;
						current.sensor_range.y	= message.initialization.max_sensor;
						current.max_speed		= message.initialization.max_speed;
						current.max_turn		= message.initialization.max_turn;
						current.max_hard_turn	= message.initialization.max_hard_turn;
						break;
					case Protocol::TAG_TELEMETRY_STREAM:
						current.time_stamp		= message.telemetry.timestamp;
						current.vehicle_ctl[0]	= message.telemetry.vehicle_ctl[0];
						current.vehicle_ctl[1]	= message.telemetry.vehicle_ctl[1];
						current.vehicle_pos.x	= message.telemetry.vehicle_x;
						current.vehicle_pos.y	= message.telemetry.vehicle_y;
						current.vehicle_dir		= message.telemetry.vehicle_dir;
						current.vehicle_speed	= message.telemetry.vehicle_speed;
						break;
				}
			}
		protected:
			ControllerState() : move(ROLLING), turn(STRAIGHT) {
				// nop
			}
			bool turnRight() {
				if (turn == HARD_RIGHT)
					return false;
				++turn;
				return true;
			}
			bool turnLeft() {
				if (turn == HARD_LEFT)
					return false;
				--turn;
				return true;
			}
			bool accel() {
				if (move == ACCELERATING)
					return false;
				++move;
				return true;
			}
			bool brake() {
				if (move != BREAKING)
					return false;
				--move;
				return true;
			}

			struct Data {
				Lock lock;
				Vector2 map_size; // map size (meters)
				int time_limit; // max time to accomplish the run (milliseconds)
				Vector2 sensor_range; // minimum and maximum sensor range (meters)
				float max_speed; // (meters per second)
				float max_turn; // (degrees per second)
				float max_hard_turn; // (degrees per second)
				int time_stamp; // timestamp (milliseconds)
				char vehicle_ctl[2]; //...
				Vector2 vehicle_pos; // current vehicle position
				float vehicle_dir;  // current direction in (degrees)
				float vehicle_speed; // current speed (meters per second)
			};

			Data current;
			Data expected;
			MoveState move;
			TurnState turn;
	};

	class Controller {
		friend class PathFind;
		public:
			Controller(ProtocolStream *proto_stream) : _proto_stream(proto_stream) {
				// nop
			}
			ControllerState& state() {
				return _state;
			}
			void accel() {
				if (_state.accel())
					_command += "a";
			}
			void brake() {
				if (_state.brake())
					_command += "b";
			}
			void turnRight() {
				if (_state.turnRight())
					_command += "r";
			}
			void turnLeft() {
				if (_state.turnLeft())
					_command += "l";
			}
			void execute() {
				if (!_command.empty()) {
					std::cout <<  "### command=" << _command
						<< " [ state.move=" << _state.move
						<< " state.turn=" << _state.turn
						<< " ]" << std::endl;
					_proto_stream->put(_command + ";");
					_command.clear();
				}
			}
			void moveTo() {
				// TODO
			}
			void turnToDir(float target_dir) {
				// TODO
			}
		protected:
			ProtocolStream *_proto_stream;
			ControllerState _state;
			std::string _command;
	};

}
