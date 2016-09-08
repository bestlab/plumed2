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
#include "ExchangePatterns.h"
#include "tools/Random.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

namespace PLMD{

ExchangePatterns::ExchangePatterns():
  Dimension(-1),
  dimensions(2,0), // initialize vector of lenght 2, filled with zeros (0,0)
  PatternFlag(NONE),
  NumberOfReplicas(1),
  random(*new Random)
{}

ExchangePatterns::~ExchangePatterns()
{
  delete &random;
}

void ExchangePatterns::setNofR(const int nrepl){
  NumberOfReplicas=nrepl;
}

void ExchangePatterns::setFlag(const int flag){
  PatternFlag=flag;
}

void ExchangePatterns::getFlag(int &flag){
  flag=PatternFlag;
}

void ExchangePatterns::setSeed(const int seed)
{
  random.setSeed(seed);
}

void ExchangePatterns::setDimensions(std::vector<int>&dims){
  dimensions[0]=dims[0];
  dimensions[1]=dims[1];
}

void ExchangePatterns::getList(int *ind)
{
  switch(PatternFlag)
  {
    case RANDOM:
      for(int i=0;i<NumberOfReplicas;i++) {
        int stat=1;
        while(stat) {
          stat=0;
          ind[i] = (int) (random.U01()*NumberOfReplicas);
          for(int j=0;j<i;j++) if(ind[i]==ind[j]) stat=1;
        }
      }
      break;
    case JANDOM:
      // TODO check if dimension[0]*dimension[1] == NumberOfRelpicas
      // TODO relpace by case switch
      // TODO this code is hard to reason about
      // std::cout << "\nJANDOM! ";
      if (Dimension<0) {
          // std::cout << "dimension=0 " << dimensions[0] << "x" << dimensions[1] < " " ;
          for(int r=0;r<dimensions[0];r++) {
              for(int c=0;c<dimensions[1];c++) {
                  int i = r*dimensions[1]+c;
                  int j = r % 2 ? (r+1)*dimensions[1]-(c+1) : r*dimensions[1]+c;
                  //printf("\nloop c=%d r=%d i=%d j=%d", c, r, i, j);
                  ind[i] = j;
              }
          }

          std::string indicies; ostringstream convert;
          for(int i=0;i<NumberOfReplicas;i++) convert << ind[i] << " ";
          indicies = convert.str();
          printf("Replica Dimension=%2d ind=%s\n", Dimension, indicies.c_str());
          //std::cout << "Replica Dimension=" << Dimension << ", ind= " ; for(int i=0;i<NumberOfReplicas;i++)  std::cout << ind[i] << ", "; std::cout << std::endl;
          Dimension--;
          if (Dimension < -2) Dimension = 1;
          break;
      }

      if (Dimension>0) {
          // std::cout << "dimension=1 " << dimensions[0] << "x" << dimensions[1] << " " ;

          for(int c=0;c<dimensions[1];c++) {
              for(int r=0;r<dimensions[0];r++) {
                  int i = c*dimensions[0]+r;
                  int j = c % 2 ? dimensions[1]*(dimensions[0]-r) - (dimensions[1]-c) : r*dimensions[1]+c;
                  //printf("\nloop c=%d r=%d i=%d j=%d", c, r, i, j);
                  ind[i] = j;
              }
          }
          std::string indicies; ostringstream convert;
          for(int i=0;i<NumberOfReplicas;i++) convert << ind[i] << " ";
          indicies = convert.str();
          printf("Replica Dimension=%2d ind=%s\n", Dimension, indicies.c_str());
          //std::cout << "Replica Dimension=" << Dimension << ", ind= " ; for(int i=0;i<NumberOfReplicas;i++)  std::cout << ind[i] << ", " ; std::cout << std::endl;
          Dimension++;
          if (Dimension > 2) Dimension = -1;
          break;
      }
      // TODO add a general, multidimensional relpex scheme
      // do NEIGHBOR exchange only, easy
      //for(int i=0;i<NumberOfReplicas;i++) ind[i]=i;
      break;
    case NEIGHBOR:
      for(int i=0;i<NumberOfReplicas;i++) ind[i]=i;
      break;
  }

}

}
