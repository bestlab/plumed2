import numpy as np

template = """

#RESTART
JANDOM_EXCHANGES DIMENSIONS=2,2

phi: TORSION ATOMS=5,7,9,15
psi: TORSION ATOMS=7,9,15,17

#restraint: RESTRAINT ARG=phi AT={} KAPPA=0.0 # 2.479

PRINT STRIDE=1000 ARG=* FILE=COLVAR

"""

phi_umbrellas = np.linspace(-1.5, 1.5, 2)
psi_umbrellas = np.linspace(-1.5, 1.5, 2)
umbrellas = []
for u1 in phi_umbrellas:
	for u2 in psi_umbrellas:
		umbrellas.append((u1, u2))

umbrellas = [
	-1.5, -0.5, 0.5, 1.5,
	-1.5, -0.5, 0.5, 1.5,
	-1.5, -0.5, 0.5, 1.5,
]

for i in range(4*3):
	open("plumed.{}.dat".format(i), "w").write(template.format(umbrellas[i]))
