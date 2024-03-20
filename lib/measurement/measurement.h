#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <string>
#include <Arduino.h>

class BasicMeasurment
{
    std::string description;
    std::string unit;

    std::string trimString(const std::string &str)
    {
        if (str.length() > 5)
        {
            return str.substr(0, 5); // Extract the first 5 characters
        }
        return str; // Return the original string if it's not longer than 5
    }

public:
    std::string getString(unsigned int descriptionLength = UINT_MAX)
    {
        return description.substr(0, descriptionLength);
    }
};

class Measurement
{
private:
    double value;
    std::string description;
    std::string unit;

public:
    Measurement(const std::string &description, const std::string &unit = "");

    double getValue() const;
    void setValue(double value);

    std::string getDescription() const;

    std::string getUnit() const;
};

#endif /* MEASUREMENT_H */