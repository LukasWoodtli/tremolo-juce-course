#pragma once

namespace tremolo {
class Tremolo {
public:
  enum class LfoWaveform : size_t {
    sine = 0,
    triangle = 1,
  };

  Tremolo()
  {
    for (auto& lfo : lfos)
    {
      lfo.setFrequency(5.0f, // Hz
        true);
    }
  }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec{
        .sampleRate = sampleRate,
        .maximumBlockSize =
            static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = 1u,
    };
    for (auto& lfo : lfos) {
      lfo.prepare(processSpec);
    }
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    updateLfoWaveform();

    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      juce::dsp::Oscillator<float> &lfo = lfos[static_cast<size_t>(currentLfo)];
      const auto lfoValue = lfo.processSample(0.f);

      // calculate the modulation value
      const auto modulationValue = modulationDepth * lfoValue + 1.f;

      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // modulate the sample
        const auto outputSample = modulationValue * inputSample;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void reset() noexcept
  {
    for (auto& lfo : lfos) {
      lfo.reset();
    }
  }

  void setLfoWaveform(LfoWaveform waveform) noexcept
  {
    lfoToSet = waveform;
  }



private:
  static constexpr float modulationDepth = 0.4f;

  static float triangle(float phase) {return std::abs(2 * phase / juce::MathConstants<float>::pi) - 1.f;}

  void updateLfoWaveform() noexcept {
    if (lfoToSet != currentLfo)
    {
      currentLfo = lfoToSet;
    }
  }
  std::array<juce::dsp::Oscillator<float>, 2u> lfos = {
  juce::dsp::Oscillator<float>{[](const auto phase) { return std::sin(phase); }},
  juce::dsp::Oscillator<float>{triangle}
};
  LfoWaveform currentLfo = LfoWaveform::sine;
  LfoWaveform lfoToSet = currentLfo;
};
}  // namespace tremolo
