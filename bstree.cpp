#include <iostream>

namespace mytree
{
	template <class K>
	struct BSTNode
	{
		BSTNode(const K& val = K())
			:_left(nullptr),_right(nullptr),_key(val)
		{}

		BSTNode<K>* _left;
		BSTNode<K>* _right;
		K _key;
	};

	template <class K>
	class BSTree
	{
		typedef BSTNode<K> Node;
	public:
		BSTree()
			:_root(nullptr)
		{}
		~BSTree()
		{
			Delect(_root);
		}
		void Delect(Node*& root)
		{
			if (root == nullptr)
				return;

			Delect(root->_left);
			Delect(root->_right);
			delete root;
			root = nullptr;
		}
		bool Find(const K& val)
		{
			Node* cur = _root;
			while (cur)
			{
				if (cur->_key == val)
					return true;
				else if (val < cur->_key)
					cur = cur->_left;
				else if (val > cur->_key)
					cur = cur->_right;
			}
			if (!cur)
				return false;
			else
				return true;
		}

		bool Insert(const K& val)
		{
			if (_root == nullptr)
			{
				_root = new Node(val);
				return true;
			}

			Node* cur = _root;
			Node* parent = nullptr;
			while (cur)   //找到空节点
			{
				parent = cur;
				if (val < cur->_key)
					cur = cur->_left;
				else if (val > cur->_key)
					cur = cur->_right;
				else
					return false;
			}
			cur = new Node(val);
			if (val > parent->_key)
				parent->_right = cur;
			else
				parent->_left = cur;
			return true;
		}
		bool Erase(const K& val)
		{
			if (_root == nullptr)
				return false;

			Node* cur = _root;
			Node* parent = nullptr;
			while (cur)  //找到需要删除的节点
			{
				if (cur->_key == val)
				{
					break;
				}
				else if (val < cur->_key)
				{
					parent = cur;
					cur = cur->_left;
				}
				else if (val > cur->_key)
				{
					parent = cur;
					cur = cur->_right;
				}
			}
			if (cur == nullptr) //没有对应节点时
				return false;

			//当左边节点为空时，父节点指向我的右节点
			if (cur->_left == nullptr)
			{
				if (cur == _root)
				{
					_root = cur->_right;
				}
				else
				{
					if (parent->_left == cur)
						parent->_left = cur->_right;
					else
						parent->_right = cur->_right;
				}
			}   //当右节点为空时，父节点指向我的左
			else if (cur->_right == nullptr)
			{
				if (cur == _root)
				{
					_root = cur->_left;
				}
				else
				{
					if (parent->_left == cur)
						parent->_left = cur->_left;
					else
						parent->_right = cur->_left;
				}
			}
			else  //当左右节点都不为空时，找到右子树的最小节点代替我，最转化为上面两种情况删除
			{
				Node* minNodeParent = nullptr;
				Node* minNode = cur->_right;
				while (minNode->_left)
				{
					minNodeParent = minNode;
					minNode = minNode->_left;
				}
				cur->_key = minNode->_key;
				//转化为上面两种情况
				if (minNodeParent->_left == minNode)
					minNodeParent->_left = minNode->_right;
				else
					minNodeParent->_right = minNode->_right;
				cur = minNode;
			}
			delete cur;
			return true;
		}

		void InOrder(Node*& root)
		{
			if (root == nullptr)
				return;

			InOrder(root->_left);
			std::cout << root->_key << " ";
			InOrder(root->_right);
		}
		void InOrder()
		{
			InOrder(_root);
			std::cout << std::endl;
		}
	private:
		Node* _root;
	};
};

