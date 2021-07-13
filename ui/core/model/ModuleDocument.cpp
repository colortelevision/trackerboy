
#include "core/model/ModuleDocument.hpp"

#include <QFileInfo>

#include <fstream>

template <bool tPermanent>
ModuleDocument::EditContext<tPermanent>::EditContext(ModuleDocument &document) :
    mDocument(document)
{
    mDocument.mMutex.lock();
}

template <bool tPermanent>
ModuleDocument::EditContext<tPermanent>::~EditContext() {
    mDocument.mMutex.unlock();
    if constexpr (tPermanent) {
        mDocument.makeDirty();
    }
}

template class ModuleDocument::EditContext<true>;
template class ModuleDocument::EditContext<false>;

ModuleDocument::ModuleDocument(QObject *parent) :
    QObject(parent),
    mPermaDirty(false),
    mModified(false),
    mModule(),
    mMutex(),
    mUndoStack(),
    mInstrumentModel(*this),
    mOrderModel(*this),
    mSongModel(*this),
    mPatternModel(*this),
    mWaveModel(*this),
    mLastError(trackerboy::FormatError::none),
    mFilename(),
    mFilepath(),
    mTitle(),
    mArtist(),
    mCopyright(),
    mComments(),
    mChannelEnables(CH1 | CH2 | CH3 | CH4),
    mKeyRepetition(true),
    mEditStep(1),
    mInstrument(0)
{
    connect(&mUndoStack, &QUndoStack::cleanChanged, this, &ModuleDocument::onStackCleanChanged);
}

trackerboy::FormatError ModuleDocument::lastError() {
    return mLastError;
}

void ModuleDocument::clear() {
    mUndoStack.clear();
    mModule.clear();

    mLastError = trackerboy::FormatError::none;
    mFilename.clear();
    mFilepath.clear();
    mTitle.clear();
    mArtist.clear();
    mCopyright.clear();
    mComments.clear();
    mInstrument = 0;

    clean();

    mInstrumentModel.reload();
    mWaveModel.reload();
    mOrderModel.reload();
    mPatternModel.reload();
    mSongModel.reload();

    emit reloaded();
}

bool ModuleDocument::open(QString const& path) {
    bool success = false;
    std::ifstream in(path.toStdString(), std::ios::binary | std::ios::in);
    if (in.good()) {
        mLastError = mModule.deserialize(in);
        if (mLastError == trackerboy::FormatError::none) {

            updateFilename(path);

            mTitle = QString::fromStdString(mModule.title());
            mArtist = QString::fromStdString(mModule.artist());
            mCopyright = QString::fromStdString(mModule.copyright());
            mComments = QString::fromStdString(mModule.comments());

            mInstrumentModel.reload();
            mWaveModel.reload();
            mOrderModel.reload();
            mPatternModel.reload();
            mSongModel.reload();

            emit reloaded();
            
            success = true;
        } else {
            clear();
        }
    }

    in.close();
    return success;
}

bool ModuleDocument::save() {
    if (mFilepath.isEmpty()) {
        return false;
    } else {
        return doSave(mFilepath);
    }
}

bool ModuleDocument::save(QString const& filename) {
    auto result = doSave(filename);
    if (result) {
        updateFilename(filename);
    }

    return result;
   
}

QString ModuleDocument::name() const noexcept {
    return mFilename;
}

void ModuleDocument::setName(QString const& name) noexcept {
    mFilename = name;
}

QString ModuleDocument::filepath() const noexcept {
    return mFilepath;
}

bool ModuleDocument::hasFile() const noexcept {
    return !mFilepath.isEmpty();
}

bool ModuleDocument::isModified() const {
    return mModified;
}

trackerboy::Module& ModuleDocument::mod() {
    return mModule;
}

QUndoStack& ModuleDocument::undoStack() {
    return mUndoStack;
}

InstrumentListModel& ModuleDocument::instrumentModel() noexcept {
    return mInstrumentModel;
}

OrderModel& ModuleDocument::orderModel() noexcept {
    return mOrderModel;
}

PatternModel& ModuleDocument::patternModel() noexcept {
    return mPatternModel;
}

SongModel& ModuleDocument::songModel() noexcept {
    return mSongModel;
}

WaveListModel& ModuleDocument::waveModel() noexcept {
    return mWaveModel;
}

ModuleDocument::EditContext<true> ModuleDocument::beginEdit() {
    return { *this };
}

ModuleDocument::EditContext<false> ModuleDocument::beginCommandEdit() {
    return { *this };
}



bool ModuleDocument::doSave(QString const& filename) {
    bool success = false;
    std::ofstream out(filename.toStdString(), std::ios::binary | std::ios::out);
    if (out.good()) {
        mInstrumentModel.commit();
        mWaveModel.commit();

        mModule.setTitle(mTitle.toStdString());
        mModule.setArtist(mArtist.toStdString());
        mModule.setCopyright(mCopyright.toStdString());
        mModule.setComments(mComments.toStdString());

        success = mModule.serialize(out) == trackerboy::FormatError::none;
        if (success) {
            clean();
        }

    }

    out.close();
    return success;
}

void ModuleDocument::makeDirty() {
    if (!mPermaDirty) {
        mPermaDirty = true;
        if (!mModified) {
            mModified = true;
            emit modifiedChanged(true);
        }
    }
}

void ModuleDocument::onStackCleanChanged(bool clean) {
    bool modified = mPermaDirty || !clean;
    if (mModified != modified) {
        mModified = modified;
        emit modifiedChanged(mModified);
    }
}

void ModuleDocument::clean() {
    mPermaDirty = false;
    if (mModified) {
        mModified = false;
        emit modifiedChanged(false);
    }
    mUndoStack.setClean();
}


void ModuleDocument::lock() {
    mMutex.lock();
}

void ModuleDocument::unlock() {
    mMutex.unlock();
}

void ModuleDocument::updateFilename(QString const& path) {
    mFilepath = path;
    QFileInfo info(path);
    mFilename = info.fileName();
}

QString ModuleDocument::title() const noexcept {
    return mTitle;
}

QString ModuleDocument::artist() const noexcept {
    return mArtist;
}

QString ModuleDocument::copyright() const noexcept {
    return mCopyright;
}

QString ModuleDocument::comments() const noexcept {
    return mComments;
}

void ModuleDocument::setTitle(QString const& title) {
    mTitle = title;
    makeDirty();
}

void ModuleDocument::setArtist(QString const& artist) {
    mArtist = artist;
    makeDirty();
}

void ModuleDocument::setCopyright(QString const& copyright) {
    mCopyright = copyright;
    makeDirty();
}

void ModuleDocument::setComments(QString const& comments) {
    mComments = comments;
    makeDirty();
}

ModuleDocument::OutputFlags ModuleDocument::channelOutput() {
    return mChannelEnables;
}

void ModuleDocument::setChannelOutput(OutputFlags flags) {
    if (flags != mChannelEnables) {
        mChannelEnables = flags;
        emit channelOutputChanged(flags);
    }
}

void ModuleDocument::toggleChannelOutput() {
    auto track = mPatternModel.cursorTrack();
    mChannelEnables ^= (OutputFlag)(1 << track);
    emit channelOutputChanged(mChannelEnables);
}

void ModuleDocument::solo() {
    auto track = mPatternModel.cursorTrack();
    auto soloFlags = (OutputFlag)(1 << track);
    if (mChannelEnables == soloFlags) {
        mChannelEnables = AllOn;
    } else {
        mChannelEnables = soloFlags;
    }
    emit channelOutputChanged(mChannelEnables);
}

void ModuleDocument::setKeyRepetition(bool repetition) {
    mKeyRepetition = repetition;
}

bool ModuleDocument::keyRepetition() const {
    return mKeyRepetition;
}

void ModuleDocument::setEditStep(int editStep) {
    if (editStep >= 0) {
        mEditStep = editStep;
    }
}

int ModuleDocument::editStep() const {
    return mEditStep;
}

int ModuleDocument::instrument() const {
    return mInstrument;
}

void ModuleDocument::setInstrument(int instrument) {
    mInstrument = instrument;
}

float ModuleDocument::framerate() const {
    return mModule.framerate();
}

void ModuleDocument::setFramerate(int rate) {
    if (mModule.system() == trackerboy::System::custom && mModule.customFramerate() == rate) {
        return;
    }
    
    auto ctx = beginEdit();
    mModule.setFramerate(rate);
    emit framerateChanged((float)rate);
    
}

void ModuleDocument::setFramerate(trackerboy::System system) {
    if (system == trackerboy::System::custom || mModule.system() == system) {
        return;
    }

    auto ctx = beginEdit();
    mModule.setFramerate(system);
    emit framerateChanged(mModule.framerate());
}
