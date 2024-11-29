#pragma once
#include <chrono>
#include <exception>
#include <mutex>
namespace HKUltra {

    // Typedefs for convenience to use chrono types for dates
    typedef std::chrono::year Year;     // Represents a year
    typedef std::chrono::month Month;   // Represents a month
    typedef std::chrono::day Day;       // Represents a day

    // Typedefs for convenience to represent durations of years, months, and days
    typedef std::chrono::years Years;   // Represents a duration of years
    typedef std::chrono::months Months; // Represents a duration of months
    typedef std::chrono::days Days;     // Represents a duration of days

    // Date class represents a specific date and provides utilities for date manipulation
    class Date {
    public:
        // SerialType is a long integer used to represent the date in serial format
        typedef long SerialType;

        // Default constructor: Initializes a date object with a default value (typically the epoch date)
        Date();

        // Constructor: Initializes a date with a specific year, month, and day
        Date(Year year, Month month, Day day);

        //Copy constructor (required due to mutex)
        Date(const Date& date);
        Date& operator=(const Date& rhs);

        // Destructor: Default destructor, no special cleanup needed
        ~Date() = default;

        // Accessor for the day component of the date
        Day day() const;

        // Accessor for the month component of the date
        Month month() const;

        // Accessor for the year component of the date
        Year year() const;

        // Accessor for the serial number representing the date (useful for calculations, comparisons, etc.)
        SerialType serialNumber() const;

        // Mathematical operators for manipulating dates by adding or subtracting durations of years, months, or days

        // Add a duration of years to the current date
        Date& operator+=(const Years& years);

        // Subtract a duration of years from the current date
        Date& operator-=(const Years& years);

        // Add a duration of months to the current date
        Date& operator+=(const Months& months);

        // Subtract a duration of months from the current date
        Date& operator-=(const Months& months);

        // Add a duration of days to the current date
        Date& operator+=(const Days& days);

        // Subtract a duration of days from the current date
        Date& operator-=(const Days& days);

        // Create a new date by adding a duration of years to the current date
        Date operator+(const Years& years) const;

        // Create a new date by subtracting a duration of years from the current date
        Date operator-(const Years& years) const;

        // Create a new date by adding a duration of months to the current date
        Date operator+(const Months& months) const;

        // Create a new date by subtracting a duration of months from the current date
        Date operator-(const Months& months) const;

        // Create a new date by adding a duration of days to the current date
        Date operator+(const Days& days) const;

        // Create a new date by subtracting a duration of days from the current date
        Date operator-(const Days& days) const;

        // Logic operators to compare dates

        // Check if the current date is equal to another date
        bool operator==(const Date& rhs) const;

        // Check if the current date is greater than another date
        bool operator>(const Date& rhs) const;

        // Check if the current date is less than another date
        bool operator<(const Date& rhs) const;

        // Check if the current date is greater than or equal to another date
        bool operator>=(const Date& rhs) const;

        // Check if the current date is less than or equal to another date
        bool operator<=(const Date& rhs) const;

    private:
        // Mutex for thread safety
        mutable std::mutex mtx_;

        // Internal representation of the date using chrono's year_month_day type
        std::chrono::year_month_day year_month_day_;

        // Serial number for the date, used for quick comparison and calculations
        Date::SerialType serial_number_;

        // Resets the serial number when the date changes
        void resetSerial();

        // Static helper method to calculate the last day of a given month
        static Day lastDayOfMonth(std::chrono::year_month_day year_month_day);
    };

} // namespace HKUltra
