

#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include <thread>
#include <Eigen/Eigen>
#include <exception>


using namespace std::chrono_literals;

#include"controller/serial.hpp"
#include"controller/frame.hpp"
#include"controller/protocol.hpp"
#include"controller/LQR4.hpp"
#include <sched.h>

class MinimalTimer : public rclcpp::Node
{
public:
  
  MinimalTimer()
  : Node("minimal_timer")
  {
     //if (!serial.connect("/dev/ttyTHS2",3000000))
	//if (!serial.connect("/dev/ttyTHS2",921600))
	if (!serial.connect("/dev/ttyTHS2",460800))
  {
		std::cout << "desconectando\n";
		exit(0);
  }
   
      
      t = new std::thread(std::bind(&MinimalTimer::thread, this));
      
	
  }
  
 

private:
	
	 void thread()
	 {
		std::chrono::high_resolution_clock::time_point tf;
		std::chrono::high_resolution_clock::time_point to;

		std::cout<< "thread2" <<std::endl;
		std::vector<double> data;
		try
		{
			//int i = 0;
			instancia.config();	
			while(1){
							
			
				Frame frame;
				frame = receive(serial);

				if(frame.unbuild()){
					
					for(int i = 0; i<16;i++)
					{		
						input.at(i) = frame.getFloat();
					}
					for(int i = 0; i<3;i++)
					{		
						ref.at(i) = frame.getFloat();
					}
					output = instancia.execute(input,ref);
					Frame frameSend;
					frameSend.addFloat(output.at(0));
					frameSend.addFloat(output.at(1));
					frameSend.addFloat(output.at(2));
					frameSend.addFloat(output.at(3));
					frameSend.build();
					serial.send(frameSend.buffer(),frameSend.buffer_size());
					
				}
				else std::cout<< "Error" <<std::endl;
				
			}
			
		}catch(std::exception& e)
		{
			std::cout <<"Error\n";
		}

	}


	
	lqr_vant4 instancia;
	std::vector<double> output{0,0,0,0};
	int i = 0;
	std::vector<double> input{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	std::vector<double> ref{0,0,2,0,0,0,0,0,0,0,0,0};
	rclcpp::TimerBase::SharedPtr timer_;
	Serial serial;
	std::thread *t;
	
};

int main(int argc, char * argv[])
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<MinimalTimer>());
	rclcpp::shutdown();
	return 0;
}
