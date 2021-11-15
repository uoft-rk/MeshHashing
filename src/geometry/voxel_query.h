//
// Created by wei on 17-10-25.
//

#ifndef GEOMETRY_VOXEL_QUERY_H
#define GEOMETRY_VOXEL_QUERY_H

#include <matrix.h>
#include "geometry_helper.h"

#include "core/hash_table.h"
#include "core/block_array.h"


// TODO(wei): refine it
// function:
// at @param world_pos
// get Voxel in @param blocks
// with the help of @param hash_table and geometry_helper
__device__

// function:
// block-pos @param curr_entry -> voxel-pos @param voxel_local_pos
// get Voxel in @param blocks
// with the help of @param hash_table and geometry_helper
__device__
inline Voxel &GetVoxelRef(
    const HashEntry &curr_entry,
    const int3 voxel_pos,
    BlockArray &blocks,
    const HashTable &hash_table,
    GeometryHelper &geometry_helper
) {
  int3 block_pos = geometry_helper.VoxelToBlock(voxel_pos);
  uint3 offset = geometry_helper.VoxelToOffset(block_pos, voxel_pos);

  if (curr_entry.pos == block_pos) {
    uint i = geometry_helper.VectorizeOffset(offset);
    return blocks[curr_entry.ptr].voxels[i];
  } else {
    int entry_ptr = 0;
#ifdef __CUDA_ARCH__
    asm volatile("mad24.lo.s32 %0, %1, %2, %3;"
                : "=r"(entry_ptr)
                : "r"(block_pos.x),
                  "r"(block_pos.y),
                  "r"(block_pos.z));

    bool cache_miss;
    if (cache_miss = (entry_ptr == 0))
#endif
      entry_ptr = hash_table.GetEntry(block_pos).ptr;
#ifdef __CUDA_ARCH__
    if (cache_miss)
      asm volatile("bfi.b32 %0, %1, %2, %3, %4;"
                  : "=r"(entry_ptr)
                  : "r"(block_pos.x),
                    "r"(block_pos.y),
                    "r"(block_pos.z),
                    "r"(entry_ptr));
#endif
    if (entry_ptr == FREE_ENTRY) {
      printf("GetVoxelRef: should never reach here!\n");
    }
    uint i = geometry_helper.VectorizeOffset(offset);
    return blocks[entry_ptr].voxels[i];
  }
}

__device__
inline MeshUnit &GetMeshUnitRef(
    const HashEntry &curr_entry,
    const int3 voxel_pos,
    BlockArray &blocks,
    const HashTable &hash_table,
    GeometryHelper &geometry_helper
) {
  int3 block_pos = geometry_helper.VoxelToBlock(voxel_pos);
  uint3 offset = geometry_helper.VoxelToOffset(block_pos, voxel_pos);

  if (curr_entry.pos == block_pos) {
    uint i = geometry_helper.VectorizeOffset(offset);
    return blocks[curr_entry.ptr].mesh_units[i];
  } else {
    int entry_ptr = 0;
#ifdef __CUDA_ARCH__
    asm volatile("mad24.lo.s32 %0, %1, %2, %3;"
                : "=r"(entry_ptr)
                : "r"(block_pos.x),
                  "r"(block_pos.y),
                  "r"(block_pos.z));

    bool cache_miss;
    if (cache_miss = (entry_ptr == 0))
#endif
      entry_ptr = hash_table.GetEntry(block_pos).ptr;
#ifdef __CUDA_ARCH__
    if (cache_miss)
      asm volatile("bfi.b32 %0, %1, %2, %3, %4;"
                  : "=r"(entry_ptr)
                  : "r"(block_pos.x),
                    "r"(block_pos.y),
                    "r"(block_pos.z),
                    "r"(entry_ptr));
#endif
    if (entry_ptr == FREE_ENTRY) {
      printf("GetVoxelRef: should never reach here!\n");
    }
    uint i = geometry_helper.VectorizeOffset(offset);
    return blocks[entry_ptr].mesh_units[i];
  }
}

// function:
// block-pos @param curr_entry -> voxel-pos @param voxel_local_pos
// get SDF in @param blocks
// with the help of @param hash_table and geometry_helper
__device__
inline bool GetVoxelValue(
    const HashEntry &curr_entry,
    const int3 voxel_pos,
    const BlockArray &blocks,
    const HashTable &hash_table,
    GeometryHelper &geometry_helper,
    Voxel* voxel) {
  int3 block_pos = geometry_helper.VoxelToBlock(voxel_pos);
  uint3 offset = geometry_helper.VoxelToOffset(block_pos, voxel_pos);

  if (curr_entry.pos == block_pos) {
    uint i = geometry_helper.VectorizeOffset(offset);
    *voxel = blocks[curr_entry.ptr].voxels[i];
  } else {
  int entry_ptr = 0;
#ifdef __CUDA_ARCH__
  asm volatile("mad24.lo.s32 %0, %1, %2, %3;"
               : "=r"(entry_ptr)
               : "r"(block_pos.x),
                 "r"(block_pos.y),
                 "r"(block_pos.z));

  bool cache_miss;
  if (cache_miss = (entry_ptr == 0))
#endif
    entry_ptr = hash_table.GetEntry(block_pos).ptr;
    
    if (entry_ptr == FREE_ENTRY) return false;
    uint i = geometry_helper.VectorizeOffset(offset);
    *voxel = blocks[entry_ptr].voxels[i];
#ifdef __CUDA_ARCH__
  if (cache_miss)
    asm volatile("bfi.b32 %0, %1, %2, %3, %4;"
                 : "=r"(entry_ptr)
                 : "r"(block_pos.x),
                   "r"(block_pos.y),
                   "r"(block_pos.z),
                   "r"(entry_ptr));
#endif
  }
  return true;
}

__device__
inline bool GetVoxelValue(
    const float3 world_pos,
    const BlockArray &blocks,
    const HashTable &hash_table,
    GeometryHelper &geometry_helper,
    Voxel* voxel
) {
  int3 voxel_pos = geometry_helper.WorldToVoxeli(world_pos);
  int3 block_pos = geometry_helper.VoxelToBlock(voxel_pos);
  uint3 offset = geometry_helper.VoxelToOffset(block_pos, voxel_pos);

  int entry_ptr = 0;
#ifdef __CUDA_ARCH__
  asm volatile("mad24.lo.s32 %0, %1, %2, %3;"
               : "=r"(entry_ptr)
               : "r"(block_pos.x),
                 "r"(block_pos.y),
                 "r"(block_pos.z));

  bool cache_miss;
  if (cache_miss = (entry_ptr == 0))
#endif
    entry_ptr = hash_table.GetEntry(block_pos).ptr;

  bool ret_val;
  if (entry_ptr == FREE_ENTRY) {
    voxel->sdf = 0;
    voxel->inv_sigma2 = 0;
    voxel->color = make_uchar3(0,0,0);
    ret_val = false;
  } else {
    uint i = geometry_helper.VectorizeOffset(offset);
    const Voxel& v = blocks[entry_ptr].voxels[i];
    voxel->sdf = v.sdf;
    voxel->inv_sigma2 = v.inv_sigma2;
    voxel->color = v.color;
    voxel->a = v.a;
    voxel->b = v.b;
    ret_val = true;
  }

#ifdef __CUDA_ARCH__
  if (cache_miss)
    asm volatile("bfi.b32 %0, %1, %2, %3, %4;"
                 : "=r"(entry_ptr)
                 : "r"(block_pos.x),
                   "r"(block_pos.y),
                   "r"(block_pos.z),
                   "r"(entry_ptr));
#endif

  return ret_val;
}

#endif //MESH_HASHING_SPATIAL_QUERY_H
