#ifndef WINDOWSMANAGER_H
#define WINDOWSMANAGER_H

#include <hpp/gui/fwd.h>
#include <gepetto/viewer/corba/windows-manager.h>

class WindowsManager : public graphics::WindowsManager
{
public:
  typedef graphics::WindowsManager Parent_t;

  static WindowsManagerPtr_t create ();

  WindowID createWindow(const char *windowNameCorba);
  WindowID createWindow(const char *windowNameCorba, osg::GraphicsContext *gc);

protected:
  WindowsManager ();
};

#endif // WINDOWSMANAGER_H
