// Copyright (c) 2015, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-gui.
// hpp-gui is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-gui is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-gui. If not, see <http://www.gnu.org/licenses/>.

#include <hpp/gui/settings.hh>

std::ostream& Settings::print (std::ostream& os) {
  return os
    <<   "Configuration file:     \t" << configurationFile
    << "\nPredefined robots:      \t" << predifinedRobotConf
    << "\nPredefined environments:\t" << predifinedEnvConf
    << "\nVerbose:                \t" << verbose
    << "\nNo plugin:              \t" << noPlugin
    ;
}
