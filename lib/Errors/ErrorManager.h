#ifndef ErrorManager_h
#define ErrorManager_h
#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "Error.h"
#include "config.h"

class ErrorManager
{
public:
  ErrorManager()
  {
  }

  void addError(Error &error)
  {
    errors.push_back(&error);
  }

  uint8_t getMaxPrio()
  {
    // Serial.println("Checking Errors");
    uint8_t maxPrio = 0;
    for (const Error *err : (this->errors))
    {
      if (err->isActive())
      {
        if (err->getPriority() > maxPrio)
        {
          maxPrio = err->getPriority();
        }
      }
    }
    return maxPrio;
  }

  uint8_t getMaxPrioNotAck()
  {
    // Serial.println("Checking Errors");
    uint8_t maxPrio = 0;
    for (const Error *err : (this->errors))
    {
      if (err->isActive() && !err->isAck())
      {
        if (err->getPriority() > maxPrio)
        {
          maxPrio = err->getPriority();
        }
      }
    }
    return maxPrio;
  }

  void printActiveErrors()
  {
    for (const Error *err : (this->errors))
    {
      if (err->isActive())
      {
        debug("Error Active");
        debugln(err->getErrorStr());
      }
    }
  }

private:
  std::vector<Error *> errors;
};

#endif
