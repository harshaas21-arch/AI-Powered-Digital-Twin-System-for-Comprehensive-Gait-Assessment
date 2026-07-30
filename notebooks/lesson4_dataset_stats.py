from pathlib import Path
import numpy as np

# Locating the project folder

project_root = Path(__file__).resolve().parent.parent
dataset_path = project_root / "dataset" / "UCI-HAR Dataset"

# File paths

x_train_file = dataset_path / "train" / "x_train.txt"
y_train_file = dataset_path / "train" / "y_train.txt"

# Load dataset

x_train = np.loadtxt(x_train_file)
y_train = np.loadtxt(y_train_file)

# Display

print("Training data shape: ", x_train.shape)
print("Labels shape: ", y_train.shape)
