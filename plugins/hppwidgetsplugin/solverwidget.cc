#include "hppwidgetsplugin/solverwidget.hh"
#include "hppwidgetsplugin/ui_solverwidget.h"

#include <hpp/corbaserver/client.hh>

#include <QFormLayout>
#include <QMessageBox>
#include "hpp/gui/mainwindow.hh"
#include "hpp/gui/windows-manager.hh"

#include "hppwidgetsplugin/roadmap.hh"

namespace hpp {
  namespace gui {
    namespace {
      void clearQComboBox (QComboBox* c) {
        while (c->count() > 0) c->removeItem(0);
      }
    }

    SolverWidget::SolverWidget (HppWidgetsPlugin *plugin, QWidget *parent) :
      QWidget (parent),
      ui_ (new ::Ui::SolverWidget),
      plugin_ (plugin),
      main_(MainWindow::instance()),
      solveDoneId_ (-1),
      solveAndDisplay_ (plugin, this)
    {
      ui_->setupUi (this);
      selectButtonSolve(true);
      connect (&main_->worker(), SIGNAL (done(int)), this, SLOT (handleWorkerDone (int)));

      connect(planner(), SIGNAL (activated(const QString&)), this, SLOT (selectPathPlanner(const QString&)));
      connect(ui_->pathOptimizerButton, SIGNAL (clicked()), this, SLOT (openPathOptimizerSelector()));
      connect(projector(), SIGNAL (activated(int)), this, SLOT (selectPathProjector(int)));
      connect(ui_->pushButtonSolve, SIGNAL (clicked ()), this, SLOT (solve ()));
      connect(ui_->pushButtonInterrupt, SIGNAL (clicked ()), this, SLOT (interrupt ()));
      connect(ui_->pushButtonSolveAndDisplay, SIGNAL (clicked ()),
          SLOT (solveAndDisplay ()));
      connect(&solveAndDisplay_.watcher, SIGNAL (finished()),
          SLOT(solveAndDisplayDone ()));
      connect(ui_->loadRoadmap, SIGNAL (clicked()), SLOT (loadRoadmap()));
      connect(ui_->saveRoadmap, SIGNAL (clicked()), SLOT (saveRoadmap()));
    }

    SolverWidget::~SolverWidget()
    {
      delete ui_;
    }

    void SolverWidget::update (Select s) {
      hpp::Names_t_var names;
      switch (s) {
        case All:
        case Planner:
          clearQComboBox(planner());
          names = plugin_->client()->problem()->getAvailable("PathPlanner");
          for (CORBA::ULong i = 0; i < names->length(); ++i)
            planner()->addItem(QString::fromLocal8Bit(names[i]));
          if (s == Planner) break;
        case Optimizer:
          names = plugin_->client()->problem()->getAvailable("PathOptimizer");
          optimizers_.clear();
          for (CORBA::ULong i = 0; i < names->length(); ++i)
            optimizers_ << QString::fromLocal8Bit(names[i]);
          if (s == Optimizer) break;
        case Projector:
          clearQComboBox(projector());
          names = plugin_->client()->problem()->getAvailable("PathProjector");
          for (CORBA::ULong i = 0; i < names->length(); ++i)
            projector()->addItem(QString::fromLocal8Bit(names[i]), QVariant (0.2));
          if (s == Projector) break;
      }
    }

    void SolverWidget::selectPathPlanner (const QString& text) {
      plugin_->client()->problem()->selectPathPlanner (text.toStdString().c_str());
    }

    void SolverWidget::selectPathOptimizers (const QStringList& list) {
      plugin_->client()->problem()->clearPathOptimizers();
      foreach(QString s, list)
        plugin_->client()->problem()->addPathOptimizer (s.toStdString().c_str());
    }

    void SolverWidget::selectPathProjector (int index) {
      plugin_->client()->problem()->selectPathProjector (
          projector()->itemText(index).toStdString().c_str(),
          projector()->itemData(index).toDouble());
    }

    void SolverWidget::openPathOptimizerSelector ()
    {
      QDialog dialog(this);
      // Use a layout allowing to have a label next to each field
      QVBoxLayout lay(&dialog);

      // Add some text above the fields
      lay.addWidget(new QLabel("Select path optimizers:"));

      // Add the lineEdits with their respective labels
      QListWidget* list = new QListWidget (&dialog);
      list->addItems(optimizers_);
      list->setSelectionMode(QAbstractItemView::ExtendedSelection);
      lay.addWidget(list);

      QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
          Qt::Horizontal, &dialog);
      lay.addWidget(&buttonBox);
      QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
      QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

      // Show the dialog as modal
      if (dialog.exec() == QDialog::Accepted) {
          // If the user didn't dismiss the dialog, do something with the fields
          QStringList selected;
          foreach (QListWidgetItem* item, list->selectedItems())
              selected << item->text();
          qDebug () << "Selected path optimizers: " << selected;
          selectPathOptimizers(selected);
        }
    }

    void SolverWidget::solve()
    {
      /* double time = */
      WorkItem* item = new WorkItem_0 <hpp::corbaserver::_objref_Problem, hpp::intSeq*>
        (plugin_->client()->problem().in(), &hpp::corbaserver::_objref_Problem::solve);
      main_->emitSendToBackground(item);
      main_->logJobStarted(item->id(), "solve problem.");
      solveDoneId_ = item->id();
      selectButtonSolve(false);
    }

    void SolverWidget::solveAndDisplay()
    {
      solveAndDisplay_.interrupt = false;
      solveAndDisplay_.status = QtConcurrent::run (&solveAndDisplay_, &SolveAndDisplay::solve);
      solveAndDisplay_.watcher.setFuture (solveAndDisplay_.status);
      selectButtonSolve (false);
    }

    void SolverWidget::solveAndDisplayDone()
    {
      qDebug () << "Step by step done";
      selectButtonSolve (true);
      if (solveAndDisplay_.isSolved) {
        emit problemSolved ();
        QMessageBox::information(this, "Problem solver", "Problem is solved.");
      }
    }

    void SolverWidget::interrupt()
    {
      if (solveAndDisplay_.status.isRunning ()) {
        solveAndDisplay_.interrupt = true;
        solveAndDisplay_.status.waitForFinished ();
      } else {
        plugin_->client()->problem()->interruptPathPlanning();
      }
      selectButtonSolve(true);
    }

    void SolverWidget::loadRoadmap()
    {
      QString file = QFileDialog::getOpenFileName(this, tr ("Select a roadmap file"));
      if (file.isNull()) return;
      try {
        plugin_->client()->problem()->readRoadmap (file.toLocal8Bit().data());
      } catch (const hpp::Error& e) {
        MainWindow::instance()->logError(QString::fromLocal8Bit(e.msg));
      }
    }

    void SolverWidget::saveRoadmap()
    {
      QString file = QFileDialog::getSaveFileName(this, tr("Select a destination"));
      if (file.isNull()) return;
      try {
        plugin_->client()->problem()->saveRoadmap (file.toLocal8Bit().data());
      } catch (const hpp::Error& e) {
        MainWindow::instance()->logError(QString::fromLocal8Bit(e.msg));
      }
    }

    void SolverWidget::handleWorkerDone(int id)
    {
      if (id == solveDoneId_) {
        emit problemSolved();
        selectButtonSolve(true);
        QMessageBox::information(this, "Problem solver", "Problem is solved.");
        main_->logJobDone(id, "Problem solved.");
      }
    }

    void SolverWidget::SolveAndDisplay::solve()
    {
      isSolved = false;
      HppWidgetsPlugin::HppClient* hpp = plugin->client();
      std::string jn = plugin->getSelectedJoint();
      if (jn.empty()) {
        qDebug () << "Please, select a joint in the joint tree window.";
        return;
      }
      isSolved = hpp->problem()->prepareSolveStepByStep();
      Roadmap* r = plugin->createRoadmap(plugin->getSelectedJoint());
      while (!isSolved) {
        isSolved = hpp->problem()->executeOneStep();
        r->displayRoadmap();
        if (interrupt) break;
      }
      if (isSolved)
        hpp->problem()->finishSolveStepByStep();
      delete r;
    }

    void SolverWidget::selectButtonSolve(bool solve)
    {
      if (solve) {
        ui_->pushButtonInterrupt->setVisible(false);
        ui_->pushButtonSolve->setVisible(true);
        ui_->pushButtonSolveAndDisplay->setVisible(true);
      } else {
        ui_->pushButtonInterrupt->setVisible(true);
        ui_->pushButtonSolve->setVisible(false);
        ui_->pushButtonSolveAndDisplay->setVisible(false);
      }
    }

    QComboBox *SolverWidget::planner()
    {
      return ui_->pathPlannerComboBox;
    }

    QComboBox *SolverWidget::projector()
    {
      return ui_->pathProjectorComboBox;
    }
  } // namespace gui
} // namespace hpp
