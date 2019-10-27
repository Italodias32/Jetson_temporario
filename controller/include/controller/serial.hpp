#ifndef SERIAL_H
#define SERIAL_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "serial_timeout.h"


class Serial
{
private:
	// Pointer of the callback function that will be executed when data 
	//arrives.
	boost::function<void(const boost::system::error_code& e,std::size_t size)> functor;

	//Buffer in which to read the serial data
	boost::asio::streambuf _buffer;
	// quit Flag
	bool quitFlag;
	// vetor de dados de entrada
	std::vector<double> input;

	//Boost.Asio I/O service required for asynchronous 
	//operation
	boost::asio::io_service io;
	// private:
	//Serial port accessor class
	boost::asio::serial_port port;
	//Background thread
	boost::thread runner;

public:
	//Class constructor
	Serial();
	//Class destructor
	~Serial();

	const char* buffer();
	std::string read(int bytes = 1);
        std::string read_timeout(int bytes, int ms);
	char readByte();
	//Connection method that will setup the serial port
	bool connect(const std::string& port_name, int baud = 9600);

	//The method that will be called to issue a new 
	//asynchronous read
	void startReceive();
	  
	void onData(const boost::system::error_code& e,std::size_t size);

	  
	//Function for sending a data string
	void send(void * text, size_t size);
	  
	//Quit Function
	bool quit();
};

#endif
