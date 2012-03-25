#include "Listener_Return.h"

Listener_Return::Listener_Return()
{
}

void Listener_Return::newCopy(unsigned int orderId,std::vector<std::string> sources,std::string destination)
{
	emit sendNewCopy(orderId,QStringList() << "not converted",QString::fromStdString(destination));
}

void Listener_Return::newMove(unsigned int orderId,std::vector<std::string> sources,std::string destination)
{
	emit sendNewMove(orderId,QStringList() << "not converted",QString::fromStdString(destination));
}
