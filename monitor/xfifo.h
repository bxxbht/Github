/*
 * A simple patient monitor demo program.
 *
 * Copyright (C) 2011 longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef X_FIFO_H
#define X_FIFO_H

#include <QVector>

template <class T>
class XFiFo : public QVector<T>
{
public:
	XFiFo(int size) : QVector<T>(size)
	{
		depth = size;
		clear();
	}

	inline bool isEmpty()
	{
		return counter == 0;
	}

	inline bool isFull()
	{
		return counter == depth;
	}

	inline int spare()
	{
		return depth - counter;
	}

	inline int count()
	{
		return counter;
	}

	inline void clear()
	{
		in = out = counter = 0;
	}

	inline void enqueue(const T &t)
	{
		Q_ASSERT_X(counter < depth, "XFiFo<T>::enqueue", "fifo vector is full");
		this->data()[in] = t;
		in++;
		counter++;
		in %= depth;
	}

	inline T dequeue()
	{
		Q_ASSERT_X(counter > 0, "XFiFo<T>::dequeue", "fifo vector is empty");
		T t = this->data()[out];
		out++;
		counter--;
		out %= depth;
		return t;
	}

	int depth;
	int counter;
	int in; 
	int out; 
};

#endif
