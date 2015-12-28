// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Nerilicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and irrXML.h

#ifndef __EZARRAY_H_INCLUDED__
#define __EZARRAY_H_INCLUDED__

#include "EzDebugHelper.h"
#include "EzHeapsort.h"
#include "EzAllocator.h"


//! Self reallocating template array (like stl vector) with additional features.
/** Some features are: Heap sorting, binary search methods, easier debugging.
*/

template <class T, typename TAlloc = EzAllocatorInterface<T> >

class EZCORE_ENTRY EzArray
{

public:

	//! Default constructor for empty array.
	EzArray()
		: data(0), allocated(0), used(0),
			free_when_destroyed(true), is_sorted(true)
	{
	}

	//! Constructs an array and allocates an initial chunk of memory.
	/** \param start_count Amount of elements to pre-allocate. */
	EzArray(unsigned int start_count)
		: data(0), allocated(start_count), used(0),
			free_when_destroyed(true), is_sorted(true)
	{
		reallocate(start_count);
	}


	//! Copy constructor
	EzArray(const EzArray<T>& other)
		: data(0)
	{
		*this = other;
	}



	//! Destructor.
	/** Frees allocated memory, if set_free_when_destroyed was not set to
	false by the user before. */
	~EzArray()
	{
		if (free_when_destroyed)
		{
			for (unsigned int i=0; i<used; ++i)
				TAlloc::destruct(&data[i]);

			TAlloc::deallocate(data);
		}
	}


	//! Reallocates the array, make it bigger or smaller.
	/** \param new_size New size of array. */
	void reallocate(unsigned int new_size)
	{
		T* old_data = data;

		data =  TAlloc::allocate(new_size);
		allocated = new_size;

		// copy old data
		int end = used < new_size ? used : new_size;

		for (int i=0; i<end; ++i)
		{
			//data[i] = old_data[i];
			TAlloc::construct(&data[i], old_data[i]);
		}

		// destruct old data
		for (unsigned int j=0; j<used; ++j)
			TAlloc::destruct(&old_data[j]);

		if (allocated < used)
			used = allocated;

		TAlloc::deallocate(old_data);

		//allocator.deallocate(old_data); //delete [] old_data;
	}


	//! Adds an element at back of array.
	/** If the array is too small to add this new element it is made bigger.
	\param element: Element to add at the back of the array. */
	void push_back(const T& element)
	{
		if (used + 1 > allocated)
		{
			// reallocate(used * 2 +1);
			// this doesn't work if the element is in the same array. So
			// we'll copy the element first to be sure we'll get no data
			// corruption

			T e(element);
			reallocate(used * 2 +1); // increase data block

			TAlloc::construct(&data[used++], e); // data[used++] = e; // push_back
		}
		else
		{
			//data[used++] = element;
			// instead of using this here, we copy it the safe way:
			TAlloc::construct(&data[used++], element);
		}

		is_sorted = false;
	}


	//! Adds an element at the front of the array.
	/** If the array is to small to add this new element, the array is
	made bigger. Please note that this is slow, because the whole array
	needs to be copied for this.
	\param element Element to add at the back of the array. */
	void push_front(const T& element)
	{
		insert(element);
	}

	void assign(unsigned int size, const T& element)
	{
		if (size > allocated)
		{
			// reallocate(used * 2 +1);
			// this doesn't work if the element is in the same array. So
			// we'll copy the element first to be sure we'll get no data
			// corruption

			T e(element);
			reallocate(size +1); // increase data block

			unsigned int oldUsed = used;

			for(unsigned int n = oldUsed; n < size; ++n)
			{
				TAlloc::construct(&data[used++], e); // data[used++] = e; // push_back
			}

		}
		else
		{
			unsigned int oldUsed = used;

			for(unsigned int n = oldUsed; n < size; ++n)
			{
				//data[used++] = element;
				// instead of using this here, we copy it the safe way:
				TAlloc::construct(&data[used++], element);
			}
		}

		is_sorted = false;
	}


	//! Insert item into array at specified position.
	/** Please use this only if you know what you are doing (possible
	performance loss). The preferred method of adding elements should be
	push_back().
	\param element: Element to be inserted
	\param index: Where position to insert the new element. */
	void insert(const T& element, unsigned int index=0)
	{
		EZASSERT(index<used) // access violation

		if (used + 1 > allocated)
			reallocate(used +1);

		for (unsigned int i=used; i>index; --i)
		{
			if (i<used)
				TAlloc::destruct(&data[i]);
		
			TAlloc::construct(&data[i], data[i-1]); // data[i] = data[i-1];
		}

		if (used > index)
			TAlloc::destruct(&data[index]);
		TAlloc::construct(&data[index], element); // data[index] = element;
		is_sorted = false;
		++used;
	}


	//! Clears the array and deletes all allocated memory.
	void clear()
	{
		for (unsigned int i=0; i<used; ++i)
			TAlloc::destruct(&data[i]);

		TAlloc::deallocate(data);

		data = 0;
		used = 0;
		allocated = 0;
		is_sorted = true;
	}


	//! Sets pointer to new array, using this as new workspace.
	/** \param newPointer: Pointer to new array of elements.
	\param size: Size of the new array. */
	void set_pointer(T* newPointer, unsigned int size)
	{
		for (unsigned int i=0; i<used; ++i)
			TAlloc::destruct(&data[i]);

		TAlloc::deallocate(data);
		data = newPointer;
		allocated = size;
		used = size;
		is_sorted = false;
	}


	//! Sets if the array should delete the memory it uses upon destruction.
	/** \param f If true, the array frees the allocated memory in its
	destructor, otherwise not. The default is true. */
	void set_free_when_destroyed(bool f)
	{
		free_when_destroyed = f;
	}

	//! Sets the size of the array and allocates new elements if necessary.
	/** Please note: This is only secure when using it with simple types,
	because no default constructor will be called for the added elements.
	\param usedNow Amount of elements now used. */
	//void set_used(unsigned int usedNow)
	//{
	//	if (allocated < usedNow)
	//		reallocate(usedNow);

	//	used = usedNow;
	//}


	//! bug fix 생성자가 호출이 안되는 문제가 있는 버그 수정 
	void set_used(unsigned int usedNow) 
	{ 
		set_used(usedNow, T()); 
	} 

	void set_used(unsigned int usedNow, const T& val) 
	{ 
		if (allocated < usedNow) 
		{ 
			unsigned int i = allocated; 
			reallocate(usedNow); 

	      for (/**/; i < allocated; ++i) 
		    TAlloc::construct(&data[i], val); 
       
		} 

		used = usedNow; 
	} 


	//! Assignment operator
	void operator=(const EzArray<T>& other)
	{
		if (data)
		{
			for (unsigned int i=0; i<used; ++i)
				TAlloc::destruct(&data[i]);

			TAlloc::deallocate(data);

		}

		//if (allocated < other.allocated)
		if (other.allocated == 0)
			data = 0;
		else
			data = TAlloc::allocate(other.allocated); // new T[other.allocated];

		used = other.used;
		free_when_destroyed = other.free_when_destroyed;
		is_sorted = other.is_sorted;
		allocated = other.allocated;

		for (unsigned int i=0; i<other.used; ++i)
			TAlloc::construct(&data[i], other.data[i]); // data[i] = other.data[i];
	}


	//! Equality operator
	bool operator == (const EzArray<T>& other) const
	{
		if (used != other.used)
			return false;

		for (unsigned int i=0; i<other.used; ++i)
			if (data[i] != other[i])
				return false;
		return true;
	}

	//! Inequality operator
	bool operator != (const EzArray<T>& other) const
	{
		return !(*this==other);
	}


	//! Direct access operator
	T& operator [](unsigned int index)
	{
		EZVERIFY(index>=used) // access violation

		return data[index];
	}


	//! Direct const access operator
	const T& operator [](unsigned int index) const
	{
		EZVERIFY(index>=used) // access violation

		return data[index];
	}


	//! Gets last element.
	T& getLast()
	{
		EZVERIFY(!used) // access violation

		return data[used-1];
	}


	//! Gets last element
	const T& getLast() const
	{
		EZVERIFY(!used) // access violation

		return data[used-1];
	}


	//! Gets a pointer to the array.
	/** \return Pointer to the array. */
	T* pointer()
	{
		return data;
	}


	//! Gets a const pointer to the array.
	/** \return Pointer to the array. */
	const T* const_pointer() const
	{
		return data;
	}


	//! Get size of array.
	/** \return Size of elements used in the array. */
	unsigned int size() const
	{
		return used;
	}


	//! Get amount of memory allocated.
	/** \return Amount of memory allocated. The amount of bytes
	allocated would be allocated_size() * sizeof(ElementsUsed); */
	unsigned int allocated_size() const
	{
		return allocated;
	}


	//! Check if array is empty.
	/** \return True if the array is empty false if not. */
	bool empty() const
	{
		return used == 0;
	}


	//! Sorts the array using EzHeapsort.
	/** There is no additional memory waste and the algorithm performs
	O(n*log n) in worst case. */
	void sort()
	{
		if (is_sorted || used<2)
			return;

		EzHeapsort(data, used);
		is_sorted = true;
	}


	//! Performs a binary search for an element, returns -1 if not found.
	/** The array will be sorted before the binary search if it is not
	already sorted.
	\param element Element to search for.
	\return Position of the searched element if it was found,
	otherwise -1 is returned. */
	int binary_search(const T& element)
	{
		sort();
		return binary_search(element, 0, used-1);
	}


	//! Performs a binary search for an element, returns -1 if not found.
	/** The array must be sorted prior
	\param element Element to search for.
	\return Position of the searched element if it was found, otherwise -1
	is returned. */
	int binary_search_const(const T& element) const
	{
		return binary_search(element, 0, used-1);
	}


	//! Performs a binary search for an element, returns -1 if not found.
	/** \param element: Element to search for.
	\param left First left index
	\param right Last right index.
	\return Position of the searched element if it was found, otherwise -1
	is returned. */
	int binary_search(const T& element, int left, int right) const
	{
		if (!used)
			return -1;

		int m;

		do
		{
			m = (left+right)>>1;

			if (element < data[m])
				right = m - 1;
			else
				left = m + 1;

		} while((element < data[m] || data[m] < element) && left<=right);
		// this last line equals to:
		// " while((element != array[m]) && left<=right);"
		// but we only want to use the '<' operator.
		// the same in next line, it is "(element == array[m])"


		if (!(element < data[m]) && !(data[m] < element))
			return m;

		return -1;
	}


	//! Finds an element in linear time, which is very slow.
	/** Use binary_search for faster finding. Only works if ==operator is
	implemented.
	\param element Element to search for.
	\return Position of the searched element if it was found, otherwise -1
	is returned. */
	int linear_search(const T& element) const
	{
		for (unsigned int i=0; i<used; ++i)
			if (element == data[i])
				return (int)i;

		return -1;
	}


	//! Finds an element in linear time, which is very slow.
	/** Use binary_search for faster finding. Only works if ==operator is
	implemented.
	\param element: Element to search for.
	\return Position of the searched element if it was found, otherwise -1
	is returned. */
	int linear_reverse_search(const T& element) const
	{
		for (int i=used-1; i>=0; --i)
			if (data[i] == element)
				return i;

		return -1;
	}


	//! Erases an element from the array.
	/** May be slow, because all elements following after the erased
	element have to be copied.
	\param index: Index of element to be erased. */
	void erase(unsigned int index)
	{
		DEBUG_BREAK(index>=used) // access violation

		for (unsigned int i=index+1; i<used; ++i)
		{
			TAlloc::destruct(&data[i-1]);
			TAlloc::construct(&data[i-1], data[i]); // data[i-1] = data[i];
		}

		TAlloc::destruct(&data[used-1]);

		--used;
	}


	//! Erases some elements from the array.
	/** May be slow, because all elements following after the erased
	element have to be copied.
	\param index: Index of the first element to be erased.
	\param count: Amount of elements to be erased. */
	void erase(unsigned int index, int count)
	{
		EZVERIFY(index>=used || count<1 || index+count>used) // access violation

		unsigned int i;
		for (i=index; i<index+count; ++i)
			TAlloc::destruct(&data[i]);

		for (i=index+count; i<used; ++i)
		{
			if (i > index+count)
				TAlloc::destruct(&data[i-count]);

			allocator.construct(&data[i-count], data[i]); // data[i-count] = data[i];

			if (i >= used-count)
				TAlloc::destruct(&data[i]);
		}

		used-= count;
	}


	//! Sets if the array is sorted
	void set_sorted(bool _is_sorted)
	{
		is_sorted = _is_sorted;
	}

protected:
	T* data;

private:
	unsigned int		allocated;	
	unsigned int		used;
	bool	free_when_destroyed;
	bool	is_sorted;

};


#endif

