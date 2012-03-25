#ifndef LISTENER_RETURN_H
#define LISTENER_RETURN_H

#include "PluginInterface_Listener.h"

#include <QObject>
#include <QString>
#include <QStringList>

/// \todo see to remplace std::string by std::wstring
class Listener_Return : public QObject, public PluginInterface_Listener_Return
{
    Q_OBJECT
public:
	Listener_Return();
	void newCopy(unsigned int orderId,std::vector<std::string> sources,std::string destination);
	void newMove(unsigned int orderId,std::vector<std::string> sources,std::string destination);
signals:
	void sendNewCopy(unsigned int orderId,QStringList sources,QString destination);
	void sendNewMove(unsigned int orderId,QStringList sources,QString destination);
};

#endif // LISTENER_RETURN_H
