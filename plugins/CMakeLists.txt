#
# Copyright (c) 2015 CNRS
# Authors: Joseph Mirabel
#
#
# This file is part of hpp-gui
# hpp-gui is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# hpp-gui is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# hpp-gui  If not, see
# <http://www.gnu.org/licenses/>.

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#ADD_SUBDIRECTORY(plugintest)
IF (HPP_CORBASERVER_FOUND)
  IF (REMOTEIMU_FOUND)
    ADD_SUBDIRECTORY(remoteimuplugin)
  ENDIF (REMOTEIMU_FOUND)
  ADD_SUBDIRECTORY(hppcorbaserverplugin)
  ADD_SUBDIRECTORY(hppwidgetsplugin)

  IF (HPP_MANIPULATION_CORBA_FOUND)
    ADD_SUBDIRECTORY(hppmanipulationwidgetsplugin)
  ENDIF (HPP_MANIPULATION_CORBA_FOUND)
ENDIF (HPP_CORBASERVER_FOUND)
