#pragma once

#include "PluginProcessor.h"

class BloodLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(
        juce::Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;
};

class BloodScrapeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BloodScrapeAudioProcessorEditor(BloodScrapeAudioProcessor&);
    ~BloodScrapeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BloodScrapeAudioProcessor& processor;

    BloodLookAndFeel bloodLookAndFeel;

    juce::Slider hardnessKnob;
    juce::Label hardnessLabel;
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        hardnessAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        BloodScrapeAudioProcessorEditor)
};
