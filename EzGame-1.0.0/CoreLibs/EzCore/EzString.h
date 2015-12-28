#ifndef __STRING_H_INCLUDED__
#define __STRING_H_INCLUDED__

#include "EzCoreLibType.h"
#include "EzMemoryDefines.h"
#include "EzDebugHelper.h"
#include "EzMemObject.h"


inline bool isUpperChar(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

inline bool isDigitChar(char ch)
{
    return ch >= '0' && ch <= '9';
}

inline char toLowerChar(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 'a';
    else
        return ch;
}

template <class T>
class EZCORE_ENTRY EzString : public EzMemObject
{
public:

	//! Default constructor
	EzString()
	: array(0), allocated(1), used(1)
	{
		array = EzAlloc(T, 1);
		array[0] = 0x0;
	}


	//! Constructor
	EzString(const EzString<T>& other)
	: array(0), allocated(0), used(0)
	{
		*this = other;
	}

	//! Constructor from other EzStringc types
	template <class B>
	EzString(const EzString<B>& other)
	: array(0), allocated(0), used(0)
	{
		*this = other;
	}


	//! Constructs a EzStringc from a float
	explicit EzString(const double number)
	: array(0), allocated(0), used(0)
	{
		char tmpbuf[255];
		snprintf(tmpbuf, 255, "%0.6f", number);
		*this = tmpbuf;
	}


	//! Constructs a EzStringc from an int
	explicit EzString(int number)
	: array(0), allocated(0), used(0)
	{
		// store if negative and make positive

		bool negative = false;
		if (number < 0)
		{
			number *= -1;
			negative = true;
		}

		// temporary buffer for 16 numbers

		char tmpbuf[16];
		tmpbuf[15] = 0;
		unsigned int idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (char)('0' + (number % 10));
			number /= 10;
		}

		// add sign

		if (negative)
		{
			--idx;
			tmpbuf[idx] = '-';
		}

		*this = &tmpbuf[idx];
	}


	//! Constructs a EzStringc from an unsigned int
	explicit EzString(unsigned int number)
	: array(0), allocated(0), used(0)
	{
		// temporary buffer for 16 numbers

		char tmpbuf[16];
		tmpbuf[15] = 0;
		unsigned int idx = 15;

		// special case '0'

		if (!number)
		{
			tmpbuf[14] = '0';
			*this = &tmpbuf[14];
			return;
		}

		// add numbers

		while(number && idx)
		{
			--idx;
			tmpbuf[idx] = (char)('0' + (number % 10));
			number /= 10;
		}

		*this = &tmpbuf[idx];
	}


	//! Constructor for copying a EzStringc from a pointer with a given length
	template <class B>
	EzString(const B* const c, unsigned int length)
	: array(0), allocated(0), used(0)
	{
		if (!c)
		{
			// correctly init the EzStringc to an empty one
			*this="";
			return;
		}

		allocated = used = length+1;

		array = EzAlloc(T, used);

		for (unsigned int l = 0; l<length; ++l)
			array[l] = (T)c[l];

		array[length] = 0;
	}


	//! Constructor for unicode and ascii strings
	template <class B>
	EzString(const B* const c)
	: array(0), allocated(0), used(0)
	{
		*this = c;
	}


	//! destructor
	~EzString()
	{
		EzFree(array); // delete [] array;
	}


	//! Assignment operator
	EzString<T>& operator=(const EzString<T>& other)
	{
		if (this == &other)
			return *this;

		allocated = used = other.size()+1;

		array = (T*) EzRealloc(array, used);
		
		const T* p = other.c_str();
		for (unsigned int i=0; i<used; ++i, ++p)
			array[i] = *p;

		return *this;
	}

	//! Assignment operator for other EzStringc types
	template <class B>
	EzString<T>& operator=(const EzString<B>& other)
	{
		*this = other.c_str();
		return *this;
	}


	//! Assignment operator for strings, ascii and unicode
	template <class B>
	EzString<T>& operator=(const B* const c)
	{
		if (!c)
		{
			if (!array)
			{
				array = (T*) EzRealloc(array, 1);
				allocated = 1;
			}
			used = 1;
			array[0] = 0x0;
			return *this;
		}

		if ((void*)c == (void*)array)
			return *this;

		unsigned int len = 0;
		const B* p = c;
		while(*p)
		{
			++len;
			++p;
		}

		++len;
		allocated = used = len;
		array = (T*) EzRealloc(array, used);

		for (unsigned int l = 0; l<len; ++l)
			array[l] = (T)c[l];

		return *this;
	}


	//! Append operator for other strings
	EzString<T> operator+(const EzString<T>& other) const
	{
		EzString<T> str(*this);
		str.append(other);

		return str;
	}


	//! Append operator for strings, ascii and unicode
	template <class B>
	EzString<T> operator+(const B* const c) const
	{
		EzString<T> str(*this);
		str.append(c);

		return str;
	}


	//! Direct access operator
	T& operator [](const unsigned int index)
	{
		DEBUG_BREAK(index>=used) // bad index
		return array[index];
	}


	//! Direct access operator
	const T& operator [](const unsigned int index) const
	{
		DEBUG_BREAK(index>=used) // bad index
		return array[index];
	}


	//! Equality operator
	bool operator ==(const T* const str) const
	{
		if (!str)
			return false;

		unsigned int i;
		for(i=0; array[i] && str[i]; ++i)
			if (array[i] != str[i])
				return false;

		return !array[i] && !str[i];
	}


	//! Equality operator
	bool operator ==(const EzString<T>& other) const
	{
		for(unsigned int i=0; array[i] && other.array[i]; ++i)
			if (array[i] != other.array[i])
				return false;

		return used == other.used;
	}


	//! Is smaller comparator
	bool operator <(const EzString<T>& other) const
	{
		for(unsigned int i=0; array[i] && other.array[i]; ++i)
		{
			int diff = array[i] - other.array[i];
			if ( diff )
				return diff < 0;
/*
			if (array[i] != other.array[i])
				return (array[i] < other.array[i]);
*/
		}

		return used < other.used;
	}


	//! Inequality operator
	bool operator !=(const T* const str) const
	{
		return !(*this == str);
	}


	//! Inequality operator
	bool operator !=(const EzString<T>& other) const
	{
		return !(*this == other);
	}


	//! Returns length of EzStringc
	/** \return Length of the EzStringc in characters. */
	unsigned int size() const
	{
		return used-1;
	}


	//! Returns character EzStringc
	/** \return pointer to C-style zero terminated EzStringc. */
	const T* c_str() const
	{
		return array;
	}


	//! Makes the EzStringc lower case.
	void make_lower()
	{
		for (unsigned int i=0; i<used; ++i)
			array[i] = ansi_lower ( array[i] );
	}


	//! Makes the EzStringc upper case.
	void make_upper()
	{
		const T a = (T)'a';
		const T z = (T)'z';
		const T diff = (T)'A' - a;

		for (unsigned int i=0; i<used; ++i)
		{
			if (array[i]>=a && array[i]<=z)
				array[i] += diff;
		}
	}


	//! Compares the strings ignoring case.
	/** \param other: Other EzStringc to compare.
	\return True if the strings are equal ignoring case. */
	bool equals_ignore_case(const EzString<T>& other) const
	{
		for(unsigned int i=0; array[i] && other[i]; ++i)
			if (ansi_lower(array[i]) != ansi_lower(other[i]))
				return false;

		return used == other.used;
	}


	//! Compares the strings ignoring case.
	/** \param other: Other EzStringc to compare.
	\return True if this EzStringc is smaller ignoring case. */
	bool lower_ignore_case(const EzString<T>& other) const
	{
		for(unsigned int i=0; array[i] && other.array[i]; ++i)
		{
			int diff = (int) ansi_lower ( array[i] ) - (int) ansi_lower ( other.array[i] );
			if ( diff )
				return diff < 0;
		}

		return used < other.used;
	}


	//! compares the first n characters of the strings
	/** \param other Other EzStringc to compare.
	\param n Number of characters to compare
	\return True if the n first characters of this EzStringc are smaller. */
	bool equalsn(const EzString<T>& other, unsigned int n) const
	{
		unsigned int i;
		for(i=0; array[i] && other[i] && i < n; ++i)
			if (array[i] != other[i])
				return false;

		// if one (or both) of the strings was smaller then they
		// are only equal if they have the same length
		return (i == n) || (used == other.used);
	}


	//! compares the first n characters of the strings
	/** \param str Other EzStringc to compare.
	\param n Number of characters to compare
	\return True if the n first characters of this EzStringc are smaller. */
	bool equalsn(const T* const str, unsigned int n) const
	{
		if (!str)
			return false;
		unsigned int i;
		for(i=0; array[i] && str[i] && i < n; ++i)
			if (array[i] != str[i])
				return false;

		// if one (or both) of the strings was smaller then they
		// are only equal if they have the same length
		return (i == n) || (array[i] == 0 && str[i] == 0);
	}


	//! Appends a character to this EzStringc
	/** \param character: Character to append. */
	void append(T character)
	{
		if (used + 1 > allocated)
			array = (T*) EzRealloc(array, used + 1);

		++used;

		array[used-2] = character;
		array[used-1] = 0;
	}


	//! Appends a char EzStringc to this EzStringc
	/** \param other: Char EzStringc to append. */
	void append(const T* const other)
	{
		if (!other)
			return;


		unsigned int len = 0;
		const T* p = other;
		while(*p)
		{
			++len;
			++p;
		}

		if (used + len > allocated)
		{
			array = (T*) EzRealloc(array, used + len);
			allocated = used + len;
		}

		--used;
		++len;

		for (unsigned int l=0; l<len; ++l)
			array[l+used] = *(other+l);

		used += len;

	}


	//! Appends a EzStringc to this EzStringc
	/** \param other: EzString to append. */
	void append(const EzString<T>& other)
	{
		--used;
		unsigned int len = other.size()+1;

		if (used + len > allocated)
			array = (T*)  EzRealloc(array, used + len);

		for (unsigned int l=0; l<len; ++l)
			array[used+l] = other[l];

		used += len;
	}


	//! Appends a EzStringc of the length l to this EzStringc.
	/** \param other: other EzString to append to this EzStringc.
	\param length: How much characters of the other EzStringc to add to this one. */
	void append(const EzString<T>& other, unsigned int length)
	{
		if (other.size() < length)
		{
			append(other);
			return;
		}

		if (used + length > allocated)
			array = (T*) EzRealloc(array, used + length);

		--used;

		for (unsigned int l=0; l<length; ++l)
			array[l+used] = other[l];
		used += length;

		// ensure proper termination
		array[used]=0;
		++used;
	}


	//! Reserves some memory.
	/** \param count: Amount of characters to reserve. */
	void reserve(unsigned int count)
	{
		if (count < allocated)
			return;

		array = (T*) EzAlloc(T, count + 1);
		array[count] = 0;
		allocated = count;
	}


	//! finds first occurrence of character in EzStringc
	/** \param c: Character to search for.
	\return Position where the character has been found,
	or -1 if not found. */
	int findFirst(T c) const
	{
		for (unsigned int i=0; i<used; ++i)
			if (array[i] == c)
				return i;

		return -1;
	}

	//! finds first occurrence of a character of a EzList in EzStringc
	/** \param c: List of characters to find. For example if the method
	should find the first occurrence of 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the EzList. Usually,
	this should be strlen(c)
	\return Position where one of the characters has been found,
	or -1 if not found. */
	int findFirstChar(const T* const c, unsigned int count) const
	{
		if (!c)
			return -1;

		for (unsigned int i=0; i<used; ++i)
			for (unsigned int j=0; j<count; ++j)
				if (array[i] == c[j])
					return i;

		return -1;
	}


	//! Finds first position of a character not in a given EzList.
	/** \param c: List of characters not to find. For example if the method
	should find the first occurrence of a character not 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the EzList. Usually,
	this should be strlen(c)
	\return Position where the character has been found,
	or -1 if not found. */
	template <class B>
	int findFirstCharNotInList(const B* const c, unsigned int count) const
	{
		for (unsigned int i=0; i<used-1; ++i)
		{
			unsigned int j;
			for (j=0; j<count; ++j)
				if (array[i] == c[j])
					break;

			if (j==count)
				return i;
		}

		return -1;
	}

	template <class B>
	int findNextCharNotInList(unsigned int startPos, const B* const c, unsigned int count) const
	{
		for (unsigned int i=startPos; i<used-1; ++i)
		{
			unsigned int j;
			for (j=0; j<count; ++j)
				if (array[i] == c[j])
					break;

			if (j==count)
				return i;
		}

		return -1;
	}

	//! Finds last position of a character not in a given EzList.
	/** \param c: List of characters not to find. For example if the method
	should find the first occurrence of a character not 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the EzList. Usually,
	this should be strlen(c)
	\return Position where the character has been found,
	or -1 if not found. */
	template <class B>
	int findLastCharNotInList(const B* const c, unsigned int count) const
	{
		for (int i=(int)(used-2); i>=0; --i)
		{
			unsigned int j;
			for (j=0; j<count; ++j)
				if (array[i] == c[j])
					break;

			if (j==count)
				return i;
		}

		return -1;
	}

	//! finds next occurrence of character in EzStringc
	/** \param c: Character to search for.
	\param startPos: Position in EzStringc to start searching.
	\return Position where the character has been found,
	or -1 if not found. */
	int findNext(T c, unsigned int startPos) const
	{
		for (unsigned int i=startPos; i<used; ++i)
			if (array[i] == c)
				return i;

		return -1;
	}


	//! finds last occurrence of character in EzStringc
	/** \param c: Character to search for.
	\param start: start to search reverse ( default = -1, on end )
	\return Position where the character has been found,
	or -1 if not found. */
	int findLast(T c, int start = -1) const
	{
		start = clamp ( start < 0 ? (int)(used) - 1 : start, 0, (int)(used) - 1 );
		for (int i=start; i>=0; --i)
			if (array[i] == c)
				return i;

		return -1;
	}

	//! finds last occurrence of a character of a EzList in EzStringc
	/** \param c: List of strings to find. For example if the method
	should find the last occurrence of 'a' or 'b', this parameter should be "ab".
	\param count: Amount of characters in the EzList. Usually,
	this should be strlen(c)
	\return Position where one of the characters has been found,
	or -1 if not found. */
	int findLastChar(const T* const c, unsigned int count) const
	{
		if (!c)
			return -1;

		for (int i=used-1; i>=0; --i)
			for (unsigned int j=0; j<count; ++j)
				if (array[i] == c[j])
					return i;

		return -1;
	}


	//! finds another EzStringc in this EzStringc
	/** \param str: Another EzStringc
	\return Positions where the EzStringc has been found,
	or -1 if not found. */
	template <class B>
	int find(const B* const str) const
	{
		if (str && *str)
		{
			unsigned int len = 0;

			while (str[len])
				++len;

			if (len > used-1)
				return -1;

			for (unsigned int i=0; i<used-len; ++i)
			{
				unsigned int j=0;

				while(str[j] && array[i+j] == str[j])
					++j;

				if (!str[j])
					return i;
			}
		}

		return -1;
	}


	//! Returns a substring
	/** \param begin: Start of substring.
	\param length: Length of substring. */
	EzString<T> subString(unsigned int begin, int length) const
	{
		// if start after EzStringc
		// or no proper substring length
		if ((length <= 0) || (begin>=size()))
			return EzString<T>("");
		// clamp length to maximal value
		if ((length+begin) > size())
			length = size()-begin;

		EzString<T> o;
		o.reserve(length+1);

		for (int i=0; i<length; ++i)
			o.array[i] = array[i+begin];

		o.array[length] = 0;
		o.used = o.allocated;

		return o;
	}


	//! Appends a character to this EzStringc
	/** \param character: Character to append. */
	EzString<T>& operator += (T c)
	{
		append(c);
		return *this;
	}


	//! Appends a char EzStringc to this EzStringc
	/** \param other: Char EzStringc to append. */
	EzString<T>& operator += (const T* const c)
	{
		append(c);
		return *this;
	}


	//! Appends a EzStringc to this EzStringc
	/** \param other: EzString to append. */
	EzString<T>& operator += (const EzString<T>& other)
	{
		append(other);
		return *this;
	}


	EzString<T>& operator += (const int i)
	{
		append(EzString<T>(i));
		return *this;
	}


	EzString<T>& operator += (const unsigned int i)
	{
		append(EzString<T>(i));
		return *this;
	}


	EzString<T>& operator += (const long i)
	{
		append(EzString<T>(i));
		return *this;
	}


	EzString<T>& operator += (const unsigned long& i)
	{
		append(EzString<T>(i));
		return *this;
	}


	EzString<T>& operator += (const double i)
	{
		append(EzString<T>(i));
		return *this;
	}


	EzString<T>& operator += (const float i)
	{
		append(EzString<T>(i));
		return *this;
	}


	//! replaces all characters of a special type with another one
	void replace(T toReplace, T replaceWith)
	{
		for (unsigned int i=0; i<used; ++i)
			if (array[i] == toReplace)
				array[i] = replaceWith;
	}


	//! trims the EzStringc.
	/** Removes the specified characters (by default, Latin-1 whitespace) 
	from the begining and the end of the EzStringc. */
	EzString<T>& trim(const EzString<T> & whitespace = " \t\n\r")
	{
		// find start and end of the substring without the specified characters
		const int begin = findFirstCharNotInList(whitespace.c_str(), whitespace.used);
		if (begin == -1)
			return (*this="");

		const int end = findLastCharNotInList(whitespace.c_str(), whitespace.used);

		return (*this = subString(begin, (end +1) - begin));
	}


	//! Erases a character from the EzStringc.
	/** May be slow, because all elements
	following after the erased element have to be copied.
	\param index: Index of element to be erased. */
	void erase(unsigned int index)
	{
		DEBUG_BREAK(index>=used) // access violation

		for (unsigned int i=index+1; i<used; ++i)
			array[i-1] = array[i];

		--used;
	}

	void clear()
	{
		allocated = used = 1;
		array = (T*) EzRealloc(array, 1);
		array[0] = 0x0;
	}

	void format(const char* pcFormat, ...)
	{
		EZASSERT(pcFormat);

    	va_list	Args;
    	va_start(Args, pcFormat);
       	vsprintf(pcFormat, Args);
    	va_end(Args);
	}

	EzString & vsprintf(const char* cformat, va_list ap)
	{
		if (!cformat || !*cformat) 
		{
        	*this = "";
        	return *this;
    	}

    	// Parse cformat
    	EzString result;
	
		const char *c = cformat;
    	for (;;) 
		{
			// Copy non-escape chars to result
        	while (*c != '\0' && *c != '%')
            	result.append(*c++);

			if (*c == '\0')
            	break;

        	// Found '%'
        	const char *escape_start = c;
        	++c;

        	if (*c == '\0') 
			{
            	result.append('%'); // a % at the end of the string - treat as non-escape text
            	break;
        	}

	        if (*c == '%') 
			{
    	        result.append('%'); // %%
        	    ++c;
            	continue;
        	}

        	// Parse flag characters
        	unsigned int flags = 0;
        	bool no_more_flags = false;
   //     	do 
			//{
			//	switch (*c) 
			//	{
			//		case '#': flags |= QLocalePrivate::Alternate; break;
   //             	case '0': flags |= QLocalePrivate::ZeroPadded; break;
   //             	case '-': flags |= QLocalePrivate::LeftAdjusted; break;
   //             	case ' ': flags |= QLocalePrivate::BlankBeforePositive; break;
   //             	case '+': flags |= QLocalePrivate::AlwaysShowSign; break;
   //             	case '\'': flags |= QLocalePrivate::ThousandsGroup; break;
   //             	default: no_more_flags = true; break;
   //         	}

			//	if (!no_more_flags)
			//		++c;
	  //      } while (!no_more_flags);

			if (*c == '\0') 
			{
            	result.append(escape_start); // incomplete escape, treat as non-escape text
            	break;
        	}

			// Parse field width
        	int width = -1; // -1 means unspecified
        	if (isDigitChar(*c)) 
			{
            	EzString width_str;
            	while (*c != '\0' && isDigitChar(*c))
                	width_str.append(*c++);

            	// can't be negative - started with a digit
            	// contains at least one digit
            	width = atoi(width_str.c_str());
        	}
        	else if (*c == '*') 
			{
            	width = va_arg(ap, int);
            	if (width < 0)
                	width = -1; // treat all negative numbers as unspecified
            	++c;
        	}

        	if (*c == '\0') 
			{
				result.append(escape_start); // incomplete escape, treat as non-escape text
            	break;
        	}

        	// Parse precision
        	int precision = -1; // -1 means unspecified
        	if (*c == '.') 
			{
				++c;
				if (isDigitChar(*c)) 
				{
					EzString precision_str;

					while (*c != '\0' && isDigitChar(*c))
						precision_str.append(*c++);

                	// can't be negative - started with a digit
                	// contains at least one digit
					precision = atoi(precision_str.c_str());
            	}
            	else if (*c == '*') 
				{
					precision = va_arg(ap, int);
                	if (precision < 0)
                    	precision = -1; // treat all negative numbers as unspecified
                	++c;
            	}
        	}

        	if (*c == '\0') 
			{
            	result.append(escape_start); // incomplete escape, treat as non-escape text
            	break;
        	}

			// Parse the length modifier
        	enum LengthMod { lm_none, lm_hh, lm_h, lm_l, lm_ll, lm_L, lm_j, lm_z, lm_t };
        	LengthMod length_mod = lm_none;
        	switch (*c) 
			{
			case 'h':
                ++c;
                if (*c == 'h') {
                    length_mod = lm_hh;
                    ++c;
                }
                else
                    length_mod = lm_h;
                break;

            case 'l':
                ++c;
                if (*c == 'l') {
                    length_mod = lm_ll;
                    ++c;
                }
                else
                    length_mod = lm_l;
                break;

            case 'L':
                ++c;
                length_mod = lm_L;
                break;

            case 'j':
                ++c;
                length_mod = lm_j;
                break;

            case 'z':
            case 'Z':
                ++c;
                length_mod = lm_z;
                break;

            case 't':
                ++c;
                length_mod = lm_t;
                break;

            default: break;
        	}

			if (*c == '\0') 
			{
				result.append(escape_start); // incomplete escape, treat as non-escape text
            	break;
        	}

        	// Parse the conversion specifier and do the conversion
        	EzString subst;
        	switch (*c) 
			{
            case 'd':
            case 'i': 
				{
                __int64  i;
                switch (length_mod) 
				{
                    case lm_none: i = va_arg(ap, int); break;
                    case lm_hh: i = va_arg(ap, int); break;
                    case lm_h: i = va_arg(ap, int); break;
                    case lm_l: i = va_arg(ap, long int); break;
                    case lm_ll: i = va_arg(ap, __int64); break;
                    case lm_j: i = va_arg(ap, long int); break;
                    case lm_z: i = va_arg(ap, size_t); break;
                    case lm_t: i = va_arg(ap, int); break;
                    default: i = 0; break;
                }

				char buffer[64];
				_i64toa_s( i, buffer, 64, 10 );
				subst = buffer;
	            ++c;
                break;
            }
            case 'o':
            case 'u':
            case 'x':
            case 'X': 
			{
				__int64 u;

	            switch (length_mod) 
				{
				case lm_none: u = va_arg(ap, unsigned int); break;
                case lm_hh: u = va_arg(ap, unsigned int); break;
                case lm_h: u = va_arg(ap, unsigned int); break;
                case lm_l: u = va_arg(ap, unsigned long); break;
                case lm_ll: u = va_arg(ap, unsigned __int64); break;
                case lm_z: u = va_arg(ap, size_t); break;
                default: u = 0; break;
                }

               int base = 10;
               switch (toLowerChar(*c))
			   {
			   case 'o': base = 8; break;
               case 'u': base = 10; break;
               case 'x': base = 16; break;
               default: break;
    	       }

				char buffer[64];

				_i64toa_s( u, buffer, 64, base );

				subst = buffer;
                ++c;
               break;
 			}
            case 'E':
            case 'e':
            case 'F':
            case 'f':
            case 'G':
            case 'g':
            case 'A':
            case 'a': 
			{
				char *buffer;
   				double d;

				d = va_arg(ap, double);

				buffer = (char *)EzMalloc(_CVTBUFSIZE);

                switch (toLowerChar(*c)) {
                    case 'e': // not supported 
                    case 'a': // not supported
                    case 'f': // not supported
                    case 'g': _gcvt_s(buffer, _CVTBUFSIZE, d, 7); break;
                    default: break;
                }
				
				subst = EzString<char>(buffer);

				EzFree(buffer);
                ++c;
                break;
            }
            case 'c': 
			{
				const char ch = va_arg(ap, const char);
				subst.append(ch);
                ++c;
                break;
            }
            case 's': 
			{
			    subst = va_arg(ap, const char*);
                ++c;
                break;
            }
            case 'p': {
                void *arg = va_arg(ap, void*);
                __int64 i = reinterpret_cast<unsigned long>(arg);

				char buffer[64];
				int count = 7;
				subst = "00000000";
	
				_i64toa_s( i, buffer, 64, 16 );
				
				for(int n = strlen(buffer) - 1; n >= 0 ; n--)
				{
					subst[count--] = buffer[n];
				}

				subst.make_upper();
                ++c;
                break;
            }
            case 'n':
                switch (length_mod) {
                    case lm_hh: {
                        signed char *n = va_arg(ap, signed char*);
                        *n = result.size();
                        break;
                    }
                    case lm_h: {
                        short int *n = va_arg(ap, short int*);
                        *n = result.size();
                            break;
                    }
                    case lm_l: {
                        long int *n = va_arg(ap, long int*);
                        *n = result.size();
                        break;
                    }
                    case lm_ll: {
                        __int64 *n = va_arg(ap, __int64*);
                        volatile unsigned int tmp = result.size(); // egcs-2.91.66 gets internal
                        *n = tmp;                             // compiler error without volatile
                        break;
                    }
                    default: {
                        int *n = va_arg(ap, int*);
                        *n = result.size();
                        break;
                    }
                }
                ++c;
                break;

            default: // bad escape, treat as non-escape text
                for (const char *cc = escape_start; cc != c; ++cc)
                    result.append(*cc);
                continue;
        }

        //if (flags & QLocalePrivate::LeftAdjusted)
        //    result.append(subst.leftJustified(width));
        //else
        
		result.append(subst);
    }

    *this = result;

    return *this;
}


private:
/*
	T toLower(const T& t) const
	{
		if (t>=(T)'A' && t<=(T)'Z')
			return t + ((T)'a' - (T)'A');
		else
			return t;
	}
*/
	//! Returns a character converted to lower case
	inline T ansi_lower ( unsigned int x ) const
	{
		return x >= 'A' && x <= 'Z' ? (T) x + 0x20 : (T) x;
	}

	T* array;
	unsigned int allocated;
	unsigned int used;
};




//! Typedef for character strings
typedef EzString<char> EzStringc;

//! Typedef for wide character strings
typedef EzString<wchar_t> EzStringw;	





#endif
