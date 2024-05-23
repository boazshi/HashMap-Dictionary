#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <list>
#include <algorithm>

#ifndef HASHMAP_HPP
#define HASHMAP_HPP


#define STARTING_CAPACITY 16
#define LOAD_MAX 0.75
#define LOAD_MIN 0.25
using std::vector;
using std::set;
using std::pair;


template<typename KeyT, typename ValueT>
class HashMap{
public:
    typedef std::list<std::pair<KeyT, ValueT>> Bucket;

    //constructor
    HashMap(){
        _max_capacity = STARTING_CAPACITY;
        _current_amount = 0;
        _buckets = new Bucket[_max_capacity];
    }

    //copy constructor
    HashMap(const HashMap<KeyT,ValueT> &other){
        _max_capacity = other._max_capacity;
        _current_amount = other._current_amount;
        //delete[](_buckets);
        _buckets = new Bucket[_max_capacity];
        for(int i = 0; i < _max_capacity; i++){
            //we use stl list deep copy
            _buckets[i] = other._buckets[i];
        }
    }

    //constructor 1 (One)
    HashMap(const vector<KeyT>& key_vec, const vector<ValueT>& value_vec){
        _max_capacity = STARTING_CAPACITY;
        _current_amount = 0;
        _buckets = new Bucket[_max_capacity];
        if(key_vec.size() != value_vec.size()){
            throw std::domain_error("FAILURE IN CONSTRUCTOR 1: "
                                    "Vectors don't match");
        }
        //it is safe:
        for(int i = 0; i < (int)key_vec.size(); i++){
            insert_override(key_vec[i],value_vec[i]);
        }
    }

    //destructor
    virtual ~HashMap(){
        delete[] _buckets;
    }

    //getters
    int size(){
        return _current_amount;
    }

    int capacity(){
        return _max_capacity;
    }

    bool empty(){
        //Node* runner = _buckets;
        bool is_empty = true;
        for(int i = 0; i < _max_capacity; i++){
            int list_size = _buckets[i].size();
            if (list_size != 0){
                is_empty = false;
            }
        }
        return is_empty;
    }

    //insert
    bool insert(const KeyT& key, const ValueT& value){
        if( this->contains_key(key)){
            return false;
        }
        int hash = hash_it(key);

        _buckets[hash].insert(_buckets[hash].begin(),
                              std::make_pair(key,value));
        _current_amount++;
        //CHECK LOAD FACTOR AND POSSIBLY REBUILD
        while(get_load_factor() > LOAD_MAX){
            this->rebuild(1);
        }
    //    while(get_load_factor() < LOAD_MIN && _max_capacity > 1) {
    //           this->rebuild(-1);
   //     }
        return true;
    }

    //contains_key
    bool contains_key(const KeyT& key) const{
        int hash = hash_it(key);
        //we will look for key in the list:
        for (auto it=_buckets[hash].begin();
                        it != _buckets[hash].end(); it++){
            if (it->first == key){
                return true;
            }
        }
        return false;
    }

    //at
    //GETS KEY - RETURNS VALUE
    ValueT at(const KeyT& key) const{
        if (!this->contains_key(key)){
            throw std::runtime_error("ERROR! at ! key not in hashmap");
        }
        int hash = hash_it(key);
        for (auto it=_buckets[hash].begin();
                        it != _buckets[hash].end(); it++){
            if (it->first == key){
                return it->second;
            }
        }
        throw std::runtime_error("ERROR: at. KEY NOT IN HASHMAP!");
    }

    ValueT& at(const KeyT& key){
        if (!this->contains_key(key)){
            throw std::runtime_error("ERROR! at ! key not in hashmap");
        }
        int hash = hash_it(key);
        for (auto it=_buckets[hash].begin();
             it != _buckets[hash].end(); it++){
            if (it->first == key){
                return it->second;
            }
        }
        throw std::runtime_error("ERROR: at. KEY NOT IN HASHMAP!");
    }

    //erase
    virtual bool erase(KeyT key){
        if (!this->contains_key(key)){ return false; }
        int hash = hash_it(key);
        ValueT value = this->at(key);
        _buckets[hash].remove(std::make_pair(key,value));
        _current_amount--;
   //     while(get_load_factor() > LOAD_MAX){
   //        this->rebuild(1);
  //      }
        while(get_load_factor() < LOAD_MIN && _max_capacity > 1){
            this->rebuild(-1);
        }
        return true;
    }

    //get_load_factor
    double get_load_factor(){
        double load_factor;
        load_factor = (double)(_current_amount) / (double)(_max_capacity);
        return load_factor;
    }

    //bucket_size
    int bucket_size(KeyT key){
        if (!this->contains_key(key)){
            throw std::domain_error
                                ("ERROR! bucket_size ! key not in hashmap");
        }
        //can assume it exists in HashMap
        int hash = hash_it(key);
        return( (int)_buckets[hash].size());
    }
    //bucket_index
    int bucket_index(KeyT key){
        if (!this->contains_key(key)){
            throw std::domain_error
                            ("ERROR! bucket_index ! ""key not in hashmap");
        }
        //can assume it exists in HashMap
        int hash = hash_it(key);
        return hash;
    }

    //clear
    void clear(){
        for(int i = 0; i < _max_capacity; i++){
            _buckets[i] = Bucket();
        }
        _current_amount = 0;
    }

    ////////// OPERATORS //////////

    //operator[]
    //GETTER
    ValueT operator[](const KeyT& key) const{
        if(contains_key(key) ){
            return this->at (key);
        }
        else{
            this->insert(key, ValueT ());
            return this->at(key);
        }
    }
    //MUTATOR
    ValueT& operator[](const KeyT& key){
        if(contains_key(key) ){
            return at(key);
        }
        else{
            this->insert(key, ValueT ());
            return at(key);
        }
    }

    //operator =
    HashMap& operator=(const HashMap<KeyT,ValueT>& other){
        if (this == &other){
            return *this;
        }
        //erase old data;
        delete[] _buckets;
        _max_capacity = other._max_capacity;
        _current_amount = other._current_amount;

        _buckets = new Bucket[_max_capacity];
        for(int i = 0; i < _max_capacity; i++){
            //we use stl list deep copy
            _buckets[i] = other._buckets[i];
        }
        return *this;
    }

    //OPERATOR == / !=
    friend bool operator==(const HashMap<KeyT,ValueT> &lhs,
                           const HashMap<KeyT,ValueT> &rhs) {
        if(lhs._current_amount != rhs._current_amount ||
           lhs._max_capacity != rhs._max_capacity){
            return false;
        }
        for(auto it = lhs.begin(); it != lhs.end(); ++it){
            KeyT key =  it->first;

            if( !rhs.contains_key(key)){
                return false;
            }
            //we now that we know that the value is in both lhs and rhs:
            if( lhs.at(key) != rhs.at(key)){
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const HashMap<KeyT,ValueT> &lhs,
                           const HashMap<KeyT,ValueT> &rhs){
        return (!(lhs == rhs));
    }



    /////////// ITERATOR ///////////

    /**************************************************************
  * Nested class ConstInterator -                                 *
  * A const forward iterator over every word in every sentence in *
  * HashMap                                                       *
  ****************************************************************/
    class ConstIterator
    {
        friend class HashMap<KeyT,ValueT>;

    private:
        const HashMap<KeyT,ValueT>& hash_map_;
        size_t bucket_index_;
        size_t pair_index_;

    public:
        // Iterator traits
        typedef std::pair<KeyT,ValueT> value_type;

        typedef const value_type& reference;
        typedef const value_type* pointer;
        typedef int difference_type; // required but irrelevant
                                    // as we have no op-()
        typedef std::forward_iterator_tag iterator_category;

    private:

        ConstIterator (const HashMap<KeyT,ValueT>& hash,
                       size_t bucket_index,
                       size_t pair_index)
                       :
                       hash_map_ (hash),
                       bucket_index_ (bucket_index),
                       pair_index_ (pair_index)
                      {}

    public:

        ConstIterator &operator++(){
            // IMPORTANT NOTE:
            // a nested class has access to its parent's private members!

            //We must find the next real object.
            pair_index_++;
            if (pair_index_ >= hash_map_._buckets[bucket_index_].size() ){
                //we are at end of list, we move up
                while((int)bucket_index_ < hash_map_._max_capacity){
                    bucket_index_++;
                    //THIS IS THE END!
                    if ((int)bucket_index_ == hash_map_._max_capacity){
                        pair_index_ = 0;
                        return *this;
                    }
                    //safe to check if new buckets[bucket_index] is empty
                    if(hash_map_._buckets[bucket_index_].size() != 0){
                         pair_index_ = 0;
                         break;
                    }
                }
            }
            return *this;
        }

        ConstIterator operator++ (int){
            ConstIterator it (*this); // default copy-ctr at work
            this->operator++ ();
            return it;
        }

        bool operator== (const ConstIterator &rhs) const {
            bool equal = true;
            if(this->bucket_index_ != rhs.bucket_index_){
                equal = false;
            }
            if(this->pair_index_ != rhs.pair_index_){
                equal = false;
            }
            return equal;
        }

        bool operator!= (const ConstIterator &rhs) const {
            return !operator== (rhs);
        }

        reference operator* () const {
           auto it = (hash_map_._buckets[bucket_index_].begin());
           for(int j = 0; j < (int)pair_index_; j++){
               it++;
           }
           return *it;
        }

        pointer operator-> () const{
            return &(operator* ());
        }
    };

    /****************************************************************
   * Iterator member functions                                      *
   *****************************************************************/
    using const_iterator = ConstIterator;

    const_iterator cbegin () const
    {
        int i = 0;
        for(; i < _max_capacity; i ++){
            if ((int)_buckets[i].size() != 0){
                break;
            }
        }
        return ConstIterator (*this, i, 0);
    }

    const_iterator cend () const
    {
        return ConstIterator (*this, _max_capacity, 0);
    }

    const_iterator begin () const
    {
        return cbegin ();
    }
    const_iterator end () const
    {
        return cend ();
    }

protected:
    int _max_capacity; // TOTAL
    int _current_amount; //CURRENT
    Bucket* _buckets; //pointer to array of Nodes

    //HASH FUNCTION:
    int hash_it(const KeyT& key) const{
        size_t key_hash = std::hash<KeyT>{}(key);
        int x = (int)(key_hash & (_max_capacity - 1));
        return x;
    }

    /**
     * rebuild
     * "We can rebuild him, we have the technology."
     *
     * incase the load factor seeps out of limits, we chagne the capacity and
     * insert everything
     * @param int up_or_down: either -1 or 1.
     * Decides if we enlarge or minimize
     */
    void rebuild(int up_or_down){
        int new_max_capacity = 0;
        if(up_or_down == 1){
            //Enlarge!
            new_max_capacity = _max_capacity * 2;
        }
        else{
            //Minimize!
            new_max_capacity = _max_capacity / 2;
        }
        //we initliaze for big Inserts
        HashMap<KeyT,ValueT> the_map_rebuilt;
        //erase old data
        delete[](the_map_rebuilt._buckets);
        Bucket* new_buckets = new Bucket[new_max_capacity];
        the_map_rebuilt._buckets = new_buckets;
        the_map_rebuilt._max_capacity = new_max_capacity;
        the_map_rebuilt._current_amount = 0;
        //big inserts, everything rehashed
        for(auto it = this->begin(); it != this->end(); ++it){
            the_map_rebuilt.insert_for_rebuild(it->first,it->second);
        }

        delete[] _buckets;

        _max_capacity = the_map_rebuilt._max_capacity;
        _current_amount = the_map_rebuilt._current_amount;
        _buckets = new Bucket[_max_capacity];
        for(int i = 0; i < _max_capacity; i++){
            //we use stl list deep copy
            _buckets[i] = the_map_rebuilt._buckets[i];
        }
    }

    bool insert_override(const KeyT& key, const ValueT& value){
        int hash = hash_it(key);
        if(!contains_key(key)) {
            _buckets[hash].insert(_buckets[hash].begin(),
                                  std::make_pair(key, value));
            _current_amount++;
        }
        else{
            at(key) = value;
        }
        //CHECK LOAD FACTOR AND POSSIBLY REBUILD
        while(get_load_factor() > LOAD_MAX){
            this->rebuild(1);
        }
  //      while(get_load_factor() < LOAD_MIN && _max_capacity > 1) {
   //         this->rebuild(-1);
  //      }
        return true;
    }


    bool insert_for_rebuild(const KeyT& key, const ValueT& value){
        int hash = hash_it(key);
        _buckets[hash].insert(_buckets[hash].begin(),
                              std::make_pair(key,value));
        _current_amount++;
        return true;
    }

};

#endif