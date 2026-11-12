# Hydrodynamic Length – Bayesian Model Analysis & Visualization
The data was gathered by a French research team and is confidential. Therefore, only the code is provided.
Code developed for my Bachelor's thesis in Technical Physics at TU Wien. This repository contains the numerical scripts used for statistical parameter estimation, and visualization across different boundary conditions.

## Repository Structure
- `dirichlet.py` – Simulation and Bayesian inference under Dirichlet boundary conditions.
- `freeends.py` – Modeling pipeline for systems with free boundary conditions. Relies on fitting the theoretical values to the  experimental ones.
- `variableends.py` – Generalised model implementation allowing variable boundary constraints. In this context the measured value with the measured error.
- `run_differentvariants.py` – Execution script

## Key Skills Demonstrated
- **Statistical Modeling:** Bayesian parameter estimation and uncertainty quantification.
- **Scientific Computing:** Numerical simulation and structured data processing in Python.
- **Visualization:** Automated generation of figures comparing parameter estimation with different boundary conditions.
