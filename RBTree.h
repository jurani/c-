using namespace std;
enum Color{RED,BLACK};

template <class K,class V>
struct RBTreeNode //红黑树的节点
{
	RBTreeNode(const pair<K,V>& kv)
		:_left(nullptr),_right(nullptr),_parent(nullptr),
		 _col(RED),_kv(kv)
	{}

	RBTreeNode* _left;
	RBTreeNode* _right;
	RBTreeNode* _parent;

	Color _col;
	pair<K, V> _kv;
};

template <class K,class V>
class RBTree
{
	typedef RBTreeNode<K, V> Node;
public:
	RBTree():_root(nullptr)
	{}
	bool Insert(const pair<K, V>& kv)
	{
		if (!_root) //插入根节点
		{
			_root = new Node(kv);
			_root->_col = BLACK;
			return true;
		}
		//找出所插入的位置
		Node* cur = _root;
		Node* parent = nullptr;
		while (cur)
		{
			parent = cur;
			if (kv.first < cur->_kv.first)
				cur = cur->_left;
			else if (kv.first > cur->_kv.first)
				cur = cur->_right;
			else
				return false;
		}
		//插入新增节点
		cur = new Node(kv);
		if (kv.first < parent->_kv.first)
			parent->_left = cur;
		else
			parent->_right = cur;
		cur->_parent = parent;

		cur->_col = RED;
		//情况1：p时黑色，插入完成
		//情况2；p和u都是红色，把p和u变成黑色，把g变成黑色，使其符合
		//红黑数的规则。再向上更新节点
		//情况3：当u不存在或则u为黑时，当插入的节点和p和g呈直线时需要单旋转处理。
		//情况4：当u不存在或则u为黑时，当插入的节点和p和g呈折线时需要双旋转处理。
		while (parent && parent->_col == RED)
		{
			Node* grandfather = parent->_parent;
			if (grandfather->_left == parent)
			{
				Node* uncle = grandfather->_right;
				if (uncle && uncle->_col == RED) //情况1
				{
					parent->_col = BLACK;
					uncle->_col = BLACK;
					grandfather->_col = RED;

					cur = grandfather;
					parent = cur->_parent;
				}
				else
				{  //先单旋，把情况4转变为情况3
					if (parent->_right == cur)
					{
						RotateL(parent);
						swap(parent, cur);
					}

					RotateR(grandfather);
					grandfather->_col = RED;
					parent->_col = BLACK;

				}
			}
			else
			{
				Node* uncle = grandfather->_left;
				if (uncle && uncle->_col == RED)
				{
					parent->_col = BLACK;
					uncle->_col = BLACK;
					grandfather->_col = RED;

					cur = grandfather;
					parent = cur->_parent;
				}
				else
				{
					if (parent->_left == cur)
					{
						RotateR(parent);
						swap(parent, cur);
					}

					RotateL(grandfather);
					grandfather->_col = RED;
					parent->_col = BLACK;
				}
			}
		}
		_root->_col = BLACK;
		return true;
	}

	void RotateR(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;

		parent->_left = subLR;
		if (subLR)
			subLR->_parent = parent;

		subL->_right = parent;
		Node* pParent = parent->_parent;
		parent->_parent = subL;

		if (!pParent)
		{
			_root = subL;
			subL->_parent = nullptr;
		}
		else
		{
			if (pParent->_left == parent)
				pParent->_left = subL;
			else
				pParent->_right = subL;
			subL->_parent = pParent;
		}
	}

	void RotateL(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;

		parent->_right = subRL;
		if (subRL)
			subRL->_parent = parent;

		subR->_left = parent;
		Node* pParent = parent->_parent;
		parent->_parent = subR;

		if (!pParent)
		{
			_root = subR;
			subR->_parent = nullptr;
		}
		else
		{
			if (pParent->_left == parent)
				pParent->_left = subR;
			else
				pParent->_right = subR;
			subR->_parent = pParent;
		}
	}
	void InOrder(Node* root)
	{
		if (!root)
			return;

		InOrder(root->_left);
		std::cout << root->_kv.first << ":" << root->_kv.second << std::endl;
		InOrder(root->_right);
	}
	void InOrder()
	{
		InOrder(_root);
	}

	bool IsValidRBTree()
	{
		Node* root = _root;
		// 空树也是红黑树
		if (nullptr == root)
			return true;
		// 检测根节点是否满足情况
		if (BLACK != root->_col)
		{
			cout << "违反红黑树性质二：根节点必须为黑色" << endl;
			return false;
		}
		// 获取任意一条路径中黑色节点的个数
		size_t blackCount = 0;
		Node* cur = root;
		while (cur)
		{
			if (BLACK == cur->_col)
				blackCount++;
			cur = cur->_left;
		}
		// 检测是否满足红黑树的性质，k用来记录路径中黑色节点的个数
		size_t k = 0;
		return _IsValidRBTree(root, k, blackCount);
	}
	bool _IsValidRBTree(Node* root, size_t k, const size_t blackCount)
	{
		//走到null之后，判断k和black是否相等
		if (nullptr == root)
		{
			if (k != blackCount)
			{
				cout << "违反性质四：每条路径中黑色节点的个数必须相同" << endl;
				return false;
			}
			return true;
		}
		// 统计黑色节点的个数
		if (BLACK == root->_col)
			k++;
		// 检测当前节点与其双亲是否都为红色
		Node* parent = root->_parent;
		if (parent && RED == parent->_col && RED == root->_col)
		{
			cout << "违反性质三：没有连在一起的红色节点" << endl;
			return false;
		}
		return _IsValidRBTree(root->_left, k, blackCount) &&
			_IsValidRBTree(root->_right, k, blackCount);
	}
private:
	Node* _root;
};
