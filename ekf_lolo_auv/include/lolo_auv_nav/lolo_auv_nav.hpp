#ifndef SMALL_AUV_NAV_HPP
#define SMALL_AUV_NAV_HPP

#include "utils_matrices.hpp"

#include <queue>

#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/thread/mutex.hpp>

#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>

#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/TransformStamped.h>



class LoLoEKF{

public:

    LoLoEKF(std::string node_name, ros::NodeHandle &nh);
    void ekfLocalize();
    ~LoLoEKF();

private:

    // ROS variables
    ros::NodeHandle *nh_;
    std::string node_name_;
    ros::Subscriber imu_subs_;
    ros::Subscriber dvl_subs_;
    ros::Subscriber tf_gt_subs_;
    ros::Publisher odom_pub_;

    // Handlers for sensors
    std::queue<sensor_msgs::ImuPtr> imu_readings_;
    std::queue<geometry_msgs::TwistWithCovarianceStampedPtr> dvl_readings_;
    std::queue<nav_msgs::OdometryPtr> gt_readings_;
    boost::mutex msg_lock_;
    // System state variables
    boost::numeric::ublas::vector<double> mu_;
    boost::numeric::ublas::vector<double> mu_hat_;

    double delta_t_;
    // tf
    tf::TransformBroadcaster odom_bc_;
    tf::StampedTransform transf_dvl_base_;
    tf::StampedTransform transf_world_odom_;
    std::string odom_frame_;
    std::string world_frame_;
    std::string base_frame_;
    std::string dvl_frame_;

    // Callbacks
    void imuCB(const sensor_msgs::ImuPtr &imu_msg);
    void dvlCB(const geometry_msgs::TwistWithCovarianceStampedPtr &dvl_msg);
    void gtCB(const nav_msgs::OdometryPtr &pose_msg);

    // EKF methods
    void computeOdom(const geometry_msgs::TwistWithCovarianceStampedPtr &dvl_msg, const nav_msgs::OdometryPtr &gt_msg,
                const sensor_msgs::ImuPtr &imu_msg, double &t_prev, double& yaw_prev, boost::numeric::ublas::vector<double> &u_t);

    void prediction(boost::numeric::ublas::vector<double> &u_t);
    void update();

    // Aux methods
    bool sendOutput(ros::Time &t, geometry_msgs::Quaternion &odom_quat);
    double angleLimit (double angle) const;

};

#endif // SMALL_AUV_NAV_HPP
