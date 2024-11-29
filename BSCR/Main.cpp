#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "ObservableValue.h"  // Assuming this header contains your ObservableValue class definition
#include "Date.h"             // Assuming this header contains your Date class definition

namespace HKUltra {
    // Observer Interface
    class IObserver : public Observer<Date> {
    public:
        virtual void onNotify(Date date) = 0;
        virtual ~IObserver() = default;
    };

    // Concrete Observer
    class DateObserver : public IObserver {
    public:
        void onNotify(Date date) override {
            std::cout << "Date updated to: "
                << date.year() << "-"
                << date.month() << "-"
                << date.day() << std::endl;
        }
    };
}

int main() {
    // Create an ObservableValue to hold the Date
    HKUltra::ObservableValue<HKUltra::Date> observableDate(HKUltra::Date(HKUltra::Year(2023), std::chrono::month(1), std::chrono::day(1)));

    // Create multiple observers
    HKUltra::DateObserver observer1;
    HKUltra::DateObserver observer2;

    // Attach observers to the observable
    observer1.registerWith(observableDate);
    observer2.registerWith(observableDate);

    // Function to simulate changing the Date in a thread
    auto updateDate = [&observableDate](const HKUltra::Years& yearsToAdd) {
        //std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate work
        HKUltra::Date newDate = observableDate.get();  // Get current date
        observableDate.set(newDate + yearsToAdd);  // Update the date by adding years
        };

    // Start multiple threads to update the date
    std::thread t1(updateDate, HKUltra::Years(1));  // Add 1 year
    std::thread t2(updateDate, HKUltra::Years(2));  // Add 2 years
    std::thread t3(updateDate, HKUltra::Years(3));  // Add 3 years

    // Wait for threads to finish
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
