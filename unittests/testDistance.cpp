/*
 * Copyright (c) 2016, Graphics Lab, Georgia Tech Research Corporation
 * Copyright (c) 2016, Humanoid Lab, Georgia Tech Research Corporation
 * Copyright (c) 2016, Personal Robotics Lab, Carnegie Mellon University
 * All rights reserved.
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#include <gtest/gtest.h>
#include <fcl/distance.h>
#include "dart/dart.hpp"
#include "TestHelpers.hpp"

using namespace dart;

//==============================================================================
TEST(Distance, Basic)
{
  auto fclCd = collision::FCLCollisionDetector::create();
  auto bulletCd = collision::BulletCollisionDetector::create();
  auto dartCd = collision::DARTCollisionDetector::create();

//  EXPECT_TRUE(fclCd->distance(nullptr));
//  EXPECT_FALSE(bulletCd->distance(nullptr));
//  EXPECT_FALSE(dartCd->distance(nullptr));
}

//==============================================================================
void testSphereSphere(const std::shared_ptr<CollisionDetector>& cd,
                      double tol = 1e-12)
{
  if (cd->getType() != collision::FCLCollisionDetector::getStaticType())
  {
    dtwarn << "Aborting test: distance check is not supported by "
           << cd->getType() << ".\n";
    return;
  }

  auto simpleFrame1 = Eigen::make_aligned_shared<SimpleFrame>(Frame::World());
  auto simpleFrame2 = Eigen::make_aligned_shared<SimpleFrame>(Frame::World());

  ShapePtr shape1(new EllipsoidShape(Eigen::Vector3d(1.0, 1.0, 1.0)));
  ShapePtr shape2(new EllipsoidShape(Eigen::Vector3d(0.5, 0.5, 0.5)));
  simpleFrame1->setShape(shape1);
  simpleFrame2->setShape(shape2);

  auto group1 = cd->createCollisionGroup(simpleFrame1.get());
  auto group2 = cd->createCollisionGroup(simpleFrame2.get());
  auto group12 = cd->createCollisionGroup(group1.get(), group2.get());

  EXPECT_EQ(group1->getNumShapeFrames(), 1u);
  EXPECT_EQ(group2->getNumShapeFrames(), 1u);
  EXPECT_EQ(group12->getNumShapeFrames(), 2u);

  collision::DistanceOption option;
  collision::DistanceResult result;

  result.clear();
  simpleFrame1->setTranslation(Eigen::Vector3d(0.0, 0.0, 0.0));
  simpleFrame2->setTranslation(Eigen::Vector3d(0.75, 0.0, 0.0));
  EXPECT_FALSE(group12->distance(option, &result));
  EXPECT_DOUBLE_EQ(result.mMinimumDistance, 0.0);

  result.clear();
  simpleFrame1->setTranslation(Eigen::Vector3d(0.0, 0.0, 0.0));
  simpleFrame2->setTranslation(Eigen::Vector3d(1.0, 0.0, 0.0));
  EXPECT_FALSE(group12->distance(option, &result));
  EXPECT_DOUBLE_EQ(result.mMinimumDistance, 0.25);
}

//==============================================================================
TEST(Distance, SphereSphere)
{
  auto fcl_mesh_dart = FCLCollisionDetector::create();
  testSphereSphere(fcl_mesh_dart);

#if HAVE_BULLET_COLLISION
  auto bullet = BulletCollisionDetector::create();
  testSphereSphere(bullet);
#endif

  auto dart = DARTCollisionDetector::create();
  testSphereSphere(dart);
}

//==============================================================================
void testBoxBox(const std::shared_ptr<CollisionDetector>& cd,
                double tol = 1e-12)
{
  if (cd->getType() != collision::FCLCollisionDetector::getStaticType())
  {
    dtwarn << "Aborting test: distance check is not supported by "
           << cd->getType() << ".\n";
    return;
  }

  auto simpleFrame1 = Eigen::make_aligned_shared<SimpleFrame>(Frame::World());
  auto simpleFrame2 = Eigen::make_aligned_shared<SimpleFrame>(Frame::World());

  ShapePtr shape1(new BoxShape(Eigen::Vector3d(1.0, 1.0, 1.0)));
  ShapePtr shape2(new BoxShape(Eigen::Vector3d(0.5, 0.5, 0.5)));
  simpleFrame1->setShape(shape1);
  simpleFrame2->setShape(shape2);

  Eigen::Vector3d pos1 = Eigen::Vector3d(0.0, 0.0, 0.0);
  Eigen::Vector3d pos2 = Eigen::Vector3d(0.0, 0.0, 0.0);
  simpleFrame1->setTranslation(pos1);
  simpleFrame2->setTranslation(pos2);

  auto group1 = cd->createCollisionGroup(simpleFrame1.get());
  auto group2 = cd->createCollisionGroup(simpleFrame2.get());

  EXPECT_EQ(group1->getNumShapeFrames(), 1u);
  EXPECT_EQ(group2->getNumShapeFrames(), 1u);

  collision::DistanceOption option;
  collision::DistanceResult result;

//  EXPECT_TRUE(group1->distance(group2.get(), option, &result));

  Eigen::Vector3d min = Eigen::Vector3d(-0.25, 0.25, 0.0);
  Eigen::Vector3d max = Eigen::Vector3d(0.25, 0.5, 0.0);


}

//==============================================================================
TEST(Distance, BoxBox)
{
  auto fcl_mesh_dart = FCLCollisionDetector::create();
  testBoxBox(fcl_mesh_dart);

#if HAVE_BULLET_COLLISION
  auto bullet = BulletCollisionDetector::create();
  testBoxBox(bullet);
#endif

  auto dart = DARTCollisionDetector::create();
  testBoxBox(dart);
}

//==============================================================================
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
