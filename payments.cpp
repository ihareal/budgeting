#include "payments.h"

 // initial values
 int Payments::Id = 0;
 float Payments::Cost = 0.0;
 int Payments::UserId = 0;
 QString Payments::Description = "";
 int Payments::Category = 0;
 QDate Payments::Date = QDate::currentDate();

    // describes payment model
 Payments::Payments()
 {

 }
