#pragma once
using std::pair;
template <class K, class V>
struct VALTreeNode  //VALTree的节点
{
	VALTreeNode(const pair<K, V>& kv)
		:_left(nullptr),
		_right(nullptr),
		_parent(nullptr),
		_bf(0),
		_kv(kv)
	{}

	VALTreeNode<K, V>* _left;
	VALTreeNode<K, V>* _right;
	VALTreeNode<K, V>* _parent;
	int _bf;  //平衡因子
	pair<K, V> _kv;

}; 

template <class K, class V>
class VALTree
{
	typedef VALTreeNode<K, V> Node;
public:
	VALTree():_root(nullptr)
	{}
	bool insert(const pair<K, V>& kv)
	{
		if (!_root)
		{
			_root = new Node(kv);
			return true;
		}

		Node* cur = _root;
		Node* parent = nullptr;
		while (cur)     //找出新插入值的位置
		{
			
			if (kv.first < cur->_kv.first)
			{
				parent = cur;
				cur = cur->_left;
			}
			else if (kv.first > cur->_kv.first)
			{
				parent = cur;
				cur = cur->_right;
			}
			else if(kv.first==cur->_kv.first)
			{
				return false;
			}
				
		}
		cur = new Node(kv);  //链接新插入的值
		if (parent->_kv.first > cur->_kv.first)
			parent->_left = cur;
		else
			parent->_right = cur;
		cur->_parent = parent;

		while (parent)
		{    //更新平衡因子
			if (parent->_left == cur)
				parent->_bf--;
			else
				parent->_bf++;

			if (parent->_bf == 0)
			{
				break;
			}
			else if (parent->_bf == -1 || parent->_bf == 1)
			{
				cur = parent;
				parent = parent->_parent;
			}
			else if (parent->_bf == 2)
			{
				//parent所在的子树不平衡，旋转处理
				if (cur->_bf == 1)
					rotateL(parent);
				else if (cur->_bf == -1)
					rotateRL(parent);//右左双旋
				break;
			}
			else if (parent->_bf == -2)
			{
				if (cur->_bf == -1)
					rotateR(parent);
				else if (cur->_bf == 1)
					rotateLR(parent);//左右双旋
				break;
			}

		}
		return true;
	}

	void rotateL(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		//链接关系
		parent->_right = subRL;
		if (subRL)
			subRL->_parent = parent;

		subR->_left = parent;
		Node* pParent = parent->_parent;
		parent->_parent = subR;

		if (!pParent) //subR的链接情况
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
		parent->_bf = subR->_bf = 0;
	}

	void rotateR(Node* parent)
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
		parent->_bf = subL->_bf = 0;
	}

	void rotateLR(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		int bf = subLR->_bf;
		rotateL(subL);
		rotateR(parent);
		//平衡因子更新
		if (bf == -1)
		{
			parent->_bf = 1;
			subL->_bf = 0;
			subLR->_bf = 0;
		}
		else if (bf == 1)
		{
			parent->_bf = 0;
			subL->_bf = -1;
			subLR->_bf = 0;
		}
		else if(bf==0)
		{
			parent->_bf = 0;
			subL->_bf = 0;
			subLR->_bf = 0;
		}
	}

	void rotateRL(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		int bf = subRL->_bf;

		rotateR(subR);
		rotateL(parent);
		//平衡因子更新
		if (bf == -1)
		{
			parent->_bf = 0;
			subR->_bf = 1;
			subRL->_bf = 0;
		}
		else if (bf == 1)
		{
			parent->_bf = -1;
			subR->_bf = 0;
			subRL->_bf = 0;
		}
		else if(bf==0)
		{
			parent->_bf = 0;
			subR->_bf = 0;
			subRL->_bf = 0;
		}
	}
	void inorder(Node* root)
	{
		if (!root)
			return;

		inorder(root->_left);
		std::cout << root->_kv.first << ":" << root->_kv.second << std::endl;
		inorder(root->_right);
	}
	void inorder()
	{
		inorder(_root);
	}
	int Height(Node* root)
	{
		if (!root)
			return 0;
		int leftHeight = Height(root->_left);
		int rightHeight = Height(root->_right);
		return leftHeight > rightHeight ? leftHeight + 1 : rightHeight+1;
	}
	bool IsBalanceTree(Node* root)  //验证valtree
	{
		if (!root)
			return true;
		//获取左右子数高度
		int leftHeight = Height(root->_left);
		int rightHeight = Height(root->_right);
		int bf = rightHeight - leftHeight;

		if (bf!=root->_bf || (bf > 1 || bf < -1))
			return false;

		return IsBalanceTree(root->_left) && IsBalanceTree(root->_right);
	}
	bool IsBalanceTree()
	{
		return IsBalanceTree(_root);
	}
	
private:
	Node* _root;
};