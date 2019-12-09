#include "payments.h"

    // initial values
 QString startDate = "2019-01-29";
 int Payments::Id = 0;
 float Cost = 0.0;
 int UserId = 0;
 QString Description = "";
 int Category = 0;
 QDate Payments::Date = QDate::currentDate();

    // describes payment model
 Payments::Payments()
 {

 }
