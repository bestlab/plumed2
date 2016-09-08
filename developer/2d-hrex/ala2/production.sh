#!/usr/bin/env bash

if [ ! -d tprs ]; then mkdir tprs; fi

source ~/Programs/gromacs/gromacs-4.6.7/bin/GMXRC

rm top/*
for i in {0..3}; do
lambda=1.0
~/Programs/plumed/2.2-hrex-wenwei/bin/plumed partial_tempering 1.0 < processed.top > top/topol${i}.top
awk -v f=$lambda '
BEGIN {cmaptypes=0; pairtypes=0}
{
if (/\[ /) {cmaptypes=0; pairtypes=0};
if (/\[ pairtypes \]/) {pairtypes=1};
if (/\[ cmaptypes \]/) {cmaptypes=1};
if (pairtypes==1) {
sub(/;/, "; ", $0);
print $0;
if ($1!=";" && $1!="[" && NF>0) printf "%s_\t%s_\t%d\t%.12lf  %.12lf    ; scaled \n",$1,$2,$3,$4,$5*f;}
else if (cmaptypes==1) {
sub(/\\/, "", $0);
if (NF==8) printf "%s %s %s %s %s %s %s %s\\\n",$1,$2,$3,$4,$5,$6,$7,$8;
else if (NF==10) printf "%.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf\\\n",$1*f,$2*f,$3*f,$4*f,$5*f,$6*f,$7*f,$8*f,$9*f,$10*f;
else if (NF==6) printf "%.9lf %.9lf %.9lf %.9lf %.9lf %.9lf\n",$1*f,$2*f,$3*f,$4*f,$5*f,$6*f;
else print $0;}
else {print $0;}
}
' < top/topol${i}.top > top/topol_cmap${i}.top
done


rm tpr/*
for i in {0..3}; do
    grompp -f grompp.mdp -p top/topol_cmap${i}.top -o tpr/topol${i}.tpr -maxwarn 1
done
rm \#*


python plumed.py

# this works
mpirun -np 4 /sansom/s103/domanski/Programs/gromacs/gromacs-4.6.7/bin/mdrun_mpi -multi 4 -replex 5 -s tpr/topol.tpr -plumed -hrex
grep -A10 "Replica exchange statistics" md0.log 

# this doesn't
mpirun -np 4 /sansom/s103/domanski/Programs/gromacs/gromacs-4.6.7/bin/mdrun_mpi -multi 4 -replex 5 -s tpr/topol.tpr -plumed -hrex
grep -A10 "Replica exchange statistics" md0.log


rm \#* traj_comp* state* md*.log ener* confout* COLVAR.* bck.* step*.pdb

