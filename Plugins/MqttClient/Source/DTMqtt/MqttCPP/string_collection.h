// Copyright 2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com

#ifndef __mqtt_string_collection_h
#define __mqtt_string_collection_h

#include "DTMqtt/MqttC/MQTTAsync.h"
#include "types.h"
#include <vector>
#include <map>
#include <memory>

namespace mqtt {

/////////////////////////////////////////////////////////////////////////////

/**
 * Type for a collection of topics.
 * This acts like a collection of strings but carries an array of pointers
 * to the C strings for easy interactions with the Paho C library.
 */
class string_collection
{
	/** The type for the collection of strings */
	using collection_type = std::vector<string>;
	/** The type for the array of C pointers */
	using c_arr_type = std::vector<const char*>;

	/**
	 * The collection of strings for the topics.
	 */
	collection_type coll_;
	/**
	 * A colleciton of pointers to NUL-terminated C strings for the topics.
	 * This is what is required by the Paho C library, and thus the lifetime
	 * of the pointers will remain consistent with the lifetime of the
	 * object. The value is kept consistent with the current topics and
	 * updated whenever topics are added or removed.
	 */
	c_arr_type cArr_;
	/**
	 * Updated the cArr_ object to agree with the values in coll_ 
	 * This should be called any time the coll_ variable is modified 
	 * <i>in any way</i>. 
	 */
	void update_c_arr();

public:
	/** Smart/shared pointer to an object of this type */
	using ptr_t = std::shared_ptr<string_collection>;
	/** Smart/shared pointer to a const object of this type */
	using const_ptr_t = std::shared_ptr<const string_collection>;

	/**
	 * Construct an empty string collection.
	 */
	string_collection() =default;
	/**
	 * Construct a collection initially containing a single string.
	 * @param str The string
	 */
	string_collection(const string& str);
	/**
	 * Construct a collection initially containing a single string.
	 * @param str The string
	 */
	string_collection(string&& str);
	/**
	 * Constructs a string collection by copying a vector of strings.
	 * @param vec A vector of strings.
	 */
	string_collection(const collection_type& vec);
	/**
	 * Constructs a string collection by moving a vector of strings.
	 * @param vec A vector of strings.
	 */
	string_collection(collection_type&& vec);
	/**
	 * Copy constructor.
	 * @param coll An existing string collection.
	 */
	string_collection(const string_collection& coll);
	/**
	 * Move constructor.
	 * @param coll An existing string collection.
	 */
	string_collection(string_collection&& coll) = default;
	/**
	 * Construct a string collection from an initialization list of strings.
	 * @param sl An initialization list of strings.
	 */
	string_collection(std::initializer_list<string> sl);
	/**
	 * Construct a string collection from an initialization list of C string
	 * pointers.
	 * @param sl An initialization list of C character arrays.
	 */
	string_collection(std::initializer_list<const char*> sl);
	/**
	 * Create an empty string collection on the heap.
	 * @return A smart/shared pointer to a string collection.
	 */
	static ptr_t create(const string& str) {
		return std::make_shared<string_collection>(str);
	}
	/**
	 * Create a string collection on the heap, initially containing a single
	 * string.
	 * @param str The string
	 * @return A smart/shared pointer to a string collection.
	 */
	static ptr_t create(string&& str) {
		return std::make_shared<string_collection>(str);
	}
	/**
	 * Creates a string collection on the heap by copying a vector of
	 * strings.
	 * @param vec A vector of strings.
	 */
	static ptr_t create(const collection_type& vec) {
		return std::make_shared<string_collection>(vec);
	}
	/**
	 * Creates a string collection on the heap by copying a vector of
	 * strings.
	 * @param vec A vector of strings.
	 * @return A smart/shared pointer to a string collection.
	 */
	static ptr_t create(collection_type&& vec) {
		return std::make_shared<string_collection>(vec);
	}
	/**
	 * Create a string collection on the heap from an initialization list of
	 * strings.
	 * @param sl An initialization list of strings.
	 * @return A smart/shared pointer to a string collection.
	 */
	static ptr_t create(std::initializer_list<string> sl) {
		return std::make_shared<string_collection>(sl);
	}
	/**
	 * Create a string collection on the heap from an initialization list of
	 * C string pointers.
	 * @param sl An initialization list of C character arrays.
	 * @return A smart/shared pointer to a string collection.
	 */
	static ptr_t create(std::initializer_list<const char*> sl) {
		return std::make_shared<string_collection>(sl);
	}
	/**
	 * Copy assignment.
	 * Copy another string collection to this one.
	 * @param coll A string collection
	 * @return A reference to this collection.
	 */
	string_collection& operator=(const string_collection& coll);
	/**
	 * Move assignment.
	 * Move another string collection to this one.
	 * @param coll A string collection
	 * @return A reference to this collection.
	 */
	string_collection& operator=(string_collection&& coll) = default;
	/**
	 * Determines if the collection is empty.
	 * @return @em true if the collection is empty, @em false if not.
	 */
	bool empty() const { return coll_.empty(); }
	/**
	 * Gets the number of strings in the collection.
	 * @return The number of strings in the collection.
	 */
	size_t size() const { return coll_.size(); }
	/**
	 * Copies a string to the back of the collection.
	 * @param str A string.
	 */
	void push_back(const string& str);
	/**
	 * Moves a string to the back of the collection.
	 * @param str A string.
	 */
	void push_back(string&& str);
	/**
	 * Removes all the strings from the collection.
	 */
	void clear();
	/**
	 * Gets the n'th string in the collection.
	 * @param i Index to the desired string.
	 * @return A const reference to the string.
	 */
	const string& operator[](size_t i) const { return coll_[i]; }
	/**
	 * Gets a pointer to an array of NUL-terminated C string pointers.
	 * This is the collection type supported by the underlying Paho C
	 * library. The returned pointer is guaranteed valid so long as the
	 * object is not updated. The return value may change if the object is
	 * modified, so the application should not cache the return value, but
	 * rather request the value when needed.
	 * @return pointer to an array of NUL-terminated C string pointers of
	 *  	   the topics in the object.
	 *
	 */
	char* const* c_arr() const { return (char* const *) cArr_.data(); }
};

/////////////////////////////////////////////////////////////////////////////

/** Smart/shared pointer to a topic collection */
using string_collection_ptr = string_collection::ptr_t;

/** Smart/shared pointer to a const string_collection */
using const_string_collection_ptr = string_collection::const_ptr_t;

/////////////////////////////////////////////////////////////////////////////

/**
 * A colleciton of name/value string pairs.
 */
class name_value_collection
{
    /** The type for the collection of name/value pairs  */
    using collection_type = std::map<string, string>;
    /** The type for the C pointers to pass to Paho C */
    using c_arr_type = std::vector<MQTTAsync_nameValue>;

    /**
     * The name/value pairs.
     */
    collection_type map_;
    /**
     * A collection of pairs of NUL-terminated C strings.
     */
    c_arr_type cArr_;
	/**
	 * Updated the cArr_ object to agree with the values in coll_
	 * This should be called any time the coll_ variable is modified
	 * <i>in any way</i>.
	 */
	void update_c_arr();

public:
	/** Smart/shared pointer to an object of this type */
	using ptr_t = std::shared_ptr<name_value_collection>;
	/** Smart/shared pointer to a const object of this type */
	using const_ptr_t = std::shared_ptr<const name_value_collection>;
	/** The type of the string/string pair of values */
	using value_type = collection_type::value_type;
    /**
     * Default construtor for an empty collection.
     */
    name_value_collection() =default;
    /**
     * Creates a name/value collection from an underlying STL collection.
     * @param map The collection of name/value pairs.
     */
    name_value_collection(const collection_type& map) : map_(map) {
        update_c_arr();
    }
    /**
     * Creates a name/value collection from an underlying STL collection.
     * @param map The collection of name/value pairs.
     */
    name_value_collection(collection_type&& map) : map_(std::move(map)) {
        update_c_arr();
    }
    /**
     * Copy constructor.
     * @param other Another collection of name/value pairs.
     */
    name_value_collection(const name_value_collection& other)
            : map_(other.map_) {
        update_c_arr();
    }
    /**
     * Move constructor.
     * @param other Another collection of name/value pairs
     */
    name_value_collection(name_value_collection&& other) = default;
	/**
	 * Constructs the collection with an initializer list.
	 *
	 * This works identically to initializing a std::map<> with string/tring
	 * pairs.
	 *
	 * @param init Initializer list to construct the members of the
	 *  		   collection.
	 */
	name_value_collection(std::initializer_list<value_type> init)
			: map_{ init } {
		update_c_arr();
	}
    /**
     * Copy assignment.
     * @param other Another collection of name/value pairs.
     */
    name_value_collection& operator=(const name_value_collection& other) {
		map_ = other.map_;
        update_c_arr();
		return *this;
    }
    /**
     * Move constructor.
     * @param other Another collection of name/value pairs
     */
    name_value_collection& operator=(name_value_collection&& other) = default;
	/**
	 * Determines if the collection is empty.
	 * @return @em true if the container is empty, @em false if it contains
	 *  	   one or more items.
	 */
	bool empty() const { return map_.empty(); }
	/**
	 * Gets the number of name/value pairs in the collection.
	 * @return The number of name/value pairs in the collection.
	 */
	size_t size() const { return map_.size(); }
	/**
	 * Removes all items from the collection.
	 */
	void clear() {
		map_.clear();
		update_c_arr();
	}
	/**
	 * Inserts a name/value pair into the collection.
	 * @param nvpair The name/value string pair.
	 * @return @em true if the inert happened, @em false if not.
	 */
	bool insert(const value_type& nvpair) {
		if (map_.insert(nvpair).second) {
			update_c_arr();
			return true;
		}
		return false;
	}
	/**
     * Gets a pointer to an array of NUL-terminated C string pointer pairs.
     * This is a collection type supported by the underlying Paho C
     * library. The returned pointer is guaranteed valid so long as the
     * object is not updated. The return value may change if the object is
     * modified, so the application should not cache the return value, but
     * rather request the value when needed.
     * @return pointer to an array of NUL-terminated C string pointer pairs
     *         for name/values. The array is terminated by a NULL/NULL pair.
	 */
	const MQTTAsync_nameValue* c_arr() const { return cArr_.data(); }
};


/////////////////////////////////////////////////////////////////////////////
// end namespace mqtt
}

#endif		// __mqtt_string_collection_h

