#pragma once

#include <string>
#include <cmath>
#include <pthread.h>
#include "protocol.h"
#include "vision.h"
#include "movement.h"

namespace Movement {

	void *threadFunc(void *arg);

	class PathFind {
		private:
			Controller *m_controller;
			Vision m_vision;
			pthread_t m_thread;
		public:
			PathFind(Controller *controller) {
				m_controller = controller;
				pthread_create(&m_thread, 0, threadFunc, this);
			}
			~PathFind() {
				pthread_kill(m_thread, 9);
			}

			void adjustCourse() {
				// TODO Begin of test -- REMOVE IT
				m_controller->state().expected.vehicle_speed = 5.0;
				m_controller->state().expected.vehicle_dir = -40.0;
				std::cout << "DEBUG: "
					<< " speed=" << m_controller->state().current.vehicle_speed
					<< " dir=" << m_controller->state().current.vehicle_dir
					<< " angle_between=" << calcAngleBetween(
						m_controller->state().current.vehicle_pos,
						m_controller->state().expected.vehicle_pos)
					<< " distance_between=" << calcDistanceBetween(
						m_controller->state().current.vehicle_pos,
						m_controller->state().expected.vehicle_pos)
					<< std::endl;
				// TODO End of test -- REMOVE IT
				if (currentSpeed() < expectedSpeed())
					m_controller->accel();
				else if (currentSpeed() > expectedSpeed())
					m_controller->brake();
				if (currentDirection() < expectedDirection())
					m_controller->turnLeft();
				else if (currentDirection() > expectedDirection())
					m_controller->turnRight();
				m_controller->execute();
			}

			float currentSpeed() {
				return m_controller->state().current.vehicle_speed;
			}
			float currentDirection() {
				return m_controller->state().current.vehicle_dir;
			}
			float expectedSpeed() {
				return m_controller->state().expected.vehicle_speed;
			}
			float expectedDirection() {
				return m_controller->state().expected.vehicle_dir;
			}

			//
			// Auxiliar methods
			//
			float calcAngleBetween(const Vector2& v1, const Vector2& v2) const {
				Vector2 n1 = v1;
				Vector2 n2 = v2;
				return acos(n1.dot(n2));
			}
			float calcDistanceBetween(const Vector2& v1, const Vector2& v2) const {
				return std::sqrt(std::pow(v2.x-v1.x, 2) + std::pow(v2.y-v1.y, 2));
			}

	};

	void *threadFunc(void *arg) {
		PathFind *finder = static_cast<PathFind *>(arg); // I know, casting sucks.
		// TODO This is a temporary ugly hack, and should be replaced by proper synchronization
		sleep(3); // Wait until struct m_controller->state.current is initialized
		while (true) {
			finder->adjustCourse();
			usleep(100000);
		}
		return 0;
	}

};
