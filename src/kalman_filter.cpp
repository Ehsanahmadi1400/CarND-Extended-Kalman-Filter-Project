#define _USE_MATH_DEFINES

#include "kalman_filter.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using namespace std;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */
Eigen::VectorXd h_;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */

  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;

}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */

  VectorXd z_pred = H_ * x_;
	VectorXd y 		= z - z_pred;
	MatrixXd Ht 	= H_.transpose();
	MatrixXd S 		= H_ * P_ * Ht + R_;
	MatrixXd Si 	= S.inverse();
	MatrixXd PHt 	= P_ * Ht;
	MatrixXd K 		= PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - (K * H_)) * P_;

}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  h_ = VectorXd(3);
  h_ << 0, 0, 0;

  float px = x_[0];
  float py = x_[1];
  float vx = x_[2];
  float vy = x_[3];

  float c1 = (px*px)+(py*py);
  float c2 = sqrt(c1);
  float c3 = (c1*c2);

  h_ << c2, atan2(py, px), (px*vx + py*vy)/ c2;

  VectorXd z_pred 	= h_; 
	VectorXd y 		= z - z_pred;
  float phi = y[1];
    
    if(phi > 0){
        while( phi > M_PI){

            phi -= (2*M_PI);
            }
    }
    else if(phi < 0){
        while( phi < - M_PI){
            phi += (2*M_PI);
        }
    }

  y[1] = phi;

	MatrixXd Ht 	= H_.transpose();
	MatrixXd S 		= H_ * P_ * Ht + R_;
	MatrixXd Si 	= S.inverse();
	MatrixXd PHt 	= P_ * Ht;
	MatrixXd K 		= PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - (K * H_)) * P_;
}
