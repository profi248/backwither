#ifndef BACKWITHER_SIMPLEITERATOR_H
#define BACKWITHER_SIMPLEITERATOR_H
#include <vector>
#include <string>
class SimpleIterator {
public:
    virtual ~SimpleIterator () = default;

    virtual void Next () = 0;
    virtual void Rewind () = 0;
    virtual bool End () const = 0;
    virtual bool Empty () const = 0;

    virtual std::vector<std::string> TableHeader () const = 0;
    virtual std::vector<std::string> TableRow () const = 0;

    virtual void operator ++ (int) = 0;
};


#endif //BACKWITHER_SIMPLEITERATOR_H
