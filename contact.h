#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

class Contact : public QObject
{
    Q_OBJECT
public:
    explicit Contact(QObject *parent = 0);
    QString GetDisplayString() const;
    void SetNumber(QString newNumber);
    bool HasValidNumber() const;
    bool IsNumberMatching(QString number) const;


public:
    QString itsName;
    QString itsPhoneNumber;
    QString itsId;
};

#endif // CONTACT_H
