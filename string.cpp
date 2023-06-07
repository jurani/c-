namespace mystring
{
	class string
	{
		
	public:
		typedef char* iterator;
		string(const char* str = "")
		{
			_size = strlen(str);
			_capacity = _size;
			char* tmp = new char[_capacity + 1];
			strcpy(tmp, str);
			_str = tmp;
		}
		~string()
		{
			delete[] _str;
			_str = nullptr;
		}
		//c_str
		char* c_str()const
		{
			return _str;
		}

		//capacity
		size_t size()const
		{
			return _size;
		}
		size_t capacity()const
		{
			return _capacity;
		}
		bool empty()const
		{
			if (strcmp(_str, "") == 0)
				return true;
			else
				return false;
		}
		void reserve(size_t newcapacity)
		{
			//新容量大于旧容量时增容
			if (newcapacity > _capacity)
			{
				char* tmp = new char[newcapacity + 1];
				strcpy(tmp, _str);
				delete[] _str;
				_str = tmp;
				_capacity = newcapacity;
			}
		}
		void resize(size_t n, char ch = '\0')
		{
			//需要修改的有效字符个数大于原有字符个数时，
			//多出的空间由\0补充
			if (n > _size)
			{
				if (n >= _capacity)
				{
					reserve(n);
				}
				size_t index = _size;
				for (size_t i = 0; i < n; i++)
				{
					_str[index++] = ch;
				}
			}
			_size =n;
			_str[_size] = '\0';
		}

		//iterator
		iterator begin(){ return _str; }
		iterator end() { return _str + _size;}

		//access
		char& operator[](size_t i)
		{
			assert(i < _size);
			return _str[i];
		}
		const char& operator[](size_t i)const
		{
			assert(i < _size);
			return _str[i];
		}
		//赋值
		string& operator=(const string& s)
		{
			if (this != &s)
			{
				char* tmp = new char[s._size + 1];
				strcpy(tmp, s._str);
				delete[] _str;
				_str = tmp;
				_size = s._size;
				_capacity = s._capacity;
			}

			return *this;
		}
		
		//增删查改
		void push_back(char ch)
		{
			if (_size == _capacity)
			{
				size_t newcapacity = _capacity == 0 ? 2 : _capacity * 2;
				reserve(newcapacity);
			}
			_str[_size++] = ch;
			_str[_size] = '\0';
		}
		void append(const char* str)
		{
			if ((_size += strlen(str)) > _capacity)
			{
				reserve(_size);
			}
			strcat(_str, str);
		}
		string& inset(size_t pos , char ch)
		{
			if (_size == _capacity)
			{
				size_t newCapacity = _capacity == 0 ? 2 : _capacity * 2;
				reserve(newCapacity);
			}
			size_t end = _size;
			while (end >= pos)
			{
				_str[end + 1] = _str[end];
				--end;
			}
			_str[pos] = ch;
			++_size;
			return *this;
		}
		string& inset(size_t pos, const char* str)
		{
			size_t len = strlen(str);
			if (_size + len > _capacity)
				reserve(_size + len);
			size_t end = _size;
			while (end >= pos)
			{
				_str[end + len] = _str[end];
				--end;
			}
			for (int i = 0; i < len; i++)
			{
				_str[pos++] = str[i];
			}
			_size += len;
			return *this;
		}
		string& operator+=(char ch)
		{
			push_back(ch);
			return *this;
		}
		string& operator+=(const char* str)
		{
			append(str);
			return *this;
		}
		void clear()
		{
			_size = 0;
			_str[_size] = '\0';
		}
		string& erase(size_t pos = 0, size_t len = npos)
		{
			if (len >= _size - pos)
			{
				_str[pos] = '\0';
				_size = pos;
			}
			else
			{
				for (int i = pos + len; i <=_size; ++i)
				{
					_str[i - len] = _str[i];
				}
				_size -= len;
			}
			return *this;
		}
		size_t find(char ch, size_t pos = 0)const
		{
			for (int i = pos; i < _size; i++)
			{
				if (_str[i] == ch)
					return i;
			}
			return npos;
		}
		size_t find(const char*str, size_t pos = 0)const
		{
			char* tmp = strstr(_str+pos, str);
			if (tmp==NULL)
				return npos;
			else
				return tmp - _str;
		}

		//比较大小
		bool operator<(const string& s)
		{
			if (strcmp(_str, s._str) < 0)
				return true;
			else
				return false;
		}
		bool operator<=(const string& s)
		{
			if (strcmp(_str, s._str) <= 0)
				return true;
			else
				return false;
		}
		bool operator>(const string& s)
		{
			return !(*this <= s);
		}
		bool operator>=(const string& s)
		{
			return !(*this < s);
		}
		bool operator==(const string& s)
		{
			return !(*this > s || *this < s);
		}
		bool operator!=(const string& s)
		{
			return *this > s || *this < s;
		}

	private:
		size_t _size;
		size_t _capacity;
		char* _str;
		static const size_t npos;
	};
	const size_t string::npos = -1;
	ostream& operator<<(ostream& out, const string& s)
	{
		for (size_t i = 0; i < s.size(); i++)
		{
			out << s[i];
		}
		return out;
	}
}