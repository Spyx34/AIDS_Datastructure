#ifndef ADS_SET_H
#define ADS_SET_H

#include <set>
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {

public:
    //class /* iterator type (implementation-defined) */;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    //using const_iterator = /* iterator type */;
    //using iterator = const_iterator;
    //using key_compare = std::less<key_type>;                         // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;                              // Hashing
//DEBUG set private before hadn in
//private:

    size_type globalDepth{0};
    struct Bucket{
        size_type localDepth{0};
        value_type elements[N]{};
        size_type elementsSize{0};
        size_type indexPointing{0};
    };
    Bucket** hashtable{nullptr};
    size_type hashtableCapacity{0};
    size_type cashedValues{0};

    size_type last_n_bits(const key_type& k,const unsigned int& n ) const;
    size_type bin_last_n_bits(const size_type& k,const unsigned int& n ) const{
        size_type mask = (1 << n) - 1;
        return k & mask;
    };


    void doubleCapacity();

public:

    void add(const key_type &key); //DEBUG make private later again

    ADS_set();                                                           // PH1
    ADS_set(std::initializer_list<key_type> ilist);                      // PH1
    template<typename InputIt> ADS_set(InputIt first, InputIt last);     // PH1
    ADS_set(const ADS_set &other);

    ~ADS_set();

    //ADS_set &operator=(const ADS_set &other);
    //ADS_set &operator=(std::initializer_list<key_type> ilist);

    size_type size() const;                                              // PH1
    bool empty() const;                                                  // PH1

    void insert(std::initializer_list<key_type> ilist);                  // PH1
    //std::pair<iterator,bool> insert(const key_type &key);
    template<typename InputIt> void insert(InputIt first, InputIt last); // PH1

    //void clear();
    //size_type erase(const key_type &key);

    size_type count(const key_type &key) const;                          // PH1
    //iterator find(const key_type &key) const;

    //void swap(ADS_set &other);

    //const_iterator begin() const;
    //const_iterator end() const;

    void dump(std::ostream &o = std::cerr) const{
        o << "---Current layout----\n";
        o << "Global Depth: " << globalDepth << "\n";
        o << "Capacity: " << hashtableCapacity << "\n";
        o << "Size: " << size() << "\n";

        for(size_type h = 0 ; h < hashtableCapacity; h++){
            o << "_______________\n";
            o << "Bucket: " <<  hashtable[h] << "\n";
            o << "Size: " << (*hashtable[h]).elementsSize << "\n";
            o << "Hashtable Index: " << h << "\n";
            o << "local Depth: " << (*hashtable[h]).localDepth << "\n";
            o<< "Index pointed by: " << hashtable[h]->indexPointing <<"\n";
            for(size_type g = 0; g < (*hashtable[h]).elementsSize ; g++){
                //o << "value: " << (*hashtable[h]).elements[g]  << " Hashvalue: " << hasher{}((*hashtable[h]).elements[g]) << "\n";
                //std::cout  << "    index : " << last_n_bits((*hashtable[h]).elements[g],globalDepth) << "\n";
                o << " Bucket index: " << g << "\n";
            }
        }
        o << "Layout ENDS HERE\n";

        //std::cout << last_n_bits(15,1) << "   " << last_n_bits(112,1) << "   " <<last_n_bits(0,1) << "   \n";
    };

    void dumpLight(std::ostream &o = std::cerr) const{
        o << "---------\n";
        std::set<value_type> s;
        for(size_type t = 0; t < hashtableCapacity; t++){
            for(size_type u = 0; u < hashtable[t]->elementsSize; u++){
                s.insert(hashtable[t]->elements[u]);
            }
        }

        for(auto e : s){
            //o << e << ", ";
        }
        o << "---------\n";
        std::cout << s.size()  << "\n";


    };



    //friend bool operator==(const ADS_set &lhs, const ADS_set &rhs);
    //friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs);
};

//con-/destructors

template <typename Key, size_t N>
ADS_set<Key, N>::ADS_set() {

    //normal
    hashtableCapacity = 1;
    globalDepth = 0;
    hashtable = new Bucket*[1];
    Bucket* b = new Bucket;
    hashtable[0] = b;
}

template <typename Key, size_t N >
ADS_set<Key, N>::ADS_set(std::initializer_list<key_type> ilist){
    hashtableCapacity = 1;

    hashtable = new Bucket*[1];
    Bucket* b = new Bucket;

    hashtable[0] = b;
    for (const key_type& element : ilist) {

        add(element );
    }
}

template <typename Key, size_t N>
template<typename InputIt> ADS_set<Key, N>::ADS_set(InputIt first, InputIt last){
    hashtableCapacity = 1;
    globalDepth = 0;
    hashtable = new Bucket*[1];
    Bucket* b = new Bucket;
    hashtable[0] = b;
    for (InputIt it = first; it != last; ++it) {
        add(*it);
    }
}

template <typename Key, size_t N >
ADS_set<Key, N>::ADS_set(const ADS_set &other){
    hashtable = new Bucket*[other.hashtableCapacity];
    this->hashtableCapacity = other.hashtableCapacity;
    this->globalDepth = other.globalDepth;
    for(size_type i = 0; i < hashtableCapacity; i++){
        Bucket* b = new Bucket;
        b->elementsSize = (other.hashtable[i])->elementsSize;
        b->localDepth = (other.hashtable[i])->localDepth;
        //b->elements = new value_type[N]{};
        for(int j = 0; j < b->elementsSize; j++){
            b->elements[j] = (other.hashtable[i])->elements[j];
        }
        hashtable[i] = b;
    }
}


template <typename Key, size_t N>
ADS_set<Key, N>::~ADS_set() {
    //std::cout << "Destructing\n";
    //
    // std::cout << "Capacity is: " << hashtableCapacity <<  "\n";

    for(size_type i = hashtableCapacity-1; i > 0; i--) {
        if (i < (size_type) (1 << hashtable[i]->localDepth)) {
            //std::cout << "Accessing hashtable at index: " << i << "\n";
            delete hashtable[i];
        }
    }
    delete hashtable[0];

    //ich muss ehrlich gestehen kein polan was ich hier gemacht hab bzw warum ich index 0 extra löschen muss, wenn ich im loop aber >= verwende gibt er mir access auf freed pointer
    // if it works I dont touch it

    //std::cout << "\n Dhashtable itself: " << hashtable << "\n";
    //std::cout << "\n Destructor Buckets currently:\n";
    //for(int u = 0; u < hashtableCapacity; u++){
    //std::cout << "Bucket at index  " << hashtable[u] << ":" << u << "\n";
    //}
    /*
    for(size_type i = 0; i < hashtableCapacity; i++){

        if(hashtable[i] != nullptr){
            for(size_type j = i+1; j < hashtableCapacity; j++){
                if(hashtable[j] == hashtable[i]){
                    hashtable[j] = nullptr;
                }
            }
        }
        delete hashtable[i];
        //std::cout << "Freed: " << hashtable[i] <<"\n";
        hashtable[i] = nullptr;
    }

    std::set <Bucket*> s;

    for(size_type i = 0; i < hashtableCapacity; i++){
        s.insert(hashtable[i]);
    }
    for(auto e : s){
        delete e;
    }
    */



    delete[] hashtable;

}


//Methods
template <typename Key, size_t N >
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::size() const{
    return cashedValues;
}

template <typename Key, size_t N >
bool ADS_set<Key, N>::empty() const{
    return cashedValues==0;
}

template <typename Key, size_t N >
void ADS_set<Key, N>::insert(std::initializer_list<key_type> ilist){
    for(const key_type& element : ilist){

        add(element);
    }
}

template <typename Key, size_t N >
template<typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
    for (InputIt it = first; it != last; ++it) {
        add(*it);
    }
}

template <typename Key, size_t N >
typename ADS_set<Key,N>::size_type ADS_set<Key, N>::count(const key_type &key) const{
    size_type index = last_n_bits(key,globalDepth);


    for(size_type i =0; i<hashtable[index]->elementsSize; i++){


        if(key_equal{}(hashtable[index]->elements[i],key)){
            return 1;
        }

    }
    return 0;
}





template <typename Key, size_t N >
void ADS_set<Key, N>::add(const key_type &key){



    if(count(key)){return;}


    //std::cout << "\n\nAdd triggered: " << key <<"\n";

    //std::cout << "layout is:\n";


    size_type index = last_n_bits(key, this->globalDepth);

    //sollte im bucket noch platz sein
    if((*(hashtable[index])).elementsSize < N){
        //std::cout << "Index of " << key << " is " << index <<"\n";

        (*(hashtable[index])).elements[(*(hashtable[index])).elementsSize] = key;
        (*(hashtable[index])).elementsSize += 1;
        cashedValues++;
    }
        //sollte im bucket kein platz mehr sein, aber localdepth noch ungleich global depth
        //DEBUG there might be memory leakage here, dunno, no will to think abt it rn
    else if(globalDepth > hashtable[index]->localDepth){
        /*
        std::cout << "Splitting bucket with elements: \n";
        for(size_type i =0 ; i <hashtable[index]->elementsSize; i++){
            std::cout << hashtable[index]->elements[i] << " ";
        }
        std::cout << "\n";
         */
        //DEBUG
        Bucket* b1 = new Bucket;

        b1->localDepth = hashtable[index]->localDepth + 1;

        b1->elementsSize = 0;

        Bucket* b2 = new Bucket;

        b2->localDepth = hashtable[index]->localDepth + 1;
        b2->elementsSize = 0;

        //std::cout << "Set depth of " <<  hashtable[index] << "from " << hashtable[index]->localDepth << "on " << b1<< " & " << b2 << " to " << b1->localDepth << "\n";
        //iteriert über alle elemente im betroffenen bucket und fügt diese jewils entweder in den ersten oder zweiten neuen bucket ein
        for(size_type i = 0; i <  hashtable[index]->elementsSize; i++) {
            //beginnt der index mit 0 wird in b1 eingesetzt, beginnt der index mit 1 in b2
            //e.g. index ist 101 mit dem pendant 001 und einer tiefe von 3. berechnet wird also 2 hoch (3-1) = 4, 4 in binär ist 100. je nachdem ob der index nun größer/gleich oder kleiner ist
            //lässt sich sagen ob der index mit 0 oder 1 anfängt
            if (last_n_bits(hashtable[index]->elements[i], b1->localDepth) < (size_type) (1<< (b1->localDepth-1))) { // ebenso könnte man local depth von b2 oder auch dem alten bucket + 1 checken
                //DEBUG

                b1->elements[b1->elementsSize] = hashtable[index]->elements[i];
                b1->elementsSize +=1;

            }else{
                //DEBUG

                b2->elements[b2->elementsSize] = hashtable[index]->elements[i];
                b2->elementsSize +=1;
            }
        }

        size_type lBits = last_n_bits(key, b1->localDepth);

        size_type lower =  lBits < (size_type) (1 << (b1->localDepth-1)) ? lBits : lBits - (1 << (b1->localDepth-1)) ;
        size_type upper = lower + (1 << (b1->localDepth-1));

        //std::cout << "\n\nAddFreed: " << hashtable[index] <<"\n";
        auto tmp = hashtable[index];
        b1->indexPointing = lower;
        b2->indexPointing = upper;
        //hashtable[lower] = b1;
        //hashtable[upper] = b2;

        //std::cout << "The index pointing to b1: " << upper <<"\n";
        //std::cout << "The index pointing to b2: " << lower <<"\n";
        //std::cout << "new Localdepth: " << b1->localDepth <<"\n";
        /*
        for(size_type a =0; a <hashtableCapacity; a++){
            std::cout << "The last n bits: " <<  bin_last_n_bits(a,b1->localDepth) << "\n";
            if(bin_last_n_bits(a,b1->localDepth) == bin_last_n_bits(b1->indexPointing,b1->localDepth)){
                hashtable[a] = b1;
            }else{
                hashtable[a] = b2;
            }
        }
        */
        size_type diff = globalDepth - b1->localDepth;
        size_type numberOfPointers = 1 << diff; //gibt an wie viele indexes aus dem Hashtable auf den Bucket zeigen
        size_type sprungWeite = hashtableCapacity / numberOfPointers; // gibt an in welchen intervallen pointer im hashtable vorkommen
        //size_type start = index %sprungWeite;
        for(size_type a =lower%sprungWeite; a <hashtableCapacity; a += sprungWeite){
            hashtable[a] = b1;
        }
        for(size_type a =upper% sprungWeite; a <hashtableCapacity; a += sprungWeite){
            hashtable[a] = b2;
        }

        delete tmp; //alter bucket muss gelöscht werden wei ldie daten jetzt auf zwei neue aufgeteilt sind
        //std::cout << "new layout is:\n";






        add(key);
    }
        // wenn lokale tiefe = lokale tiefe(hashtable wird verdoppelt und add rekursiv erneut aufgerufen)
    else if(globalDepth == (*(hashtable[index])).localDepth){
        doubleCapacity();
        //std::cout << "DOubling\n";
        //std::cout << "Size: " << size() << "\n";
        //std::cout << "Capacity: " << hashtableCapacity << "\n";
        add(key);
    }
}




//alle privaten Methoden
template <typename Key, size_t N >
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::last_n_bits(const key_type& k,const unsigned int& n ) const {
    unsigned int ret;
    ret =  hasher{}(k) % (1 << n);
    return ret;
}

template <typename Key, size_t N >
//doubles the capacity by creating a hashtable double the size, then creating one bucket and using the add Method to insert all old values
void ADS_set<Key, N>::doubleCapacity(){
    size_type oldCapacity = hashtableCapacity;
    hashtableCapacity *= 2;


    Bucket** newTable = new Bucket*[hashtableCapacity];
    //creating a copy of the old hashtable to keep acces but still be able to use the add method for re-inserting
    for(size_type k = 0; k < oldCapacity; k++){
        newTable[k] = hashtable[k];
    }

    for(size_type m = oldCapacity; m < hashtableCapacity; m++){
        newTable[m] = hashtable[m-oldCapacity];
    }
    delete[] hashtable;
    hashtable = newTable;
    /*
    //filling the new allocated double sized hashtable with one pointer pointing to one bucket
    //then letting "add" do the rest of work
    Bucket* b = new Bucket;
    b->localDepth = 0;
    b->elementsSize = 0;
    std::cout << "called ouble table and allocated pointer: " << b <<"\n";

    for(int l = 0; l < hashtableCapacity; l++){
        hashtable[l] = b;
    }

    //iterating through each bucket
    for(int i = 0; i < oldCapacity; i++){
        //iterating over all bucket elements
        for(int j = 0; j<(*(tempTable[i])).elementsSize; j++){
            add((*(tempTable[i])).elements[j]);
        }
        delete tempTable[i]; //deletes the bucket after its values have been rehashed
    }
    delete[] tempTable;
    */
    globalDepth +=1;
    /*
    std::cout << "after doubling table the pointers are:\n";
    for(int a = 0; a < hashtableCapacity; a++){
        std::cout << hashtable[a] <<"\n";
    }
    */

}




#if 0
template <typename Key, size_t N>
class ADS_set<Key,N>::/* iterator type */ {
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

    explicit /* iterator type */(/* implementation-dependent */);
    reference operator*() const;
    pointer operator->() const;
    /* iterator type */ &operator++();
    /* iterator type */ operator++(int);
    friend bool operator==(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
    friend bool operator!=(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }
#endif

#endif // ADS_SET_H


#if 0

Performance sachen:

beim verdoppeln des hashtables die struktur übernehmen und nicht von vorn eintragen

#endif
