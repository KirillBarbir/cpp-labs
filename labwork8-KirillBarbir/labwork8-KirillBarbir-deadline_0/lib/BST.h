#pragma once
#include<cstdint>
#include<memory>

template<typename Type>
struct Node {
    Type val;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
};

template<typename Type>
class Compare {
  public:
    bool operator()(const Type& lhs, const Type& rhs) const {
        return lhs > rhs;
    }
};

struct inorder_tag {};

struct postorder_tag {};

struct preorder_tag {};

template<typename Type, typename allocator = std::allocator<Type>, typename Compare = Compare<Type>>
class BST {
  public:
    // CONTAINER REQUIREMENTS****************************************************************************8
    using value_type = Type;
    using reference = Type&;
    using const_reference = const Type&;
    using NodeAlloc = std::allocator_traits<allocator>::template rebind_alloc<Node<Type>>;

    template<typename traversal_tag = inorder_tag>
    class MyIterator {
        friend BST;
      public:

        MyIterator() {
            end_ghost = nullptr;
        }

        MyIterator(Node<value_type>*& ghost) {
            end_ghost = ghost;
        }

        MyIterator(const MyIterator& rhs) {
            current_bst = rhs.current_bst;
            end_ghost = rhs.end_ghost;
        }

        MyIterator& operator=(MyIterator rhs) {
            current_bst = rhs.current_bst;
            end_ghost = rhs.end_ghost;
            return *this;
        }

        value_type* operator->() const {
            return &(this->current_bst->val);
        }

        MyIterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        MyIterator operator--(int) {
            auto copy = *this;
            --(*this);
            return copy;
        }

        ~MyIterator() = default;

        bool operator==(const MyIterator& rhs) {
            return current_bst == rhs.current_bst;
        }

        bool operator!=(const MyIterator& rhs) {
            return (*this == rhs) ^ true;
        }

        MyIterator& operator++() {
            if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
                PreorderIncrement_();
            }
            if constexpr (std::is_same<traversal_tag, inorder_tag>::value) {
                InorderIncrement_();
            }
            if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
                PostorderIncrement_();
            }
            return *this;
        }

        MyIterator& operator--() {
            if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
                PreorderDecrement_();
            }
            if constexpr (std::is_same<traversal_tag, inorder_tag>::value) {
                InorderDecrement_();
            }
            if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
                PostorderDecrement_();
            }
            return *this;
        }

        const_reference operator*() {
            return current_bst->val;
        }

      private:
        Node<value_type>* current_bst = nullptr;
        Node<value_type>* end_ghost = nullptr;

        void PreorderIncrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->left != nullptr) {
                current_bst = current_bst->left;
                return;
            }
            if (current_bst->right != nullptr) {
                current_bst = current_bst->right;
                return;
            }
            if (current_bst->parent == nullptr) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->parent->left == current_bst && current_bst->parent->right != nullptr) {
                current_bst = current_bst->parent->right;
                return;
            }
            while (current_bst->parent != nullptr
                && (current_bst->parent->left != current_bst || current_bst->parent->right == nullptr)) {
                current_bst = current_bst->parent;
            }

            if (current_bst->parent != nullptr && current_bst->parent->left == current_bst
                && current_bst->parent->right != nullptr) {
                current_bst = current_bst->parent->right;
                return;
            }
            current_bst = end_ghost;
        }

        void PreorderDecrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = current_bst->parent;
                return;
            }

            if (current_bst->parent == nullptr) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->parent->right == current_bst && current_bst->parent->left != nullptr) {
                current_bst = current_bst->parent->left;
                current_bst = GetRightmostLeaf_(current_bst);
                return;
            }

            current_bst = current_bst->parent;
        }

        void InorderIncrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->right != nullptr) {
                current_bst = current_bst->right;
                while (current_bst->left != nullptr) {
                    current_bst = current_bst->left;
                }
                return;
            }

            while (current_bst->parent != nullptr && current_bst->parent->left != current_bst) {
                current_bst = current_bst->parent;
            }

            if (current_bst->parent != nullptr && current_bst->parent->left == current_bst) {
                current_bst = current_bst->parent;
                return;
            }

            current_bst = end_ghost;
        }

        void InorderDecrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = current_bst->parent;
                return;
            }
            if (current_bst->left != nullptr) {
                current_bst = current_bst->left;
                while (current_bst->right != nullptr) {
                    current_bst = current_bst->right;
                }
                return;
            }

            while (current_bst->parent != nullptr && current_bst->parent->right != current_bst) {
                current_bst = current_bst->parent;
            }

            if (current_bst->parent != nullptr && current_bst->parent->right == current_bst) {
                current_bst = current_bst->parent;
                return;
            }
            current_bst = end_ghost;
        }

        void PostorderIncrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->parent == nullptr) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->parent->right == current_bst) {
                current_bst = current_bst->parent;
                return;
            }
            if (current_bst->parent->right != nullptr) {
                current_bst = GetLeftmostLeaf_(current_bst->parent->right);
                return;
            }
            current_bst = current_bst->parent;
        }

        void PostorderDecrement_() {
            if (current_bst == nullptr) {
                return;
            }
            if (current_bst->right == current_bst) {
                current_bst = current_bst->parent;
                return;
            }
            if (current_bst->right != nullptr) {
                current_bst = current_bst->right;
                return;
            }
            if (current_bst->left != nullptr) {
                current_bst = current_bst->left;
                return;
            }
            if (current_bst->parent == nullptr) {
                current_bst = end_ghost;
                return;
            }
            if (current_bst->parent->right == current_bst && current_bst->parent->left != nullptr) {
                current_bst = current_bst->parent->left;
                return;
            }

            while (current_bst->parent != nullptr
                && (current_bst->parent->right != current_bst || current_bst->parent->left == nullptr)) {
                current_bst = current_bst->parent;
            }

            if (current_bst->parent != nullptr && current_bst->parent->right == current_bst
                && current_bst->parent->left != nullptr) {
                current_bst = current_bst->parent->left;
                return;
            }
            current_bst = end_ghost;
        }
    };

    template<typename traversal_tag = inorder_tag>
    using iterator = MyIterator<traversal_tag>;

    template<typename traversal_tag = inorder_tag>
    using const_iterator = MyIterator<traversal_tag>;

    template<typename traversal_tag = inorder_tag>
    class MyReverseIterator {
        friend class MyIterator<traversal_tag>;

        friend class BST<value_type, allocator, Compare>;

      public:
        MyReverseIterator(iterator<traversal_tag> it) {
            it_ = it;
        }

        ~MyReverseIterator() = default;

        MyReverseIterator(const MyReverseIterator& rhs) {
            it_ = rhs.it_;
        }

        MyReverseIterator& operator=(MyReverseIterator rhs) {
            it_ = rhs.it_;
        }

        value_type* operator->() const {
            return &(it_->current_bst->val);
        }

        MyReverseIterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        MyReverseIterator operator--(int) {
            auto copy = *this;
            --(*this);
            return copy;
        }

        bool operator==(const iterator<traversal_tag>& rhs) {
            return it_ == it_;
        }

        bool operator!=(const iterator<traversal_tag>& rhs) {
            return (this == rhs) ^ true;
        }

        MyReverseIterator& operator++() {
            if (it_.current_bst != nullptr && it_.current_bst->right == it_.current_bst) {
                return *this;
            }
            --it_;
            return *this;
        }

        MyReverseIterator& operator--() {
            if (it_.current_bst != nullptr && it_.current_bst->right == it_.current_bst) {
                it_.current_bst = it_.current_bst->left;
                return *this;
            }
            ++it_;
            return *this;
        }

        const_reference operator*() {
            return *it_;
        }
      private:
        iterator<traversal_tag> it_;
    };

    template<typename traversal_tag = inorder_tag>
    using reverse_iterator = MyReverseIterator<traversal_tag>;

    template<typename traversal_tag = inorder_tag>
    using const_reverse_iterator = MyReverseIterator<traversal_tag>;

    class Eq {
        friend BST;
      public:
        bool IsEqual(value_type lhs, value_type rhs) {
            return lhs == rhs;
        }
    };

    class Tran {
        friend BST;
      public:
        bool IsEqual(value_type lhs, value_type rhs) {
            return !Compare{}(lhs, rhs) && !Compare{}(rhs, lhs);
        }

    };

    using CompareClass = std::conditional<requires(){ typename Compare::is_transparent; }, Tran, Eq>::type;

    BST() {
        AllocateGhosts();
    }

    BST(const BST& rhs) {
        AllocateGhosts();
        if (rhs.root == nullptr) {
            this->root = nullptr;
            return;
        }
        Node<value_type>* to_add = node_allocator_.allocate(1);
        to_add = std::construct_at(to_add);
        to_add->val = rhs.root->val;
        root = to_add;
        size_ = rhs.size_;
        CopyRecursion_(this->root, rhs.root);
        PreorderGhostUpdate();
        InorderGhostUpdate();
        PostorderGhostUpdate();
    }

    BST& operator=(const BST& rhs) {
        if (*this == rhs) {
            return *this;
        }
        (*this).~BST();
        AllocateGhosts();
        if (rhs.root == nullptr) {
            this->root = nullptr;
            return *this;
        }
        if (std::allocator_traits<decltype(rhs.node_allocator_)>::propagate_on_container_copy_assignment::value) {
            this->node_allocator_ = rhs.node_allocator_;
        }
        size_ = rhs.size_;
        Node<value_type>* to_add = node_allocator_.allocate(1);
        to_add = std::construct_at(to_add);;
        to_add->val = rhs.root->val;
        root = to_add;
        CopyRecursion_(this->root, rhs.root);
        PostorderGhostUpdate();
        InorderGhostUpdate();
        PreorderGhostUpdate();
        return *this;
    }

    ~BST() {
        ClearRecursion_(root);
        node_allocator_.deallocate(postorder_ghost, 1);
        node_allocator_.deallocate(preorder_ghost, 1);
        node_allocator_.deallocate(inorder_ghost, 1);
    }

    bool operator==(const BST& rhs) {
        return CompareRecursion_(root, rhs.root);
    }

    bool operator!=(const BST& rhs) {
        return (*this == rhs) ^ true;
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> begin() {
        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> beg(preorder_ghost);
            beg.current_bst = preorder_ghost->left;
            return beg;
        }

        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> beg(postorder_ghost);
            beg.current_bst = postorder_ghost->left;
            return beg;
        }
        iterator<traversal_tag> beg(inorder_ghost);
        beg.current_bst = inorder_ghost->left;
        return beg;
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> end() {
        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> end(preorder_ghost);
            end.current_bst = preorder_ghost;
            return end;
        }
        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> end(postorder_ghost);
            end.current_bst = postorder_ghost;
            return end;
        }
        iterator<traversal_tag> end(inorder_ghost);
        end.current_bst = inorder_ghost;
        return end;
    }

    template<typename traversal_tag = inorder_tag>
    const_iterator<traversal_tag> cbegin() {
        return begin<traversal_tag>();
    }

    template<typename traversal_tag = inorder_tag>
    const_iterator<traversal_tag> cend() {
        return end<traversal_tag>();
    }

    template<typename traversal_tag = inorder_tag>
    reverse_iterator<traversal_tag> rbegin() {
        iterator<traversal_tag> it = end<traversal_tag>();
        --it;
        reverse_iterator<traversal_tag> out(it);
        return out;
    }

    template<typename traversal_tag = inorder_tag>
    reverse_iterator<traversal_tag> rend() {
        iterator<traversal_tag> it = end<traversal_tag>();
        reverse_iterator<traversal_tag> out(it);
        return out;
    }

    template<typename traversal_tag = inorder_tag>
    const_reverse_iterator<traversal_tag> crbegin() {
        return rbegin<traversal_tag>();
    }

    template<typename traversal_tag = inorder_tag>
    const_reverse_iterator<traversal_tag> crend() {
        return rend<traversal_tag>();
    }

    void swap(BST& rhs) {
        BST temp = *this;
        *this = rhs;
        rhs = temp;
        if (!std::allocator_traits<allocator>::propagate_on_container_swap::value) {
            swap(value_allocator_, rhs.value_allocator_);
        }
    }

    static void swap(BST& lhs, BST& rhs) {
        lhs.swap(rhs);
    }

    size_t size() {
        return size_;
    }

    size_t max_size() {
        return node_allocator_.max_size();
    }

    bool empty() {
        return size_ == 0;
    }

    // ALLOCATOR AWARE CONTAINER REQUIREMENTS******************************************************************************************
    BST(std::allocator<value_type> m) {
        value_allocator_ = m;
        BST();
    }

    std::allocator<value_type> get_allocator() {
        return value_allocator_;
    }

    // Associative **********************************************************************************

    using key_type = value_type;
    using key_compare = Compare;
    using value_compare = key_compare;
    using node_type = Node<value_type>;

    BST(key_compare c) {
        cmp_ = c;
        AllocateGhosts();
    }

    template<typename traversal_tag = inorder_tag>
    BST(iterator<traversal_tag> i, iterator<traversal_tag> j, key_compare c = key_compare()) {
        cmp_ = c;
        AllocateGhosts();
        while (i != j) {
            this->InsertInterface_(*i);
            ++i;
        }

    }

    BST(std::initializer_list<value_type> il, key_compare c = key_compare()) {
        cmp_ = c;
        AllocateGhosts();
        for (value_type i : il) {
            this->InsertInterface_(i);
        }

    }

    BST& operator=(std::initializer_list<value_type> il) {
        *this.~BST();
        BST<value_type, allocator, Compare> out(il);
        return out;
    }

    key_compare key_comp() {
        return cmp_;
    }

    value_compare value_comp() {
        return cmp_;
    }

    template<typename traversal_tag = inorder_tag>
    void insert(iterator<traversal_tag> i, iterator<traversal_tag> j) {
        while (i != j) {
            this->InsertInterface_(*i);
            ++i;
        }
    }

    void insert(std::initializer_list<value_type> il) {
        for (value_type i : il) {
            this->InsertInterface_(i);
        }
    }

    node_type extract(key_type k) {
        Node<value_type>* found = Find_(root, k);
        if (found != nullptr) {
            node_type* temp = node_allocator_.allocate(1);
            temp = node_allocator_.construct_at(temp);
            temp->parent = found->parent;
            temp->left = found->left;
            temp->right = found->right;
            temp->val = found->val;
            Delete_(found->val);
            return temp;
        }
        Node<value_type>* out;
        return out;
    }

    template<typename traversal_tag = inorder_tag>
    node_type extract(const_iterator<traversal_tag> q) {
        if (size_ > 0) {
            --size_;
        }
        node_type* temp = node_allocator_.allocate(1);
        temp = node_allocator_.construct_at(temp);
        temp->parent = q.current_bst->parent;
        temp->left = q.current_bst->left;
        temp->right = q.current_bst->right;
        temp->val = q.current_bst->val;
        Delete_(root, q.current_bst);
        PreorderGhostUpdate();
        InorderGhostUpdate();
        PostorderGhostUpdate();
        return temp;
    }

    void merge(BST<value_type, allocator, Compare> a2) {
        for (int i : a2) {
            node_type x = a2.extract(cbegin());
            x.right = nullptr;
            x.left = nullptr;
            x.parent = nullptr;
            node_type* to_add = &x;
            ++size_;
            if (Find_(root, x.val) != nullptr) {
                continue;
            }
            RecursiveInsert_(root, to_add);
            PreorderGhostUpdate();
            InorderGhostUpdate();
            PostorderGhostUpdate();
        }
    }

    size_t erase(key_type k) {
        if (Find_(root, k) != nullptr) {
            DeleteInterface_(k);
            return 1;
        }
        return 0;
    }

    template<typename traversal_tag = inorder_tag>

    iterator<traversal_tag> erase(const_iterator<traversal_tag> q) {
        const_iterator<traversal_tag> temp = q;
        ++temp;
        DeleteInterface_(*temp);
        if (size_ > 0) {
            --size_;
        }
        PreorderGhostUpdate();
        InorderGhostUpdate();
        PostorderGhostUpdate();
        return temp;
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> erase(const_iterator<traversal_tag> q1, const_iterator<traversal_tag> q2) {
        while (q1 != q2) {
            const_iterator<traversal_tag> temp = q1;
            ++temp;
            erase(q1);
            q1 = temp;
        }
        return q1;
    }

    template<typename traversal_tag = inorder_tag>
    void clear() {
        erase(cbegin<traversal_tag>(), cend<traversal_tag>());
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> find(key_type k) {
        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> it(preorder_ghost);
            it.current_bst = Find_(root, k);
            if (it.current_bst == nullptr) {
                return end<traversal_tag>();
            }
            return it;
        }
        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> it(postorder_ghost);
            it.current_bst = Find_(root, k);
            if (it.current_bst == nullptr) {
                return end<traversal_tag>();
            }
            return it;
        }
        iterator<traversal_tag> it(inorder_ghost);
        it.current_bst = Find_(root, k);
        if (it.current_bst == nullptr) {
            return end<traversal_tag>();
        }
        return it;
    }

    template<typename traversal_tag = inorder_tag>
    bool contains(key_type k) {
        return find<traversal_tag>(k) != end<traversal_tag>();
    }

    template<typename traversal_tag = inorder_tag>
    size_t count(key_type k) {
        if (contains<traversal_tag>(k)) {
            return 1;
        }
        return 0;
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> lower_bound(key_type k) {
        if (contains<traversal_tag>(k)) {
            return find<traversal_tag>(k);
        }
        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> it(preorder_ghost);
            it.current_bst = Prev_(root, k);
            if (it.current_bst == nullptr) {
                return preorder_ghost;
            }
            return it;
        }
        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> it(postorder_ghost);
            it.current_bst = Prev_(root, k);
            if (it.current_bst == nullptr) {
                return postorder_ghost;
            }
            return it;
        }
        iterator<traversal_tag> it(inorder_ghost);
        it.current_bst = Prev_(root, k);
        if (it.current_bst == nullptr) {
            return inorder_ghost;
        }
        return it;
    }

    template<typename traversal_tag = inorder_tag>
    iterator<traversal_tag> upper_bound(key_type k) {
        if (contains(k)) {
            return find(k);
        }

        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> it(preorder_ghost);
            it.current_bst = Next_(root, k);
            if (it.current_bst == nullptr) {
                return preorder_ghost;
            }
            return it;
        }
        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> it(postorder_ghost);
            it.current_bst = Next_(root, k);
            if (it.current_bst == nullptr) {
                return postorder_ghost;
            }
            return it;
        }
        iterator<traversal_tag> it(inorder_ghost);
        it.current_bst = Next_(root, k);
        if (it.current_bst == nullptr) {
            return inorder_ghost;
        }
        return it;
    }

    template<typename traversal_tag = inorder_tag>
    std::pair<iterator<traversal_tag>, iterator<traversal_tag>> equal_range(key_type k) {
        return std::make_pair(lower_bound<traversal_tag>(k), upper_bound<traversal_tag>(k));
    }

    template<typename traversal_tag = inorder_tag>
    std::pair<iterator<traversal_tag>, bool> insert(key_type t) {
        if (Find_(root, t) != nullptr) {
            if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
                iterator<traversal_tag> it(preorder_ghost);
                it.current_bst = Find_(root, t);
                return std::make_pair(it, false);
            }
            if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
                iterator<traversal_tag> it(postorder_ghost);
                it.current_bst = Find_(root, t);
                return std::make_pair(it, false);
            }
            iterator<traversal_tag> it(inorder_ghost);
            it.current_bst = Find_(root, t);
            return std::make_pair(it, false);
        }
        InsertInterface_(t);
        if constexpr (std::is_same<traversal_tag, preorder_tag>::value) {
            iterator<traversal_tag> it(preorder_ghost);
            it.current_bst = Find_(root, t);
            return std::make_pair(it, false);
        }
        if constexpr (std::is_same<traversal_tag, postorder_tag>::value) {
            iterator<traversal_tag> it(postorder_ghost);
            it.current_bst = Find_(root, t);
            return std::make_pair(it, false);
        }
        iterator<traversal_tag> it(inorder_ghost);
        it.current_bst = Find_(root, t);
        return std::make_pair(it, false);
    }

  private:
    int64_t size_ = 0;
    key_compare cmp_ = key_compare();
    Node<value_type>* preorder_ghost;
    Node<value_type>* inorder_ghost;
    Node<value_type>* postorder_ghost;

    Node<value_type>* root = nullptr;
    NodeAlloc node_allocator_;
    allocator value_allocator_;

    void InsertInterface_(value_type val_to_add) {
        ++size_;
        if (Find_(root, val_to_add) != nullptr) {
            return;
        }
        Node<value_type>* to_add = node_allocator_.allocate(1);
        to_add = std::construct_at(to_add);
        to_add->val = val_to_add;
        RecursiveInsert_(root, to_add);
        PreorderGhostUpdate();
        InorderGhostUpdate();
        PostorderGhostUpdate();
    }

    void DeleteInterface_(value_type value) {
        if (size_ > 0) {
            --size_;
        }
        Node<value_type>* found = Find_(root, value);
        Delete_(root, found);
        PreorderGhostUpdate();
        InorderGhostUpdate();
        PostorderGhostUpdate();
    }

    static Node<value_type>* GetMax_(Node<value_type>*& current_bst) {
        if (current_bst == nullptr) {
            return nullptr;
        }
        if (current_bst->right == current_bst) {
            return current_bst;
        }
        if (current_bst->right != nullptr) {
            return GetMax_(current_bst->right);
        }
        return current_bst;
    }

    static Node<value_type>* GetMin_(Node<value_type>*& current_bst) {
        if (current_bst == nullptr) {
            return nullptr;
        }
        if (current_bst->right == current_bst) {
            return current_bst;
        }
        if (current_bst->left != nullptr) {
            return GetMin_(current_bst->left);
        }
        return current_bst;
    }

    static Node<value_type>* GetLeftmostLeaf_(Node<value_type>*& current_bst) {
        if (current_bst == nullptr) {
            return nullptr;
        }
        if (current_bst->right == current_bst) {
            return current_bst;
        }
        if (current_bst->left == nullptr && current_bst->right == nullptr) {
            return current_bst;
        }
        if (current_bst->left != nullptr) {
            return GetLeftmostLeaf_(current_bst->left);
        }
        if (current_bst->right != nullptr) {
            return GetLeftmostLeaf_(current_bst->right);
        }
        return nullptr;
    }

    static Node<value_type>* GetRightmostLeaf_(Node<value_type>*& current_bst) {
        if (current_bst == nullptr) {
            return nullptr;
        }
        if (current_bst->right == current_bst) {
            return current_bst;
        }
        if (current_bst->left == nullptr && current_bst->right == nullptr) {
            return current_bst;
        }
        if (current_bst->right != nullptr) {
            return GetRightmostLeaf_(current_bst->right);
        }
        if (current_bst->left != nullptr) {
            return GetRightmostLeaf_(current_bst->left);
        }
        return nullptr;
    }

    static Node<value_type>* Next_(Node<value_type>*& bst, value_type value) {
        if (bst == nullptr) return nullptr;
        if (value < bst->val) {
            if (GetMax_(bst->left) != nullptr && value < GetMax_(bst->left)->val) {
                return Next_(bst->left, value);
            } else {
                return bst;
            }
        } else {
            if (GetMax_(bst->right) != nullptr && value < GetMax_(bst->right)->val) {
                return Next_(bst->right, value);
            } else {
                return nullptr;
            }
        }
    }

    static Node<value_type>* Prev_(Node<value_type>*& bst, value_type value) {
        if (bst == nullptr) return nullptr;
        if (value <= bst->val) {
            if (GetMin_(bst->left) != nullptr && value > GetMin_(bst->left)->val) {
                return Prev_(bst->left, value);
            } else {
                return nullptr;
            }
        } else {
            if (GetMin_(bst->right) != nullptr && value > GetMin_(bst->right)->val) {
                return Prev_(bst->right, value);
            } else {
                return bst;
            }
        }
    }

    Node<value_type>* Find_(Node<value_type>*& current_bst, value_type value) {
        if (current_bst == nullptr) return nullptr;
        if (CompareClass{}.IsEqual(current_bst->val, value)) return current_bst;
        else if (cmp_(value, current_bst->val)) {
            return Find_(current_bst->right, value);
        } else {
            return Find_(current_bst->left, value);
        }
    }

    void RecursiveInsert_(Node<value_type>*& current_bst, Node<value_type>*& to_add) {
        if (current_bst == nullptr) {
            current_bst = to_add;
            return;
        }
        if (cmp_(to_add->val, current_bst->val)) {
            to_add->parent = current_bst;
            RecursiveInsert_(current_bst->right, to_add);
        } else if (!cmp_(to_add->val, current_bst->val)) {
            to_add->parent = current_bst;
            RecursiveInsert_(current_bst->left, to_add);
        }
    }

    bool CompareRecursion_(Node<value_type>*& lhs, Node<value_type>* rhs) {
        if (lhs == nullptr && rhs == nullptr) {
            return true;
        }
        if (lhs == nullptr || rhs == nullptr) {
            return false;
        }
        if (lhs->val != rhs->val) {
            return false;
        }
        bool OK = true;
        if (lhs->left != nullptr && rhs->left != nullptr) {
            OK &= CompareRecursion_(lhs->left, rhs->left);
        }
        if (lhs->right != nullptr && rhs->right != nullptr) {
            OK &= CompareRecursion_(lhs->right, rhs->right);
        }
        if (lhs->left == nullptr && rhs->left == nullptr) {
            OK &= true;
        }
        if (lhs->right == nullptr && rhs->right == nullptr) {
            OK &= true;
        }
        if (lhs->left == nullptr ^ rhs->left == nullptr) {
            return false;
        }
        if (lhs->right == nullptr ^ rhs->right == nullptr) {
            return false;
        }
        return OK;
    }

    void CopyRecursion_(Node<value_type>*& lhs, Node<value_type>* rhs) {
        if (rhs->left != nullptr) {
            Node<value_type>* to_add = node_allocator_.allocate(1);
            to_add = std::construct_at(to_add);
            to_add->val = rhs->left->val;
            to_add->parent = lhs;
            lhs->left = to_add;
            CopyRecursion_(lhs->left, rhs->left);
        }
        if (rhs->right != nullptr) {
            Node<value_type>* to_add = node_allocator_.allocate(1);
            to_add = std::construct_at(to_add);
            to_add->val = rhs->right->val;
            to_add->parent = lhs;
            lhs->right = to_add;
            CopyRecursion_(lhs->right, rhs->right);
        }
    }

    void ClearRecursion_(Node<value_type>*& lhs) {
        if (lhs == nullptr) {
            return;
        }
        if (lhs->left != nullptr) {
            ClearRecursion_(lhs->left);
        }
        if (lhs->right != nullptr) {
            ClearRecursion_(lhs->right);
        }
        node_allocator_.deallocate(lhs, 1);
    }

    void Delete_(Node<value_type>*& current_bst, Node<value_type>*& found) {
        if (current_bst == nullptr || found == nullptr) {
            return;
        }
        if (found->left == nullptr && found->right == nullptr) {
            if (found->parent == nullptr) {
                current_bst = nullptr;
                found = nullptr;
                return;
            }

            if (found->parent->left == found) {
                found->parent->left = nullptr;
                node_allocator_.deallocate(found, 1);
            } else {
                found->parent->right = nullptr;
                node_allocator_.deallocate(found, 1);
            }
        } else if (found->left != nullptr && found->right != nullptr) {
            Node<value_type>* nt = Next_(found, found->val);
            found->val = nt->val;
            if (nt->parent->left == nt) {
                nt->parent->left = nt->right;
                if (nt->right != nullptr) {
                    nt->right->parent = nt->parent;
                }
            } else {
                nt->parent->right = nt->right;
                if (nt->right != nullptr) {
                    nt->right->parent = nt->parent;
                }
            }
            node_allocator_.deallocate(nt, 1);
        } else {
            if (found->parent == nullptr) {
                if (found->left == nullptr) {
                    found->right->parent = nullptr;
                    current_bst = found->right;
                    node_allocator_.deallocate(found, 1);
                } else {
                    found->left->parent = nullptr;
                    current_bst = found->left;
                    node_allocator_.deallocate(found, 1);
                }
                return;
            }
            if (found->parent->left == found) {
                if (found->left == nullptr) {
                    found->parent->left = found->right;
                    found->right->parent = found->parent;
                } else if (found->right == nullptr) {
                    found->parent->left = found->left;
                    found->left->parent = found->parent;
                }
                node_allocator_.deallocate(found, 1);
            } else {
                if (found->left == nullptr) {
                    found->parent->right = found->right;
                    found->right->parent = found->parent;
                } else if (found->right == nullptr) {
                    found->parent->right = found->left;
                    found->left->parent = found->parent;
                }
                node_allocator_.deallocate(found, 1);
            }
        }
    }

    void AllocateGhosts() {
        preorder_ghost = node_allocator_.allocate(1);
        preorder_ghost->right = preorder_ghost;
        inorder_ghost = node_allocator_.allocate(1);
        inorder_ghost->right = inorder_ghost;
        postorder_ghost = node_allocator_.allocate(1);
        postorder_ghost->right = postorder_ghost;
    }

    void PreorderGhostUpdate() {
        preorder_ghost->left = root;
        preorder_ghost->parent = GetMax_(root);
    }

    void InorderGhostUpdate() {
        inorder_ghost->left = GetMin_(root);
        inorder_ghost->parent = GetMax_(root);
    }

    void PostorderGhostUpdate() {
        postorder_ghost->left = GetLeftmostLeaf_(root);
        postorder_ghost->parent = root;
    }
};
