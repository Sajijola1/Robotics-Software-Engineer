#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    //bool uniform_image = true;

    // Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            int index = i % img.step; // Dividing by img.step gives you the index
            if(index < img.step * 1/3){
            	// The white pixel is on the left side of the image
            	// Drive the robot left
            	drive_robot(0.1,-0.2);
            	ROS_INFO_STREAM("LEFT");
            	break;
            } 
            else if((index >= img.step * 1/3) && (index < img.step * 2/3)){
            	// The white pixel is at the middle of the image
            	// Drive the robot forward
            	drive_robot(0.1,0.0);
            	ROS_INFO_STREAM("MIDDLE");
            	break;
            }
            else if((index >= img.step * 2/3) && (index < img.step)){
            	// The white pixel is on the right side of the image
            	// Drive the robot left
            	drive_robot(0.1,0.2);
            	ROS_INFO_STREAM("RIGHT");
            	break;
            }
            //uniform_image = false; 
            //break;
        }
        
        // After looping through the image
        if (i == (img.height * img.step) - 1){
            // If we get to the end of the loop without the break statement executing,
            // No ball has been seen
            
            // Stop the robot
            drive_robot(0.0,0.0);
        }
    }

    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
