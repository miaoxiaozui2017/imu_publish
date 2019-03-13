/*// Step 1: Include Library Headers: 
//发布imu的数据到IMU_data 
#include <ros/ros.h> 
#include <sensor_msgs/Imu.h> 
int main(int argc, char** argv) { 
    // Step 2: Initialization: 
    ros::init(argc, argv, "imu_gps");     
    ros::NodeHandle n;  
    ros::Publisher IMU_pub = n.advertise<sensor_msgs::Imu>("IMU_data", 20);  
    ros::Rate loop_rate(50);  
    while(ros::ok()) { 
        sensor_msgs::Imu imu_data; 
	imu_data.header.stamp = ros::Time::now(); 
	imu_data.header.frame_id = "base_link"; 
	//四元数位姿,所有数据设为固定值，可以自己写代码获取ＩＭＵ的数据，，然后进行传递
	imu_data.orientation.x = 0; 
	imu_data.orientation.y = -1; 
	imu_data.orientation.z = -5; 
	imu_data.orientation.w = 6; 
	//线加速度 
	imu_data.linear_acceleration.x = 0.01; 
	imu_data.linear_acceleration.y = 0.02; 
	imu_data.linear_acceleration.z = 0.03; 
	//角速度 
	imu_data.angular_velocity.x = 0.05; 
	imu_data.angular_velocity.y = 0.06; 
	imu_data.angular_velocity.z = 0.07; 
	IMU_pub.publish(imu_data); 
	ros::spinOnce();  
	loop_rate.sleep();  
    } 
    return 0; 
}
*/


//1.通过sudo apt-get install ros-<distro>-serial下载ROS对应版本的工具包
//serial会安装在opt/ros/<distro>/share/serial目录下，
//重启终端，输入 roscd serial，可以检测到serial包的路径，说明serial包已经安装成功
//2.使用ros自带的serial包，编写节点
#include <ros/ros.h> 
#include <serial/serial.h>  //ROS已经内置了的串口包 
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include <sensor_msgs/Imu.h> 
serial::Serial ser; //声明串口对象 

//回调函数 
void write_callback(const std_msgs::String::ConstPtr& msg) 
{ 
    ROS_INFO_STREAM("Writing to serial port" <<msg->data); 
    ser.write(msg->data);   //发送串口数据 
} 

int main (int argc, char** argv) 
{ 
    //初始化节点 
    ros::init(argc, argv, "serial_example_node"); 
    //声明节点句柄 
    ros::NodeHandle nh; 

    //订阅主题，并配置回调函数 
    ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback); 
    //发布主题 
    ros::Publisher read_pub = nh.advertise<sensor_msgs::Imu>("/IMU_data", 20); 

    try 
    { 
    //设置串口属性，并打开串口 
        ser.setPort("/dev/ttyUSB0"); 
        ser.setBaudrate(115200); 
        serial::Timeout to = serial::Timeout::simpleTimeout(1000); 
        ser.setTimeout(to); 
        ser.open(); 
    } 
    catch (serial::IOException& e) 
    { 
        ROS_ERROR_STREAM("Unable to open port "); 
        return -1; 
    } 

    //检测串口是否已经打开，并给出提示信息 
    if(ser.isOpen()) 
    { 
        ROS_INFO_STREAM("Serial Port initialized"); 
    } 
    else 
    { 
        return -1; 
    } 

    //指定循环的频率 
    ros::Rate loop_rate(100); 
    while(ros::ok()) 
    { 

        if(ser.available()){ 
            //ROS_INFO_STREAM("Reading from serial port\n"); 
            std_msgs::String result; 
            result.data = ser.read(ser.available());
            // size_t available ()
            // std::string read(size_t size=1)
            
            // if(result.data.length() == 51)
            // {
            //         double ImuArray[6];
            //         memcpy(ImuArray,&result.data[0],result.data.size());	
                
            //     sensor_msgs::Imu imu_data; 
            //     imu_data.header.stamp = ros::Time::now(); 
            //     imu_data.header.frame_id = "base_link"; 
            //     //四元数位姿,所有数据设为固定值，可以自己写代码获取ＩＭＵ的数据，，然后进行传递
            //     //imu_data.orientation.x = 0; 
            //     //imu_data.orientation.y = -1; 
            //     //imu_data.orientation.z = -5; 
            //     //imu_data.orientation.w = 6; 
            //     //线加速度 
            //     imu_data.linear_acceleration.x = ImuArray[0];//0.01; 
            //     imu_data.linear_acceleration.y = ImuArray[1];//0.02; 
            //     imu_data.linear_acceleration.z = ImuArray[2];//0.03; 
            //     //角速度 
            //     imu_data.angular_velocity.x = ImuArray[3];//0.05; 
            //     imu_data.angular_velocity.y = ImuArray[4];//0.06; 
            //     imu_data.angular_velocity.z = ImuArray[5];//0.07;
                        
            //     read_pub.publish(imu_data); 
            // }
            
            double ImuArray[6];
            memcpy(ImuArray,&result.data[0],result.data.size()-3);
            sensor_msgs::Imu imu_data; 
            imu_data.header.stamp = ros::Time::now(); 
            imu_data.header.frame_id = "base_link"; 
            //线加速度 
            imu_data.linear_acceleration.x = ImuArray[0];//0.01; 
            imu_data.linear_acceleration.y = ImuArray[1];//0.02; 
            imu_data.linear_acceleration.z = ImuArray[2];//0.03; 
            //角速度 
            imu_data.angular_velocity.x = ImuArray[3];//0.05; 
            imu_data.angular_velocity.y = ImuArray[4];//0.06; 
            imu_data.angular_velocity.z = ImuArray[5];//0.07;
                        
            read_pub.publish(imu_data); 
	        
          
          // ROS_INFO_STREAM("linear_acceleration.x = " << ImuArray[0] << '\n'
          //            << "linear_acceleration.y = " << ImuArray[1] << '\n'
		      //            << "linear_acceleration.z = " << ImuArray[2] << '\n' 	                 
          //            << "angular_velocity.x = " << ImuArray[3] << '\n'
    		  //            << "angular_velocity.y = " << ImuArray[4] << '\n'
		      //            << "angular_velocity.z = " << ImuArray[5] << '\n');
            
          //ROS_INFO_STREAM(ImuArray[0] << ',' << ImuArray[1] << ',' << ImuArray[2] << ',' << ImuArray[3] << ',' << ImuArray[4] << ',' << ImuArray[5]);
          //ROS_INFO_STREAM("Read: " << result.data); 
          //ROS_INFO_STREAM("Read size: " << result.data.length()); 
          //read_pub.publish(result); 
	    
        } 

        //处理ROS的信息，比如订阅消息,并调用回调函数 
        ros::spinOnce(); 
        loop_rate.sleep(); 

    } 
} 

