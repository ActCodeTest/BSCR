#include "Date.h"
#include <mutex>

namespace HKUltra {

    // Default constructor: Initializes Date to January 1, 1900
    Date::Date() {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ = { Year(1900) / Month(1) / Day(1) };  // Set to the default date: 1st Jan 1900
        resetSerial();  // Update the serial number after initialization
    }

    // Constructor with Year, Month, and Day as inputs
    Date::Date(Year year, Month month, Day day) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ = { year / month / day };  // Construct the date with the provided values
        if (!year_month_day_.ok()) {  // Check if the date is valid
            std::exception err("Invalid date.");
            throw err;  // Throw an exception if the date is invalid
        }
        resetSerial();  // Update the serial number after setting the date
    }

    // Copy constructor: Creates a new Date by copying the state of another Date object
    Date::Date(const Date& date) {
        *this = date;
    }

    Date& Date::operator=(const Date& rhs) {
        if (this != &rhs) {  // Check for self-assignment
            // Lock both the source and destination Date objects in a deadlock-free order
            std::lock(rhs.mtx_, mtx_);
            std::lock_guard<std::mutex> lockA(rhs.mtx_, std::adopt_lock);
            std::lock_guard<std::mutex> lockB(mtx_, std::adopt_lock);

            year_month_day_ = rhs.year_month_day_;  // Copy year, month, and day
            serial_number_ = rhs.serial_number_;  // Copy the serial number from the other Date
        }
        return *this;
    }

    // Getter for the year component of the date
    Year Date::year() const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return year_month_day_.year();  // Return the year
    }

    // Getter for the month component of the date
    Month Date::month() const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return year_month_day_.month();  // Return the month
    }

    // Getter for the day component of the date
    Day Date::day() const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return year_month_day_.day();  // Return the day
    }

    // Getter for the serial number associated with the date
    Date::SerialType Date::serialNumber() const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_;  // Return the serial number
    }

    // Addition operator (+=) for years, updates the date by adding the given years
    Date& Date::operator+=(const Years& years) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ += years;  // Add years to the date
        if (!year_month_day_.ok()) {  // If the date is not valid after addition
            // Adjust the date to the last day of the month
            year_month_day_ = std::chrono::year_month_day{ year_month_day_.year() / year_month_day_.month() / lastDayOfMonth(year_month_day_) };
        }
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    // Subtraction operator (-=) for years, updates the date by subtracting the given years
    Date& Date::operator-=(const Years& years) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ -= years;  // Subtract years from the date
        if (!year_month_day_.ok()) {  // If the date is not valid after subtraction
            // Adjust the date to the last day of the month
            year_month_day_ = std::chrono::year_month_day{ year_month_day_.year() / year_month_day_.month() / lastDayOfMonth(year_month_day_) };
        }
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    // Similar operators for Months and Days (+=, -=) to modify the date by adding or subtracting months or days
    Date& Date::operator+=(const Months& months) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ += months;  // Add months to the date
        if (!year_month_day_.ok()) {  // If the date is not valid after addition
            // Adjust the date to the last day of the month
            year_month_day_ = std::chrono::year_month_day{ year_month_day_.year() / year_month_day_.month() / lastDayOfMonth(year_month_day_) };
        }
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    Date& Date::operator-=(const Months& months) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ -= months;  // Subtract months from the date
        if (!year_month_day_.ok()) {  // If the date is not valid after subtraction
            // Adjust the date to the last day of the month
            year_month_day_ = std::chrono::year_month_day{ year_month_day_.year() / year_month_day_.month() / lastDayOfMonth(year_month_day_) };
        }
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    Date& Date::operator+=(const Days& days) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ = std::chrono::sys_days(year_month_day_) + days;  // Add days to the date
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    Date& Date::operator-=(const Days& days) {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        year_month_day_ = std::chrono::sys_days(year_month_day_) - days;  // Subtract days from the date
        resetSerial();  // Update the serial number after modification
        return *this;
    }

    // Other operators: + and - for adding or subtracting years, months, or days
    Date Date::operator+(const Years& years) const {
        Date date = *this;
        date += years;  // Modify the date by adding years
        return date;  // Return the modified date
    }

    Date Date::operator-(const Years& years) const {
        Date date = *this;
        date -= years;  // Modify the date by subtracting years
        return date;  // Return the modified date
    }

    // Similar operators for Months and Days
    Date Date::operator+(const Months& months) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        Date date = *this;
        date += months;  // Modify the date by adding months
        return date;  // Return the modified date
    }

    Date Date::operator-(const Months& months) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        Date date = *this;
        date -= months;  // Modify the date by subtracting months
        return date;  // Return the modified date
    }

    Date Date::operator+(const Days& days) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        Date date = *this;
        date += days;  // Modify the date by adding days
        return date;  // Return the modified date
    }

    Date Date::operator-(const Days& days) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        Date date = *this;
        date -= days;  // Modify the date by subtracting days
        return date;  // Return the modified date
    }

    // Comparison operators
    bool Date::operator==(const Date& rhs) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_ == rhs.serial_number_;  // Compare the serial numbers for equality
    }

    bool Date::operator>(const Date& rhs) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_ > rhs.serial_number_;  // Compare the serial numbers
    }

    bool Date::operator<(const Date& rhs) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_ < rhs.serial_number_;  // Compare the serial numbers
    }

    bool Date::operator>=(const Date& rhs) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_ >= rhs.serial_number_;  // Compare the serial numbers
    }

    bool Date::operator<=(const Date& rhs) const {
        std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        return serial_number_ <= rhs.serial_number_;  // Compare the serial numbers
    }

    // Resets the serial number based on the current date
    void Date::resetSerial() {
        //std::lock_guard<std::mutex> lock(mtx_);  // Lock the mutex to ensure thread safety
        serial_number_ = std::chrono::sys_days(year_month_day_).time_since_epoch().count();  // Compute the serial number
    }

    // Helper function to find the last day of the given month
    Day Date::lastDayOfMonth(std::chrono::year_month_day year_month_day) {
        unsigned days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };  // Days in each month
        unsigned month = unsigned(year_month_day.month());
        if (year_month_day.year().is_leap() && month == 2) {  // Check for leap year in February
            return Day(29);  // Leap year February has 29 days
        }
        return Day(days_in_month[month - 1]);  // Return the last day of the month
    }

}
