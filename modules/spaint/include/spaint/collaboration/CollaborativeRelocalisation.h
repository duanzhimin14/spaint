/**
 * spaint: CollaborativeRelocalisation.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2017. All rights reserved.
 */

#ifndef H_SPAINT_COLLABORATIVERELOCALISATION
#define H_SPAINT_COLLABORATIVERELOCALISATION

#include <boost/optional.hpp>

#ifdef WITH_OPENCV
#include <opencv2/core/core.hpp>
#endif

#include <itmx/base/ITMObjectPtrTypes.h>

#include <orx/base/ORImagePtrTypes.h>
#include <orx/relocalisation/Relocaliser.h>

namespace spaint {

/**
 * \brief An instance of this struct represents a relocalisation between the scenes of two agents during a collaborative reconstruction.
 *
 * These are used by the collaborative relocaliser, which repeatedly chooses a scene pair (i,j) and attempts to relocalise an individual
 * frame of scene j using the (local) relocaliser of scene i.
 */
struct CollaborativeRelocalisation
{
  //#################### PUBLIC VARIABLES ####################

  /** The score of this relocalisation as a candidate (used during relocalisation scheduling). */
  float m_candidateScore;

  /** The intrinsics of the depth camera used to capture scene i. */
  Vector4f m_depthIntrinsicsI;

  /** The index of the frame in scene j's trajectory that is being relocalised against scene i. */
  int m_frameIndexJ;

  /** The quality of the initial relocalisation result (before verification). */
  orx::Relocaliser::Quality m_initialRelocalisationQuality;

  /** The local pose of the frame being relocalised in scene j's coordinate system. */
  ORUtils::SE3Pose m_localPoseJ;

#ifdef WITH_OPENCV
  /** The (masked) mean difference between the synthetic depth images we render of scenes i and j during relocalisation. */
  cv::Scalar m_meanDepthDiff;
#endif

  /** The estimated relative transformation (if determined) from scene j's coordinate system to scene i's coordinate system. */
  boost::optional<ORUtils::SE3Pose> m_relativePose;

  /** The ID of scene i. */
  std::string m_sceneI;

  /** The ID of scene j. */
  std::string m_sceneJ;

  /** The fraction of pixels in the synthetic depth image we render of scene i (the target scene) that have valid depths. */
  float m_targetValidFraction;

  //#################### CONSTRUCTORS ####################

  CollaborativeRelocalisation(const std::string& sceneI, const Vector4f& depthIntrinsicsI, const std::string& sceneJ, int frameIndexJ, const ORUtils::SE3Pose& localPoseJ)
  : m_candidateScore(0.0f),
    m_depthIntrinsicsI(depthIntrinsicsI),
    m_frameIndexJ(frameIndexJ),
    m_initialRelocalisationQuality(orx::Relocaliser::RELOCALISATION_POOR),
    m_localPoseJ(localPoseJ),
    m_sceneI(sceneI),
    m_sceneJ(sceneJ),
    m_targetValidFraction(0.0f)
  {}
};

}

#endif
