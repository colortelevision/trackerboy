
#pragma once

#include "core/midi/IMidiReceiver.hpp"
#include "core/midi/Midi.hpp"
#include "core/model/ModuleDocument.hpp"
#include "core/model/ModuleModel.hpp"
#include "core/model/InstrumentChoiceModel.hpp"
#include "forms/AboutDialog.hpp"
#include "forms/AudioDiagDialog.hpp"
#include "forms/ConfigDialog.hpp"
#include "widgets/docks/ModuleSettingsWidget.hpp"
#include "widgets/editors/InstrumentEditor.hpp"
#include "widgets/editors/WaveEditor.hpp"
#include "widgets/PatternEditor.hpp"
#include "widgets/Sidebar.hpp"
#include "widgets/visualizers/AudioScope.hpp"
#include "widgets/visualizers/PeakMeter.hpp"

#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QToolBar>
#include <QTabBar>
#include <QUndoView>
#include <QShortcut>
#include <QSplitter>
#include <QTreeView>

#include <array>

//
// Main form for the application
//
class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

    QMenu* createPopupMenu() override;

protected:

    void closeEvent(QCloseEvent *evt) override;

    void showEvent(QShowEvent *evt) override;

private slots:

    // actions
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileClose();
    void onFileCloseAll();
    
    void onViewResetLayout();

    void onWindowNext();
    void onWindowPrevious();

    // config changes
    void onConfigApplied(Config::Categories categories);

    // dialog show slots (lazy loading)
    void showAboutDialog();
    void showAudioDiag();
    void showConfigDialog();
    void showExportWavDialog();

    void onAudioStart();
    void onAudioError();
    void onAudioStop();
    void onFrameSync();

    void onTabChanged(int tabIndex);

    void onBrowserDoubleClick(QModelIndex const& index);

    void onDocumentModified(bool modified);

    void updateWindowMenu();

    void closeTab(int index);

private:
    Q_DISABLE_COPY(MainWindow)

    enum class PlayingStatusText {
        ready,
        playing,
        error
    };

    void setupUi();

    void initState();

    void setupViewMenu(QMenu &menu);

    void setupSongMenu(QMenu &menu);

    //
    // Shows and adds a "Change settings" button that opens the configuration
    // to the given QMessageBox
    //
    void settingsMessageBox(QMessageBox &msgbox);

    // document management

    void addDocument(ModuleDocument *doc);

    bool saveDocument(ModuleDocument *doc);

    bool saveDocumentAs(ModuleDocument *doc);

    bool closeDocument(ModuleDocument *doc);

    void updateWindowTitle();

    void updateOrderActions();

    void setPlayingStatus(PlayingStatusText type);

    void handleFocusChange(QWidget *oldWidget, QWidget *newWidget);

    //
    // Shows a message and disables the configured midi device.
    // If causedByError is true, then the messagebox states it was caused by
    // an error, otherwise it states that it failed to initialize the device.
    //
    void disableMidi(bool causedByError);

    Config mConfig;

    Midi mMidi;
    IMidiReceiver *mMidiReceiver;
    bool mMidiNoteDown;

    PianoInput mPianoInput;


    // counter for how many times a new document has been created
    int mDocumentCounter;

    ModuleModel mBrowserModel;

    bool mErrorSinceLastConfig;
    trackerboy::Frame mLastEngineFrame;
    int mFrameSkip;

    // dialogs
    AboutDialog *mAboutDialog;
    AudioDiagDialog *mAudioDiag;
    ConfigDialog *mConfigDialog;


    // toolbars
    QToolBar mToolbarFile;
    QToolBar mToolbarEdit;
    QToolBar mToolbarSong;
    QToolBar mToolbarTracker;
    QToolBar mToolbarInput;
        QLabel mOctaveLabel;
        QSpinBox mOctaveSpin;
        QLabel mEditStepLabel;
        QSpinBox mEditStepSpin;
    QToolBar mToolbarInstrument;
        QComboBox mInstrumentCombo;
        InstrumentChoiceModel mInstrumentChoiceModel;


    // dock widgets
    QDockWidget mDockModuleSettings;
        ModuleSettingsWidget mModuleSettingsWidget;

    QDockWidget mDockInstrumentEditor;
        InstrumentEditor mInstrumentEditor;

    QDockWidget mDockWaveformEditor;
        WaveEditor mWaveEditor;

    QDockWidget mDockHistory;
        QUndoView mUndoView;

    // central widget (must be heap-alloc'd)
    QSplitter *mHSplitter;
        QTreeView mBrowser;
        QWidget mMainWidget;
            QVBoxLayout mMainLayout;
                QTabBar mTabs;
                QWidget mEditorWidget;
                    QHBoxLayout mEditorLayout;
                        Sidebar mSidebar;
                        PatternEditor mPatternEditor;


    // statusbar widgets
    QLabel mStatusRenderer;
    QLabel mStatusSpeed;
    QLabel mStatusTempo;
    QLabel mStatusElapsed;
    QLabel mStatusPos;
    QLabel mStatusSamplerate;

    // menus

    // action indices
    enum ActionIndex {
        ActionFileNew,
        ActionFileOpen,
        ActionFileConfig,
        ActionFileQuit,
        ActionViewResetLayout,
        ActionHelpAudioDiag,
        ActionHelpAbout,
        ActionHelpAboutQt,

        // document actions - these actions require an open document
        // when there are no open documents they are disabled
        DOCUMENT_ACTIONS_BEGIN,

        ActionFileSave = DOCUMENT_ACTIONS_BEGIN,
        ActionFileSaveAs,
        ActionFileExportWav,
        ActionFileClose,
        ActionFileCloseAll,

        ActionEditCopy,
        ActionEditCut,
        ActionEditPaste,
        ActionEditPasteMix,
        ActionEditErase,
        ActionEditSelectAll,
        ActionEditNoteIncrease,
        ActionEditNoteDecrease,
        ActionEditOctaveIncrease,
        ActionEditOctaveDecrease,
        ActionEditTranspose,
        ActionEditReverse,
        ActionEditKeyRepetition,

        ActionSongOrderInsert,
        ActionSongOrderRemove,
        ActionSongOrderDuplicate,
        ActionSongOrderMoveUp,
        ActionSongOrderMoveDown,

        ActionTrackerPlay,
        ActionTrackerRestart,
        ActionTrackerPlayCurrentRow,
        ActionTrackerStepRow,
        ActionTrackerStop,
        ActionTrackerRecord,
        ActionTrackerToggleChannel,
        ActionTrackerSolo,
        ActionTrackerKill,
        ActionTrackerRepeat,
        ActionTrackerFollow,

        ActionWindowPrev,
        ActionWindowNext,

        ACTION_COUNT
    };

    std::array<QAction, ACTION_COUNT> mActions;

    // File
    QMenu mMenuFile;

    // Edit
    QMenu mMenuEdit;
        QAction *mActionEditUndo;
        QAction *mActionEditRedo;
        QMenu mMenuTranspose;
    
    // Song
    QMenu mMenuSong;

    // View
    QMenu mMenuView;
        QMenu mMenuViewToolbars;

    // Tracker
    QMenu mMenuTracker;

    // Window
    QMenu mMenuWindow;

    // Help
    QMenu mMenuHelp;

    QMenu mContextMenuOrder;

    QShortcut mPlayAndStopShortcut;

    Renderer mRenderer;


};
