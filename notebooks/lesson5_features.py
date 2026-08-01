from pathlib import Path

project_root = Path(__file__).resolve().parent.parent
dataset_path = project_root / "dataset" / "UCI-HAR Dataset"

features1 = dataset_path / "features.txt"

with open(features1, "r") as file:
    first_sample = file.readline()

features = first_sample.split()

for value in features[:20]:
    print(value)
