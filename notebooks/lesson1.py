from pathlib import Path

# Go to the project root
project_root = Path(__file__).resolve().parent.parent

# Dataset folder
dataset_path = project_root / "dataset" / "UCI-HAR Dataset"

print("Project Folder:")
print(project_root)

print("\nDataset Folder:")
print(dataset_path)

print("\nDoes the dataset exist?")
print(dataset_path.exists())