import numpy as np
from random import shuffle

rng = np.random.default_rng()

class GSystem:
    def __init__(self, width, height, proportion, temperature):
        self.width = width
        self.height = height
        self.temperature = temperature

        total_cells = width * height
        filled_cells = int(proportion * total_cells)

        self.cell_states = [True] * filled_cells + [False] * (total_cells - filled_cells)
        shuffle(self.cell_states)

        self.filled_idxs = []
        self.blank_idxs = []

        for i in range(total_cells):
            (self.filled_idxs if self.cell_states[i] else self.blank_idxs).append(i)

    def is_filled(self, i, j):
        return 0 <= i < self.width and 0 <= j < self.height \
            and self.cell_states[i + j * self.width]

    def get_adjacent(self, i, j):
        return self.is_filled(i - 1, j) + self.is_filled(i + 1, j) \
            + self.is_filled(i, j - 1) + self.is_filled(i, j + 1)

    def get_adjacent_i(self, idx):
        i = idx % self.width
        j = idx // self.width
        return self.get_adjacent(i, j)

    def count_contacts(self):
        count = 0
        for j in range(self.height):
            for i in range(self.width):
                if not self.is_filled(i, j):
                    continue

                count += self.get_adjacent(i, j)

        return count // 2

    def get_image_array(self):
        return np.array(self.cell_states).reshape((self.height, self.width))

    def metropolis_step(self):
        swap_fli = rng.choice(len(self.filled_idxs))
        swap_bli = rng.choice(len(self.blank_idxs))

        swap_fi = self.filled_idxs[swap_fli]
        swap_bi = self.blank_idxs[swap_bli]

        energy_change = self.get_adjacent_i(swap_fi) - self.get_adjacent_i(swap_bi)
        diff = np.abs(swap_fi - swap_bi)
        if diff == 1 or diff == self.width:
            energy_change += 1

        acceptance = np.exp(-energy_change / self.temperature)

        if rng.uniform() > acceptance:
            return 0

        self.filled_idxs[swap_fli], self.blank_idxs[swap_bli] = \
            self.blank_idxs[swap_bli], self.filled_idxs[swap_fli]
        self.cell_states[swap_fi] = False
        self.cell_states[swap_bi] = True

        return energy_change
