
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QIntValidator>

#include <sstream>
#include <iomanip>

#include "trackerboy.hpp"

#include "InstEditor.hpp"

namespace instedit {

InstEditor::InstEditor(QWidget *parent) :
    programEdit(new QPlainTextEdit),
    outputEdit(new QPlainTextEdit),
    playButton(new QPushButton("Play")),
    loopCheckbox(new QCheckBox("Loop")),
    frequencyEdit(new QLineEdit("0")),
    worker(new SynthWorker(this)),
    currentTrackId(trackerboy::TrackId::ch1),
    runtimeTable{nullptr},
    programTable{nullptr},
    playing(false),
    QWidget(parent)
{
    connect(worker, &SynthWorker::finished, this, &InstEditor::workerFinished);


    const QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    outputEdit->setReadOnly(true);
    outputEdit->setFont(font);
    programEdit->setFont(font);

    runtimeTable[0] = new trackerboy::PulseInstrumentRuntime(true);
    runtimeTable[1] = new trackerboy::PulseInstrumentRuntime(false);
    runtimeTable[2] = new trackerboy::WaveInstrumentRuntime();
    runtimeTable[3] = new trackerboy::NoiseInstrumentRuntime();
    
    for (int i = 0; i != 4; ++i) {
        programList.append("");
        outputList.append("");
        programTable[i] = new std::vector<trackerboy::Instruction>;
        runtimeTable[i]->setProgram(programTable[i]);
    }

    QButtonGroup *group = new QButtonGroup(this);
    QRadioButton *radioCh1 = new QRadioButton("CH1");
    radioCh1->setChecked(true);
    QRadioButton *radioCh2 = new QRadioButton("CH2");
    QRadioButton *radioCh3 = new QRadioButton("CH3");
    QRadioButton *radioCh4 = new QRadioButton("CH4");
    group->addButton(radioCh1, static_cast<int>(trackerboy::TrackId::ch1));
    group->addButton(radioCh2, static_cast<int>(trackerboy::TrackId::ch2));
    group->addButton(radioCh3, static_cast<int>(trackerboy::TrackId::ch3));
    group->addButton(radioCh4, static_cast<int>(trackerboy::TrackId::ch4));
    connect(group, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonReleased), this, &InstEditor::channelSelected);


    QGridLayout *layout = new QGridLayout;

    // row 0, channel selection
    QHBoxLayout *channelLayout = new QHBoxLayout;
    QLabel *label = new QLabel("Channel: ");
    channelLayout->addStretch();
    channelLayout->addWidget(label);
    channelLayout->addWidget(radioCh1);
    channelLayout->addWidget(radioCh2);
    channelLayout->addWidget(radioCh3);
    channelLayout->addWidget(radioCh4);
    channelLayout->addStretch();
    layout->addLayout(channelLayout, 0, 0, 1, 2);

    // row 1, labels for edit controls
    label = new QLabel("Source");
    layout->addWidget(label, 1, 0);
    label = new QLabel("Output");
    layout->addWidget(label, 1, 1);

    // row 2, source editor, output
    layout->addWidget(programEdit, 2, 0);
    layout->addWidget(outputEdit, 2, 1);

    // row 3, playback controls
    QHBoxLayout *playbackLayout = new QHBoxLayout;
    /*QPushButton *parseButton = new QPushButton("Parse");
    connect(parseButton, &QPushButton::released, this, &InstEditor::parse);
    playbackLayout->addWidget(parseButton);*/
    //playButton->setEnabled(false);
    connect(playButton, &QPushButton::released, this, &InstEditor::play);
    playbackLayout->addWidget(playButton);
    
    playbackLayout->addWidget(loopCheckbox);
    playbackLayout->addWidget(new QLabel("Frequency (0-2047):"));
    playbackLayout->addWidget(frequencyEdit);
    frequencyEdit->setValidator(new QIntValidator(0, trackerboy::Channel::MAX_FREQUENCY, this));

    playbackLayout->addStretch();
    layout->addLayout(playbackLayout, 3, 0, 1, 2);
    


    layout->setRowStretch(2, 1);
    setLayout(layout);

}

InstEditor::~InstEditor() {
    for (int i = 0; i != 4; ++i) {
        delete runtimeTable[i];
        delete programTable[i];
    }
}


void InstEditor::channelSelected(int id) {
    int lastId = static_cast<int>(currentTrackId);
    if (lastId != id) {
        programList.replace(lastId, programEdit->toPlainText());
        outputList.replace(lastId, outputEdit->toPlainText());
        programEdit->setPlainText(programList.at(id));
        outputEdit->setPlainText(outputList.at(id));

        currentTrackId = static_cast<trackerboy::TrackId>(id);
    }
}


bool InstEditor::parse() {
    auto *program = programTable[static_cast<int>(currentTrackId)];
    program->clear();
    bool success = true;

    QString source = programEdit->toPlainText();
    // don't parse an empty string due to a known bug in the parser
    if (!source.isEmpty()) {
        QStringList lines = source.split('\n');
        for (auto iter = lines.begin(); iter != lines.end(); ++iter) {
            try {
                program->push_back(trackerboy::parse(currentTrackId, iter->toStdString()));
            } catch (trackerboy::ParseError err) {
                success = false;
                break;
            }
        }
    }

    if (success) {
        std::ostringstream stream;
        for (auto iter = program->begin(); iter != program->end(); ++iter) {
            stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(iter->duration)
                << ' ' << std::setw(2) << std::hex << static_cast<unsigned>(iter->ctrl)
                << ' ' << std::setw(2) << std::hex << static_cast<unsigned>(iter->settings)
                << ' ' << std::setw(2) << std::hex << static_cast<unsigned>(iter->envSettings)
                << ' ' << std::setw(2) << std::hex << static_cast<unsigned>(iter->note)
                << '\n';
        }
        outputEdit->setPlainText(QString::fromStdString(stream.str()));
    } else {
        outputEdit->setPlainText("ERROR!");
    }

    return success;

}

void InstEditor::play() {
    if (playing) {
        worker->stop();
    } else {
        if (parse()) {
            int pos = 0;
            auto validator = frequencyEdit->validator();
            auto freqStr = frequencyEdit->text();
            if (validator->validate(freqStr, pos) == QValidator::Acceptable) {
                worker->setFrequency(freqStr.toInt());
            }

            playing = true;
            worker->setRuntime(runtimeTable[static_cast<int>(currentTrackId)]);
            worker->setLoop(loopCheckbox->isChecked());
            worker->start();
            playButton->setText("Stop");
        }
    }
}

void InstEditor::workerFinished() {
    playButton->setText("Play");
    playing = false;
}




}

