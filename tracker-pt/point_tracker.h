/* Copyright (c) 2012 Patrick Ruoff
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#pragma once

#include "compat/timer.hpp"
#include "ftnoir_tracker_pt_settings.h"
#include "affine.hpp"
#include "numeric.hpp"

#include <opencv2/core.hpp>
#include <cstddef>
#include <memory>
#include <vector>
#include <array>
#include <QObject>

namespace impl {

// ----------------------------------------------------------------------------
// Describes a 3-point model
// nomenclature as in
// [Denis Oberkampf, Daniel F. DeMenthon, Larry S. Davis: "Iterative Pose Estimation Using Coplanar Feature Points"]

using namespace types;

struct PointModel final
{
    static constexpr unsigned N_POINTS = 3;

    vec3 M01;      // M01 in model frame
    vec3 M02;      // M02 in model frame

    vec3 u;        // unit vector perpendicular to M01,M02-plane

    mat22 P;

    f focal_length;

    enum Model { Clip, Cap, Custom };

    PointModel(settings_pt& s, f focal_length);
    void set_model(settings_pt& s);
    void get_d_order(const vec2* points, unsigned* d_order, const vec2& d) const;
};

// ----------------------------------------------------------------------------
// Tracks a 3-point model
// implementing the POSIT algorithm for coplanar points as presented in
// [Denis Oberkampf, Daniel F. DeMenthon, Larry S. Davis: "Iterative Pose Estimation Using Coplanar Feature Points"]
class PointTracker final
{
public:
    PointTracker();
    // track the pose using the set of normalized point coordinates (x pos in range -0.5:0.5)
    // f : (focal length)/(sensor width)
    // dt : time since last call
    void track(const std::vector<vec2>& projected_points, const PointModel& model, bool dynamic_pose, int init_phase_timeout, int w, int h);
    Affine pose() { return X_CM; }
    vec2 project(const vec3& v_M, f focal_length);
    vec2 project(const vec3& v_M, f focal_length, const Affine& X_CM);

private:
    // the points in model order
    using PointOrder = std::array<vec2, PointModel::N_POINTS>;

    PointOrder find_correspondences(const vec2* projected_points, const PointModel &model);
    PointOrder find_correspondences_previous(const vec2* points, const PointModel &model, int w, int h);
    int POSIT(const PointModel& point_model, const PointOrder& order);  // The POSIT algorithm, returns the number of iterations

    Affine X_CM; // trafo from model to camera

    Timer t;
    bool init_phase;
};

} // ns types

using impl::PointTracker;
using impl::PointModel;
