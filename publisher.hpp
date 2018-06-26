#pragma once

#include <boost/thread.hpp>

/** @brief Publish/Subscribe Pattern
 *
 * Based on https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html .
 * 
 * TODO: Adjust comments
 */
template<typename Data>
class Publisher 
{
private:
    /// Queue yielding the actual data
    Data the_value;
    /// A mutex for sequencing concurrent access
    mutable boost::mutex the_mutex;
    /// Condition variable to wait upon before reading
    boost::condition_variable the_condition_variable;
public:
    
    Publisher(Data const& data) : the_value(data) {
    }
    
    /**
     * @brief Put an element to the message box. Overwrites existing element.
     * @param data The element to be added
     */
    void publish(Data const& data) {
        boost::mutex::scoped_lock lock(the_mutex);
        the_value = data;
        lock.unlock();
        the_condition_variable.notify_all();
    }

    /**
     * @brief Retrieves an element from the queue. Does not remove element from the queue. Blocks until an element can be retrieved.
     * @return The retrieved element.
     */
    Data read() {
        boost::mutex::scoped_lock lock(the_mutex);
        the_condition_variable.wait(lock);
        Data value = the_value;
        lock.unlock();
        return value;
    }

};