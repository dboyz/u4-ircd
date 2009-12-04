/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         m_null.cpp
 * Description  A dummy module for development purposes
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 ******************************************************************/

#include "modules.h"

class m_null
  : Module()
{
public:
  void run();

protected:
  void initialize(Conf&) throw(std::runtime_error);
  bool cleanup();

private:
}

UNREALIRCD_MODULE(null)

void
m_null::run()
{
  std::cerr << "null module loaded" << std::endl;
}

void
m_null::initialize(Conf& conf) throw(std::runtime_error)
{
  std::cerr << "null module initialized" << std::endl;
}

bool
m_null::cleanup()
{
  std::cerr << "null module cleanup()ed" << std::endl;

  return true;
}
