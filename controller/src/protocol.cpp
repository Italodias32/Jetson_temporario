#include <iostream>
#include "controller/protocol.hpp"

Frame receive(Serial &serial)
{

	Frame frame;
	u8 header = frame.header();
	u8 end = frame.end();
	i32  status = 0;
	u8 last = 'a';
	bool fim  = false;
	//std::cout << header << "  header" << std::endl;
	//std::cout << end << "  end" << std::endl;


	while(!fim) {
		//std::string byte = "";
		//std::cout << "byte.size()" << byte.size() << std::endl;
		//byte = serial.read();
		std::string output  = serial.read_timeout(1,3);
		char byte = output.at(0);
		//char byte  = serial.readByte();
		//std::cout << "byte.size()" << byte.size() << std::endl;
		//std::cout << "byte " << byte << std::endl;
		//std::cout << "byte " << byte << std::endl;
		//std::cout << "byte.c_str()" << byte.c_str() << std::endl;
		//std::cout << "byte[0] " << byte.at(0) << std::endl;
		//std::cout << "byte[1] " << byte.at(1) << std::endl;	
		//std::cout << byte.size() << std::endl;		
		//std::cout << i << std::endl ;
		//i++;
		u8 b[1];
		//const char *teste;
		//teste = byte.c_str(); 
		b[0] = byte;
		//std::cout << byte.c_str() << std::endl;	
		//std::cout << b[0] << std::endl;	
		switch(status) {
		case 1:
			//std::cout << "Entrei 1" << std::endl << std::endl;
			if (b[0] == last && last == header && header == end)
			{
				//std::cout << "Aqui" << std::endl;
				break;
			}
			if (b[0] != end)
			{
				//std::cout << "Aqui2" << std::endl;
				frame.addByte(b[0]);
			}
			if (b[0] == end) {
				//std::cout << "Aqui3" << std::endl;
				frame.addEnd();
				status = 0;
				fim = true;
			}
			break;
		case 0:
			//std::cout << "Entrei 2" << std::endl << std::endl;
			if (b[0] == header) {
				//std::cout << "Aqui4" << std::endl;
				frame.addHeader();
				status = 1;
			}
			break;
		}
		last = b[0];
	}
	return frame;
}

void print_frame(Frame frame, u8 type)
{
	// std::cout << "Raw frame:" << std::endl;
	// u8 *data = frame.data();
	// i32 size  = frame.size();
	// print_buffer(data,size);
	// std::cout << "\n\n";
	if (frame.unbuild()) {
		u8 *data = frame.data();
		i32 size  = frame.size();
		std::cout << "Decoded frame:" << std::endl;
		if (type == FLOAT_FRAME)
			print_float_buffer(frame);
		else
			print_buffer(frame.data(),frame.size());
					
		frame.build();
	} else {
		std::cout << "Error: corrupted frame...\n";
	}
}

void print_buffer(u8 *data, i32 size)
{
	for(int i=0;i<size;i++)
		std::cout << data[i];
}

void print_float_buffer(Frame frame)
{
	i32 num_floats = frame.size()/sizeof(float);
	std::cout << num_floats << std::endl; 
	for(int i=0;i<num_floats;i++) {
		float n = frame.getFloat();
		std::cout << n << std::endl;
	}
}

void delay( unsigned long ms )
{
	usleep( ms * 1000 );
}
