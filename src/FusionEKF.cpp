#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;

/**
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_      = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
              0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0,      0,
              0,    0.0009, 0,
              0,    0,      0.09;

  /**
   * TODO: Finish initializing the FusionEKF.
   * TODO: Set the process and measurement noises
   */
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;

  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1, 0, 1, 0,
			       0, 1, 0, 1,
			       0, 0, 1, 0,
			       0, 0, 0, 1;

}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /**
   * Initialization
   */
  if (!is_initialized_) {
    /**
     * TODO: Initialize the state ekf_.x_ with the first measurement.
     * TODO: Create the covariance matrix.
     * You'll need to convert radar from polar to cartesian coordinates.
     */

    // first measurement
    cout << "EKF: " << endl;
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;
    previous_timestamp_ = measurement_pack.timestamp_;

    ekf_.P_ = MatrixXd(4, 4);
    ekf_.P_ << 1, 0, 0,    0,
			         0, 1, 0,    0,
			         0, 0, 1000, 0,
			         0, 0, 0,    1000;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      // TODO: Convert radar from polar to cartesian coordinates 
      //         and initialize state.
      float a = measurement_pack.raw_measurements_[0]*cos(measurement_pack.raw_measurements_[1]);
      float b = measurement_pack.raw_measurements_[0]*sin(measurement_pack.raw_measurements_[1]);
      ekf_.x_ << a, b , 0, 0;
      cout << "init with RADAR is done " << endl;

    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      // TODO: Initialize state.
      ekf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 0, 0;
      cout << "init with LASER is done " << endl;
    }
    

    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  Tools t;
  /**
   * Prediction
   */

  /**
   * TODO: Update the state transition matrix F according to the new elapsed time.
   * Time is measured in seconds.
   * TODO: Update the process noise covariance matrix.
   * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  float dt = (measurement_pack.timestamp_ - previous_timestamp_)/ 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;

  ekf_.F_(0, 2) = dt;
  ekf_.F_(1, 3) = dt;

  float noise_ax = 9;
  float noise_ay = 9;

	ekf_.Q_ = MatrixXd(4, 4);
	ekf_.Q_ <<  (0.25)*pow(dt, 4)*noise_ax, 0, (0.5)*pow(dt, 3)*noise_ax, 0,
			        0, (0.25)*pow(dt, 4)*noise_ay,  0, (0.5)*pow(dt,  3)*noise_ay,
			        (0.5)*pow(dt, 3)*noise_ax, 0, pow(dt, 2)*noise_ay, 0,
			        0, (0.5)*pow(dt, 3)*noise_ay, 0, pow(dt, 2)*noise_ay; 

  ekf_.Predict();

  /**
   * Update
   */

  /**
   * TODO:
   * - Use the sensor type to perform the update step.
   * - Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // TODO: Radar updates

    Hj_ << tools.CalculateJacobian(ekf_.x_);

    ekf_.Init(ekf_.x_, ekf_.P_, ekf_.F_, Hj_, R_radar_, ekf_.Q_);
		ekf_.UpdateEKF(measurement_pack.raw_measurements_);

  cout << "check lidar" << endl;

  } else {
    // TODO: Laser updates

    ekf_.Init(ekf_.x_, ekf_.P_, ekf_.F_, H_laser_, R_laser_, ekf_.Q_);
		ekf_.Update(measurement_pack.raw_measurements_);

  }

  // print the output
  // cout << "x_ = " << ekf_.x_ << endl;
  // cout << "P_ = " << ekf_.P_ << endl;
}
