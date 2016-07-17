struct vector2D {
	vector2D(float x, float y);
	// float components[2];

	float x;
	float y;

	vector2D operator+ (vector2D other);
	vector2D operator- (vector2D other);
	vector2D operator/ (vector2D other);

	vector2D operator* (int scalar);
	//vector2D operator* (int scalar, const vector2D& other);

	float dot(vector2D other);
	vector2D cross(vector2D other);
	float length(void);
};

vector2D::vector2D(float x, float y) {
	this->x = x;
	this->y = y;
}

vector2D vector2D::operator+(vector2D other) {
	return vector2D(x + other.x, y + other.y);
}

vector2D vector2D::operator-(vector2D other) {
	return vector2D(x - other.x, y - other.y);
}

vector2D vector2D::operator*(int scalar) {
	return vector2D(scalar * x, scalar *y);
}

inline vector2D operator* (int scalar, vector2D other) {
	return vector2D(scalar * other.x, scalar * other.y);
}

float length() {
	return 0;
}