 #include "PluginEditor.h"

void BloodLookAndFeel::drawRotarySlider (
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider&)
{
    const float diameter = (float) juce::jmin (width, height) - 16.0f;
    const float radius = diameter * 0.5f;
    const float centreX = (float) x + (float) width * 0.5f;
    const float centreY = (float) y + (float) height * 0.5f;

    const float angle = rotaryStartAngle
        + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Тень ручки.
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (
        centreX - radius + 5.0f,
        centreY - radius + 7.0f,
        diameter,
        diameter);

    // Кровавое внешнее кольцо.
    g.setColour (juce::Colour (0xff700000));
    g.fillEllipse (
        centreX - radius,
        centreY - radius,
        diameter,
        diameter);

    // Светлая окантовка.
    g.setColour (juce::Colour (0xffded9d0));
    g.fillEllipse (
        centreX - radius + 8.0f,
        centreY - radius + 8.0f,
        diameter - 16.0f,
        diameter - 16.0f);

    // Тёмная поверхность ручки.
    g.setColour (juce::Colour (0xff171313));
    g.fillEllipse (
        centreX - radius + 14.0f,
        centreY - radius + 14.0f,
        diameter - 28.0f,
        diameter - 28.0f);

    // Красный указатель значения.
    juce::Path pointer;
    pointer.addRoundedRectangle (
        -3.5f,
        -radius + 20.0f,
        7.0f,
        radius * 0.68f,
        2.0f);

    pointer.applyTransform (
        juce::AffineTransform::rotation (angle)
            .translated (centreX, centreY));

    g.setColour (juce::Colour (0xffe00000));
    g.fillPath (pointer);

    // Капли вокруг ручки.
    g.setColour (juce::Colour (0xff970000).withAlpha (0.85f));

    for (int i = 0; i < 18; ++i)
    {
        const float dotAngle =
            juce::MathConstants<float>::twoPi * ((float) i / 18.0f);

        const float dotRadius = radius + 9.0f + (float) (i % 3) * 3.0f;
        const float dotX = centreX + std::cos (dotAngle) * dotRadius;
        const float dotY = centreY + std::sin (dotAngle) * dotRadius;

        g.fillEllipse (dotX - 2.0f, dotY - 2.0f, 4.0f, 4.0f);
    }
}

BloodScrapeAudioProcessorEditor::BloodScrapeAudioProcessorEditor (
    BloodScrapeAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p)
{
    setSize (760, 460);

    hardnessKnob.setSliderStyle (
        juce::Slider::RotaryHorizontalVerticalDrag);

    hardnessKnob.setTextBoxStyle (
        juce::Slider::NoTextBox,
        false,
        0,
        0);

    hardnessKnob.setLookAndFeel (&bloodLookAndFeel);
    addAndMakeVisible (hardnessKnob);

    titleLabel.setText ("BLOODSCRAPE", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (42.0f, juce::Font::bold));
    titleLabel.setColour (
        juce::Label::textColourId,
        juce::Colour (0xff650000));

    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText (
        "CLIP THE SIGNAL. LEAVE THE SCARS.",
        juce::dontSendNotification);

    subtitleLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    subtitleLabel.setColour (
        juce::Label::textColourId,
        juce::Colour (0xff1a1515));

    subtitleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (subtitleLabel);

    hardnessLabel.setText ("HARDNESS", juce::dontSendNotification);
    hardnessLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    hardnessLabel.setColour (
        juce::Label::textColourId,
        juce::Colour (0xff4b0000));

    hardnessLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (hardnessLabel);

    hardnessAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.apvts,
                "hardness",
                hardnessKnob);
}
BloodScrapeAudioProcessorEditor::~BloodScrapeAudioProcessorEditor()
{
    hardnessKnob.setLookAndFeel (nullptr);
}

void BloodScrapeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Белый фон.
    g.fillAll (juce::Colour (0xffe7e3da));

    // Брызги крови.
    g.setColour (juce::Colour (0xff850000).withAlpha (0.78f));

    for (int i = 0; i < 55; ++i)
    {
        const float px = (float) ((i * 137) % getWidth());
        const float py = (float) ((i * 79) % getHeight());
        const float size = 2.0f + (float) ((i * 11) % 15);

        g.fillEllipse (px, py, size, size);
    }

    // Потёки крови сверху.
    g.setColour (juce::Colour (0xff780000).withAlpha (0.72f));

    for (int i = 0; i < 11; ++i)
    {
        const float x = 25.0f + (float) i * 72.0f;
        const float h = 18.0f + (float) ((i * 31) % 130);

        g.fillRect (x, 0.0f, 5.0f + (float) (i % 4), h);
        g.fillEllipse (x - 2.0f, h - 5.0f, 10.0f, 13.0f);
    }

    // Абстрактный хоррор-силуэт справа.
    juce::Path silhouette;

    silhouette.startNewSubPath (595.0f, 340.0f);
    silhouette.cubicTo (575.0f, 320.0f, 575.0f, 270.0f, 602.0f, 238.0f);
    silhouette.cubicTo (625.0f, 210.0f, 668.0f, 215.0f, 680.0f, 250.0f);
    silhouette.cubicTo (692.0f, 286.0f, 667.0f, 315.0f, 647.0f, 340.0f);
    silhouette.closeSubPath();

    silhouette.addEllipse (615.0f, 157.0f, 63.0f, 72.0f);

    g.setColour (juce::Colour (0xff1b1515).withAlpha (0.87f));
    g.fillPath (silhouette);

    // Кровавые пятна на силуэте.
    g.setColour (juce::Colour (0xff9f0000).withAlpha (0.84f));
    g.fillEllipse (630.0f, 177.0f, 16.0f, 38.0f);
    g.fillEllipse (652.0f, 251.0f, 25.0f, 11.0f);
    g.fillRect (604.0f, 270.0f, 8.0f, 59.0f);

    // Рамка.
    g.setColour (juce::Colour (0xff260000));
    g.drawRect (10, 10, getWidth() - 20, getHeight() - 20, 3);

    g.setColour (juce::Colour (0xffa30000));
    g.drawRect (16, 16, getWidth() - 32, getHeight() - 32, 1);
}

void BloodScrapeAudioProcessorEditor::resized()
{
    titleLabel.setBounds (45, 36, 470, 54);
    subtitleLabel.setBounds (45, 87, 470, 22);

    hardnessKnob.setBounds (220, 135, 250, 250);
    hardnessLabel.setBounds (220, 374, 250, 30);
}
