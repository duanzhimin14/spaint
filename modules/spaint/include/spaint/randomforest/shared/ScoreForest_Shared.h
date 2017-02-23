/**
 * spaint: ScoreForest_Shared.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2016. All rights reserved.
 */

#ifndef H_SPAINT_SCOREFORESTSHARED
#define H_SPAINT_SCOREFORESTSHARED

namespace spaint
{
_CPU_AND_GPU_CODE_
inline void evaluate_forest_shared(const ScoreForest::NodeEntry* forestTexture,
    const RGBDPatchDescriptor* descriptorsData, const Vector2i &imgSize,
    LeafIndices* leafData, int x, int y)
{
  const int linear_feature_idx = y * imgSize.width + x;
  const RGBDPatchDescriptor &currentFeature =
      descriptorsData[linear_feature_idx];

  for (int treeIdx = 0; treeIdx < SCOREFOREST_NTREES; ++treeIdx)
  {
    int currentNodeIdx = 0;
    ScoreForest::NodeEntry node = forestTexture[currentNodeIdx
        * SCOREFOREST_NTREES + treeIdx];
    bool isLeaf = node.leafIdx >= 0;

    while (!isLeaf)
    {
      // Evaluate feature
      currentNodeIdx = node.leftChildIdx
          + (currentFeature.data[node.featureIdx] > node.featureThreshold);

      // Update node
      node = forestTexture[currentNodeIdx * SCOREFOREST_NTREES + treeIdx];
      isLeaf = node.leafIdx >= 0; // a bit redundant but clearer
    }

    leafData[linear_feature_idx][treeIdx] = node.leafIdx;
  }
}

_CPU_AND_GPU_CODE_
inline void get_prediction_for_leaf_shared(
    const Prediction3DColour* leafPredictions,
    const ScoreForest::LeafIndices* leafIndices,
    Prediction3DColour* outPredictions, Vector2i imgSize, int x, int y)
{
  const int linearIdx = y * imgSize.width + x;
  const ScoreForest::LeafIndices selectedLeaves = leafIndices[linearIdx];

  // Setup the indices of the selected mode for each prediction
  int treeModeIdx[SCOREFOREST_NTREES];
  for (int treeIdx = 0; treeIdx < SCOREFOREST_NTREES; ++treeIdx)
  {
    treeModeIdx[treeIdx] = 0;
  }

  // TODO: maybe copying them is faster...
  const Prediction3DColour *selectedPredictions[SCOREFOREST_NTREES];
  for (int treeIdx = 0; treeIdx < SCOREFOREST_NTREES; ++treeIdx)
  {
    selectedPredictions[treeIdx] = &leafPredictions[selectedLeaves[treeIdx]];
  }

  Prediction3DColour &outPrediction = outPredictions[linearIdx];
  outPrediction.nbModes = 0;

  // Merge first MAX_MODES from the sorted mode arrays
  while (outPrediction.nbModes < Prediction3DColour::MAX_MODES)
  {
    int bestTreeIdx = 0;
    int bestTreeNbInliers = 0;

    // Find the tree with most inliers
    for (int treeIdx = 0; treeIdx < SCOREFOREST_NTREES; ++treeIdx)
    {
      if (selectedPredictions[treeIdx]->nbModes > treeModeIdx[treeIdx]
          && selectedPredictions[treeIdx]->modes[treeModeIdx[treeIdx]].nbInliers
              > bestTreeNbInliers)
      {
        bestTreeIdx = treeIdx;
        bestTreeNbInliers =
            selectedPredictions[treeIdx]->modes[treeModeIdx[treeIdx]].nbInliers;
      }
    }

    if (bestTreeNbInliers == 0)
    {
      // No more modes
      break;
    }

    // Copy its mode into the output array, increment its index
    outPrediction.modes[outPrediction.nbModes] =
        selectedPredictions[bestTreeIdx]->modes[treeModeIdx[bestTreeIdx]];
    outPrediction.nbModes++;
    treeModeIdx[bestTreeIdx]++;
  }
}
}

#endif