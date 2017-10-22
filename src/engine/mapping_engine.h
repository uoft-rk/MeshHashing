//
// Created by wei on 17-4-5.
//
// MappingEngine: managing HashTable<Block> and might be other structs later

#ifndef VH_MAP_H
#define VH_MAP_H

#include "core/hash_table.h"
#include "core/block_array.h"
#include "core/entry_array.h"
#include "core/mesh.h"

#include "visualization/compact_mesh.h"
#include "visualization/bounding_box.h"
#include "sensor/rgbd_sensor.h"

class MappingEngine {
private:
  HashTable        hash_table_;
  BlockArray       blocks_;
  Mesh             mesh_;

  CoordinateConverter coordinate_converter_;
  EntryArray       candidate_entries_;
  CompactMesh      compact_mesh_;

  uint             integrated_frame_count_;
  bool             use_fine_gradient_;

  BBox             bbox_;

  /// Focus on Pass1, Pass2, and lock free
  std::fstream     time_profile_;
  std::fstream     memo_profile_;

////////////////////
/// Core
////////////////////

public:

  /// Life cycle
  MappingEngine(const HashParams& hash_params,
      const MeshParams& mesh_params,
      const SDFParams&  sdf_params);
  ~MappingEngine();

  /// Reset and recycle
  void Reset();
  void Recycle(int frame_count);

////////////////////
/// Fusion
////////////////////
public:
  void Integrate(Sensor &sensor);

////////////////////
/// Meshing
////////////////////
public:
  void CompressMesh(int3& stats);
  void MarchingCubes();
  void PlaneFitting(float3 camera_pos);

  void SaveMesh(std::string path);
  void SavePly(std::string path);


////////////////////
/// Access functions
////////////////////
  /// Only classes with Kernel function should call it
  /// The other part of the hash_table should be hidden
  const uint& frame_count() {
    return integrated_frame_count_;
  }
  bool& use_fine_gradient() {
    return use_fine_gradient_;
  }

  /// Access for RayCaster
  HashTable& hash_table() {
    return hash_table_;
  }
  EntryArray& candidate_entries() {
    return candidate_entries_;
  }
  BlockArray& blocks() {
    return blocks_;
  }
  Mesh& mesh() {
    return mesh_;
  }
  CompactMesh& compact_mesh() {
    return compact_mesh_;
  }

  BBox& bbox() {
    return bbox_;
  }
  CoordinateConverter& converter() {
    return coordinate_converter_;
  }

public:
  void GetBoundingBoxes();
};


#endif //VH_MAP_H
