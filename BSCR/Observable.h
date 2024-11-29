#pragma once

#include <memory>
#include <unordered_map>
#include <set>
#include <mutex>
#include "Signal.h"

namespace HKUltra {

    /*
    * Observable class represents a subject in the observer pattern.
    * It holds a list of registered observers and notifies them when an event occurs.
    * It uses the Signal class to emit signals to observers.
    */
    template <typename... _args>
    class Observable;

    /*
    * Observer class represents an entity that listens for events from an Observable.
    * Observers can register themselves to an Observable to receive notifications
    * when the Observable emits a signal.
    */
    template <typename... _args>
    class Observer {
        friend class Observable<_args...>;  // Allow Observable to access private members of Observer

    public:
        typedef Observable<_args...> ObservableType;  // Alias for the Observable type the observer is listening to

        // Destructor ensures that the observer unsubscribes from all observables it's registered with
        virtual ~Observer() {
            // Unsubscribe from all observables upon destruction to prevent dangling references
            for (auto observable : observables_) {
                observable->unregisterObserver(this);  // Call unregisterObserver on each observable the observer is subscribed to
            }
        }

        /*
        * This method is called when the observable emits a signal.
        * Derived classes must implement this method to define custom behavior when notified.
        */
        virtual void onNotify(_args... args) = 0;

        /*
        * Registers this observer with an observable, making it start receiving notifications
        * when the observable emits signals.
        */
        void registerWith(ObservableType& observable) {
            observable.registerObserver(this);  // Register the observer with the observable
            observables_.insert(&observable);  // Keep track of the observable this observer is watching
        }

        /*
        * Unregisters this observer from an observable, so it will no longer receive notifications.
        */
        void unregisterWith(ObservableType& observable) {
            observable.unregisterObserver(this);  // Unregister the observer from the observable
            observables_.erase(&observable);  // Remove the observable from the list of observables the observer is watching
        }

    private:
        // Set to track all observables this observer is currently subscribed to
        std::set<ObservableType*> observables_;
    };

    /*
    * Observable class manages a list of observers and notifies them when a signal is emitted.
    * It stores a signal and connects observers to that signal.
    */
    template <typename... _args>
    class Observable {
        friend class Observer<_args...>;  // Allow Observer to access private members of Observable

    public:
        typedef Observer<_args...> ObserverType;  // Alias for the Observer type
        typedef Signal<_args...> SignalType;  // Alias for the Signal type used by this observable
        typedef typename SignalType::ConnectionType ConnectionType;  // Alias for the ConnectionType returned by Signal

        /*
        * Notifies all registered observers by emitting a signal.
        * This will call each observer's onNotify method.
        */
        void notifyObservers(_args... args) {
            signal_.emit(args...);  // Emit the signal, passing the arguments to the observers
        }

    private:
        mutable std::mutex mtx_;  // Mutex for thread safety when modifying the observer connections
        SignalType signal_;  // The signal that will be emitted to notify observers
        std::unordered_map<ObserverType*, ConnectionType> connections_;  // Map of observers to their corresponding signal connections

        /*
        * Registers an observer with this observable, so that the observer will be notified
        * when the observable emits a signal.
        */
        void registerObserver(ObserverType* observer) {
            std::lock_guard<std::mutex> lock(mtx_);  // Lock for thread safety while modifying the connections map

            // Only add the observer if it hasn't been registered yet
            if (connections_.find(observer) == connections_.end()) {
                // Define the slot (callback) to call the observer's onNotify method when the signal is emitted
                auto slot = [observer](_args... args) {
                    observer->onNotify(args...);  // Call the observer's onNotify method
                    };
                // Register the observer with the signal, storing the connection in the connections map
                connections_[observer] = signal_.connect(slot);
            }
        }

        /*
        * Unregisters an observer from this observable, so the observer will no longer receive notifications.
        */
        void unregisterObserver(ObserverType* observer) {
            std::lock_guard<std::mutex> lock(mtx_);  // Lock for thread safety while modifying the connections map

            // Check if the observer is already registered
            auto it = connections_.find(observer);
            if (it != connections_.end()) {
                // Disconnect the observer from the signal (removes the callback)
                signal_.disconnect(it->second);
                // Erase the observer from the connections map
                connections_.erase(it);
            }
        }
    };

} // namespace HKUltra
