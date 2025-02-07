/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#include "modules/planning/common/open_space_info.h"

namespace apollo {
namespace planning {

void CopyTrajectory(const DiscretizedTrajectory trajectory_src,
                    apollo::common::Trajectory* trajectory_tgt_ptr) {
  const size_t horizon = trajectory_src.NumOfPoints();
  for (size_t i = 0; i < horizon; ++i) {
    *trajectory_tgt_ptr->add_trajectory_point() =
        trajectory_src.TrajectoryPointAt(i);
  }
}

// record more trajectory information to info debug
void OpenSpaceInfo::RecordDebug(apollo::planning_internal::Debug* ptr_debug) {
  // 1, merge debug info into ptr_debug

  ptr_debug->Clear();

  ptr_debug->MergeFrom(debug_instance_);

  // 2, record partitioned trajectories into ptr_debug
  auto* ptr_partitioned_trajectories = ptr_debug->mutable_planning_data()
                                           ->mutable_open_space()
                                           ->mutable_partitioned_trajectories();

  for (auto& iter : paritioned_trajectories_) {
    const auto& picked_trajectory = iter.first;
    auto* ptr_added_trajectory = ptr_partitioned_trajectories->add_trajectory();
    CopyTrajectory(picked_trajectory, ptr_added_trajectory);
  }

  // 3, record chosed partitioned into ptr_debug
  auto* ptr_chosen_trajectory = ptr_debug->mutable_planning_data()
                                    ->mutable_open_space()
                                    ->mutable_chosen_trajectory()
                                    ->add_trajectory();
  const auto& chosen_trajectory = chosen_paritioned_trajectory_.first;
  CopyTrajectory(chosen_trajectory, ptr_chosen_trajectory);

  // 4, record if the trajectory is fallback trajecotry
  ptr_debug->mutable_planning_data()
      ->mutable_open_space()
      ->set_is_fallback_trajectory(fallback_flag_);

  // 5, record fallback trajectory if needed
  if (fallback_flag_) {
    auto* ptr_fallback_trajectory = ptr_debug->mutable_planning_data()
                                        ->mutable_open_space()
                                        ->mutable_fallback_trajectory()
                                        ->add_trajectory();
    const auto& fallback_trajectory = fallback_trajectory_.first;
    CopyTrajectory(fallback_trajectory, ptr_fallback_trajectory);
  }
}

}  // namespace planning
}  // namespace apollo
