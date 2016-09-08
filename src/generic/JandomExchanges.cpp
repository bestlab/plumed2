/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012-2016 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed.org for more information.

   This file is part of plumed, version 2.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#include "core/Action.h"
#include "core/ActionRegister.h"
#include "core/PlumedMain.h"
#include "tools/Exception.h"
#include "core/ExchangePatterns.h"

using namespace std;

namespace PLMD{
namespace generic{

//+PLUMEDOC GENERIC RANDOM_EXCHANGES
/*
Set random pattern for exchanges.

In this way, exchanges will not be done between replicas with consecutive index, but
will be done using a random pattern.  Typically used in bias exchange \cite piana.

\par Examples

Using the following three input files one can run a bias exchange
metadynamics simulation using a different angle in each replica.
Exchanges will be randomly tried between replicas 0-1, 0-2 and 1-2

Here is plumed.0.dat
\verbatim
RANDOM_EXCHANGES
t: TORSION ATOMS=1,2,3,4
METAD ARG=t HEIGHT=0.1 PACE=100 SIGMA=0.3
\endverbatim

Here is plumed.1.dat
\verbatim
RANDOM_EXCHANGES
t: TORSION ATOMS=2,3,4,5
METAD ARG=t HEIGHT=0.1 PACE=100 SIGMA=0.3
\endverbatim

Here is plumed.2.dat
\verbatim
RANDOM_EXCHANGES
t: TORSION ATOMS=3,4,5,6
METAD ARG=t HEIGHT=0.1 PACE=100 SIGMA=0.3
\endverbatim

\warning Multi replica simulations are presently only working with gromacs.

\warning The directive should appear in input files for every replicas. In case SEED is specified, it
should be the same in all input files.

*/
//+ENDPLUMEDOC

class JandomExchanges:
  public Action
{
  std::vector<int> dimensions;
public:
  static void registerKeywords( Keywords& keys );
  explicit JandomExchanges(const ActionOptions&ao);
  void calculate(){}
  void apply(){}
};

PLUMED_REGISTER_ACTION(JandomExchanges,"JANDOM_EXCHANGES")

void JandomExchanges::registerKeywords( Keywords& keys ){
  Action::registerKeywords(keys);
  keys.add("compulsory","DIMENSIONS","the dimensionality of the replica exchanges");
  keys.add("optional","SEED","seed for random exchanges");
}

JandomExchanges::JandomExchanges(const ActionOptions&ao):
dimensions(2,0.0), // dimensions=(0.0,0.0)
Action(ao)
{
  plumed.getExchangePatterns().setFlag(ExchangePatterns::JANDOM);
  // I convert the seed to -seed because I think it is more general to use a positive seed in input
  int seed=-1;
  parse("SEED",seed);
  if(seed>=0) plumed.getExchangePatterns().setSeed(-seed);

  parseVector("DIMENSIONS",dimensions);
  checkRead();

  // TODO check that dimensions multiple to nrepl/multi

  log.printf(" multi-dimensional replica exchange, with following dimensions:");
  for(unsigned i=0;i<dimensions.size();i++) log.printf(" %d",dimensions[i]);
  log.printf("\nlet's go\n");

  plumed.getExchangePatterns().setDimensions(dimensions);
}

}
}

