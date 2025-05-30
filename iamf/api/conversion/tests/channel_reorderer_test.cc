/*
 * Copyright (c) 2025, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License
 * and the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
 * License was not distributed with this source code in the LICENSE file, you
 * can obtain it at www.aomedia.org/license/software-license/bsd-3-c-c. If the
 * Alliance for Open Media Patent License 1.0 was not distributed with this
 * source code in the PATENTS file, you can obtain it at
 * www.aomedia.org/license/patent.
 */

#include "iamf/api/conversion/channel_reorderer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/types/span.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "iamf/obu/mix_presentation.h"
#include "iamf/obu/types.h"

namespace iamf_tools {

using ::absl::MakeSpan;
using ::testing::ContainerEq;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAreArray;

namespace {
// Helper to get an appropriate number of samples for tests.
int32_t GetNumberOfChannels(
    LoudspeakersSsConventionLayout::SoundSystem sound_system) {
  int32_t num_channels = 0;
  absl::Status status = MixPresentationObu::GetNumChannelsFromLayout(
      {.layout_type = Layout::kLayoutTypeLoudspeakersSsConvention,
       .specific_layout =
           LoudspeakersSsConventionLayout{.sound_system = sound_system}},
      num_channels);
  return num_channels;
}

// Helper to make some random samples of the appropriate size.
std::vector<absl::Span<const InternalSampleType>> CreateAudioSamples(
    LoudspeakersSsConventionLayout::SoundSystem sound_system,
    size_t num_ticks = 5) {
  static std::vector<std::vector<InternalSampleType>> samples;

  const auto num_channels = GetNumberOfChannels(sound_system);
  samples.resize(num_channels, std::vector<InternalSampleType>(num_ticks));
  std::vector<absl::Span<const InternalSampleType>> sample_spans(num_channels);
  int32_t i = 0;
  for (int c = 0; c < num_channels; c++) {
    for (auto& sample : samples[c]) {
      sample =
          static_cast<InternalSampleType>(i++) / (num_channels * num_ticks);
    }
    sample_spans[c] = absl::MakeConstSpan(samples[c]);
  }
  return sample_spans;
}

struct ReordererTestCase {
  std::string test_name;
  LoudspeakersSsConventionLayout::SoundSystem sound_system;
  ChannelReorderer::RearrangementScheme scheme;
};

// Parameterized test for when the samples should be unaltered.
using ChannelReordererTest_NoChange = TestWithParam<ReordererTestCase>;
TEST_P(ChannelReordererTest_NoChange, SamplesAreUnaltered) {
  const auto& sound_system = GetParam().sound_system;
  const auto scheme = GetParam().scheme;
  auto reorderer = ChannelReorderer::Create(sound_system, scheme);
  auto samples = CreateAudioSamples(sound_system);
  auto expected = samples;  // Save a copy for comparison.

  reorderer.Reorder(samples);

  EXPECT_THAT(samples, ContainerEq(expected));
}

// When using kDefaultNoOp, no layout should be altered.
INSTANTIATE_TEST_SUITE_P(
    ChannelReordererTest_NoOpInstantiation, ChannelReordererTest_NoChange,
    testing::ValuesIn<ReordererTestCase>({
        {"NoOp_kSoundSystemA_0_2_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemA_0_2_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemB_0_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemB_0_5_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemC_2_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemC_2_5_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemD_4_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemD_4_5_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemE_4_5_1",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemE_4_5_1,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemF_3_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemF_3_7_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemG_4_9_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemG_4_9_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemH_9_10_3",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemH_9_10_3,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemI_0_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemI_0_7_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystemJ_4_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemJ_4_7_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystem10_2_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem10_2_7_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystem11_2_3_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem11_2_3_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystem12_0_1_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem12_0_1_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
        {"NoOp_kSoundSystem13_6_9_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem13_6_9_0,
         ChannelReorderer::RearrangementScheme::kDefaultNoOp},
    }),
    [](const testing::TestParamInfo<ChannelReordererTest_NoChange::ParamType>&
           info) { return info.param.test_name; });

// These layouts should be unaltered for Android.
INSTANTIATE_TEST_SUITE_P(
    ChannelReordererTest_Android_UnchangedLayouts_Instantiation,
    ChannelReordererTest_NoChange,
    testing::ValuesIn<ReordererTestCase>({
        {"Android_kSoundSystemA_0_2_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemA_0_2_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystemB_0_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemB_0_5_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystemC_2_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemC_2_5_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystemD_4_5_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemD_4_5_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystemE_4_5_1",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemE_4_5_1,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystem11_2_3_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem11_2_3_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystem12_0_1_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem12_0_1_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystem13_6_9_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem13_6_9_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
    }),
    [](const testing::TestParamInfo<ChannelReordererTest_NoChange::ParamType>&
           info) { return info.param.test_name; });

using ChannelReordererTest_SwapBackAndSides = TestWithParam<ReordererTestCase>;

TEST_P(ChannelReordererTest_SwapBackAndSides, SamplesAreUnaltered) {
  const auto& sound_system = GetParam().sound_system;
  const auto scheme = GetParam().scheme;
  auto reorderer = ChannelReorderer::Create(sound_system, scheme);
  auto samples = CreateAudioSamples(sound_system, /*num_ticks=*/1);
  const auto original = samples;  // Save a copy for comparison.

  reorderer.Reorder(samples);

  EXPECT_EQ(samples[4], original[6]);
  EXPECT_EQ(samples[5], original[7]);
  EXPECT_EQ(samples[6], original[4]);
  EXPECT_EQ(samples[7], original[5]);
  EXPECT_EQ(MakeSpan(samples).first(4), MakeSpan(original).first(4));
  if (original[0].size() > 7) {
    EXPECT_EQ(MakeSpan(samples).subspan(8), MakeSpan(original).subspan(8));
  }
}

INSTANTIATE_TEST_SUITE_P(
    ChannelReordererTest_AndroidSwapBackAndSides_Instantiation,
    ChannelReordererTest_SwapBackAndSides,
    testing::ValuesIn<ReordererTestCase>({
        {"Android_kSoundSystemI_0_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemI_0_7_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystemJ_4_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemJ_4_7_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
        {"Android_kSoundSystem10_2_7_0",
         LoudspeakersSsConventionLayout::SoundSystem::kSoundSystem10_2_7_0,
         ChannelReorderer::RearrangementScheme::kReorderForAndroid},
    }),
    [](const testing::TestParamInfo<
        ChannelReordererTest_SwapBackAndSides::ParamType>& info) {
      return info.param.test_name;
    });

TEST(ChannelReordererTest, TestLayoutFForAndroid) {
  auto reorderer = ChannelReorderer::Create(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemF_3_7_0,
      ChannelReorderer::RearrangementScheme::kReorderForAndroid);
  auto samples = CreateAudioSamples(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemF_3_7_0,
      /*num_ticks=*/1);
  const auto original = samples;  // Save a copy for comparison.

  reorderer.Reorder(samples);

  // Check we have all the same samples.
  EXPECT_THAT(samples, UnorderedElementsAreArray(original));
  // Check the reordering.
  EXPECT_EQ(samples[0], original[1]);
  EXPECT_EQ(samples[1], original[2]);
  EXPECT_EQ(samples[2], original[0]);
  EXPECT_EQ(samples[3], original[10]);
  EXPECT_EQ(samples[4], original[7]);
  EXPECT_EQ(samples[5], original[8]);
  EXPECT_EQ(samples[6], original[5]);
  EXPECT_EQ(samples[7], original[6]);
  EXPECT_EQ(samples[8], original[9]);
  EXPECT_EQ(samples[9], original[3]);
  EXPECT_EQ(samples[10], original[4]);
}

TEST(ChannelReordererTest, TestLayoutGForAndroid) {
  auto reorderer = ChannelReorderer::Create(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemG_4_9_0,
      ChannelReorderer::RearrangementScheme::kReorderForAndroid);
  auto samples = CreateAudioSamples(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemG_4_9_0,
      /*num_ticks=*/1);
  auto original = samples;  // Save a copy for comparison.

  reorderer.Reorder(samples);

  // Check we have all the same samples.
  EXPECT_THAT(samples, UnorderedElementsAreArray(original));
  // Check the reordering.
  for (int c = 0; c < 4; c++) {
    EXPECT_EQ(samples[c], original[c]);
  }
  EXPECT_EQ(samples[4], original[6]);
  EXPECT_EQ(samples[5], original[7]);
  EXPECT_EQ(samples[6], original[12]);
  EXPECT_EQ(samples[7], original[13]);
  EXPECT_EQ(samples[8], original[4]);
  EXPECT_EQ(samples[9], original[5]);
  EXPECT_EQ(samples[10], original[8]);
  EXPECT_EQ(samples[11], original[9]);
  EXPECT_EQ(samples[12], original[10]);
  EXPECT_EQ(samples[13], original[11]);
}

TEST(ChannelReordererTest, TestLayoutHForAndroid) {
  auto reorderer = ChannelReorderer::Create(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemH_9_10_3,
      ChannelReorderer::RearrangementScheme::kReorderForAndroid);
  auto samples = CreateAudioSamples(
      LoudspeakersSsConventionLayout::SoundSystem::kSoundSystemH_9_10_3,
      /*num_ticks=*/1);
  auto original = samples;  // Save a copy for comparison.

  reorderer.Reorder(samples);

  // Check we have all the same samples.
  EXPECT_THAT(samples, UnorderedElementsAreArray(original));
  // Check the reordering.
  // 0-8 are the same.
  for (int c = 0; c < 8; c++) {
    EXPECT_EQ(samples[c], original[c]);
  }
  EXPECT_EQ(samples[9], original[10]);
  EXPECT_EQ(samples[10], original[11]);
  EXPECT_EQ(samples[11], original[15]);
  // 12 is the same
  EXPECT_EQ(samples[12], original[12]);
  EXPECT_EQ(samples[13], original[14]);
  EXPECT_EQ(samples[14], original[13]);
  EXPECT_EQ(samples[15], original[16]);
  EXPECT_EQ(samples[16], original[20]);
  // 17-19 are the same.
  EXPECT_EQ(samples[17], original[17]);
  EXPECT_EQ(samples[18], original[18]);
  EXPECT_EQ(samples[19], original[19]);
  EXPECT_EQ(samples[20], original[22]);
  EXPECT_EQ(samples[21], original[21]);
  EXPECT_EQ(samples[22], original[23]);
  EXPECT_EQ(samples[23], original[9]);
}

}  // namespace
}  // namespace iamf_tools
