namespace myvector
{
	template<class T>
	class vector
	{
	public:
		//iterator
		typedef T* iterator;
		typedef const T* const_iterator;
		iterator begin() { return _start; }
		iterator end(){ return _finish; }
		const_iterator cbegin()const { return _start; }
		const_iterator cend()const { return _finish; }

		vector()
			:_start(nullptr)
			, _finish(nullptr)
			, _endOfStorage(nullptr)
		{}

		vector(const vector<T>& v)
			:_start(nullptr)
			,_finish(nullptr)
			, _endOfStorage(nullptr)
		{
			reserve(v.capacity());
			iterator it = begin();
			const_iterator vit = v.cbegin();
			while (vit != v.cend())
			{
				*it++ = *vit++;
			}
			_finish = _start + v.size();
		}

		vector(size_t n, const T& value = T())
			:_start(nullptr)
			, _finish(nullptr)
			, _endOfStorage(nullptr)
		{
			reserve(n);
			while (n--)
				push_back(value);
		}

		vector<T>& operator=(vector<T> v)
		{
			swap(v);
			return *this;
		}

		~vector()
		{
			delete[] _start;
			_start = nullptr;
			_finish = nullptr;
			_endOfStorage = nullptr;
		}
		//capacity
		size_t size()const { return _finish - _start; }
		size_t capacity()const { return _endOfStorage - _start; }
		bool empty()const { return _start == _finish; }
	
		void reserve(size_t n)
		{
			size_t sz = size();
			T* tmp = new T[n];
			if (_start)
			{
				for (int i = 0; i < size(); i++)
					tmp[i] = _start[i];
			}

			delete[] _start;
			_start = tmp;
			_finish = _start + sz;
			_endOfStorage = _start + n;

		}

		void resize(size_t n, const T& value = T())
		{
			if (n <= size())
			{
				_finish -= size() - n;
				return;
			}
			else if (n > capacity())
			{
				reserve(n);
			}
			size_t count = n - size();
			while (count--)
				push_back(value);
		}

		//增删查改
		T& operator[](size_t i)
		{
			return _start[i];
		}
	
		void push_back(T x)
		{
			/*if (_finish == _endOfStorage)
			{
				size_t newCapacity = capacity() == 0 ? 1 : capacity() * 2;
				reserve(newCapacity);
			}
			
			*_finish = x;
			++_finish;*/
			insert(end(), x);
		}

		void pop_back()
		{
			assert(_start != _finish);
			--_finish;
		}

		iterator insert(iterator pos, T x)
		{
			assert(pos <= _finish);
			if (_finish == _endOfStorage)
			{
				size_t sz = pos-_start;
				size_t newCapacity = capacity() == 0 ? 1 : capacity() * 2;
				reserve(newCapacity);
				pos = _start + sz;   //增容后空间变化，pos的位置要修正
			}
			iterator end = _finish - 1;
			while (end >= pos)
			{
				*(end + 1) = *end;
				--end;
			}
			*pos = x;
			++_finish;
			return pos;
		}

		iterator erase(iterator pos)
		{
			assert(_start != _finish);
			iterator begin = pos;
			while (begin != _finish-1)
			{
				*begin = *(begin + 1);
				begin++;
			}
			--_finish;
			return pos;
		}

		void swap(vector<T>& v)
		{
			std::swap(_start, v._start);
			std::swap(_finish, v._finish);
			std::swap(_endOfStorage, v._endOfStorage);
		}
	private:
		iterator _start;
		iterator _finish;
		iterator _endOfStorage;
	};
}