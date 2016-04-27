/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2013-2016 The plumed team
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
#include "IntermolecularDRMSD.h"
#include "MetricRegister.h"

namespace PLMD {

PLUMED_REGISTER_METRIC(IntermolecularDRMSD,"INTER-DRMSD")

IntermolecularDRMSD::IntermolecularDRMSD( const ReferenceConfigurationOptions& ro ):
ReferenceConfiguration( ro ),
DRMSD( ro )
{
}

void IntermolecularDRMSD::read( const PDB& pdb ){
  //DRMSD::read( pdb );
  readAtomsFromPDB( pdb, false );
  parseFlag("NOPBC",nopbc);
  parse("LOWER_CUTOFF",lower,true);
  parse("UPPER_CUTOFF",upper,true);
  setBoundsOnDistances( !nopbc, lower, upper );

  nblocks = pdb.getNumberOfAtomBlocks() ;
  printf("nblocks=%d\n", nblocks);
  blocks.resize( nblocks );
  if( nblocks==1 ) error("Trying to compute intermolecular rmsd but found no TERs in input PDB");
  blocks[0]=0; for(unsigned i=0;i<nblocks;++i) blocks[i+1]=pdb.getAtomBlockEnds()[i];
  printf("blocks([0]=%d [1]=%d [2]=%d)\n", blocks[0], blocks[1], blocks[2]);
  setup_targets();
}

void IntermolecularDRMSD::setup_targets(){
  plumed_massert( bounds_were_set, "I am missing a call to DRMSD::setBoundsOnDistances");
  unsigned natoms = getNumberOfReferencePositions();
  printf("IntermolecularDRMSD::setup_targets natoms=%d nblocks=%d targets=%d\n",  natoms, nblocks, targets.size());
  for(unsigned i=0;i<nblocks;++i){
      for(unsigned j=i+1;j<nblocks;++j){
          for(unsigned iatom=blocks[i];iatom<blocks[i+1];++iatom){
              for(unsigned jatom=blocks[j];jatom<blocks[j+1];++jatom){
                  double distance = delta( getReferencePosition(iatom), getReferencePosition(jatom) ).modulo();
                  if(! (distance < upper && distance > lower )) continue;
                  printf("iatom=%d jatom=%d d=%.3f\n", iatom, jatom, distance);
                  targets[std::make_pair(iatom,jatom)] = distance;
              }
          }
      }
  }
  if( targets.empty() ) error("drmsd will compare no distances - check upper and lower bounds are sensible");
  printf("IntermolecularDRMSD::setup_targets targets=%d\n", targets.size());
}

}
