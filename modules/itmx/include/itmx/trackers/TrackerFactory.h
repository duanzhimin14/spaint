/**
 * itmx: TrackerFactory.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2017. All rights reserved.
 */

#ifndef H_ITMX_TRACKERFACTORY
#define H_ITMX_TRACKERFACTORY

#include <boost/property_tree/ptree.hpp>

#include "FallibleTracker.h"
#include "../base/ITMObjectPtrTypes.h"
#include "../remotemapping/MappingServer.h"

#ifdef WITH_VICON
#include "../util/ViconInterface.h"
#endif

namespace itmx {

/**
 * \brief This class can be used to construct trackers.
 */
class TrackerFactory
{
  //#################### ENUMERATIONS ####################
private:
  /**
   * \brief The values of this enumeration indicate whether or not the tracker to be constructed will ultimately be nested within a composite.
   */
  enum NestingFlag
  {
    NESTED,
    UNNESTED
  };

  //#################### TYPEDEFS ####################
private:
  typedef boost::property_tree::ptree Tree;

  //#################### PRIVATE VARIABLES ####################
private:
#ifdef WITH_VICON
  /** The interface to the Vicon system (if we're using it). */
  ViconInterface_CPtr m_vicon;
#endif

  //#################### PUBLIC MEMBER FUNCTIONS ####################
public:
  /**
   * \brief Makes a tracker based on the configuration specified in an XML file on disk.
   *
   * \param trackerConfigFilename The name of the XML file containing the tracker configuration.
   * \param sceneID               The ID of the scene for which the tracker will be used.
   * \param trackSurfels          Whether or not we're tracking against the surfel scene, rather than the voxel one.
   * \param rgbImageSize          The size of the colour input images.
   * \param depthImageSize        The size of the depth input images.
   * \param lowLevelEngine        The engine used to perform low-level image processing operations.
   * \param imuCalibrator         The IMU calibrator.
   * \param settings              The InfiniTAM settings.
   * \param fallibleTracker       A location into which to store a typed pointer to the (unique) nested tracker (if any) that can detect tracking failures.
   * \param mappingServer         The remote mapping server (if any).
   * \param nestingFlag           A flag indicating whether or not the tracker will ultimately be nested within a composite.
   * \return                      The tracker.
   */
  Tracker_Ptr make_tracker_from_file(const std::string& trackerConfigFilename, const std::string& sceneID, bool trackSurfels,
                                     const Vector2i& rgbImageSize, const Vector2i& depthImageSize,
                                     const LowLevelEngine_CPtr& lowLevelEngine, const IMUCalibrator_Ptr& imuCalibrator,
                                     const Settings_CPtr& settings, FallibleTracker*& fallibleTracker,
                                     const MappingServer_Ptr& mappingServer = MappingServer_Ptr(),
                                     NestingFlag nestingFlag = UNNESTED) const;

  /**
   * \brief Makes a tracker based on the configuration specified in an XML string.
   *
   * \param trackerConfig   The XML string containing the tracker configuration.
   * \param sceneID         The ID of the scene for which the tracker will be used.
   * \param trackSurfels    Whether or not we're tracking against the surfel scene, rather than the voxel one.
   * \param rgbImageSize    The size of the colour input images.
   * \param depthImageSize  The size of the depth input images.
   * \param lowLevelEngine  The engine used to perform low-level image processing operations.
   * \param imuCalibrator   The IMU calibrator.
   * \param settings        The InfiniTAM settings.
   * \param fallibleTracker A location into which to store a typed pointer to the (unique) nested tracker (if any) that can detect tracking failures.
   * \param mappingServer   The remote mapping server (if any).
   * \param nestingFlag     A flag indicating whether or not the tracker will ultimately be nested within a composite.
   * \return                The tracker.
   */
  Tracker_Ptr make_tracker_from_string(const std::string& trackerConfig, const std::string& sceneID, bool trackSurfels,
                                       const Vector2i& rgbImageSize, const Vector2i& depthImageSize,
                                       const LowLevelEngine_CPtr& lowLevelEngine, const IMUCalibrator_Ptr& imuCalibrator,
                                       const Settings_CPtr& settings, FallibleTracker*& fallibleTracker,
                                       const MappingServer_Ptr& mappingServer = MappingServer_Ptr(),
                                       NestingFlag nestingFlag = UNNESTED) const;

#ifdef WITH_VICON
  /**
   * \brief Sets the interface to the Vicon system (if we're using it).
   *
   * \param vicon The interface to the Vicon system (if we're using it).
   */
  void set_vicon(const ViconInterface_CPtr& vicon);
#endif

  //#################### PRIVATE MEMBER FUNCTIONS ####################
private:
  /**
   * \brief Makes a "simple" tracker (i.e. a tracker that is not a composite, or one that is imported from a file)
   *        based on a specified tracker type and parameter string.
   *
   * \param trackerType     The type of tracker to construct.
   * \param sceneID         The ID of the scene for which the tracker will be used.
   * \param trackerParams   A string specifying the parameters for the tracker (if any).
   * \param trackSurfels    Whether or not we're tracking against the surfel scene, rather than the voxel one.
   * \param rgbImageSize    The size of the colour input images.
   * \param depthImageSize  The size of the depth input images.
   * \param lowLevelEngine  The engine used to perform low-level image processing operations.
   * \param imuCalibrator   The IMU calibrator.
   * \param settings        The InfiniTAM settings.
   * \param fallibleTracker A location into which to store a typed pointer to the (unique) nested tracker (if any) that can detect tracking failures.
   * \param mappingServer   The remote mapping server (if any).
   * \param nestingFlag     A flag indicating whether or not the tracker will ultimately be nested within a composite.
   * \return                The tracker.
   */
  Tracker_Ptr make_simple_tracker(const std::string& trackerType, const std::string& sceneID, std::string trackerParams,
                                  bool trackSurfels, const Vector2i& rgbImageSize, const Vector2i& depthImageSize,
                                  const LowLevelEngine_CPtr& lowLevelEngine, const IMUCalibrator_Ptr& imuCalibrator,
                                  const Settings_CPtr& settings, FallibleTracker*& fallibleTracker,
                                  const MappingServer_Ptr& mappingServer, NestingFlag nestingFlag) const;

  /**
   * \brief Makes a tracker based on the configuration specified in a property tree.
   *
   * \param trackerTree     The property tree containing the tracker configuration.
   * \param sceneID         The ID of the scene for which the tracker will be used.
   * \param trackSurfels    Whether or not we're tracking against the surfel scene, rather than the voxel one.
   * \param rgbImageSize    The size of the colour input images.
   * \param depthImageSize  The size of the depth input images.
   * \param lowLevelEngine  The engine used to perform low-level image processing operations.
   * \param imuCalibrator   The IMU calibrator.
   * \param settings        The InfiniTAM settings.
   * \param fallibleTracker A location into which to store a typed pointer to the (unique) nested tracker (if any) that can detect tracking failures.
   * \param mappingServer   The remote mapping server (if any).
   * \param nestingFlag     A flag indicating whether or not the tracker will ultimately be nested within a composite.
   * \return                The tracker.
   */
  Tracker_Ptr make_tracker(const Tree& trackerTree, const std::string& sceneID, bool trackSurfels,
                           const Vector2i& rgbImageSize, const Vector2i& depthImageSize,
                           const LowLevelEngine_CPtr& lowLevelEngine, const IMUCalibrator_Ptr& imuCalibrator,
                           const Settings_CPtr& settings, FallibleTracker*& fallibleTracker,
                           const MappingServer_Ptr& mappingServer = MappingServer_Ptr(),
                           NestingFlag nestingFlag = UNNESTED) const;
};

}

#endif
