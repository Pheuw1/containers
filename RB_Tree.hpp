#ifndef RB_TREE_H
#define RB_TREE_H
#define RED 1
#define BLACK 0
#include <memory>
#include "tree_iterators.hpp"
#include "pair.hpp"
#include "utils.hpp"
#include <string>

namespace ft {

    template<typename T>
    struct RB_Node{
        RB_Node():parent(NULL),left(NULL),right(NULL),color(BLACK),data(T()) {;}
        RB_Node(T val = T()):parent(NULL),left(NULL),right(NULL),color(RED),data(val) {;}
        RB_Node *parent;
        RB_Node *left;
        RB_Node *right;
        bool     color;
        T        data;
    };
    
    template<typename T, typename Compare = ft::less<T>, typename Alloc = std::allocator<RB_Node<T> > >
    class RB_Tree {
    public:
		typedef RB_Node<T> Node;
        typedef Alloc allocator_type;
		typedef Compare value_compare;
        typedef bidirectional_iterator<T, Compare, allocator_type, RB_Node<T>  >                  iterator;
        typedef bidirectional_iterator<const T, Compare, allocator_type, RB_Node<T>  >             const_iterator;
        typedef ft::reverse_iterator<iterator>                                                          reverse_iterator;
        typedef ft::reverse_iterator<const_iterator>                                                    const_reverse_iterator;
        typedef pair<Node *, bool> return_type ;
        typedef Node * NodePtr;

        
        Node                    *_end;
        Node                    *root;
    private:
        Alloc                   alloc;
        const   value_compare   comp;
        size_t                  count;
        std::allocator<Node> _nodeAlloc;

    public:
        RB_Tree(Compare comp = Compare()) :_end(newNode()), root(_end), alloc(Alloc()), comp(comp), count(0),_nodeAlloc(std::allocator<RB_Node<T> >()) {
            _end->color = BLACK;
        };
       

        size_t size() const {return count;}    
        
        int erase(const T &data) {
            return deleteNodeHelper(this->root, data);
        }
        
 		void deleteNode(Node *node) {
			_nodeAlloc.destroy(node);
			_nodeAlloc.deallocate(node, 1);
		}

    	
        Node * newNode(const T &val = T()) {
            Node * ret = _nodeAlloc.allocate(1);
            _nodeAlloc.construct(ret, val);
			return ret;
		}

        void destruct(Node *cur) {
			if (!cur || cur == _end) 
                return;
			destruct(cur->left);
			destruct(cur->right);
			deleteNode(cur);
		}
 
		void clear() {
            destruct(root);
			count = 0;
            root = _end;
        }

        ~RB_Tree() {
            destruct(root);
            deleteNode(_end);
        }

        // fix the rb tree modified by the delete operation
		void fixDelete(NodePtr x) {
			while (x != root && x->color == BLACK) {
				if (x == x->parent->left) {
					NodePtr s = x->parent->right;
					if (s && s->color == RED) {
						s->color = BLACK;
						x->parent->color = RED;
						leftRotate(x->parent);
						s = x->parent->right;
					}
					if (s && (!s->left || s->left->color == BLACK) && (!s->right || s->right->color == BLACK)) {
						s->color = RED;
						x = x->parent;
					} else {
						if (s && (!s->right || s->right->color == BLACK)) {
							if (s->left)
								s->left->color = BLACK;
							s->color = RED;
							rightRotate(s);
							s = x->parent->right;
						}
						if (s) {
							s->color = x->parent->color;
							s->right->color = BLACK;
						}
						x->parent->color = BLACK;
						leftRotate(x->parent);
						x = root;
					}
				} else {
					NodePtr s = x->parent->left;
					if (s && s->color == RED) {
						s->color = BLACK;
						x->parent->color = RED;
						rightRotate(x->parent);
						s = x->parent->left;
					}
					if (s && (!s->right || s->right->color == BLACK) && (!s->left || s->left->color == BLACK)) {
						s->color = RED;
						x = x->parent;
					} else {
						if (s && (!s->left || s->left->color == BLACK)) {
							if (s->right)
								s->right->color = BLACK;
							s->color = RED;
							leftRotate(s);
							s = x->parent->left;
						}
						if (s) {
							s->color = x->parent->color;
							s->left->color = BLACK;
						}
						x->parent->color = BLACK;
						rightRotate(x->parent);
						x = root;
					}
				}
			}
			x->color = 0;
		}


        void rbTransplant(NodePtr u, NodePtr v){
            if (u->parent == NULL) 
                root = v;
             else if (u == u->parent->left)
                u->parent->left = v;
             else 
                u->parent->right = v;
            v->parent = u->parent;
        }

        int deleteNodeHelper(NodePtr node,const T &val) {
            // find the node containing val
            NodePtr z = _end;
            NodePtr x, y;
            while (node != _end){
                if (isEqual(node->data, val)) 
                    z = node;
                if (comp(node->data, val)) 
                    node = node->right;
                else if (comp( val,node->data))
                    node = node->left;
                else
                    node = node->right;
            }
            if (z == _end)
                return 0;
            y = z;
            int y_original_color = y->color;
            if (z->left == _end) {
                x = z->right;
                rbTransplant(z, z->right);
            } else if (z->right == _end) {
                x = z->left;
                rbTransplant(z, z->left);
            } else {
                y = minimum(z->right);
                y_original_color = y->color;
                x = y->right;
                if (y->parent == z) {
                    x->parent = y;
                } else {
                    rbTransplant(y, y->right);
                    y->right = z->right;
                    y->right->parent = y;
                }


                rbTransplant(z, y);
                y->left = z->left;
                y->left->parent = y;
                y->color = z->color;
            }
            deleteNode(z);
            count--;
            if (y_original_color == BLACK)
                fixDelete(x);
            _end->parent = maximum(root);
            return 1;
        }
        // fix the red-black tree
        void fixInsert(NodePtr k){
            NodePtr u;
            while (k && k->parent->color == RED) {
                if (k->parent == k->parent->parent->right) {
                    u = k->parent->parent->left; // uncle
                    if (u->color == RED) {
                        u->color = BLACK;
                        k->parent->color = BLACK;
                        k->parent->parent->color = RED;
                        k = k->parent->parent;
                    } else {
                        if (k == k->parent->left) {
                            k = k->parent;
                            rightRotate(k);
                        }
                        k->parent->color = BLACK;
                        k->parent->parent->color = RED;
                        leftRotate(k->parent->parent);
                    }
                } else {

                    u = k->parent->parent->right; // uncle
                    
                    if (u->color == RED) {
                        u->color = BLACK;
                        k->parent->color = BLACK;
                        k->parent->parent->color = RED;
                        k = k->parent->parent;	

                    } else {

                        if (k == k->parent->right) {
                            k = k->parent;
                            leftRotate(k);
                        }
                        k->parent->color = BLACK;
                        k->parent->parent->color = RED;
                        rightRotate(k->parent->parent);

                    }
                }
                if (k == root) {
                    break;
                }
            }
            root->color = BLACK;
        }

    public:

        NodePtr search(const T &val) const{
            return searchTreeHelper(this->root, val);
        }
       
        NodePtr searchTreeHelper(NodePtr node, T val) const {
            if (node == _end || isEqual(val,node->data)) {
                return node;
            }

            if (comp(val,node->data)) {
                return searchTreeHelper(node->left, val);
            } 
            return searchTreeHelper(node->right, val);
        }
    
        // find the node with the minimum val
        NodePtr minimum(NodePtr node) const {
            while (node && node->left && node->left != _end) {
                node = node->left;
            }
            return node;
        }

        // find the node with the maximum val
        NodePtr maximum(NodePtr node) const {
            while (node && node->right && node->right != _end) {
                node = node->right;
            }
            return node;
        }

        // find the successor of a given node
        NodePtr successor(NodePtr x) {
            // if the right subtree is not null,
            // the successor is the leftmost node in the
            // right subtree
            if (x->right != _end) {
                return minimum(x->right);
            }

            // else it is the lowest ancestor of x whose
            // left child is also an ancestor of x.
            NodePtr y = x->parent;
            while (y != _end && x == y->right) {
                x = y;
                y = y->parent;
            }
            return y;
        }

        // find the predecessor of a given node
        NodePtr predecessor(NodePtr x) {
            // if the left subtree is not null,
            // the predecessor is the rightmost node in the 
            // left subtree
            if (x->left != _end) {
                return maximum(x->left);
            }

            NodePtr y = x->parent;
            while (y != _end && x == y->left) {
                x = y;
                y = y->parent;
            }

            return y;
        }

        // rotate left at node x
        void leftRotate(NodePtr x) {
            NodePtr y = x->right;
            x->right = y->left;
            if (y->left != _end) {
                y->left->parent = x;
            }
            y->parent = x->parent;
            if (x->parent == NULL) {
                this->root = y;
            } else if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
            y->left = x;
            x->parent = y;
        }

        // rotate right at node x
        void rightRotate(NodePtr x) {
            NodePtr y = x->left;
            x->left = y->right;
            if (y->right != _end) {
                y->right->parent = x;
            }
            y->parent = x->parent;
            if (x->parent == NULL) {
                this->root = y;
            } else if (x == x->parent->right) {
                x->parent->right = y;
            } else {
                x->parent->left = y;
            }
            y->right = x;
            x->parent = y;
        }

        // insert the val to the tree in its appropriate position
        // and fix the tree
        int insert(const T &val) {
            // Ordinary Binary Search Insertion
            if (search(val) != _end)
                return 0;
            NodePtr node = newNode(val);
            count++;
            node->left = _end;
		    node->right = _end;
            node->parent = NULL;
            node->color = RED;
            NodePtr y = NULL;
            NodePtr x = this->root;
            while (x != _end) {
                y = x;
                if (comp(node->data, x->data)) {
                    x = x->left;
                } else {
                    x = x->right;
                }
            }
            // y is parent of x
            node->parent = y;
            if (y == NULL) {
                root = node;
            } else if (comp(node->data,y->data)) {
                y->left = node;
            } else {
                y->right = node;
            }

            // if new node is a root node, simply return
            if (node->parent == NULL){
                node->color = BLACK;
                return 1;
            }

            // if the grandparent is null, simply return
            if (node->parent->parent == NULL) {
                return 1;
            }

            // Fix the tree
            fixInsert(node);
            _end->parent = maximum(root);
            return 1;
        }
        
        bool isEqual(const T &l, const T &r) const {
		    return (!comp(l, r) && !comp(r, l));
	    }
    };
}
#undef RED
#undef BLACK
#endif