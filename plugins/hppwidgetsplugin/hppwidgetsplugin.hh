#ifndef HPPWIDGETSPLUGIN_HH
#define HPPWIDGETSPLUGIN_HH

#include <hpp/gui/plugin-interface.h>
#include <hpp/corbaserver/client.hh>

class SolverWidget;
class PathPlayer;
class JointTreeWidget;
class ConfigurationListWidget;
class JointTreeItem;
class Roadmap;

class HppWidgetsPlugin : public QObject, public PluginInterface,
    public ModelInterface, public CorbaInterface
{
  Q_OBJECT
  Q_INTERFACES (PluginInterface ModelInterface CorbaInterface)

public:
  struct JointElement {
    std::string name;
    std::string bodyName;
    JointTreeItem* item;
    bool updateViewer;

    JointElement ()
      : name (), bodyName (), item (NULL), updateViewer (false) {}
    JointElement (std::string n, std::string bn, JointTreeItem* i, bool updateV = true)
      : name (n), bodyName (bn), item (i), updateViewer (updateV) {}
  };
  typedef QMap <std::string, JointElement> JointMap;
  typedef hpp::corbaServer::Client HppClient;

  explicit HppWidgetsPlugin ();

  virtual ~HppWidgetsPlugin ();

  // PluginInterface interface
public:
  void init();
  QString name() const;

  // ModelInterface interface
public:
  void loadRobotModel (DialogLoadRobot::RobotDefinition rd);
  void loadEnvironmentModel (DialogLoadEnvironment::EnvironmentDefinition ed);
  std::string getBodyFromJoint (const std::string& jointName) const;
signals:
  void configurationValidationStatus (bool valid);
  void configurationValidationStatus (QStringList collision);

  // CorbaInterface interface
public:
  virtual bool corbaException (int jobId, const CORBA::Exception &excep) const;

  virtual void openConnection ();
  virtual void closeConnection ();
signals:
  void logJobFailed  (int id, const QString& text) const;

public slots:
  void applyCurrentConfiguration ();
  void configurationValidation ();
  void selectJointFromBodyName (const std::string& bodyName);

public:
  QList <QAction*> getJointActions (const std::string &jointName);

public:
  HppClient* client () const;
  JointMap& jointMap ();

  virtual void updateRobotJoints (const QString robotName);
  std::string getSelectedJoint ();
  virtual Roadmap* createRoadmap (const std::string& jointName);

protected slots:
  virtual void displayRoadmap (const std::string& jointName);
  void showHideJointFrame (const std::string& jointName);

private:
  void computeObjectPosition();

  PathPlayer* pathPlayer_;
  SolverWidget* solverWidget_;
  JointTreeWidget* jointTreeWidget_;
  ConfigurationListWidget* configListWidget_;
  QList <QDockWidget*> dockWidgets_;

  HppClient* hpp_;

protected:
  JointMap jointMap_;
  std::list <std::string> jointFrames_;
};

#endif // HPPWIDGETSPLUGIN_HH
