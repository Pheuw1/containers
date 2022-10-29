#ifndef MAP_H
#define MAP_H
#include "utils.hpp"
#include "pair.hpp"
#include "RB_Tree.hpp"
#include "tree_iterators.hpp"
#include "iterators.hpp"
namespace ft
{
template
    <
    typename Key,
    typename T,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<ft::pair<const Key, T> >
    > 
    class map {
    public:
        typedef Key key_type;
        typedef T mapped_type;
        typedef ft::pair<const Key, T> value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type ;
        typedef Compare key_compare;
        typedef Allocator allocator_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef typename Allocator::pointer pointer;
        typedef typename Allocator::const_pointer const_pointer	;

        class value_compare {
			friend class map;
		public:
			typedef bool result_type;
			typedef value_type first_argument_type;
			typedef value_type second_argument_type;
			bool operator() (const value_type &x, const value_type &y) const {return comp(x.first, y.first);}
            value_compare(const key_compare &c = Compare()) : comp(c) {}
			value_compare(const value_compare &c) : comp(c.comp) {}
		protected:
			Compare comp;
		};
    public:
        typedef RB_Tree<value_type, value_compare, Allocator>  Tree;
		typedef typename Tree::iterator iterator;
		typedef typename Tree::const_iterator const_iterator;
		typedef ft::reverse_iterator<iterator> reverse_iterator;
		typedef ft::reverse_iterator<const_iterator> const_reverse_iterator;	
    
        Tree                             tree;
    private:
        allocator_type                   alloc;
        key_compare                      k_comp;
        value_compare                    v_comp;
    
    public:

		explicit map(const key_compare comp = key_compare(), const allocator_type &alloc = allocator_type())
		:  tree(Tree(value_compare(comp))),alloc(alloc),k_comp(comp),v_comp(comp) {}		

		template <class InputIterator>
  		map (InputIterator first, InputIterator last, const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
		:  tree(Tree(value_compare(comp))),alloc(alloc),k_comp(comp),v_comp(comp) {		
			insert(first, last);
        }

		map(const map &cpy)
		:  tree(value_compare(cpy.k_comp)),alloc(cpy.alloc),k_comp(cpy.k_comp),v_comp(cpy.v_comp) {		
			this->insert(cpy.begin(), cpy.end());
		}

		map& operator=(const map& cpy) { 
			k_comp = cpy.k_omp;
			v_comp = cpy.v_comp;
			this->clear();
			this->insert(cpy.begin(), cpy.end());
			return *this;
		}
        
        ~map()
        {}

        allocator_type &get_allocator(void) {return alloc;}
       
        //Element access
        T &at(Key &k) {
            RB_Node<value_type> *tmp = tree.search(k);
            if (!tmp) 
                throw std::out_of_range("map");
            return tmp->val;
        }

        T &operator[](const Key &k){
            return (*((insert(ft::make_pair(k,mapped_type()))).first)).second ;
        }
        
        //Iterators
        iterator begin() {
            return (iterator(tree.minimum(tree.root),tree._end));
        }
        const_iterator begin() const {
            return (const_iterator(tree.minimum(tree.root),tree._end));
        }
        iterator end() {
            tree._end ->parent = tree.maximum(tree.root);
            return (iterator(tree._end, tree._end));
        }
        const_iterator end() const {
            tree._end->parent  = tree.maximum(tree.root);
            return (const_iterator(tree._end,tree._end));
        }
		reverse_iterator rbegin() {return reverse_iterator(end());}
		const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
		reverse_iterator rend() {return reverse_iterator(begin());}
		const_reverse_iterator rend() const {return const_reverse_iterator(begin());}

        //Capacity
        bool empty() const {
            return !tree.size();
        }

        size_type size() const {
            return tree.size();
        }

        size_type max_size() const{
            return alloc.max_size();
        }

        //Modifiers
        void clear() {tree.clear();}
        
        pair<iterator, bool> insert(const value_type& val ) {
            bool b = tree.insert(val);
            return ft::make_pair(iterator(tree.search(val), tree._end), b);
        }
    
        iterator insert( iterator pos, const value_type& value ) {
            (void)pos;
            tree.insert(value);
            return iterator(tree.search(value), tree._end);
        }
        
        template< class InputIt>
        void insert( InputIt first, InputIt last ) {
            while (first != last) insert(*first++);
        }

        void erase(iterator pos) {
                tree.erase(*pos);    
        }
        void erase( iterator first, iterator last ) {
                while (first != last) erase(first++);
        }

        size_type erase( const Key& key )  {
            return tree.erase(make_pair(key, mapped_type()));
        }

        void swap(map& other ) {
			swap(tree.root,  other.tree.root);
            swap(tree.count, other.tree.count);
            swap(tree.end, other.tree.end);
            swap(tree.alloc, other.tree.alloc);
            swap(tree.comp, other.tree.comp);
        }
        
		friend void swap (map &x, map &y) {
			x.swap(y);
		}

        size_type count( const Key& key ) const {return (tree.search(ft::make_pair(key,mapped_type())) != tree._end);}

        iterator find( const Key& key ) {return iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end);}
        
        const_iterator find( const Key& key ) const {return const_iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end);}

        iterator lower_bound( const Key& key ) {
            return (iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end));
        }

        const_iterator lower_bound( const Key& key ) const {
            return (const_iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end));
        }
        
        iterator upper_bound( const Key& key ) {
            return (++iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end));
        }
        const_iterator upper_bound( const Key& key ) const {
            return (++const_iterator(tree.search(ft::make_pair(key, mapped_type())),tree._end));
        }
        
        pair<iterator,iterator> equal_range( const Key& key ) {
            return (ft::make_pair(lower_bound(key),upper_bound(key)));
        }

        pair<const_iterator,const_iterator> equal_range( const Key& key ) const {
            return (ft::make_pair(lower_bound(key),upper_bound(key)));
        }
        //Observers
        key_compare key_comp() const {return key_compare();}

        value_compare value_comp() const {return value_compare();}

        //Non-member functions
        friend bool operator==( const map& lhs, const map& rhs ) {
            iterator l(lhs.tree.root);
            iterator r(rhs.tree.root);
            while (l != l.end() && r != r.end()) {
                if (l != r) return (0);
                l++;r++;
            }
            return l.end() == r.end() ? 1 : 0; 
        }

        friend bool operator!=( const map& lhs, const map& rhs ) {
            return !(lhs == rhs);
        }
        friend bool operator<( const map& lhs,const map& rhs ) {
            return lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(), rhs.end());
        }
        friend bool operator<=( const map& lhs, const map& rhs ) {
            return lhs == rhs || lhs < rhs;
        }
        friend bool operator>( const map& lhs,const map& rhs ) {
            return lexicographical_compare(rhs.begin(),rhs.end(),lhs.begin(), lhs.end());
        }
        friend bool operator>=( const map& lhs, const map& rhs ) {
            return lhs == rhs || lhs > rhs;

        }

    };



}


#endif 