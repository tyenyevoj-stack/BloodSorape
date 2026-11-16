#include "PluginProcessor.h"
#include "PluginEditor.h"

BloodScrapeAudioProcessor::BloodScrapeAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameters())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
BloodScrapeAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "hardness", 1 },
        "Hardness",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.45f));

    return { parameters.begin(), parameters.end() };
}

void BloodScrapeAudioProcessor::prepareToPlay (double, int)
{
    previousSamples.assign ((size_t) getTotalNumInputChannels(), 0.0f);
}

void BloodScrapeAudioProcessor::releaseResources()
{
}

bool BloodScrapeAudioProcessor::isBusesLayoutSupported (
    const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    if (input != output)
        return false;

    return input == juce::AudioChannelSet::mono()
        || input == juce::AudioChannelSet::stereo();
}

void BloodScrapeAudioProcessor::processBlock (
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const float hardness = *apvts.getRawParameterValue ("hardness");

    const float drive = 1.0f + hardness * 24.0f;
    const float scrapeAmount = 0.01f + hardness * 0.18f;
    const float scrapeIntensity = 8.0f + hardness * 95.0f;
    const float finalDrive = 1.0f + hardness * 2.2f;
    const float normalizer = std::tanh (finalDrive);

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if ((int) previousSamples.size() != numChannels)
        previousSamples.assign ((size_t) numChannels, 0.0f);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = buffer.getWritePointer (channel);
        float previous = previousSamples[(size_t) channel];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float input = samples[sample];
            const float clipped = std::tanh (input * drive);

            // Скрежет появляется на резких ударах кика, снейра и 808.
            const float transient = input - previous;
            const float metallicTone =
                std::sin (clipped * (22.0f + hardness * 240.0f));

            const float scrape = std::tanh (
                (transient * scrapeIntensity)
                + (metallicTone * hardness * 0.26f));

            float output = clipped + scrape * scrapeAmount;
            output = std::tanh (output * finalDrive) / normalizer;

            samples[sample] = juce::jlimit (-1.0f, 1.0f, output);
            previous = input;
        }

        previousSamples[(size_t) channel] = previous;
    }
}

juce::AudioProcessorEditor*
BloodScrapeAudioProcessor::createEditor()
{
    return new BloodScrapeAudioProcessorEditor (*this);
}

void BloodScrapeAudioProcessor::getStateInformation (
    juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void BloodScrapeAudioProcessor::setStateInformation (
    const void* data,
    int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (
        getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BloodScrapeAudioProcessor();
}
