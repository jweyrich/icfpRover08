#pragma once

#include <string>
#include "protocol.h"

namespace Movement {

	using namespace Communication;

	class Vision {
		public:
			bool homeIsVisible(Protocol::MessageTelemetryStream& telemetry, Protocol::Object *home) {
				typedef Protocol::MessageTelemetryStream::ObjectList::const_iterator const_iterator;
				Protocol::Object *object;
				for (
					const_iterator iter = telemetry.objects->begin();
					iter != telemetry.objects->end();
					++iter)
				{
					object = *iter;
					if (object->tag == Protocol::TAG_HOME) {
						home = object;
						return true;
					}
				}
				return false;
			}
	};

}
