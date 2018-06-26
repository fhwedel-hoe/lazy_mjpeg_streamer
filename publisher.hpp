#pragma once

#include <boost/thread.hpp>

/** @brief Publish/Subscribe Pattern
 *
 * Based on https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html .
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
    
    /**
     * @brief Constructs a Publisher yielding a default value.
     * @param data The default value.
     */
    Publisher(Data const& data) : the_value(data) {}
    
    /**
     * @brief Publish data. Replaces existing data. Notifies all reading subscribers.
     * @param data The data to be published.
     */
    void publish(Data const& data) {
        boost::mutex::scoped_lock lock(the_mutex);
        the_value = data;
        lock.unlock();
        the_condition_variable.notify_all();
    }

    /**
     * @brief Modifies data in-place and thread-safe.
     * @param data The function to apply to the published data.
     */
    void update(std::function<void(Data &)> f) {
        boost::mutex::scoped_lock lock(the_mutex);
        f(the_value);
        lock.unlock();
        the_condition_variable.notify_all();
    }

    /**
     * @brief Waits for data to be published.
     * @return The published data.
     */
    Data read() {
        boost::mutex::scoped_lock lock(the_mutex);
        the_condition_variable.wait(lock);
        Data value = the_value;
        lock.unlock();
        return value;
    }

    /**
     * @brief Reads published data without blocking. Dangerous. Not thread-safe.
     * @return The published data.
     */
    Data read_unsafe() {
        return the_value;
    }

};