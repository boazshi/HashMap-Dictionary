
#include "HashMap.hpp"
#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP




using std::string;

class InvalidKey : public std::invalid_argument {
public:
    explicit InvalidKey(const std::string &message = "")
            : std::invalid_argument
                      (message) {}
};


class Dictionary : public HashMap<string,string> {

private:

public:

    Dictionary() {};

    Dictionary(std::vector<string> keys, std::vector<string> values) :
                HashMap<string,string>(keys,values){

    }

    virtual bool erase(std::string key) override {
        if (!this->contains_key(key)) {
            throw InvalidKey("Key not found in Dictionairy!");
        }
        this->HashMap::erase(key);
        //int hash = hash_it(key);
     //   string value = this->at(key);
    //    this->_buckets[hash].remove(std::make_pair(key, value));
    //    this->_current_amount--;
     //   while (get_load_factor() < LOAD_MIN && _max_capacity > 1) {
     //       this->rebuild(-1);
    //    }
        return true;
    }

    template<typename Iter>
    void update(Iter begin, Iter end) {
        for (Iter it = begin; it != end; it++) {
                this->HashMap::insert_override(it->first, it->second);
        }
    }

};

#endif