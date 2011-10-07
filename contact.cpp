#include "contact.h"
#include <QRegExp>
#include <QDebug>


/*!
  The constructor sets default values for its memeber variables.
*/
Contact::Contact(QObject *parent) :
    QObject(parent)
{
    // setup default values
    itsName = "";
    itsId = "";
    itsPhoneNumber = "";
}


/*!
  GetDisplayString provides a well formated contact display string.
*/
QString Contact::GetDisplayString() const
{
    if (!itsName.isEmpty())
        return QString("%1 (%2)").arg(itsName).arg(itsPhoneNumber);
    else
        return itsPhoneNumber;
}


/*!
  SetNumber stores a new phone number and deletes the other fields because they
  may not match anymore.
*/
void Contact::SetNumber(QString newNumber)
{
    itsPhoneNumber = newNumber;

    // clear other entries, they do not fit anymore
    itsName = "";
    itsId = "";
}


/*!
  HasValidNumber checks for a well formed phone number.
*/
bool Contact::HasValidNumber() const
{
    // only allow the following characters in phone number: +,*,#, ,-,(,),0-9
    QRegExp regexChars("^[\\+\\*# \\-\\(\\)\\d]+$");
    // at least 1 digit must be present in the number
    QRegExp regexDigit("\\d+");

    if ( (regexChars.indexIn(itsPhoneNumber) != -1) &&
         (regexDigit.indexIn(itsPhoneNumber) != -1) )
        return true;
    else
        return false;
}


/*!
  IsNumberMatching checks whether the given phone number corresponds to the stored one.
*/
bool Contact::IsNumberMatching(QString number) const
{
    // if the numbers are equal, they match
    if (itsPhoneNumber == number)
        return true;

    // remove leading zeros and '+' characters from both numbers
    QString n1 = itsPhoneNumber;
    QString n2 = number;
    QRegExp leadingSigns("^[0\\+]+([1-9].*)$");
    if (leadingSigns.indexIn(n1) > -1)
        n1 = leadingSigns.cap(1);
    if (leadingSigns.indexIn(n2) > -1)
        n2 = leadingSigns.cap(1);
    // we only compare up to the length of the shorter string to omit the other predailing digits
    int size = (n1.size() < n2.size() ? n1.size() : n2.size());

    // this shorter strings must match and the sizes must still somehow fit...
    if ( (size > 5) && (n1.size()-size <= 5) && (n2.size()-size <= 5) &&
         (n1.right(size) == n2.right(size)) )
        return true;

    // otherwise the numbers to not match
    return false;
}
