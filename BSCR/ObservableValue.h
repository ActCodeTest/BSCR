#pragma once
#include <mutex>
#include "Observable.h"  // Include the base Observable class

namespace HKUltra {

    /*
    * ObservableValue is a specialization of the Observable class,
    * which allows a value of type _type to be observed.
    * The class provides methods to get and set the value, and it notifies
    * all observers whenever the value changes.
    */
    template<typename _type>
    class ObservableValue : public Observable<_type> {
    public:
        // Constructor to initialize the value, defaulting to the default constructor of _type
        ObservableValue(const _type& value = _type()) : value_(value) {}

        /*
        * Getter for the current value of the observable.
        * Returns the current stored value.
        */
        _type get() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return value_;  // Return the current value
        }

        /*
        * Setter for the value.
        * If the new value is different from the current one, it updates the value
        * and notifies all observers about the change.
        * The method ensures that observers are only notified when the value actually changes.
        */
        void set(const _type& value) {
            // Only update and notify if the value has changed
            {
                std::lock_guard<std::mutex> lock(mtx_);  // Lock for thread safety during value update
                if (value != value_) {
                    value_ = value;  // Update the value
                }
            }
            this->notifyObservers(value_);  // Notify all observers about the value change
        }

    private:
        mutable std::mutex mtx_;
        _type value_;     // The value being observed
    };

}
