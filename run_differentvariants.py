import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


#from variableends_prettycode import bayes
#from freeends_prettycode import bayes
from dirichlet import bayes

#file paths
file_path = "../mean_velocities_intensities_139.csv" 
#file_path = "../mean_velocities_intensities_135.csv" 
#file_path = "../mean_velocities_intensities_107.csv" 
#file_path = "../mean_velocities_intensities_096.csv" 
#file_path = "../mean_velocities_intensities_050 (1).csv" #bei dem anders einlesen!!!
#file_path = "../mean_velocities_intensities_007.csv"


df = pd.read_csv(file_path)

meanvx = df.iloc[0].tolist()[2:]
stdvx = df.iloc[1].tolist()[2:]
meanvy = df.iloc[3].tolist()[2:]
stdvy = df.iloc[4].tolist()[2:]
intensity_myosin = df.iloc[6].tolist()[2:]
intensity_std_myosin = df.iloc[7].tolist()[2:]
intensity_actin = df.iloc[9].tolist()[2:]
intensity_std_actin = df.iloc[10].tolist()[2:]

df_new = pd.DataFrame({
    "meanvx": meanvx,
    "stdvx": stdvx,
    "meanvy": meanvy,
    "stdvy": stdvy,
    "intensity_myosin": intensity_myosin,
    "intensity_std_myosin": intensity_std_myosin,
    "intensity_actin": intensity_actin,
    "intensity_std_actin": intensity_std_actin
})

for col in df_new.columns:
    df_new[col] = pd.to_numeric(df_new[col], errors="coerce")


df_new = df_new[
    (df_new['intensity_std_myosin'] != 0) |
    (df_new['intensity_std_actin'] != 0) |
    (df_new['stdvx'] != 0) |
    (df_new['stdvy'] != 0)
]


df_new = df_new.dropna(subset=[
    "meanvx","meanvy","stdvx","stdvy",
    "intensity_myosin","intensity_std_myosin",
    "intensity_actin","intensity_std_actin"
])


n = len(df_new)

cut = int(0.07 * n)
#cut = 0

df_trimmed = df_new.iloc[cut : n - cut]

v_x = df_trimmed["meanvx"].to_numpy()
v_y = df_trimmed["meanvy"].to_numpy()
sem_vx = df_trimmed["stdvx"].to_numpy()
sem_vy = df_trimmed["stdvy"].to_numpy()
intensity_myosin = df_trimmed["intensity_myosin"].to_numpy()
intensity_std_myosin = df_trimmed["intensity_std_myosin"].to_numpy()
intensity_actin = df_trimmed["intensity_actin"].to_numpy()
intensity_std_actin = df_trimmed["intensity_std_actin"].to_numpy()



x_data = np.linspace(0, 1, len(v_x))

plt.figure()
plt.errorbar(x_data, intensity_myosin, yerr=intensity_std_myosin, fmt="o", color="b", ecolor = "b", alpha = 0.1, label="Myosin")
plt.ylabel("Intensity_myosin")
plt.legend()
plt.twinx()
plt.errorbar(x_data, intensity_actin, yerr=intensity_std_actin, fmt="x", color="r", ecolor = "pink", alpha = 0.2, label="Actin")
plt.ylabel("Intensity_actin")
plt.xlabel("x$_{norm}$")
plt.legend(loc = "lower right")

plt.title("Intensity Profile")
plt.savefig(f"../IntensityProfileComparison.png")



bayes(v_x, v_y, sem_vx, sem_vy, intensity_myosin, intensity_std_myosin)