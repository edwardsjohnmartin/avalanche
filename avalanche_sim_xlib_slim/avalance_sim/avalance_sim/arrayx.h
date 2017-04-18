#ifndef ARRAYX_H
#define ARRAYX_H
#include<cstdlib>
#include<vector>


template <class T>
class arrayx {
		//xarray (Extended Array) class
		//by Jahrain
		//A simple 1d and 2d array structure.
		//Can be initialized as a 1 dimensional or 2 dimensional array
		//3 dimensional arrays not supported yet
		//Similar to the STL vector class, but more like a standard array
		//single dimensional data can be accessed using the [] operator
		//2 dimensional data can be accessed using (x,y)

	private:
		T* _x;
		unsigned int _xsize;
		unsigned int _ysize;
	public:
		arrayx() {
			_x = NULL;
			_xsize = 0;
			_ysize = 1;
		}
		arrayx(const arrayx<T> &xa) {
			_x = new T[xa._xsize*xa._ysize];
			_xsize = xa._xsize;
			_ysize = xa._ysize;
			for (unsigned int i = 0; i < xa._xsize*xa._ysize; ++i) {
				_x[i] = xa._x[i];
			}
		}
		arrayx(unsigned int xsize) {
			_x = new T[xsize];
			_ysize = 1;
			_xsize = xsize;
		}
		arrayx(unsigned int xsize, unsigned int ysize) {
			_x = new T[xsize*ysize];
			_xsize = xsize;
			_ysize = ysize;
		}
		arrayx(const T* arry, unsigned int xsize) {
			_x = new T[xsize];
			_xsize = xsize;
			_ysize = 1;
			memcpy(_x, arry, sizeof(T)*_xsize);
		}
		~arrayx() {
			delete[] _x;
		}
		inline arrayx operator =(const arrayx<T> &xa) {
			delete[] _x;
			_x = new T[xa._xsize*xa._ysize];
			_xsize = xa._xsize;
			_ysize = xa._ysize;
			for (unsigned int i = 0; i < xa._xsize*xa._ysize; ++i) {
				_x[i] = xa._x[i];
			}
			return *this;
		}
		inline arrayx operator =(const std::vector<T> &v) {
			clear();
			resize(v.size());
			for (unsigned int i = 0; i < v.size(); i++) {
				_x[i] = v[i];
			}
			return *this;
		}

		inline void resize(unsigned int xsize) {
			T *temp = new T[xsize];
			if (_x != NULL) {
				memcpy(temp, _x, sizeof(T)*xsize);
			}
			clear();
			_x = temp;
			_xsize = xsize;
			_ysize = 1;
		}
		inline void resize(unsigned int xsize, unsigned int ysize) {
			T *temp = new T[ysize*xsize];
			if (_x != NULL) memcpy(temp, _x, sizeof(T)*ysize*xsize);
			clear();
			_x = temp;
			_xsize = xsize;
			_ysize = ysize;
		}
		inline void fill(T val) {
			for (unsigned int i = 0; i < _xsize*_ysize; ++i) {
				_x[i] = val;
			}
		}
		inline void clear() {
			if (_x == NULL) return;
			delete[] _x;
			_xsize = 0;
			_ysize = 1;
			_x = NULL;
		}
		inline std::vector<T> toVector() const {
			std::vector<T> v;
			for (int i = 0; i < (int)size(); i++) {
				v.push_back(_x[i]);
			}
			return v;
		}

		inline void push_back(const T &val) {
			//only for single dimensional arrays
			if (_ysize != 1) return;
			T *temp = new T[_xsize + 1];
			if (_x != NULL) {
				memcpy(temp, _x, sizeof(T)*_xsize);
				delete[] _x;
			}
			_x = temp;
			++_xsize;
			_x[_xsize - 1] = val;
		}
		inline void push_front(const T &val) {
			//only for single dimensional arrays
			if (_ysize != 1) return;
			T *temp = new T[_xsize + 1];
			memcpy(temp + 1, _x, sizeof(T)*_xsize);
			delete[] _x;
			_x = temp;
			++_xsize;
			_x[0] = val;
		}
		inline unsigned int size() const {
			return _ysize*_xsize;
		}
		inline unsigned int size_x() const {
			return _xsize;
		}
		inline unsigned int size_y() const {
			return _ysize;
		}
		inline void remove(unsigned int index) {
			if (_ysize != 1) return;
			memmove(&_x[index], &_x[index + 1], sizeof(T)*(_xsize - index));
			_xsize--;
		}

		inline void reverse() {
			for (unsigned int i = 0; i < (_ysize*_xsize) >> 1; ++i) {
				swap(i, _ysize*_xsize - i - 1);
			}
		}
		inline void swap(unsigned int index1, unsigned int index2) {
			T temp = _x[index1];
			_x[index1] = _x[index2];
			_x[index2] = temp;
		}
		inline void swap(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
			T temp = (*this)(x1, y1);
			(*this)(x1, y1) = (*this)(x2, y2);
			(*this)(x2, y2) = temp;
		}
		inline T& operator [] (int index) {
			//if (index >= (int)size()) index = size() -1;
			//if (index < 0) index = 0;
			return _x[index];
		}
		inline const T& operator [] (int index) const {
			//if (index >= (int)size()) index = size() -1;
			//if (index < 0) index = 0;
			return _x[index];
		}

		inline T& operator () (unsigned int x, unsigned int y) {
			//int i = x*_ysize + y;
			//if (i >= (int)size()) i = size() - 1;
			//if (i < 0) i = 0;
			return _x[x*_ysize + y];
		}
		inline const T& operator () (unsigned int x, unsigned int y) const {
			//int i = x*_ysize + y;
			//if (i >= (int)size()) i = size() - 1;
			//f (i < 0) i = 0;
			return _x[x*_ysize + y];
		}

		friend bool operator ==(const arrayx<T>& xa, const arrayx<T>& xb) {
			if (xa._xsize*xa._ysize != xb._xsize*xb._ysize) return false;
			for (unsigned int i = 0; i < xa._xsize*xb._xsize; ++i) {
				if (xa._x[i] != xb._x[i]) return false;
			}
			return true;
		}
		friend bool operator !=(const arrayx<T>& xa, const arrayx<T>& xb) {
			return !(xa == xb);
		}
};

template <class T>
struct Array3d {
	//structure for containting 3 dimensional data 
	int width;		//size of each horizontal row
	int height;		//size of each vertical column
	int depth;		//number of 2 dimensional slices
	T* data;		//raw data stored in a 1 dimensional array

	Array3d() {
		width = 0;
		height = 0;
		depth = 0;
		data = NULL;
	}
	Array3d(unsigned int w, unsigned int h, unsigned int d) {
		data = new T[w*h*d];
		width = w;
		height = h;
		depth = d;
	}
	unsigned int size() {
		return width*height*depth;
	}
	void clear() {
		delete[] data;
		data = NULL;
		width = 0;
		depth = 0;
		height = 0;
	}
	T& operator () (unsigned int x, unsigned int y, unsigned int z) {
		return data[z*width*height + y*width + x];
	}
	const T& operator () (unsigned int x, unsigned int y, unsigned int z) const {
		return data[z*width*height + y*width + x];
	}
	T& operator [] (unsigned int i) {
		return data[i];
	}
	const T& operator [] (unsigned int i) const {
		return data[i];
	}
	~Array3d() {
		delete[] data;
	}
};

#endif