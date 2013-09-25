#pragma once

#include <cassert>
#include <cmath>
#include <iostream>

//
// NOTE: Most of the following class was copied/adapted from someone
// else's code, whom I sincerely do not remember. If you recognize
// most of this code as yours, please contact me and I'll add the
// respective license and give you proper credit. Please, accept my
// apologies in advance.
//

class Vector2 {
	public:
		float x, y;
	public:
		Vector2() {
		}

		Vector2(const float X, const float Y) {
			set(X, Y);
		}

		explicit Vector2(const float scalar) {
			set(scalar);
		}

		explicit Vector2(const float coordinates[2]) {
			set(coordinates);
		}

		explicit Vector2(const int coordinates[2]) {
			set(coordinates);
		}

		explicit Vector2(float* const r) {
			set(r);
		}

		Vector2(const Vector2& other) {
			set(other);
		}

		Vector2& set(const float X, const float Y) {
			x = X, y = Y;
			return *this;
		}

		Vector2& set(const float coordinates[2]) {
			x = coordinates[0], y = coordinates[1];
			return *this;
		}

		Vector2& set(const int coordinates[2]) {
			x = (float)coordinates[0];
			y = (float)coordinates[1];
			return *this;
		}

		Vector2& set(float* const r) {
			x = r[0], y = r[1];
			return *this;
		}

		float operator[](const size_t i) const {
			assert(i < 2);
			return *(&x+i);
		}

		float& operator[](const size_t i) {
			assert(i < 2);
			return *(&x+i);
		}

		float* ptr() {
			return &x;
		}

		const float* ptr() const {
			return &x;
		}

		Vector2& set(const Vector2& other) {
			return (*this = other);
		}
		Vector2& operator=(const Vector2& other) {
			x = other.x;
			y = other.y;
			return *this;
		}

		Vector2& set(const float scalar) {
			return (*this = scalar);
		}
		Vector2& operator=(const float scalar) {
			x = y = scalar;
			return *this;
		}

		bool equals(const Vector2& other) const {
			return *this == other;
		}
		bool operator==(const Vector2& other) const {
			return x == other.x && y == other.y;
		}
		bool operator!=(const Vector2& other) const {
			return x != other.x || y != other.y;
		}

		Vector2 operator+(const Vector2& other) const {
			return Vector2(x + other.x, y + other.y);
		}

		Vector2 operator-(const Vector2& other) const {
			return Vector2(x - other.x, y - other.y);
		}

		Vector2 operator*(const float scalar) const {
			return Vector2(x * scalar, y * scalar);
		}
		Vector2 operator*(const Vector2& rhs) const {
			return Vector2(x * rhs.x, y * rhs.y);
		}

		Vector2 operator/(const float scalar) const {
			assert( scalar != 0.0 );
			float inv = 1.0 / scalar;
			return Vector2(x * inv, y * inv);
		}
		Vector2 operator/(const Vector2& rhs) const {
			return Vector2(x / rhs.x, y / rhs.y);
		}

		const Vector2& operator+() const {
			return *this;
		}

		Vector2 operator-() const {
			return Vector2(-x, -y);
		}

		friend Vector2 operator*(const float scalar, const Vector2& other) {
			return Vector2(scalar * other.x, scalar * other.y);
		}

		friend Vector2 operator/(const float scalar, const Vector2& other) {
			return Vector2(scalar / other.x, scalar / other.y);
		}

		friend Vector2 operator+(const Vector2& lhs, const float rhs) {
			return Vector2(lhs.x + rhs, lhs.y + rhs);
		}
		friend Vector2 operator+(const float lhs, const Vector2& rhs) {
			return Vector2(lhs + rhs.x, lhs + rhs.y);
		}

		friend Vector2 operator-(const Vector2& lhs, const float rhs) {
			return Vector2(lhs.x - rhs, lhs.y - rhs);
		}
		friend Vector2 operator-(const float lhs, const Vector2& rhs) {
			return Vector2(lhs - rhs.x, lhs - rhs.y);
		}

		Vector2& add(const Vector2& other) {
			return (*this += other);
		}
		Vector2& operator+=(const Vector2& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		Vector2& add(const float scalar) {
			return (*this += scalar);
		}
		Vector2& operator+=(const float scalar) {
			x += scalar;
			y += scalar;
			return *this;
		}

		Vector2& sub(const Vector2& other) {
			return (*this -= other);
		}
		Vector2& operator-=(const Vector2& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2& sub(const float scalar) {
			return (*this -= scalar);
		}
		Vector2& operator-=(const float scalar) {
			x -= scalar;
			y -= scalar;
			return *this;
		}

		Vector2& mul(const float scalar) {
			return (*this *= scalar);
		}
		Vector2& operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector2& mul(const Vector2& other) {
			return (*this *= other);
		}
		Vector2& operator*=(const Vector2& other) {
			x *= other.x;
			y *= other.y;
			return *this;
		}

		Vector2& div(const float scalar) {
			return (*this /= scalar);
		}
		Vector2& operator/=(const float scalar) {
			assert(scalar != 0.0);
			float fInv = 1.0 / scalar;
			x *= fInv;
			y *= fInv;
			return *this;
		}

		Vector2& div(const Vector2& other) {
			return (*this /= other);
		}
		Vector2& operator/=(const Vector2& other) {
			x /= other.x;
			y /= other.y;
			return *this;
		}

		float length() const {
			return std::sqrt(x * x + y * y);
		}
		float lengthSq() const {
			return (x * x + y * y);
		}

		float squaredLength() const {
			return x * x + y * y;
		}

		float dot(const Vector2& vec) const {
			return x * vec.x + y * vec.y;
		}

		float normalize() {
			float length = std::sqrt(x * x + y * y);

			// Will also work for zero-sized vectors, but will change nothing
			if (length > 1e-08) {
				float invLength = 1.0 / length;
				x *= invLength;
				y *= invLength;
			}

			return length;
		}

		Vector2 midPoint(const Vector2& vec) const {
			return Vector2((x + vec.x ) * 0.5, (y + vec.y ) * 0.5);
		}

		bool operator<(const Vector2& rhs) const {
			if (x < rhs.x && y < rhs.y)
				return true;
			return false;
		}

		bool operator>(const Vector2& rhs) const {
			if (x > rhs.x && y > rhs.y)
				return true;
			return false;
		}

		void makeFloor(const Vector2& cmp) {
			if (cmp.x < x)
				x = cmp.x;
			if (cmp.y < y)
				y = cmp.y;
		}

		void makeCeil(const Vector2& cmp) {
			if (cmp.x > x)
				x = cmp.x;
			if (cmp.y > y)
				y = cmp.y;
		}

		Vector2 perpendicular(void) const {
			return Vector2(-y, x);
		}

		float cross(const Vector2& other) const {
			return x * other.y - y * other.x;
		}

		bool isZeroLength(void) const {
			float sqlen = (x * x) + (y * y);
			return (sqlen < (1e-06 * 1e-06));

		}

		Vector2 normalizedCopy(void) const {
			Vector2 ret = *this;
			ret.normalize();
			return ret;
		}

		Vector2 reflect(const Vector2& normal) const {
			return Vector2(*this - (2 * this->dot(normal) * normal));
		}

		// special points
		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 UNIT_SCALE;

		friend std::ostream& operator<<(std::ostream& o, const Vector2& v) {
			o << "Vector2(" << v.x << ", " << v.y << ")";
			return o;
		}

};
