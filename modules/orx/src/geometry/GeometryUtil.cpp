/**
 * orx: GeometryUtil.cpp
 * Copyright (c) Torr Vision Group, University of Oxford, 2016. All rights reserved.
 */

#include "geometry/GeometryUtil.h"

#include <tvgutil/containers/MapUtil.h>
using namespace tvgutil;

#include "geometry/DualQuaternion.h"

namespace orx {

//#################### PUBLIC STATIC MEMBER FUNCTIONS ####################

ORUtils::SE3Pose GeometryUtil::blend_poses(const std::vector<ORUtils::SE3Pose>& poses)
{
  std::vector<DualQuatd> dqs;
  std::vector<double> weights;
  const int count = static_cast<int>(poses.size());

  // Compute a uniformly-weighted linear blend of all of the poses and return it.
  const double weight = 1.0 / count;
  for(int i = 0; i < count; ++i)
  {
    dqs.push_back(pose_to_dual_quat<double>(poses[i]));
    weights.push_back(weight);
  }

  return dual_quat_to_pose(DualQuatd::linear_blend(&dqs[0], &weights[0], count));
}

Eigen::Matrix4f GeometryUtil::estimate_rigid_transform(const Eigen::Matrix3f& P, const Eigen::Matrix3f& Q)
{
  // Estimate the rotation matrix and translation vector.
  Eigen::Matrix3f R;
  Eigen::Vector3f t;
  estimate_rigid_transform(P, Q, R, t);

  // Combine them to form the final transformation matrix.
  Eigen::Matrix4f M = Eigen::Matrix4f::Identity();
  M.block<3,3>(0, 0) = R;
  M.block<3,1>(0, 3) = t;

  return M;
}

void GeometryUtil::estimate_rigid_transform(const Eigen::Matrix3f& P, const Eigen::Matrix3f& Q, Eigen::Matrix3f& R, Eigen::Vector3f& t)
{
  /*
   * Step 1: Compute the centroids of the two sets of points.
   *
   * centroid = (x1 x2 x3) * (1/3) = ((x1 + x2 + x3) / 3) = (cx)
   *            (y1 y2 y3)   (1/3)   ((y1 + y2 + y3) / 3)   (cy)
   *            (z1 z2 z3)   (1/3)   ((z1 + z2 + z3) / 3)   (cz)
   */
  const Eigen::Vector3f ones = Eigen::Vector3f::Ones();
  const Eigen::Vector3f thirds = ones / 3;
  const Eigen::Vector3f centroidP = P * thirds;
  const Eigen::Vector3f centroidQ = Q * thirds;

  /*
   * Step 2: Translate the points in each set so that their centroid coincides with the origin of the coordinate system.
   *         To do this, we subtract the centroid from each point.
   *
   * centred = (x1 x2 x3) - (cx) * (1 1 1) = (x1 x2 x3) - (cx cx cx) = (x1-cx x2-cx x3-cx)
   *           (y1 y2 y3)   (cy)             (y1 y2 y3)   (cy cy cy)   (y1-cy y2-cy y3-cy)
   *           (z1 z2 z3)   (cz)             (z1 z2 z3)   (cz cz cz)   (z1-cz z2-cz z3-cz)
   */
  const Eigen::RowVector3f onesT = ones.transpose();
  const Eigen::Matrix3f centredP = P - centroidP * onesT;
  const Eigen::Matrix3f centredQ = Q - centroidQ * onesT;

  // Step 3: Compute the cross-covariance between the two matrices of centred points.
  const Eigen::Matrix3f A = centredP * centredQ.transpose();

  // Step 4: Calculate the SVD of the cross-covariance matrix: A = V * S * W^T.
  Eigen::JacobiSVD<Eigen::Matrix3f> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);

  // Step 5: Decide whether or not we need to correct our rotation matrix, and set the I matrix accordingly.
  const Eigen::Matrix3f V = svd.matrixU();
  const Eigen::Matrix3f W = svd.matrixV();
  Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
  if((V * W.transpose()).determinant() < 0)
  {
    I(2,2) = -1;
  }

  // Step 6: Recover the rotation and translation estimates.
  R = W * I * V.transpose();
  t = centroidQ - R * centroidP;
}

ORUtils::SE3Pose GeometryUtil::find_best_hypothesis(const std::vector<ORUtils::SE3Pose>& poseHypotheses,
                                                    std::vector<ORUtils::SE3Pose>& inliersForBestHypothesis,
                                                    double rotThreshold, float transThreshold)
{
  std::map<std::string,ORUtils::SE3Pose> poseHypothesesMap;
  for(size_t i = 0, size = poseHypotheses.size(); i < size; ++i)
  {
    poseHypothesesMap.insert(std::make_pair(boost::lexical_cast<std::string>(i), poseHypotheses[i]));
  }

  std::string bestHypothesis = find_best_hypothesis(poseHypothesesMap, inliersForBestHypothesis, rotThreshold, transThreshold);
  return MapUtil::lookup(poseHypothesesMap, bestHypothesis);
}

std::string GeometryUtil::find_best_hypothesis(const std::map<std::string,ORUtils::SE3Pose>& poseHypotheses,
                                               std::vector<ORUtils::SE3Pose>& inliersForBestHypothesis,
                                               double rotThreshold, float transThreshold)
{
  std::string bestHypothesis;

  // For each pose hypothesis:
  for(std::map<std::string,ORUtils::SE3Pose>::const_iterator it = poseHypotheses.begin(), iend = poseHypotheses.end(); it != iend; ++it)
  {
    // Calculate the inliers for the hypothesis.
    std::vector<ORUtils::SE3Pose> inliers;
    for(std::map<std::string,ORUtils::SE3Pose>::const_iterator jt = poseHypotheses.begin(), jend = poseHypotheses.end(); jt != jend; ++jt)
    {
      if(poses_are_similar(it->second, jt->second, rotThreshold, transThreshold))
      {
        inliers.push_back(jt->second);
      }
    }

    // Update the current best hypothesis as necessary.
    if(inliers.size() > inliersForBestHypothesis.size())
    {
      bestHypothesis = it->first;
      inliersForBestHypothesis = inliers;
    }
  }

  return bestHypothesis;
}

bool GeometryUtil::poses_are_similar(const ORUtils::SE3Pose& pose1, const ORUtils::SE3Pose& pose2, double rotThreshold, float transThreshold)
{
  Vector3f r1, t1, r2, t2;
  pose1.GetParams(t1, r1);
  pose2.GetParams(t2, r2);

  double rot = DualQuatd::angle_between_rotations(DualQuatd::from_rotation(r1), DualQuatd::from_rotation(r2));
  float trans = length(t1 - t2);

  return rot <= rotThreshold && trans <= transThreshold;
}

std::string GeometryUtil::to_matlab(const Matrix4f& M)
{
  std::ostringstream oss;
  oss << '['
      << M.m[0] << ' ' << M.m[4] << ' ' << M.m[8] << ' ' << M.m[12] << "; "
      << M.m[1] << ' ' << M.m[5] << ' ' << M.m[9] << ' ' << M.m[13] << "; "
      << M.m[2] << ' ' << M.m[6] << ' ' << M.m[10] << ' ' << M.m[14] << "; "
      << M.m[3] << ' ' << M.m[7] << ' ' << M.m[11] << ' ' << M.m[15]
      << ']';
  return oss.str();
}

}
