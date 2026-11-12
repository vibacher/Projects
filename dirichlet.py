import numpy as np
from scipy.interpolate import interp1d
import scipy.integrate as integrate
import matplotlib.pyplot as plt
import pymc as pm
import arviz as az
import pytensor.tensor as at


def bayes(v_x, v_y, sem_vx, sem_vy, intensity, sem_intensity):

    #Reduce amount of data points to n
    n = 100
    x_data = np.linspace(0.07, 0.93, len(v_x))
    
    v_x_fit = interp1d(x_data, v_x)
    vx_sem_fit = interp1d(x_data, sem_vx)
    
    v_y_fit = interp1d(x_data, v_y)
    vy_sem_fit = interp1d(x_data, sem_vy)
    
    int_fit = interp1d(x_data, intensity)
    int_sem_fit = interp1d(x_data, sem_intensity)
    
    x_data = np.linspace(0.07, 0.93, n)
    
    
    v_x = v_x_fit(x_data)
    sem_vx = vx_sem_fit(x_data)
    
    v_y = v_y_fit(x_data)
    sem_vy = vy_sem_fit(x_data)
    
    intensity = int_fit(x_data)
    sem_intensity = int_sem_fit(x_data)
    
    #plot the intensity profile
    plt.figure()
    plt.errorbar(x_data, intensity, yerr=sem_intensity, fmt="o", color="b", ecolor = "orchid", alpha = 0.5, capsize=5, label="Experiment")
    plt.xlabel("x$_{norm}$")
    plt.ylabel("Intensity")
    plt.title("Intensity Profile")
    plt.savefig(f"../IntensityProfile.png")
    
    #set prior for hydrodynamic length and friction
    l_prior = 0.3
    gamma_prior = 1e4
    
    #calculate the prior for alpha
    alpha_prior=[]
    for i in range(len(intensity)): 
        alpha_prior.append(- gamma_prior * integrate.cumulative_trapezoid(v_x, x_data, initial=0)[i]/intensity[i]) 
    alpha_prior = np.array(alpha_prior) 
    alpha_prior = alpha_prior.mean()
    
    #calculate the prior for the chirality index
    c_prior = []
    for i in range(len(intensity)):
        c_prior.append(v_y[i]/v_x[i])
    c_prior = np.array(c_prior)
    c_prior = c_prior.mean()

    #define one step
    dx = x_data[1] - x_data[0]
    
    #discrete first derivative
    def first_derivative_matrix(n=len(v_x)):
        F = np.zeros((n,n))
        F[0, 0:3] = [-3/2, 2, -1/2]
        F[-1, -3:] = [-1/2, 2, -3/2]
        for i in range(1,n-1):
            F[i, i-1:i+2] = [1, 0, -1]
        return F/dx
    
    #discrete second derivative
    def second_derivative_matrix(n = len(v_x)): 
        D = np.zeros((n,n)) 
        D[0, 0:4] = [2, -5, 4, -1] 
        D[-1, -4:] = [-1, 4, -5, 2] 
        for i in range(1, n-1): 
            D[i, i-1:i+2] = [1, -2, 1] 
        return D/dx**2

    D_np = second_derivative_matrix()
    F_np = first_derivative_matrix()
    I_np = np.eye(len(v_x))

    #convert into tensor
    D = at.as_tensor_variable(D_np)
    F = at.as_tensor_variable(F_np)
    I = at.as_tensor_variable(I_np)
   
    

    with pm.Model() as model:
        #set fitting parameters
        l = pm.LogNormal("l", np.log(l_prior), 0.25)
        
        gamma = pm.LogNormal("gamma", np.log(gamma_prior), sigma=1.5)
        
        alpha = pm.Normal("alpha", mu = alpha_prior, sigma = 23.0)
        c = pm.Normal("c", mu = c_prior, sigma = 23.0)
        
        intensity = pm.Normal("intensity", mu = intensity, sigma = sem_intensity, shape = len(intensity))

        
        #define the matrix M for the linear system
        M = l ** 2* D -  I
        
        #calculating the derivative of the intensity
        dI = F @ intensity
        
        
        #boundary conditions
        M_mod = M.copy()

        M_mod = at.set_subtensor(M_mod[0, :], 0)
        M_mod = at.set_subtensor(M_mod[0, 0], 1)
        
        M_mod = at.set_subtensor(M_mod[-1, :], 0)
        M_mod = at.set_subtensor(M_mod[-1, -1], 1)

        
        b = alpha / gamma * dI
        
      
        b_mod = b.copy()
        b_mod = at.set_subtensor(b_mod[0], v_x[0])
        b_mod = at.set_subtensor(b_mod[-1], v_x[-1])

        #x-direction
        mu_vx = pm.Deterministic("mu_vx", at.slinalg.solve(M_mod, b_mod)) 
       
        vx = pm.Normal("vx", mu=mu_vx, sigma=sem_vx, observed=v_x)
        
        #boundary conditions
        L = l** 2* D - 2  * I
        
    
        L_mod = L.copy()

        L_mod = at.set_subtensor(L_mod[0, :], 0)
        L_mod = at.set_subtensor(L_mod[0, 0], 1)
        
        L_mod = at.set_subtensor(L_mod[-1, :], 0)
        L_mod = at.set_subtensor(L_mod[-1, -1], 1)

        
        d = 2 * c * alpha / gamma * dI
        
        
        d_mod = d.copy()
        d_mod = at.set_subtensor(d_mod[0], v_y[0])
        d_mod = at.set_subtensor(d_mod[-1], v_y[-1])

        #y-direction
        mu_vy = pm.Deterministic("mu_vy", at.slinalg.solve(L_mod, d_mod))
        vy = pm.Normal("vy", mu=mu_vy, sigma=sem_vy, observed=v_y)
        trace = pm.sample(1500, tune=1000, target_accept=0.95, cores=4, return_inferencedata=True)

    summary = az.summary(trace, var_names=["alpha", "c", "l", "mu_vx", "mu_vy"])
    print(summary)


    az.plot_posterior(trace, var_names = ["alpha", "c", "l"])
    plt.savefig("../posterior_plot.png")
    
    az.plot_pair(trace, var_names = ["alpha", "c", "l"], kind = ["scatter", "kde"], kde_kwargs={"fill_last": False},
    marginals=True, point_estimate="median")
    plt.savefig("../pairposterior.png")

    ppc = pm.sample_posterior_predictive(trace, model=model)

    y_ppc_x = ppc.posterior_predictive["vx"].values
    y_ppc_y = ppc.posterior_predictive["vy"].values
    
    ppc_mean_x = y_ppc_x.mean(axis=(0, 1))
    ppc_hdi_x  = az.hdi(y_ppc_x, hdi_prob=0.68)
    
    ppc_mean_y = y_ppc_y.mean(axis=(0, 1))
    ppc_hdi_y  = az.hdi(y_ppc_y, hdi_prob=0.68)    

    def plot_theory_vs_data(x, data, sem, ppc_mean, ppc_hdi, label):
        plt.figure(figsize=(10, 6))
        
        plt.fill_between(x, ppc_hdi[:, 0], ppc_hdi[:, 1], alpha=0.3, label="94% Posterior Predictive Band")
        
        plt.errorbar(x, data, yerr=sem, fmt="o", color="r", ecolor = "orchid", capsize=5, alpha = 0.5, label="Experiment")
        plt.plot(x, ppc_mean, lw=2, label="Posterior Predictive Mean")
        
        
        plt.xlabel("x")
        plt.ylabel(label)
        plt.title(f"Theory vs Data ({label})")
        
        plt.legend()
       

    plot_theory_vs_data(x=x_data, data= v_x, sem=sem_vx, ppc_mean=ppc_mean_x, ppc_hdi=ppc_hdi_x, label="v$_x$")
    plt.savefig(f"../vx.png")
    plot_theory_vs_data(x=x_data, data= v_y, sem=sem_vy, ppc_mean=ppc_mean_y, ppc_hdi=ppc_hdi_y, label="v$_y$")
    plt.savefig(f"../vy.png")
