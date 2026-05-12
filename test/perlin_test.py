import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

data_file = 'perlin_noise.txt'

data = []
with open(data_file, 'r') as f:
    for line in f:
        values = [float(x) for x in line.strip().split()]
        if values:
            data.append(values)

data_array = np.array(data)

rows, cols = data_array.shape
print(f"数据维度: {rows} x {cols}")
print(f"数据范围: [{data_array.min():.6f}, {data_array.max():.6f}]")

grid = data_array
grid_normalized = (grid - grid.min()) / (grid.max() - grid.min())

fig_size = min(6.0, max(rows, cols) / max(rows, cols) * 6.0)
fig, ax = plt.subplots(figsize=(fig_size, fig_size))

def output(grid):
    with open('perlin_grid.txt', 'w') as f:
        rows, cols = grid.shape
        for i in range(rows):
            row_data = []
            for j in range(cols):
                row_data.append(str(grid[i, j]))
            f.write(' '.join(row_data) + '\n')
    print(f"网格数据已保存为 perlin_grid.txt")

output(grid_normalized)

cell_size = 1
for i in range(rows):
    for j in range(cols):
        color_value = grid_normalized[i, j]
        gray_color = 1 - color_value
        
        rect = mpatches.Rectangle(
            (j * cell_size, (rows - 1 - i) * cell_size),
            cell_size,
            cell_size,
            facecolor=str(gray_color),
            edgecolor='black',
            linewidth=0.5
        )
        ax.add_patch(rect)

ax.set_xlim(0, cols * cell_size)
ax.set_ylim(0, rows * cell_size)
ax.set_aspect('equal')
ax.set_title(f'Perlin Noise Visualization ({rows}x{cols} Grid)', fontsize=14)
ax.set_xlabel('Column', fontsize=12)
ax.set_ylabel('Row', fontsize=12)

cbar = plt.colorbar(plt.cm.ScalarMappable(cmap='gray'), ax=ax, shrink=0.8)
cbar.set_label('Normalized Value', fontsize=10)

plt.tight_layout()
plt.savefig('perlin_visualization.png', dpi=150, bbox_inches='tight')
print(f"可视化图像已保存为 perlin_visualization.png")

plt.show()