#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "lock.h"

namespace Communication {

	class Protocol {
		public:
			//
			// Tags
			//
			enum MessageTag {
				TAG_INITIALIZATION		= 'I',
				TAG_TELEMETRY_STREAM	= 'T',
				TAG_CRASH				= 'B',
				TAG_FELL_INTO_CRATER	= 'C',
				TAG_KILLED_BY_MARTIAN	= 'K',
				TAG_SUCCESS				= 'S',
				TAG_END_OF_RUN			= 'E',
			};
			enum ObjectTag {
				TAG_BOULDER	= 'b',
				TAG_CRATER	= 'c',
				TAG_HOME	= 'h',
				TAG_ENEMY	= 'm'
			};

			//
			// Objects
			//
			struct ObjectCommon {
				float x; // ...
				float y; // ...
				float radius; // ...
				void clear() {
					x = y = radius = 0.0f;
				}
				inline friend std::ostream& operator<<(std::ostream& o, const ObjectCommon& v) {
					o << "ObjectCommon { " << v.x << ", " << v.y << ", " << v.radius << " }";
					return o;
				}
			};
			struct ObjectMartian {
				float x; // ...
				float y; // ...
				float dir; // ...
				float speed; // meters per second
				void clear() {
					x = y = dir = speed = 0.0f;
				}
				inline friend std::ostream& operator<<(std::ostream& o, const ObjectMartian& v) {
					o << "ObjectMartian { " << v.x << ", " << v.y << ", " << v.dir << ", " << v.speed << " }";
					return o;
				}
			};
			struct Object {
				ObjectTag tag;
				union {
					ObjectCommon common;
					ObjectMartian enemy;
				};
				void clear() {
					switch (tag) {
						case TAG_BOULDER:
						case TAG_CRATER:
						case TAG_HOME:
							common.clear();
							break;
						case TAG_ENEMY:
							enemy.clear();
							break;
					}
					memset(&tag, 0, sizeof(ObjectTag));
				}
				inline friend std::ostream& operator<<(std::ostream& o, const Object& v) {
					switch (v.tag) {
						case TAG_BOULDER:
						case TAG_CRATER:
						case TAG_HOME:
							o << v.common;
							break;
						case TAG_ENEMY:
							o << v.enemy;
							break;
					}
					return o;
				}
			};

			//
			// Messages
			//
			struct MessageInitialization {
				float dx; // meters
				float dy; // meters
				int time_limit; // milliseconds
				float min_sensor; // meters
				float max_sensor; // meters
				float max_speed; // meters per second
				float max_turn; // degrees per second
				float max_hard_turn; // degrees per second
				void clear() {
					dx = dy = min_sensor = max_sensor = max_speed = max_turn = max_hard_turn = 0.0f;
					time_limit = 0;
				}
				inline friend std::ostream& operator<<(std::ostream& o, const MessageInitialization& v) {
					o << "MessageInitialization { " << v.dx << ", " << v.dy << ", " << v.time_limit << ", "
						<< v.min_sensor << ", " << v.max_sensor << ", " << v.max_speed << ", "
						<< v.max_turn << ", " << v.max_hard_turn << " }";
					return o;
				}
			};
			struct MessageTelemetryStream {
				typedef std::list<Protocol::Object *> ObjectList;
				int timestamp; // milliseconds
				char vehicle_ctl[2]; // char
				float vehicle_x; // meters
				float vehicle_y; // meters
				float vehicle_dir; // counterclockwise angle from the x-axis in degrees
				float vehicle_speed; // meters per second
				char string_objects[4096]; // TODO fixed size? should find a better solution...
				ObjectList *objects;
				void clear() {
					if (objects != NULL) {
						for (ObjectList::const_iterator iter = objects->begin(); iter != objects->end(); ++iter) {
							delete *iter;
						}
						objects->clear();
						SAFE_DELETE(objects);
					}
					timestamp = 0;
					memset(vehicle_ctl, 0, sizeof(vehicle_ctl));
					vehicle_x = vehicle_y = vehicle_dir = vehicle_speed = 0.0f;
					memset(string_objects, 0, sizeof(string_objects));
				}
				inline friend std::ostream& operator<<(std::ostream& o, const ObjectList& v) {
					Object *obj;
					for (ObjectList::const_iterator iter = v.begin(); iter != v.end(); ) {
						obj = *iter;
						o << *obj;
						if (++iter != v.end())
							o << ", ";
					}
					return o;
				}
				inline friend std::ostream& operator<<(std::ostream& o, const MessageTelemetryStream& v) {
					o << "MessageTelemetryStream { "
						<< v.timestamp << ", "
						<< v.vehicle_ctl[0] << v.vehicle_ctl[1] << ", "
						<< v.vehicle_x  << ", " << v.vehicle_y << ", "
						<< v.vehicle_dir << ", "
						<< v.vehicle_speed << ", "
						<< v.objects << " }";
					return o;
				}
			};
			struct MessageEvent {
				int time_stamp; // milliseconds
				inline friend std::ostream& operator<<(std::ostream& o, const MessageEvent& v) {
					o << "MessageEvent { " << v.time_stamp << " }";
					return o;
				}
				void clear() {
					time_stamp = 0;
				}
			};
			struct MessageEndOfRun {
				int time_stamp; // milliseconds
				int score; // milliseconds (time_stamp + penalties)
				void clear() {
					time_stamp = score = 0;
				}
				inline friend std::ostream& operator<<(std::ostream& o, const MessageEndOfRun& v) {
					o << "MessageEndOfRun { " << v.time_stamp << ", " <<  v.score << " }";
					return o;
				}
			};

			struct Message {
				MessageTag tag;
				union {
					MessageInitialization initialization;
					MessageTelemetryStream telemetry;
					MessageEvent event;
					MessageEndOfRun end;
				};
				void clear() {
					switch (tag) {
						case TAG_INITIALIZATION: initialization.clear(); break;
						case TAG_TELEMETRY_STREAM: telemetry.clear(); break;
						case TAG_KILLED_BY_MARTIAN:
						case TAG_FELL_INTO_CRATER:
						case TAG_SUCCESS:
							event.clear();
							break;
						case TAG_END_OF_RUN: end.clear(); break;
					}
					tag = TAG_INITIALIZATION;
				}
			};
	};

	class StreamBuffer {
		public:
			std::list<std::string> m_buffer;
			Lock m_lock;
	};

	class ProtocolStream {
		private:
			Socket *m_socket;
			StreamBuffer m_incoming;
			StreamBuffer m_outgoing;
		public:
			ProtocolStream(Socket& socket) {
				m_socket = &socket;
			}
			void put(const std::string& message) {
				ScopeLock lock(&m_outgoing.m_lock);
				m_outgoing.m_buffer.push_back(message);
			}
			bool get(std::string& message) {
				ScopeLock lock(&m_incoming.m_lock);
				if (m_incoming.m_buffer.empty())
					return false;
				message = m_incoming.m_buffer.front();
				m_incoming.m_buffer.pop_front();
				return true;
			}
			void poll() {
				// This function is a complete mess... not written by me.
				// receive commands
				static char incoming_buffer[1024];
				char tmp_buf[1024];
				static char last_buf[1024];
				int bytes;
				int x, y;

				while ((bytes = m_socket->read(tmp_buf, 1023-strlen(last_buf))) > 0) {
					// TODO review if every command is complete (terminates with \n)
					for (x=0; x<strlen(last_buf); x++) {
						incoming_buffer[x] = last_buf[x];
					}
					for (; x<strlen(last_buf)+bytes; x++) {
						incoming_buffer[x] = tmp_buf[x-strlen(last_buf)];
					}

					incoming_buffer[bytes+strlen(last_buf)] = 0;
					for (x=0; x<=1023; x++)
						last_buf[x] = 0;
					for (x=bytes; x>0; x--) {
						if (incoming_buffer[x] == ';' && x < bytes)
							break;
					}
					x++;
					for (y=0;y<(bytes-x);y++) {
						last_buf[y] = incoming_buffer[x+y];
					}
					incoming_buffer[x] = 0;
					//cout << "LAST_BUF:" << last_buf << endl;
					m_incoming.m_lock.acquire();
					stringTokenize(incoming_buffer, m_incoming.m_buffer, "\n");
					m_incoming.m_lock.release();
				}
//				// receive commands
//				static char incoming_buffer[1024];
//				static string m_incoming_rest;
//				int bytes;
//				sleep(2);
//				while ((bytes = m_socket->read(incoming_buffer, 1023)) > 0) {
//						incoming_buffer[bytes] = 0;
//						// TODO review if every command is complete (terminates with \n)
//						m_incoming.m_lock.acquire();
//						stringTokenize(incoming_buffer, m_incoming.m_buffer, m_incoming_rest, "\n");
//						m_incoming.m_lock.release();
//				}
				// send commands
				static const char *outgoing_buffer;
				while (!m_outgoing.m_buffer.empty()) {
					m_outgoing.m_lock.acquire();
					outgoing_buffer = m_outgoing.m_buffer.front().c_str();
					m_socket->write((void *)outgoing_buffer, strlen(outgoing_buffer));
					m_outgoing.m_buffer.pop_front();
					m_outgoing.m_lock.release();
				}
			}
			void stringTokenize(
				const std::string& str,
				std::list<std::string>& tokens,
				const std::string& delimiters
			) {
				// Skip delimiters at beginning.
				std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
				// Find first "non-delimiter".
				std::string::size_type pos = str.find_first_of(delimiters, lastPos);
				while (std::string::npos != pos || std::string::npos != lastPos) {
					// Found a token, add it to the vector.
					tokens.push_back(str.substr(lastPos, pos - lastPos));
					// Skip delimiters.  Note the "not_of"
					lastPos = str.find_first_not_of(delimiters, pos);
					// Find next "non-delimiter"
					pos = str.find_first_of(delimiters, lastPos);
				}
			}
	};

	class ProtocolParser {
		private:
			std::vector<Protocol::Message> m_messages;

			void parseInitialization(Protocol::Message& msg, const char *stream) {
				char msg_tag;
				sscanf(stream, "%c %f %f %d %f %f %f %f %f ;",
					&msg_tag,
					&msg.initialization.dx,
					&msg.initialization.dy,
					&msg.initialization.time_limit,
					&msg.initialization.min_sensor,
					&msg.initialization.max_sensor,
					&msg.initialization.max_speed,
					&msg.initialization.max_turn,
					&msg.initialization.max_hard_turn
				);
				msg.tag = static_cast<Protocol::MessageTag>(msg_tag);
				std::cout << msg.initialization << std::endl;
			}

			void parseTelemetry(Protocol::Message& msg, const char *stream) {
				char msg_tag;
				sscanf(stream, "%c %d %c%c %f %f %f %f %[-0123456789. bchm] ;",
					&msg_tag,
					&msg.telemetry.timestamp,
					&msg.telemetry.vehicle_ctl[0], &msg.telemetry.vehicle_ctl[1],
					&msg.telemetry.vehicle_x,
					&msg.telemetry.vehicle_y,
					&msg.telemetry.vehicle_dir,
					&msg.telemetry.vehicle_speed,
					msg.telemetry.string_objects // was &msg.telemetry.string_objects, wrong wrong!
				);
				msg.tag = static_cast<Protocol::MessageTag>(msg_tag);

				msg.telemetry.objects = new Protocol::MessageTelemetryStream::ObjectList();
				msg.telemetry.objects->clear();
				for (
					char *ptr = strpbrk(msg.telemetry.string_objects, "bchm");
					ptr != NULL;
					ptr = strpbrk(ptr+1, "bchm")
				) {
					//printf("##### = %s\n", ptr);
					Protocol::Object *obj = new Protocol::Object();
					msg.telemetry.objects->push_back(obj);
					char obj_tag;
					switch (ptr[0]) {
						case Protocol::TAG_BOULDER:
						case Protocol::TAG_CRATER:
						case Protocol::TAG_HOME:
							sscanf(ptr, "%c %f %f %f ",
								&obj_tag,
								&obj->common.x,
								&obj->common.y,
								&obj->common.radius
							);
							obj->tag = static_cast<Protocol::ObjectTag>(obj_tag);
							break;
						case Protocol::TAG_ENEMY:
							sscanf(ptr, "%c %f %f %f %f ",
								&obj_tag,
								&obj->enemy.x,
								&obj->enemy.y,
								&obj->enemy.dir,
								&obj->enemy.speed
							);
							obj->tag = static_cast<Protocol::ObjectTag>(obj_tag);
							break;
						default:
							std::cerr << "unknown object tag" << std::endl;
					}
				}
				std::cout << msg.telemetry << std::endl;
			}

			void parseEvent(Protocol::Message& msg, const char *stream) {
				char msg_tag;
				sscanf(stream, "%c %d ;",
					&msg_tag,
					&msg.event.time_stamp
				);
				msg.tag = static_cast<Protocol::MessageTag>(msg_tag);
				std::cout << msg.event << std::endl;
			}

			void parseEndOfRun(Protocol::Message& msg, const char *stream) {
				char msg_tag;
				sscanf(stream, "%c %d %d ;",
					&msg_tag,
					&msg.end.time_stamp,
					&msg.end.score
				);
				msg.tag = static_cast<Protocol::MessageTag>(msg_tag);
				std::cout << msg.end << std::endl;
			}

		public:
			void parse(Protocol::Message& msg, const std::string& command) {
				std::cout << "[RawMessage] " << command << std::endl;
				const char *stream = command.c_str();
				switch (stream[0]) {
					case Protocol::TAG_INITIALIZATION:
						parseInitialization(msg, stream);
						break;
					case Protocol::TAG_TELEMETRY_STREAM:
						parseTelemetry(msg, stream);
						break;
					case Protocol::TAG_KILLED_BY_MARTIAN:
					case Protocol::TAG_FELL_INTO_CRATER:
					case Protocol::TAG_SUCCESS:
						parseEvent(msg, stream);
						break;
					case Protocol::TAG_END_OF_RUN:
						parseEndOfRun(msg, stream);
						break;
					default:
						std::cerr << "unknown message tag" << std::endl;
				}
			}
	};

}
