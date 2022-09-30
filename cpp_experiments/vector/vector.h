#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

template <typename T>
class vector_iterator {
public:
	// iterator traits
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = size_t;
	using pointer = value_type*;
	using reference = value_type&;

public:
	vector_iterator(pointer ptr) : ptr(ptr) {}

	vector_iterator operator++(int) {
		auto temp = *this;
		++* this;
		return temp;
	}
	vector_iterator& operator++() {
		++ptr;
		return *this;
	}

	vector_iterator operator--(int) {
		auto temp = *this;
		--* this;
		return temp;
	}
	vector_iterator& operator--() {
		--ptr;
		return *this;
	}

	vector_iterator& operator+=(const difference_type n) {
		ptr += n;
		return *this;
	}
	vector_iterator& operator-=(const difference_type n) {
		ptr -= n;
		return *this;
	}

	reference operator*() const noexcept {
		return *ptr;
	}
	pointer operator->() const noexcept {
		return ptr;
	}

	reference operator[](const difference_type n) const noexcept {
		return *(*this + n);
	}

	vector_iterator operator+(const difference_type n) const noexcept {
		auto temp = *this;
		temp += n;
		return temp;
	}

	vector_iterator operator-(const difference_type n) const noexcept {
		auto temp = *this;
		temp -= n;
		return temp;
	}
	difference_type operator-(const vector_iterator& rhs) const noexcept {
		return ptr - rhs.ptr;
	}

	bool operator==(const vector_iterator& rhs) const noexcept {
		return ptr == rhs.ptr;
	}
	bool operator!=(const vector_iterator& rhs) const noexcept {
		return !((*this) == rhs);
	}
	bool operator<(const vector_iterator& rhs) const noexcept {
		return rhs - *this > 0;
	}
	bool operator>(const vector_iterator& rhs) const noexcept {
		return rhs < *this;
	}
	bool operator<=(const vector_iterator& rhs) const noexcept {
		return !(*this > rhs);
	}
	bool operator>=(const vector_iterator& rhs) const noexcept {
		return !(*this < rhs);
	}

	pointer ptr;
};

template <typename T, typename Alloc = std::allocator<T>>
class vector {
public:
	// using
	using size_type = size_t;
	using allocator_type = Alloc;
	using allocator_traits = std::allocator_traits<Alloc>;
	using value_type = T;
	using pointer = typename allocator_traits::pointer;
	using reference = value_type&;
	using const_pointer = typename allocator_traits::const_pointer;
	using const_reference = const value_type&;

	using iterator = vector_iterator<T>;

public:
	// methods
	vector(const allocator_type& allocator = allocator_type()) : 
		arr(nullptr), sz(0), cap(0), alloc(allocator) {}

	explicit vector(size_type count, const_reference value = value_type(), const allocator_type& allocator = allocator_type())
		: arr(nullptr), sz(count), cap(count), alloc(allocator)
	{
		arr = allocator_traits::allocate(alloc, sz);
		for (size_t i = 0; i < sz; ++i) {
			try {
				allocator_traits::construct(alloc, arr + i, value);
			}
			catch (...) {
				sz = i; // real number of constructed elements
				throw;
			}
		}
	}

	vector(const vector& other) : arr(nullptr), sz(other.sz), cap(other.cap), alloc(other.alloc) {
		arr = allocator_traits::allocate(alloc, sz);
		for (size_t i = 0; i < sz; ++i) {
			try {
				allocator_traits::construct(alloc, arr + i, other.arr[i]);
			}
			catch (...) {
				sz = i; // real number of constructed elements
				throw;
			}
		}
	}

	vector(const vector& other, const allocator_type& alloc) : arr(nullptr), sz(other.sz), cap(other.cap), alloc(alloc) {
		arr = allocator_traits::allocate(alloc, sz);
		for (size_t i = 0; i < sz; ++i) {
			try {
				allocator_traits::construct(alloc, arr + i, other.arr[i]);
			}
			catch (...) {
				sz = i; // real number of constructed elements
				throw;
			}
		}
	}

	vector(vector&& other) noexcept : arr(other.arr), sz(other.sz), cap(other.cap), alloc(other.alloc) {
		other.arr = nullptr;
		other.sz = 0;
		other.cap = 0;
	}

	vector(vector&& other, const allocator_type& alloc) noexcept 
		: arr(other.arr), sz(other.sz), cap(other.cap), alloc(alloc)
	{
		other.arr = nullptr;
		other.sz = 0;
		other.cap = 0;
	}

	template <typename InputIt>
	vector(InputIt first, InputIt last, const allocator_type& allocator = allocator_type())
		: arr(nullptr), sz(0), cap(0), alloc(allocator)
	{
		auto dst = std::distance(first, last);
		if (dst <= 0)
			return;

		sz = cap = dst;
		arr = allocator_traits::allocate(alloc, cap);

		for (size_t i = 0; i < sz; ++i, ++first) {
			try {
				allocator_traits::construct(alloc, arr + i, *first);
			}
			catch (...) {
				sz = i; // real number of constructed elements
				throw;
			}
		}

	}

	vector(const std::initializer_list<value_type> init, const allocator_type& allocator = allocator_type()) 
	: vector(init.begin(), init.end(), allocator) {}

	vector& operator=(const vector& other) {
		if (arr == other.arr) // self assignment
			return *this;

		// Delete old
		clear();
		allocator_traits::deallocate(alloc, arr, cap);

		// Copy from other
		sz = other.sz;
		cap = other.cap;
		alloc = other.alloc;

		arr = allocator_traits::allocate(alloc, cap);
		for (size_t i = 0; i < sz; ++i) {
			try {
				allocator_traits::construct(alloc, arr + i, other.arr[i]);
			}
			catch (...) {
				sz = i; // real number of constructed elements
				throw;
			}
		}

		return *this;
	}

	vector& operator=(vector&& other) noexcept {
		if (arr == other.arr) // self assignment
			return *this;

		// Delete old
		clear();
		allocator_traits::deallocate(alloc, arr, cap);

		// Move from other
		arr = other.arr;
		sz = other.sz;
		cap = other.cap;
		alloc = other.alloc;

		other.arr = nullptr;
		other.sz = 0;
		other.cap = 0;

		return *this;
	}

	void clear() { // Destroy all elements, capacity does not change
		for (size_t i = 0; i < sz; ++i) {
			allocator_traits::destroy(alloc, arr + i);
		}
		sz = 0;
	}

	~vector() {
		clear();
		allocator_traits::deallocate(alloc, arr, cap);
	}

	iterator begin() noexcept {
		return iterator(arr);
	}

	iterator end() noexcept {
		return iterator(arr + sz);
	}


	size_type size() const noexcept {
		return sz;
	}
	size_type capacity() const noexcept {
		return cap;
	}

	reference operator[](size_type pos) {
		return arr[pos];
	}
	const_reference operator[](size_type pos) const {
		return arr[pos];
	}

	reference at(size_type pos) {
		if (!(pos < sz))
			throw std::out_of_range("Invalid position");
		return arr[pos];
	}
	const_reference at(size_type pos) const {
		if (!(pos < sz))
			throw std::out_of_range("Invalid position");
		return arr[pos];
	}

	void reserve(size_type new_capacity) {
		if (new_capacity <= cap)
			return;

		reallocate(new_capacity);
	}

	template <typename ...Args>
	void emplace_back(Args&&... args) {
		if (sz == cap)
			if (cap) // cap != 0
				reallocate(cap * 2);
			else  
				reallocate(1);

		allocator_traits::construct(alloc, arr + sz, std::forward<Args>(args)...); // no try-catch
		++sz;
	}

	void push_back(const_reference value) {
		emplace_back(value);
	}
	void push_back(value_type&& value) {
		emplace_back(std::move(value));
	}

	void pop_back() {
		allocator_traits::destroy(alloc, arr + sz - 1);
		--sz;
	}

	void shrink_to_fit() {
		if (sz == cap)
			return;

		reallocate(sz);
	}

	template <typename ...Args>
	iterator emplace(iterator pos, Args&&... args) {
		if (sz == cap) { // need reallocation
			auto iter = begin();

			auto new_arr = allocator_traits::allocate(alloc, cap * 2);
			auto new_arr_iter = iterator(new_arr);

			try {
				while (iter != pos) { // move ctors before pos
					allocator_traits::construct(alloc, new_arr_iter.ptr, std::move(*iter));
					++new_arr_iter;
					++iter;
				}

				allocator_traits::construct(alloc, new_arr_iter.ptr, std::forward<Args>(args)...);
			}
			catch (...) {
				// Destroy all correctly constructed before new_element in new_arr
				for (size_t i = 0; i < new_arr_iter - iterator(new_arr); ++i) {
					allocator_traits::destroy(alloc, new_arr + i);
				}
				allocator_traits::deallocate(alloc, new_arr, cap * 2);

				throw;
			}

			const auto return_value = new_arr_iter; // iterator to the emplaced element

			++new_arr_iter;
			auto last = end();

			try {
				while (iter != last) { // move ctors after pos
					allocator_traits::construct(alloc, new_arr_iter.ptr, std::move(*iter));
					++new_arr_iter;
					++iter;
				}
			}
			catch (...) {
				// Destroy all correctly constructed after new_element in new_arr 
				for (size_t i = pos - begin() + 1; i < new_arr_iter - iterator(new_arr); ++i) {
					allocator_traits::destroy(alloc, new_arr + i);
				}
				allocator_traits::deallocate(alloc, new_arr, cap * 2);

				throw;
			}

			const auto saved_size = sz;
			clear();
			allocator_traits::deallocate(alloc, arr, cap);

			arr = new_arr;
			sz = saved_size + 1;

			return return_value;
		}
		else {
			auto iter = end();
			
			allocator_traits::construct(alloc, iter.ptr, std::move(*(iter - 1))); // move last element
			--iter;

			while (iter != pos) { // moves elements after pos
				*iter = std::move(*(iter - 1));
				--iter;
			}

			// Constructing inserted element at another(temp) location
			auto temp_location = allocator_traits::allocate(alloc, 1);

			try {
				allocator_traits::construct(alloc, temp_location, std::forward<Args>(args)...);
			}
			catch (...) {
				allocator_traits::deallocate(alloc, temp_location, 1);
			}

			*iter = std::move(*temp_location); // move assigned into the required location

			// Deallocation temporary location
			allocator_traits::destroy(alloc, temp_location);
			allocator_traits::deallocate(alloc, temp_location, 1);

			++sz;

			return iter;
		}
	}

private:
	pointer arr;
	allocator_type alloc;
	size_type sz;
	size_type cap;

private:
	void reallocate(size_type n) {
		// Allocate and construct new
		auto new_arr = allocator_traits::allocate(alloc, n);

		for (size_t i = 0; i < sz; ++i) {
			try {
				allocator_traits::construct(alloc, new_arr + i, std::move(arr[i]));
			}
			catch (...) {
				// Destroy correctly constructed in new_arr
				for (size_t j = 0; j < i; ++j) {
					allocator_traits::destroy(alloc, new_arr + j);
				}
				allocator_traits::deallocate(alloc, new_arr, n);
				throw;
			}
		}

		// Delete old
		for (size_t i = 0; i < sz; ++i) {
			allocator_traits::destroy(alloc, arr + i);
		}
		allocator_traits::deallocate(alloc, arr, cap);

		arr = new_arr;
		cap = n;
	}


};
