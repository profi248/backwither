#ifndef BACKWITHER_SIMPLEITERATOR_H
#define BACKWITHER_SIMPLEITERATOR_H
#include <vector>
#include <string>

/**
 * Base class for iterators used in this project.
 * Table methods allows iterators to be printed as a table.
 */

class SimpleIterator {
public:
    virtual ~SimpleIterator () = default;
    /**
     * Advance to next item.
     */
    virtual void Next () = 0;
    /**
     * Rewind to beginning of list.
     */
    virtual void Rewind () = 0;
    /**
     * Check if iterator reached end.
     * @return True if reached end, false otherwise.
     */
    virtual bool End () const = 0;
    /**
     * Check if iterator is empty.
     * @return True if empty, false otherwise.
     */
    virtual bool Empty () const = 0;


    /**
     * Get header of table for printing the iterator.
     * Number of items in header and all rows must be identical.
     * @return Vector of string with column headers.
     */
    virtual std::vector<std::string> TableHeader () const = 0;
    /**
     * Get a current row with formatted strings for printing the data.
     * @return Vector of string with data to show in this row.
     */
    virtual std::vector<std::string> TableRow () const = 0;

    /**
     * Advance to next item.
     */
    virtual void operator ++ (int) = 0;
};


#endif //BACKWITHER_SIMPLEITERATOR_H
