import numpy as np
import matplotlib.pyplot as plt
from phasic_oop import GSystem
import multiprocessing as mp
from time import time

WIDTH = 100
HEIGHT = 100
PROPORTION = 0.15

METROPOLIS_TRIALS = 1_000_000
METROPOLIS_WARMUP = 250_000
ENERGY_RECORD_INTERVAL = 25_000

start_time = time()

def sim_metro_g(temp):
    g = GSystem(WIDTH, HEIGHT, PROPORTION, temp)

    for i in range(METROPOLIS_TRIALS):
        if i % 100_000 == 0:
            print(f'[{time() - start_time:.1f}] T = {temp:.2f} :: {100 * i / METROPOLIS_TRIALS:.1f}%')

        g.metropolis_step()

    print(f'[{time() - start_time:.1f}] T = {temp:.2f} :: complete')
    return g

def sim_metro_energy(temp):
    g = GSystem(WIDTH, HEIGHT, PROPORTION, temp)
    energy_hist = []

    for i in range(METROPOLIS_TRIALS):
        if i % 150_000 == 0:
            # print(f'[{time() - start_time:.1f}] T = {temp:.2f} :: {i} ({100 * i / METROPOLIS_TRIALS:.1f}%)')
            print(f'[{time() - start_time:.1f}] T = {temp:.2f} :: {100 * i / METROPOLIS_TRIALS:.1f}%')

        g.metropolis_step()
        if i >= METROPOLIS_WARMUP and i % ENERGY_RECORD_INTERVAL == 0:
            energy_hist.append(-g.count_contacts())

    print(f'[{time() - start_time:.1f}] T = {temp:.2f} :: complete')
    return np.mean(energy_hist)

# sim_temps = [0.05 * i for i in range(1, 35)]
sim_temps = [0.2, 0.53, 1.2]
with mp.Pool(3) as pool:
    gs = pool.map(sim_metro_g, sim_temps)

for g in gs:
    plt.figure()
    plt.imshow(g.get_image_array())
plt.show()

# plt.plot(sim_temps, energies)
# plt.grid()
# plt.show()
