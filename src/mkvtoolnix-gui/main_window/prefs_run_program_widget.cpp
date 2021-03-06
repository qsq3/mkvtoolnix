#include "common/common_pch.h"

#include <QAction>
#include <QCursor>
#include <QDir>
#include <QMenu>

#include "common/qt.h"
#include "mkvtoolnix-gui/forms/main_window/prefs_run_program_widget.h"
#include "mkvtoolnix-gui/jobs/program_runner.h"
#include "mkvtoolnix-gui/main_window/prefs_run_program_widget.h"
#include "mkvtoolnix-gui/util/file_dialog.h"
#include "mkvtoolnix-gui/util/string.h"
#include "mkvtoolnix-gui/util/widget.h"

namespace mtx { namespace gui {

class PrefsRunProgramWidgetPrivate {
  friend class PrefsRunProgramWidget;

  std::unique_ptr<Ui::PrefsRunProgramWidget> ui;
  std::unique_ptr<QMenu> variableMenu;
  QString executable;
  QMap<QCheckBox *, Util::Settings::RunProgramForEvent> flagsByCheckbox;

  explicit PrefsRunProgramWidgetPrivate()
    : ui{new Ui::PrefsRunProgramWidget}
  {
  }
};

PrefsRunProgramWidget::PrefsRunProgramWidget(QWidget *parent,
                                             Util::Settings::RunProgramConfig const &cfg)
  : QWidget{parent}
  , d_ptr{new PrefsRunProgramWidgetPrivate{}}
{
  setupUi(cfg);
  setupToolTips();
  setupMenu();
  setupConnections();
}

PrefsRunProgramWidget::~PrefsRunProgramWidget() {
}

void
PrefsRunProgramWidget::enableControls() {
  Q_D(PrefsRunProgramWidget);

  auto active   = d->ui->cbConfigurationActive->isChecked();
  auto controls = findChildren<QWidget *>();

  for (auto const &control : controls)
    if (control == d->ui->pbExecuteNow)
      d->ui->pbExecuteNow->setEnabled(active && isValid());

    else if (control != d->ui->cbConfigurationActive)
      control->setEnabled(active);
}

void
PrefsRunProgramWidget::setupUi(Util::Settings::RunProgramConfig const &cfg) {
  Q_D(PrefsRunProgramWidget);

  // Setup UI controls.
  d->ui->setupUi(this);

  d->flagsByCheckbox[d->ui->cbAfterJobQueueStopped] = Util::Settings::RunAfterJobQueueFinishes;
  d->flagsByCheckbox[d->ui->cbAfterJobSuccessful]   = Util::Settings::RunAfterJobCompletesSuccessfully;
  d->flagsByCheckbox[d->ui->cbAfterJobError]        = Util::Settings::RunAfterJobCompletesWithErrors;

  d->ui->cbConfigurationActive->setChecked(cfg.m_active);

  d->executable = cfg.m_commandLine.value(0);
  d->ui->leCommandLine->setText(Util::escape(cfg.m_commandLine, Util::EscapeShellUnix).join(" "));

  for (auto const &checkBox : d->flagsByCheckbox.keys())
    if (cfg.m_forEvents & d->flagsByCheckbox[checkBox])
      checkBox->setChecked(true);

  auto html = QStringList{};
  html << Q("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
            "<html><head><meta name=\"qrichtext\" content=\"1\" />"
            "<style type=\"text/css\">"
            "p, li { white-space: pre-wrap; }\n"
            "</style>"
            "</head><body>"
            "<h3>")
       << QYH("Usage and examples")
       << Q("</h3><ul><li>")
       << QYH("The command line here uses Unix-style shell escaping via the backslash character even on Windows.")
       << QYH("This means that either backslashes must be doubled or the whole argument must be enclosed in single quotes.")
#if defined(SYS_WINDOWS)
       << QYH("Note that on Windows forward slashes can be used instead of backslashes in path names, too.")
#endif
       << QYH("See below for examples.")
       << Q("</li>")
       << Q("<li>")
       << QYH("Always use full path names even if the application is located in the same directory as the GUI.")
       << Q("</li>")
#if !defined(SYS_APPLE)
       << Q("<li>")
# if defined(SYS_WINDOWS)
       << QYH("Start file types other than executable files via cmd.exe.")
# else
       << QYH("Start file types other than executable files via xdg-open.")
# endif
       << QYH("See below for examples.")
       << Q("</li>")
#endif
       << Q("</ul><h3>")
       << QYH("Examples")
       << Q("</h3><ul><li>")
       << QYH("Play a WAV file with the default application:")
#if defined(SYS_WINDOWS)
       << Q("<code>")
       << QH("C:\\\\windows\\\\system32\\\\cmd.exe /c C:\\\\temp\\\\signal.wav")
       << Q("</code>")
       << QYH("or")
       << Q("<code>")
       << QH("C:/windows/system32/cmd.exe /c C:/temp/signal.wav")
       << Q("</code>")
       << QYH("or")
       << Q("<code>")
       << QH("'C:\\windows\\system32\\cmd.exe' /c 'C:\\temp\\signal.wav'")
       << Q("</code>")
#elif defined(SYS_APPLE)
       << Q("<code>")
       << QH("/usr/bin/afplay /Users/janedoe/Audio/signal.wav")
       << Q("</code>")
#else
       << Q("<code>")
       << QH("/usr/bin/xdg-open /home/janedoe/Audio/signal.wav")
       << Q("</code>")
#endif
       << Q("</li><li>")
       << QYH("Shut down the system in one minute:")
#if defined(SYS_WINDOWS)
       << Q("<code>")
       << QH("'c:\\windows\\system32\\shutdown.exe' /s /t 60")
       << Q("</code>")
#elif defined(SYS_APPLE)
       << Q("<code>")
       << QH("/usr/bin/sudo /sbin/shutdown -h +1")
       << Q("</code>")
#else
       << Q("<code>")
       << QH("/usr/bin/sudo /sbin/shutdown --poweroff +1")
       << Q("</code>")
#endif
       << Q("</li>")
       << Q("</li><li>")
       << QYH("Open the multiplexed file with a player:")
       << Q("</li>")
#if defined(SYS_WINDOWS)
       << Q("<code>")
       << QH("'C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe' '<MTX_DESTINATION_FILE_NAME>'")
       << Q("</code>")
#elif defined(SYS_APPLE)
       << Q("<code>")
       << QH("/usr/bin/vlc '<MTX_DESTINATION_FILE_NAME>'")
       << Q("</code>")
#else
       << Q("<code>")
       << QH("/usr/bin/vlc '<MTX_DESTINATION_FILE_NAME>'")
       << Q("</code>")
#endif
       << Q("</ul></body></html>");

  d->ui->tbUsageNotes->setHtml(html.join(Q(" ")));

  enableControls();
}

void
PrefsRunProgramWidget::setupToolTips() {
  Q_D(PrefsRunProgramWidget);

  Util::setToolTip(d->ui->cbConfigurationActive, QY("Deactivating this checkbox is a way to disable a configuration temporarily without having to change its parameters."));
  Util::setToolTip(d->ui->pbExecuteNow, Q("%1 %2").arg(QY("Executes the program now as a test run.")).arg(QY("Note that most <MTX_…> variables are empty and will be removed for this test run.")));
}

void
PrefsRunProgramWidget::setupMenu() {
  Q_D(PrefsRunProgramWidget);

  QList<std::pair<QString, QString> > entries{
    { QY("Variables for all job types"),                                 Q("")                           },
    { QY("Job description"),                                             Q("JOB_DESCRIPTION")            },
    { QY("Job start date && time in ISO 8601 format"),                   Q("JOB_START_TIME")             },
    { QY("Job end date && time in ISO 8601 format"),                     Q("JOB_END_TIME")               },
    { QY("Exit code (0: ok, 1: warnings occurred, 2: errors occurred)"), Q("JOB_EXIT_CODE")              },

    { QY("Variables for multiplex jobs"),                                Q("")                           },
    { QY("Destination file's name"),                                     Q("DESTINATION_FILE_NAME")      },
    { QY("Destination file's directory"),                                Q("DESTINATION_FILE_DIRECTORY") },
    { QY("Source file names"),                                           Q("SOURCE_FILE_NAMES")          },

    { QY("General variables"),                                           Q("")                           },
    { QY("Current date && time in ISO 8601 format"),                     Q("CURRENT_TIME")               },
  };

  d->variableMenu.reset(new QMenu{this});

  for (auto const &entry : entries)
    if (entry.second.isEmpty())
      d->variableMenu->addSection(entry.first);

    else {
      auto action = d->variableMenu->addAction(Q("<MTX_%1> – %2").arg(entry.second).arg(entry.first));
      connect(action, &QAction::triggered, [this, entry]() {
        this->addVariable(Q("<MTX_%1>").arg(entry.second));
      });
    }
}

void
PrefsRunProgramWidget::setupConnections() {
  Q_D(PrefsRunProgramWidget);

  connect(d->ui->leCommandLine,         &QLineEdit::textEdited, this, &PrefsRunProgramWidget::commandLineEdited);
  connect(d->ui->pbBrowseExecutable,    &QPushButton::clicked,  this, &PrefsRunProgramWidget::changeExecutable);
  connect(d->ui->pbAddVariable,         &QPushButton::clicked,  this, &PrefsRunProgramWidget::selectVariableToAdd);
  connect(d->ui->pbExecuteNow,          &QPushButton::clicked,  this, &PrefsRunProgramWidget::executeNow);
  connect(d->ui->cbConfigurationActive, &QCheckBox::toggled,    this, &PrefsRunProgramWidget::enableControls);
}

void
PrefsRunProgramWidget::selectVariableToAdd() {
  Q_D(PrefsRunProgramWidget);

  d->variableMenu->exec(QCursor::pos());
}

void
PrefsRunProgramWidget::addVariable(QString const &variable) {
  changeArguments([&](QStringList &arguments) {
    arguments << variable;
  });
}

bool
PrefsRunProgramWidget::isValid()
  const {
  Q_D(const PrefsRunProgramWidget);

  auto arguments = Util::unescapeSplit(d->ui->leCommandLine->text(), Util::EscapeShellUnix);
  return !arguments.value(0).isEmpty();
}

void
PrefsRunProgramWidget::executeNow() {
  if (isValid())
    Jobs::ProgramRunner::run(Util::Settings::RunNever, [](Jobs::ProgramRunner::VariableMap &) {}, config());
}

void
PrefsRunProgramWidget::changeExecutable() {
  Q_D(PrefsRunProgramWidget);

  auto filters = QStringList{};

#if defined(SYS_WINDOWS)
  filters << QY("Executable files") + Q(" (*.exe *.bat *.cmd)");
#endif
  filters << QY("All files") + Q(" (*)");

  auto newExecutable = Util::getOpenFileName(this, QY("Select executable"), d->executable, filters.join(Q(";;")));
  newExecutable      = QDir::toNativeSeparators(newExecutable);
  if (newExecutable.isEmpty() || (newExecutable == d->executable))
    return;

  changeArguments([&](QStringList &arguments) {
    if (arguments.isEmpty())
      arguments << newExecutable;
    else
      arguments[0] = newExecutable;
  });

  d->executable = newExecutable;

  enableControls();

  emit executableChanged(newExecutable);
}

void
PrefsRunProgramWidget::commandLineEdited(QString const &commandLine) {
  Q_D(PrefsRunProgramWidget);

  enableControls();

  auto arguments     = Util::unescapeSplit(commandLine, Util::EscapeShellUnix);
  auto newExecutable = arguments.value(0);

  if (d->executable == newExecutable)
    return;

  d->executable = newExecutable;

  emit executableChanged(newExecutable);
}

void
PrefsRunProgramWidget::changeArguments(std::function<void(QStringList &)> const &worker) {
  Q_D(PrefsRunProgramWidget);

  auto arguments = Util::unescapeSplit(d->ui->leCommandLine->text(), Util::EscapeShellUnix);

  worker(arguments);

  d->ui->leCommandLine->setText(Util::escape(arguments, Util::EscapeShellUnix).join(Q(" ")));
}

Util::Settings::RunProgramConfigPtr
PrefsRunProgramWidget::config()
  const {
  Q_D(const PrefsRunProgramWidget);

  auto cfg           = std::make_shared<Util::Settings::RunProgramConfig>();
  auto cmdLine       = d->ui->leCommandLine->text().replace(QRegularExpression{"^\\s+"}, Q(""));
  cfg->m_commandLine = Util::unescapeSplit(cmdLine, Util::EscapeShellUnix);
  cfg->m_active      = d->ui->cbConfigurationActive->isChecked();

  for (auto const &checkBox : d->flagsByCheckbox.keys())
    if (checkBox->isChecked())
      cfg->m_forEvents |= d->flagsByCheckbox[checkBox];

  return cfg;
}

}}
