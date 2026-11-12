# Estimation of the Hydrodynamic Length for different C.elegans variants

Code developed for my Bachelor's thesis in Technical Physics at TU Wien. This repository contains the numerical scripts used for statistical parameter estimation, and visualization across different boundary conditions. (Note: The data was gathered by a French research team and is confidential. Therefore, only the code is provided.)

## Repository Structure
- `dirichlet.py` – Simulation and Bayesian inference under Dirichlet boundary conditions.
- `freeends.py` – Modeling pipeline for systems with free boundary conditions. Relies on fitting the theoretical values to the  experimental ones.
- `variableends.py` – Generalised model implementation allowing variable boundary constraints, incorporating the measured mean and uncertainties.
- `run_differentvariants.py` – Execution script for running different boundary conditions and generating comparative plots

## Key Skills Demonstrated
- **Statistical Modeling:** Bayesian parameter estimation and uncertainty quantification.
- **Scientific Computing:** Numerical simulation and structured data processing in Python.
- **Visualisation:** Automated generation of figures comparing parameter estimation with different boundary conditions.
