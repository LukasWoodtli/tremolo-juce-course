#pragma once

namespace tremolo {
class Tremolo {
public:
  Tremolo()
  {
    lfo.setFrequency(5.0f, // Hz
      true);
  }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec spec {
      .sampleRate = sampleRate,
      .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
      .numChannels =  1u};
    lfo.prepare(spec);
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      const auto lfoVal = lfo.processSample(0.f);

      // TODO: calculate the modulation value

      // for each channel sample in the frame
      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // TODO: modulate the sample
        const auto outputSample = 0.1f * lfoVal;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void reset() noexcept {lfo.reset();}

private:
  juce::dsp::Oscillator<float> lfo{[](const auto phase) { return std::sin(phase); }};
};
}  // namespace tremolo
