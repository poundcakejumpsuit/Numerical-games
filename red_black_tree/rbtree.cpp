#include <iostream>
#include <limits>
#include <cassert>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <iterator>
#include <cstdint>
#include "rbtree.hpp"

Node::Node() {
	data = std::numeric_limits<double>::quiet_NaN();
	parent = nullptr;
	left = nullptr;
	right = nullptr;
	color = RED;
}

Node::Node(int d, Color c, Node* l, Node* r, Node* p) {
	data = d;
	parent = p;
	left = l;
	right = r;
	color = c;
}

Node* Node::get_sibling() {
	if (parent == nullptr) {
		std::cout << "No parent!" << std::endl;
		return nullptr;
	}
	//TODO: overload equality for duplicate values?
	if (this->data == parent->left->data) {
		return parent->right;
	}
	return parent->left;
}

Node* Node::get_uncle() {
	if (parent == nullptr) {
		return nullptr;
	}
	if (parent->parent == nullptr) {
		return nullptr;
	}
	if (parent->parent->left != nullptr) {
		if (parent->data == parent->parent->left->data) {
			if (parent->parent->right == nullptr) {
				return nullptr;
			}
			return parent->parent->right;
		}
	}
	if (parent->parent->right != nullptr || parent->parent->left != nullptr) {
		if (parent->data == parent->parent->right->data) {
			if (parent->parent->left == nullptr) {
				return nullptr;
			}
			return parent->parent->left;
		}
	}
	return nullptr;
}

Node* Node::get_grandparent() {
	if (parent == nullptr) {
		std::cout << "No parent!" << std::endl;
		return nullptr;
	}
	if (parent->parent == nullptr) {
		std::cout << "No grandparent!" << std::endl;
		return nullptr;
	}
	return parent->parent;
}

Node::~Node() {
	//pass, as there is no heap allocation
}

RB_Tree::RB_Tree() {
	//a new tree has no nodes
	root = nullptr;
	size = 0;
}

RB_Tree::RB_Tree(Node* r) {
	root = r;
}

RB_Tree::~RB_Tree() {
	//pass, as there is no heap allocation
}

void RB_Tree::rotate_left(Node* n) {
	Node* n_new = n->right;
	n->right = n_new->left;
	n_new->left = n;
	n_new->parent = n->parent;
	if (n->parent != nullptr) {
		if (n == n->parent->right) {
			n->parent->right = n_new;
		}
		else {
			n->parent->left = n_new;
		}
	}
	n->parent = n_new;
}

void RB_Tree::rotate_right(Node* n) {
	Node* n_new = n->left;
	n->left = n_new->right;
	n_new->right = n;
	n_new->parent = n->parent;
	if (n->parent != nullptr) {
		if (n == n->parent->right) {
			n->parent->right = n_new;
		}
		else {
			n->parent->left = n_new;
		}
	}
	n->parent = n_new;
}

void RB_Tree::postorder(Node * p, int indent) {
    if(p != nullptr) {
        if(p->right) {
            postorder(p->right, indent+4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right) std::cout<<" /\n" << std::setw(indent) << ' ';
        std::cout<< p->data << "\n ";
        if(p->left) {
            std::cout << std::setw(indent) << ' ' <<" \\\n";
            postorder(p->left, indent+4);
        }
    }
}

void RB_Tree::print() {
	std::cout << "_________________________________________________" << std::endl;
	postorder(root);
	std::cout << "_________________________________________________" << std::endl;
}

void RB_Tree::insert(Node* n) {
	insert_recurse(root, n);
	insert_repair_tree(n);
	root = n;
	while (root->parent != nullptr) {
		root = root->parent;
	}
	size++;
}

void RB_Tree::insert_recurse(Node* cur_node, Node* n) {
	//left subtree
	if (cur_node != nullptr && n->data < cur_node->data) {
		//non-leaf node
		if (cur_node->left != nullptr) {
			if (cur_node->right != nullptr) {
				insert_recurse(cur_node->left, n);
				return;
			}
			insert_recurse(cur_node->left, n);
			return;
		}
		//leaf-node
		else {
			if (root != nullptr) {
				cur_node->left = n;
			}
			else {
				root = n;
			}
		}
	}
	//right subtree
	else if (cur_node != nullptr) {		
		//non-leaf node
		if (cur_node->right != nullptr) {
			if (cur_node->left != nullptr) {
				insert_recurse(cur_node->right, n);
				return;
			}
			insert_recurse(cur_node->right, n);
			return;
		}
		//leaf-node
		else {
			if (root != nullptr) {
				cur_node->right = n;
			}
			else {
				root = n;
			}
		}
	}
	n->parent = cur_node;
	n->left = nullptr;
	n->right = nullptr;
	n->color = RED;
}

void RB_Tree::insert_repair_tree(Node* n) {
	if (n->parent == nullptr) {
		insert_case1(n);
	}
	else if (n->parent->color == BLACK) {
		insert_case2(n);
	}
	else if (n->get_uncle() != nullptr && n->get_uncle()->color == RED) {
		insert_case3(n);
	}
	else {
		insert_case4(n);
	}
}

void RB_Tree::insert_case1(Node* n) {
	//n is root
	if (n->parent == nullptr) {
		n->color = BLACK;
	}
}

void RB_Tree::insert_case2(Node* n) {
	//n has a black parent
	return;
}

void RB_Tree::insert_case3(Node* n) {
	n->parent->color = BLACK;
	n->get_uncle()->color = BLACK;
	n->get_grandparent()->color = RED;
	insert_repair_tree(n->get_grandparent());
}

void RB_Tree::insert_case4(Node* n) {
	Node* p = n->parent;
	Node* g = n->get_grandparent();
	if (g->left != nullptr) {
		if (n == g->left->right) {
			rotate_left(p);
			n = n->left;
		}
	}
	else if (g->right->left != nullptr) {
		if (n == g->right->left) {
			rotate_right(p);
			n = n->right;
		}
	}
	insert_case4step2(n);
}

void RB_Tree::insert_case4step2(Node* n) {
	Node* p = n->parent;
	Node* g = n->get_grandparent();
	if (n == p->left) {
		rotate_right(g);
	}
	else {
		rotate_left(g);
	}
	p->color = BLACK;
	g->color = RED;
}

uint64_t RB_Tree::get_size() {
	return size;
}

int main(int argc, char* argv[]) {
	std::vector<Node*> v;
	std::vector<int> intv = {7, 4, 2, 6, 9, 3, 5, 10, 8, 1};
	// for (int i = 1; i < 11; i ++) {
	for (auto i: intv) {
		Node* n = new Node(i);
		v.push_back(n);
	}
	// linux rng:
	// auto rng = std::default_random_engine {};
	// osx rng:
	std::random_device rd;
	std::mt19937 rng(rd());
	std::shuffle(std::begin(v), std::end(v), rng);
	RB_Tree* rb = new RB_Tree();
	for (auto el: v) {
		std::cout << el->data << std::endl;
		rb->insert(el);
		std::cout<<std::endl;
		std::cout << "TREE:" <<std::endl;
		rb->print();
	}
	std::cout << rb->get_size() << std::endl;
}
