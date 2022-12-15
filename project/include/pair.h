#pragma once

#include "libs.h"

/**
 * mesh edge class
 */
struct Pair
{
	GLuint _id;
	std::array<GLuint, 2> _vertices; /**< indexes should be sorted increasingly*/
	GLfloat _cost = .0f; /**< used in QEM algorithm*/

	/*
	 * default constructor
	 */
	inline Pair() {}
	/*
	 * constructor with a sort operation
	 * @param v1 index1
	 * @param v2 index2
	 */
	inline Pair(GLuint v1, GLuint v2)
	{
		v1 < v2 ? _vertices = { v1, v2 } : _vertices = { v2, v1 };
	}
	/*
	 * like above, but with adding cost value
	 * @param v1 index1
	 * @param v2 index2
	 * @param cost cost
	 */
	inline Pair(GLuint v1, GLuint v2, GLfloat cost) : _cost(cost)
	{
		v1 < v2 ? _vertices = { v1, v2 } : _vertices = { v2, v1 };
	}

	/*
	 * needed for std::set's insert method
	 * @param p reference to another Pair object
	 * @return boolean value
	 */
	inline bool operator<(Pair const &p) const {
		return _vertices[0] < p._vertices[0] || (_vertices[0] == p._vertices[0] && _vertices[1] < p._vertices[1]);
	}

	/*
	 * an ordinary setter
	 * @param v1 index1
	 * @param v2 index2
	 */
	void set(GLuint v1, GLuint v2);
};