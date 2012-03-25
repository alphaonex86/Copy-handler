/** \file PluginInterface_Listener.h
\brief Define the interface of the plugin of type: listener
\author alpha_one_x86
\version 0.3
\date 2010 */ 

#ifndef PLUGININTERFACE_LISTENER_H
#define PLUGININTERFACE_LISTENER_H

#include <iostream>
#include <string>
#include <vector>

class PluginInterface_Listener_Return
{
	public:
		virtual void newCopy(unsigned int orderId,std::vector<std::string> sources);
		virtual void newCopy(unsigned int orderId,std::vector<std::string> sources,std::string destination) = 0;
		virtual void newMove(unsigned int orderId,std::vector<std::string> sources);
		virtual void newMove(unsigned int orderId,std::vector<std::string> sources,std::string destination) = 0;
	protected:
		std::string getDirectory();
};

class PluginInterface_Listener
{
	public:
		PluginInterface_Listener(PluginInterface_Listener_Return * returnObject);
		/// \brief put this plugin in listen mode
		virtual bool listen() = 0;
		/// \brief put close the listen
		virtual void close() = 0;
		/// \brief send when copy is finished
		virtual void transferFinished(unsigned int orderId,bool withError) = 0;
		/// \brief send when copy is canceled
		virtual void transferCanceled(unsigned int orderId) = 0;
	private:
		PluginInterface_Listener_Return * returnObject;
};

#endif // PLUGININTERFACE_LISTENER_H
