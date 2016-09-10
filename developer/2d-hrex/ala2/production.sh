#!/usr/bin/env bash

if [ ! -d tprs ]; then mkdir tprs; fi

source ~/Programs/gromacs/gromacs-4.6.7/bin/GMXRC

rm top/*
lambdas=(1.0 1.0 1.0 1.0 0.9 0.9 0.9 0.9 0.8 0.8 0.8 0.8)
for i in {0..11}; do
lambda=${lambdas[i]}
lambda=1.0
~/Programs/plumed/2.2-hrex-wenwei/bin/plumed partial_tempering $lambda < processed.top > top/topol${i}.top
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
for i in {0..11}; do
    grompp -f grompp.mdp -p top/topol_cmap${i}.top -o tpr/topol${i}.tpr -maxwarn 1
done
rm \#*

python plumed.py

mpirun -np 4 /sansom/s103/domanski/Programs/gromacs/gromacs-5.1.2_plumed2.2-hrex-wenwei/bin/mdrun_mpi -multi 4 -replex 10 -nsteps 1000 -s tpr/topol.tpr -plumed -hrex

grep -A10 "Replica exchange statistics" md0.log

rm \#* traj_comp* state* md*.log ener* confout* COLVAR.* bck.* step*.pdb

qsubmit.py --jobname "test" --cluster archer --partition standard --nodes 1 --script=submit.sh --duration 24:00:00
