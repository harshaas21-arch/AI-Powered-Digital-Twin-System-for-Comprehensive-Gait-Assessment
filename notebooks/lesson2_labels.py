from pathlib import Path

# Project root
project_root = Path(__file__).resolve().parent.parent

# Dataset folder
dataset_path = project_root / "dataset" / "UCI-HAR Dataset"

# Activity labels file
labels_file = dataset_path / "activity_labels.txt"

print("Reading activity labels...\n")

with open(labels_file, "r") as file:
    for line in file:
        print(line.strip())