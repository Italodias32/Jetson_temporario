#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "controller/serial.hpp"
#include <linux/serial.h>

	//Class constructor
Serial::Serial():port(io),quitFlag(false){}
	//Class destructor
Serial::~Serial()
{
	//Stop the I/O services
	io.stop();
	//Wait for the thread to finish
	runner.join();
}

const char* Serial::buffer()
{
	const char* bufPtr=boost::asio::buffer_cast<const char*>(_buffer.data());
	return bufPtr;
}

std::string Serial::read(int bytes)
{
	boost::asio::streambuf buffer;
	boost::system::error_code ec;
	// std::cout << "antes de ler" << std::endl;
	std::size_t n =  0;
	char c;
	std::string s;
	
	
	while (n < bytes) {
		n += boost::asio::read(port, boost::asio::buffer(&c,1),boost::asio::transfer_exactly(1), ec);
		s += c;
	}
	
	return s;
}	

std::string Serial::read_timeout(int bytes, int ms)
{
	blocking_reader leitor(port,ms);

	boost::asio::streambuf buffer;
	boost::system::error_code ec;
	// std::cout << "antes de ler" << std::endl;
	std::size_t n =  0;
	char c;
	std::string s;

	boost::asio::read(port, boost::asio::buffer(&c,1),boost::asio::transfer_exactly(1), ec);
	s += c;
	n = n+1;
	while (n < bytes) {
		if(leitor.read_char(c) == false) {
			s = "vazio";
			std::cout << "vazio\n";
			return s;
		}
		//n += boost::asio::read(port, boost::asio::buffer(&c,1),boost::asio::transfer_exactly(1), ec);
		n = n+1;		
		s += c;
	}
	return s;
}	

char Serial::readByte()
{
	boost::asio::streambuf buffer;
	boost::system::error_code ec;
	char c;
	int n = 0;
	while (n==0) {	
		n += boost::asio::read(port, boost::asio::buffer(&c,1),boost::asio::transfer_exactly(1), ec);
	}
	//std::cout << "n: " << n << std::endl;
	//std::cout << "c: " << c << std::endl;
	return c;
}	


//Connection method that will setup the serial port
bool Serial::connect(const std::string& port_name, int baud)
{
	try
	{
		using namespace boost::asio;
		port.open(port_name);
		//Setup port
		port.set_option(serial_port::baud_rate(baud));
		port.set_option(serial_port::flow_control(serial_port::flow_control::none));
		std::cout << "port: " << port_name << "; baud rate: " << baud << "\n";

		/*int fd = port.native_handle();
                struct serial_struct ser_info;
                int resp = ioctl(fd, TIOCGSERIAL, &ser_info);
		if(resp != 0)
		{
			if(resp == EBADF) std::cout << "Erro: EBADF"<< std::endl;
			if(resp == EFAULT) std::cout << "Erro: EFAULT"<< std::endl;
			if(resp == EINVAL) std::cout << "Erro: EINVAL"<< std::endl;
			if(resp == ENOTTY) std::cout << "Erro: ENOTTY"<< std::endl;
		}		
                ser_info.flags |= ASYNC_LOW_LATENCY; 
                resp == ioctl(fd, TIOCSSERIAL, &ser_info);
		if(resp != 0)
		{
			if(resp == EBADF) std::cout << "Erro: EBADF"<< std::endl;
			if(resp == EFAULT) std::cout << "Erro: EFAULT"<< std::endl;
			if(resp == EINVAL) std::cout << "Erro: EINVAL"<< std::endl;
			if(resp == ENOTTY) std::cout << "Erro: ENOTTY"<< std::endl;
		}*/
	} 
	catch(boost::system::system_error& e)
	{
		std::cout << "Error opening serial port " << port_name << " at baud rate " << baud << std::endl; 
		 std::cout<<"Error: " << e.what()<<std::endl;
		 std::cout<<"Info: "  << boost::diagnostic_information(e) <<std::endl;
		return false;
	}

	return port.is_open();
}

//The method that will be called to issue a new 
//asynchronous read
void Serial::startReceive()
{
	// using namespace boost::asio;
	//Issue a async receive and give it a callback
	//onData that should be called when "\r\n"
	//is matched.
	//async_read_until(port, buffer,"\r\n",functor);
	boost::asio::async_read_until(port, _buffer,"\r\n",boost::bind(&Serial::onData,this, _1,_2));
	//  boost::asio::async_read(port, buffer,boost::asio::transfer_at_least(1),boost::bind(&Serial::onData,this, _1,_2));
} 

void Serial::onData(const boost::system::error_code& e,std::size_t size)
{
	if (!e)
	{
		std::istream is(&_buffer);
		std::string data(size,'\0');
		is.read(&data[0],size);
		std::cout<<"Received data:"<<data;
		//If we receive quit()\r\n indicate
		//end of operations
		quitFlag = (data.compare("quit()\r\n") == 0);
	}

	startReceive();
}
	  
//Function for sending a data string
void Serial::send(void * text, size_t size)
{
	char* aux = (char*)text;
	//unsigned long   flags;
	//save_flags(flags);
	//cli();
	for(int i=0;i<size;i++)
	{
		boost::asio::write(port, 
		boost::asio::buffer(aux+i,1));	
		// estourando FIFO!!!!!		
		//usleep( 180 ); 
		//usleep(200);
	}
	//sti();
	//restore_flags(flags);
}
	  
//Quit Function
bool Serial::quit(){return quitFlag;}

