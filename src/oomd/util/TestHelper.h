/*
 * Copyright (C) 2019-present, Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include "oomd/CgroupContext.h"
#include "oomd/OomdContext.h"

#define ASSERT_EXISTS(opt_expr) \
  ({                            \
    auto x = (opt_expr);        \
    ASSERT_TRUE(x.has_value()); \
    std::move(*x);              \
  })

namespace Oomd {

/*
 * Friend of data classes to access their private fields for test injection.
 * This class must only be included in tests and not the main binary.
 */
class TestHelper {
 public:
  using CgroupData = CgroupContext::CgroupData;
  using CgroupArchivedData = CgroupContext::CgroupArchivedData;

  static CgroupData& getDataRef(const CgroupContext& cgroup_ctx) {
    return *cgroup_ctx.data_;
  }

  static std::unordered_map<CgroupPath, CgroupContext>& getCgroupsRef(
      OomdContext& ctx) {
    return ctx.cgroups_;
  }

  /*
   * Set the cgroup data of a CgroupContext in OomdContext.
   * This is a shortcut for setting up CgroupContext without creating control
   * file fixtures. However, retrieving CgroupContext from OomdContext via
   * addToCacheAndGet still requires the requested CgroupPath exists, which
   * could be done using the Fixture utils.
   */
  static void setCgroupData(
      OomdContext& ctx,
      const CgroupPath& cgroup,
      const CgroupData& data,
      const std::optional<CgroupArchivedData>& archive = std::nullopt) {
    auto cgroup_ctx = CgroupContext::make(ctx, cgroup);
    if (cgroup_ctx.has_value()) {
      auto& cached_ctx =
          ctx.cgroups_.emplace(cgroup, std::move(*cgroup_ctx)).first->second;
      *cached_ctx.data_ = data;
      if (archive) {
        cached_ctx.archive_ = *archive;
      }
    }
  }
};

} // namespace Oomd
