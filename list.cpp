namespace mylist
{ 
	template <class T>
	struct __ListNode
	{
		__ListNode(const T& date = T())
			:_prev(nullptr)
			, _next(nullptr)
			, _data(date)
		{}

		__ListNode<T>* _prev;
		__ListNode<T>* _next;
		T _data;
	};
	//list迭代器实现
	template <class T,class Ref,class Ptr>
	class __ListIterator
	{
	public:
		typedef __ListNode<T>* PNode;
		typedef __ListIterator<T, Ref, Ptr> Self;  //使迭代器支持普通的和const
		__ListIterator(PNode node)				//<T,T&,T*>,<T,const T&,const T*>
			:_node(node)
		{}

		__ListIterator(const Self& it)
			:_node(it._node)
		{}
		Ref operator*() { return _node->_data; }
		Ptr operator->() { return &(operator*()); }
		Self& operator++()
		{
			_node = _node->_next;
			return *this;
		}
		Self operator++(int)
		{
			Self temp(*this);
			_node = _node->_next;
			return temp;
		}
		Self& operator--()
		{
			_node = _node->_prev;
			return *this;
		}
		Self operator--(int)
		{
			Self tmp(*this);
			_node = _node->_prev;
			return temp;
		}
		bool operator!=(const Self& it)const
		{
			return _node != it._node;
		}
		bool operator==(const Self& it)const
		{
			return _node == it._node;
		}
		PNode _node;
		
	};


	template<class T>
	class list
	{
		typedef __ListNode<T> Node;
		typedef __ListNode<T>* PNode;

	public:
		typedef __ListIterator<T,T&,T*> iterator;
		typedef __ListIterator<T, const T&, const T*> const_iterator;

	public:
		list()
		{
			_head = new Node;
			_head->_prev = _head;
			_head->_next = _head;
		}
		list(const list<T>& l)
		{
			_head = new Node;
			_head->_prev = _head;
			_head->_next = _head;

			for (auto e : l)
			{
				push_back(e);
			}
		}
		list<T>& operator=(list<T> temp)
		{
			std::swap(_head, temp._head);
			return *this;
		}
		~list()
		{
			clear();
			delete _head;
			_head = nullptr;
		}

		//iterator
		iterator begin() { return _head->_next; }
		iterator end() { return _head; }
		const_iterator begin()const { return _head->_next; }
		const_iterator end()const { return _head; }

		//增删查改
		void push_back(const T& val)
		{
			/*PNode newNode = new Node;
			PNode tail = _head->_prev;

			tail->_next = newNode;
			newNode->_prev = tail;
			_head->_prev = newNode;
			newNode->_next = _head;
			newNode->_data = val;*/
			insert(end(),val);
		}
		void pop_back() { erase(--end()); }
		void push_front(const T& val) { insert(begin(), val); }
		void pop_front() { erase(begin()); }
		size_t size()const
		{
			int count = 0;
			for (auto e : *this)
				++count;
			return count;
		}
		bool empty()const { return begin() == end(); }

		T& front() { return *(begin()); }
		T& back() { return *(--end()); }
		void clear()
		{
			iterator pos = begin();
			while (pos != end())
				pos = erase(pos);
		}
		iterator erase(iterator& pos)
		{
			assert(pos._node != _head);
			PNode prev = pos._node->_prev;
			PNode next = pos._node->_next;

			prev->_next = next;
			next->_prev = prev;
			delete pos._node;
			return next;

		}
		void insert(iterator& pos, const T& val)
		{
			PNode newNode = new Node;
			newNode->_data = val;
			PNode prev = pos._node->_prev;

			prev->_next = newNode;
			newNode->_prev = prev;
			newNode->_next = pos._node;
			pos._node->_prev = newNode;
		}
	private:
		PNode _head;
	};
}